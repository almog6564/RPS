
#include "MultiGameManager.h"
#include "game.h"
#include <iostream>
#include <deque>
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


#ifndef _WIN64
#include <dirent.h>
#include <dlfcn.h>
#else
#define RTLD_LAZY 0
void* dlopen(const char* a, int b) { (void*)a; (void)b; return (void*)a; }
void dlclose(void* a) { (void*)a; }
std::string dlerror() { return ""; }
typedef void DIR;
void* opendir(const char* a) { return (void*)a; }
struct dirent
{
	const char* d_name;
};
struct dirent * readdir(DIR* a) { return (void*)a; }
void closedir(void* a) { (void*)a; }

#endif


using namespace std;

volatile bool done;
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
	if ((dp = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << strerror(errno) << ") opening " << dir << endl;
		return -1;
	}

	while ((dirp = readdir(dp)) != NULL)
	{
		string fname(dirp->d_name);

		if (ends_with(fname, ".so"))
		{
			soFileNames.push_back(fname);
			printf("Found: %s\n", fname.c_str());
		}
	}
	closedir(dp);
	return 0;
}



class TaskPool
{
	deque<unique_ptr<Game>> taskQueue;

public:
	void producer(unique_ptr<Game>& task)
	{
		lock_guard<mutex> lock(mx);
		taskQueue.push_back(move(task));
	}

	unique_ptr<Game> consumer()
	{
		unique_lock<mutex> lock(mx);

		//critical section
		while (taskQueue.empty() && !done)
		{
			tasksAvailable.wait(lock);
		}
		if (done)
			return nullptr;
		unique_ptr<Game> ret = move(taskQueue.front()); //take ownership
		taskQueue.pop_front();

		if (taskQueue.empty())
			tasksCompleted.notify_one();

		//cout << this_thread::get_id() << endl;  //debug
		//cout << ret->getGameID() << endl;		//debug
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

	deque<unique_ptr<Game>>& getQueue()
	{
		return taskQueue;
	}

};

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

int main(void)
{
	int threads_count = 10;
	int rounds = 30;
	done = false;

	/************************************************************************/
	/* Get SO Handles														*/
	/************************************************************************/

	string path = ".";

	vector<void*> soHandles;
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
	}

	for (auto& soFname : soFileNames)
	{
		void* handle = dlopen((path + "/" + soFname).c_str(), RTLD_LAZY);

		if (!handle) {
			printf("Error with %s\n", dlerror());
			return 0;
		}

		printf("Adding [%s] handle 0x%p to soHandles\n", soFname.c_str(), handle);
		soHandles.push_back(handle);
	}

	/************************************************************************/
	/* Start logic														*/
	/************************************************************************/

	auto& factories		= MultiGameManager::getGameManager().factories;
	int	players_count	= (int)factories.size();
	int randPlayer1, randPlayer2;
	int ID1, ID2, gameId = 0;
	vector<thread> threadsPool;
	vector<atomic<int>> scoreBoard(players_count);
	TaskPool taskPool;
	random_device				seed;			//Will be used to obtain a seed for the random number engine
	mt19937						gen(seed());	//Standard mersenne_twister_engine seeded with seed()

	printf("Got %d factories\n", players_count);


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

	printf("Created algosCtx, donePlaying\n");

