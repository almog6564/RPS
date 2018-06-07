
#include "MultiGameManager.h"
#include "game.h"
#include <iostream>
#include <deque>
#include <mutex>
#include <condition_variable>	
#include <thread>
#include <atomic>

using namespace std;

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

	MultiGameManager gameManager = MultiGameManager::getGameManager();
	int players_count = (int)gameManager.algos.size();
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
			unique_ptr<PlayerAlgorithm> p1 = move(gameManager.algos[i]());
			unique_ptr<PlayerAlgorithm> p2 = move(gameManager.algos[i + 1]());
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

	return 0;
}