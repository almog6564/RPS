
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

volatile bool done;
mutex mx;
condition_variable tasksAvailable, tasksCompleted;

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

int main(void)
{
	int threads_count = 4;
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

	MultiGameManager& gameManager = MultiGameManager::getGameManager();
	int players_count = (int)gameManager.factories.size();
	printf("Got %d factories\n", players_count);
	vector<thread> threadsPool;
	vector<atomic<int>> scoreBoard(players_count);
	TaskPool taskPool;


	//run threads
	for (int i = 0; i < threads_count - 1; i++)
		threadsPool.push_back(thread(run_thread, &taskPool, &scoreBoard));

	//produce tasks for consumers
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

	if (threads_count == 1) //single main thread execution
	{
		for (auto& task : taskPool.getQueue())
		{
			int winner;
			task->run(&winner);
			pair<int, int> playersGlobalIDs = task->getPlayersGlobalIDs();

			if (winner == 0) //Tie
			{
				scoreBoard[playersGlobalIDs.first]++;
				scoreBoard[playersGlobalIDs.second]++;
			}
			else if (winner == 1)
				scoreBoard[playersGlobalIDs.first] += 3;
			else
				scoreBoard[playersGlobalIDs.second] += 3;
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

	return 0;
}