	while (!algosCtx.empty()) //loop until no remaining algorithms with less than 30 games played
	{
		printf("\n\n\n------------------------------------------\nRound: counters (");
		for (auto& algo : algosCtx)
		{
			printf("%d,", algo.getGamesPlayCount());
		}
		printf(")\n");

		//each iteration is kind of Round

		vector<bool> selectionVector(algosCtx.size(), false); //in this round who played

		while (count(selectionVector.cbegin(), selectionVector.cend(), false) > 0 && algosCtx.size() > 0)	// iterate while not all games were played
		{
			//create a game and put in produce in each while iteration
			bitset<5> b(*(selectionVector.begin()._M_p)); //for debug only

			cout << "\n##### Vector = " << b << " #####" << endl;
			printf("Round: algosCtx.size = %d\n", (int)algosCtx.size());

			if (algosCtx.size() == 1)
			{
				randPlayer1 = 0;
			}
			else
			{
				//randomize first player

				uniform_int_distribution<>	getRandomAlgo(0, algosCtx.size() - 1);		//Distributed random
				printf("Randomizing p1...\n");

				do
				{
					randPlayer1 = getRandomAlgo(gen);
					printf("%d, ", randPlayer1);
				} while (selectionVector[randPlayer1]); //while this player already played this round
			}

			printf("\nrandPlayer1 = %d\n", randPlayer1);

			selectionVector[randPlayer1] = true;

			auto it = algosCtx.begin();
			advance(it, randPlayer1);

			auto& p1context = *it;

			p1context.incrementCounter();

			unique_ptr<PlayerAlgorithm> p1 = move(p1context.createAlgorithm()); //create player1

			printf("Created p1\n");


			if (p1context.getGamesPlayCount() == rounds)
			{
				//in this case, remove the algorithm from algorithms, and transfer it to donePlaying vector
				printf("Removing %d from algosCtx, adding to donePlaying\n", it->getId());

				selectionVector.erase(selectionVector.begin() + randPlayer1);
				donePlaying.push_back(*it);
				algosCtx.erase(it);

			}

			ID1 = p1context.getId();

			bitset<5> b2(*(selectionVector.begin()._M_p)); //for debug only

			cout << "##### Vector = " << b2 << " #####" << endl;
			printf("Round: algosCtx.size = %d\n", (int)algosCtx.size());

			unique_ptr<PlayerAlgorithm> p2;

			if (count(selectionVector.cbegin(), selectionVector.cend(), false) == 0) //only one player remaining - odd number of players
			{
				printf("*** ODD NUMBER ***\n");

				//in this case, get a random algorithm from donePlaying vector
				//make sure not to count their result - can be done using
				uniform_int_distribution<>	getRandomDonePlayingAlgo(0, donePlaying.size() - 1);		//Distributed random

				randPlayer2 = getRandomDonePlayingAlgo(gen);


				p2 = move(donePlaying[randPlayer2].createAlgorithm()); //create player2

				ID2 = -1; //some constant that is defined as: "DO NOT COUNT THIS PLAYERS RESULT IN SCOREBOARD"
			}

			else
			{
				if (algosCtx.size() == 1)
				{
					randPlayer2 = 0;
				}
				else
				{
					//randomize second player
					uniform_int_distribution<>	getRandomAlgo(0, algosCtx.size() - 1);		//Distributed random
					printf("Randomizing p2...\n");

					do
					{
						randPlayer2 = getRandomAlgo(gen);
						printf("%d, ", randPlayer2);
					} while (selectionVector[randPlayer2]); //verify that it is not the same player
				}
			}
			printf("randPlayer2 = %d\n", randPlayer2);

			selectionVector[randPlayer2] = true;

			auto it2 = algosCtx.begin();
			advance(it2, randPlayer2);

			auto& p2context = *it2;

			p2context.incrementCounter();

			p2 = move(p2context.createAlgorithm()); //create player2
			ID2 = p2context.getId();
			printf("Created p2\n");

			if (p2context.getGamesPlayCount() == rounds)
			{
				//in this case, remove the algorithm from algorithms, and transfer it to donePlaying vector
				donePlaying.push_back(*it2);
				algosCtx.erase(it2);
			}

			//create game
			unique_ptr<Game> game = make_unique<Game>(p1, p2, ID1, ID2, gameId++);
			printf("Game (%d) created: %d - %d\n\n", gameId - 1, ID1, ID2);
			taskPool.producer(game);
			tasksAvailable.notify_one();
		}
	}

#if 0
	for (int j = 0; j < rounds; j++)
	{
		for (int i = 0; i < players_count; i += 2)
		{
			unique_ptr<PlayerAlgorithm> p1 = move(gameManager.factories[i]());
			unique_ptr<PlayerAlgorithm> p2 = move(gameManager.factories[i + 1]());
			unique_ptr<Game> game = make_unique<Game>(p1, p2, i, i + 1, (i / 2) + (players_count / 2)*j);
			taskPool.producer(game);
			tasksAvailable.notify_one();
		}
	}
#endif

#define addToScore(pid,score)	if(pid>=0) scoreBoard[pid]+=score;

	if (threads_count == 1) //single main thread execution
	{
		for (auto& task : taskPool.getQueue())
		{
			int winner;
			task->run(&winner);
			pair<int, int> playersGlobalIDs = task->getPlayersGlobalIDs();

			if (winner == 0) //Tie
			{
				addToScore(playersGlobalIDs.first, 1);
				addToScore(playersGlobalIDs.second, 1);
			}
			else if (winner == 1)
			{
				addToScore(playersGlobalIDs.first, 3);
			}
			else
			{
				addToScore(playersGlobalIDs.second, 3);
			}
		}
	}
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
		tasksAvailable.notify_all();
	}

	//join threads
	for (auto& th : threadsPool)
		th.join();

	for (int score : scoreBoard)
		cout << score << endl;


	MultiGameManager::clearFactories();

	for (auto& so : soHandles)
	{
		dlclose(so);
	}


	/************************************************************************/
	/*                                                                      */
	/************************************************************************/







	return 0;
}