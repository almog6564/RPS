
#include "MultiGameManager.h"
#include "game.h"
#include <iostream>
#include <queue>
#include <mutex>
#include <condition_variable>	
#include <thread>
#include <atomic>
#include <iostream>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
#include <algorithm>
#include <list>
#include <dirent.h>
#include <dlfcn.h>
#include <sstream>



using namespace std;

/*Global Variables*/
volatile bool done; // Indicating that all tasks are complete
bool singleThread; // Indicating that this program runs with a single thread
mutex mx;
condition_variable tasksAvailable, tasksCompleted;


bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int getdir(string dir, vector<string>& soFileNames)
{
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) 
	{
		cout << "Error while opening the directory [" << dir << "]: " << strerror(errno) << endl;
		return -1;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		string fname(dirp->d_name);

		if (ends_with(fname, ".so"))
		{
			soFileNames.push_back(fname);
			//printf("Found: %s\n", fname.c_str());
		}
	}
	closedir(dp);
	return 0;
}


/*
	The Following class is built in the Consumer-Producer Design Pattern as learned.
	The main thread is the producer in this case, creating all games (=tasks) to play, and all other threads are the consumers
	All tasks are gathered in a "Pool", available for all threads to try and take tasks using locks
*/
class TaskPool
{
	queue<unique_ptr<Game>> taskQueue; 

public:
	/*The producer simply pushes a task into the taskQueue*/
	void producer(unique_ptr<Game>& task)
	{
		lock_guard<mutex> lock(mx);
		taskQueue.push(move(task));
	}

	/*Consumer will try to get a task from the queue. If the queue seems empty, it will sleep until awaken by producer*/
	unique_ptr<Game> consumer()
	{
		unique_lock<mutex> lock(mx);

		//critical section
		while (taskQueue.empty() && !done && !singleThread)
		{
			tasksAvailable.wait(lock); //wait until awaken by producer in case of a new task or all tasks are completed
		}
		if (done)
			return nullptr;
		unique_ptr<Game> ret = move(taskQueue.front()); //take ownership
		taskQueue.pop();

		if (taskQueue.empty())
		{
			if (singleThread)
				done = 1;
			else
				tasksCompleted.notify_one(); //notify the main thread that there are no more tasks availble
		}
		return ret;
	}

	bool isEmpty() const
	{
		return taskQueue.empty();
	}

	int size() const
	{
		return (int)taskQueue.size();
	}
};
/* END OF TASKPOOL CLASS*/


/*
	The run method of threads. 
	The threads run "forever", until the done signal is on, signaling that all tasks are completed.
	In each iteration, get one task to do, and if succeded, release the lock, and execute the game. 
	At the end of each game, write scores to scoreboard (implemented with atomic integers)
*/
void run_thread(TaskPool *taskPool, vector<atomic<int>> *scoreBoard)
{
	while (!done)
	{
		unique_ptr<Game> game = taskPool->consumer();
		if (!game) //queue is empty
			break;
		int winner = -1;
		game->run(&winner);

		pair<int, int> playersGlobalIDs = game->getPlayersGlobalIDs();
		//fill scoreBoard
		if (winner == 0) //Tie
		{
			(*scoreBoard)[playersGlobalIDs.first]++;
			(*scoreBoard)[playersGlobalIDs.second]++;
		}
		else if (winner == 1)
			(*scoreBoard)[playersGlobalIDs.first] += 3;
		else if (winner == 2)
			(*scoreBoard)[playersGlobalIDs.second] += 3;
		else
		{
			cout << "error: wrong winner value" << endl;
			return;
		}
	}
}

class AlgorithmFactoryContext
{
	PlayerAlgorithmFactory*	pFactory;
	int						id;
	int						gamesPlayedCount;

public:
	AlgorithmFactoryContext(PlayerAlgorithmFactory*	factoryArg, int idArg) :
		pFactory(factoryArg), id(idArg), gamesPlayedCount(0) {};

	AlgorithmFactoryContext() : pFactory(nullptr), id(-1), gamesPlayedCount(0) {};

	unique_ptr<PlayerAlgorithm> createAlgorithm()		{ return (*pFactory)(); }
	int getId()											{ return id; }
	void incrementCounter()								{ gamesPlayedCount++; }
	int getGamesPlayCount()								{ return gamesPlayedCount; }
};

bool parseCmdLineArgs(int argc, char* argv[], int& threads_count, string& path)
{
	string threadsStr = "-threads";
	string pathStr = "-path";
	bool ret = false;
	int temp = -1;

	switch (argc)
	{
	case 1:
		ret = true;
		break;

	case 3:
		if (threadsStr == argv[1])
		{
			istringstream chk(argv[2]);
			chk >> temp;
			if (!chk.fail())
			{
				threads_count = temp;
				ret = true;
			}
		}
		else if (pathStr == argv[1])
		{
			path = argv[2];
			ret = true;
		}
		break;

	case 5:
		if (threadsStr == argv[1])
		{
			istringstream chk(argv[2]);
			chk >> temp;
			if (!chk.fail())
			{
				threads_count = temp;

				if (pathStr == argv[3])
				{
					path = argv[4];
					ret = true;
				}
			}
		}
		else if (pathStr == argv[1])
		{
			path = argv[2];
			
			if (threadsStr == argv[3])
			{
				istringstream chk(argv[4]);
				chk >> temp;
				if (!chk.fail())
				{
					threads_count = temp;
					ret = true;
				}
			}
		}

		break;
	}

	return ret;
}

int main(int argc, char* argv[])
{
	string path = ".";
	int threads_count = 4; //default
	int rounds = 30; //maximum games for each player
	done = false;

	if (!parseCmdLineArgs(argc, argv, threads_count, path))
	{
		cout << "\nUsage: ex3 [-thread_couns <num_threads>] [-path <location_of_algorithms>]\n" << endl;
		return 1;
	}

	singleThread = threads_count == 1 ? true : false;

	
	cout << "args: threads " << threads_count << " path [" << path << "]" << endl;

	/************************************************************************/
	/* Get SO Handles														*/
	/************************************************************************/


	vector< pair<string, void*> > soHandles;
	vector<string> soFileNames;
	std::vector <unique_ptr<PlayerAlgorithm>> algos;

	int soCnt;

	if (getdir(path, soFileNames) < 0)
	{
		return 0;
	}

	soCnt = soFileNames.size();
	if (soCnt == 0)
	{
		cout << "Found 0 shared libraries in the path provided, exiting" << endl;
		return 1;
	}

	else if (soCnt == 1)
	{
		cout << "Found only one shared libraries in the path provided, can't create a tournament, exiting" << endl;
		return 1;
	}

	for (auto& soFname : soFileNames)
	{
		void* handle = dlopen((path + "/" + soFname).c_str(), RTLD_LAZY);

		if (!handle) {
			printf("Error with %s\n", dlerror());
			return 0;
		}

		//printf("Adding [%s] handle 0x%p to soHandles\n", soFname.c_str(), handle);

		int pos = soFname.find('_');

		soHandles.emplace_back(soFname.substr(pos + 1, soFname.size() - pos - 3 - 1), handle);
	}

	/************************************************************************/
	/* Start logic														*/
	/************************************************************************/

	auto& factories		= MultiGameManager::getGameManager().factories;
	int	players_count	= (int)factories.size();
	int randPlayer1, randPlayer2;
	int ID1, ID2, gameId = 0;
	vector<thread> threadsPool;
	vector<atomic<int>> scoreBoard(players_count + 1); //+1 is to collect "garbage" scores of player who already had 30 games
	TaskPool taskPool;
	random_device				seed;			//Will be used to obtain a seed for the random number engine
	mt19937						gen(seed());	//Standard mersenne_twister_engine seeded with seed()

	//run threads
	for (int i = 0; i < threads_count - 1; i++)
		threadsPool.push_back(thread(run_thread, &taskPool, &scoreBoard));

	/************************************************************************/
	/*    Produce Tasks for Consumers										*/
	/************************************************************************/


	list<AlgorithmFactoryContext> algosCtx(0); //this vector needs to be initialized with algos and ID's (0-number of players) (algos will not be needed anymore)

	int genID = 0;
	for (int factoryIndex = 0; factoryIndex < players_count; factoryIndex++)
	{
		algosCtx.emplace_back(&factories[factoryIndex], genID++);
	}

	vector<AlgorithmFactoryContext> donePlaying(0); //This vector will hold algorithms that reached 30 games to use them for extra needed games
	donePlaying.reserve(players_count);

	while (!algosCtx.empty()) //loop until no remaining algorithms with less than 30 games played
	{
		//each iteration is kind of Round

		vector<bool> selectionVector(algosCtx.size(), false); //in this round who played

		while (count(selectionVector.cbegin(), selectionVector.cend(), false) > 0 && algosCtx.size() > 0)	// iterate while not all games were played
		{
			//create a game and put in produce in each while iteration
			bool lastPlayerInCurrRound = count(selectionVector.cbegin(), selectionVector.cend(), false) == 1 ? 1 : 0;
			bool lastRemainingPlayer = algosCtx.size() == 1 ? 1 : 0;

			//randomize first player
			uniform_int_distribution<>	getRandomAlgo(0, algosCtx.size() - 1);		//Distributed random
			do
			{
				randPlayer1 = getRandomAlgo(gen);
			} while (selectionVector[randPlayer1]); //while this player already played this round

			selectionVector[randPlayer1] = true;
			auto it = algosCtx.begin();
			advance(it, randPlayer1);
			auto& p1context = *it;
			p1context.incrementCounter();
			unique_ptr<PlayerAlgorithm> p1 = move(p1context.createAlgorithm()); //create player1

			ID1 = p1context.getId();

			if (p1context.getGamesPlayCount() == rounds)
			{
				//in this case, remove the algorithm from algorithms, and transfer it to donePlaying vector
				selectionVector.erase(selectionVector.begin() + randPlayer1);
				donePlaying.push_back(*it);
				algosCtx.erase(it);
			}

			unique_ptr<PlayerAlgorithm> p2;

			if (lastRemainingPlayer) //only one player remaining with less than 30 games played
			{
				//in this case, get a random algorithm from donePlaying vector
				uniform_int_distribution<>	getRandomDonePlayingAlgo(0, donePlaying.size() - 1);	//Distributed random
				randPlayer2 = getRandomDonePlayingAlgo(gen);
				p2 = move(donePlaying[randPlayer2].createAlgorithm()); //create player2
				ID2 = players_count; //this will not be counted
			}
			else
			{
				if (lastPlayerInCurrRound)
				{
					//in this case randomize another player from all remaining algorithms
					uniform_int_distribution<>	getRandomAlgo(0, algosCtx.size() - 1);		//Distributed random
					do
					{
						randPlayer2 = getRandomAlgo(gen);
						auto it = algosCtx.begin();
						advance(it, randPlayer2);
						auto& p2context = *it;
						ID2 = p2context.getId();
					} while (ID1 == ID2); //loop while this player already played this round
				}
				else
				{
					//randomize second player
					uniform_int_distribution<>	getRandomAlgo(0, algosCtx.size() - 1);		//Distributed random
					do
					{
						randPlayer2 = getRandomAlgo(gen);
					} while (selectionVector[randPlayer2]); //verify that it is not the same player
															//}
				}
				selectionVector[randPlayer2] = true;
				auto it2 = algosCtx.begin();
				advance(it2, randPlayer2);
				auto& p2context = *it2;
				p2context.incrementCounter();
				p2 = move(p2context.createAlgorithm()); //create player2
				ID2 = p2context.getId();

				if (p2context.getGamesPlayCount() == rounds)
				{
					//in this case, remove the algorithm from algorithms, and transfer it to donePlaying vector
					selectionVector.erase(selectionVector.begin() + randPlayer2);
					donePlaying.push_back(*it2);
					algosCtx.erase(it2);
				}
			}
			
			//create game
			unique_ptr<Game> game = make_unique<Game>(p1, p2, ID1, ID2, gameId++);
			taskPool.producer(game);
			tasksAvailable.notify_one(); //notify just a single waiting thread, because only one task is added
		}
	}
	//printf("Produced all games\n");

	if (singleThread) //single main thread execution
		run_thread(&taskPool, &scoreBoard);

	else //threads_count > 1
	{
		{
			unique_lock<mutex> lock(mx);
			while (!taskPool.isEmpty())
			{
				tasksCompleted.wait(lock); //wait for tasks to be completed
			}
		}
		done = true;
		tasksAvailable.notify_all(); //wake up all sleeping threads to finish their run
	}

	//join threads
	for (auto& th : threadsPool)
		th.join();

	for (unsigned int i = 0; i < scoreBoard.size() - 1; i++)
		cout << soHandles[i].first << " " << scoreBoard[i] << endl;

	MultiGameManager::clearFactories();

	for (auto& so : soHandles)
	{
		dlclose(so.second);
	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/







	return 0;
}