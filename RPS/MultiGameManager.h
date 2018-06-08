
#ifndef _MULTI_GAME_MANAGER_
#define _MULTI_GAME_MANAGER_

#include "defs.h"
#include "PlayerAlgorithm.h"
#include <vector>
#include <functional>
#include <memory>

class MultiGameManager 
{
	MultiGameManager() { printf("creating static MultiGameManager %p\n", (void*)this); }
	
	~MultiGameManager() { printf("DELETING static MultiGameManager %p\n", (void*)this); }


	static MultiGameManager gameManager;


public:

	std::vector <std::function<std::unique_ptr<PlayerAlgorithm>()>> factories;

	static MultiGameManager& getGameManager()
	{
		printf("getting static MultiGameManager %p\n",(void*)&gameManager);
		return gameManager;
	}

	void registerAlgorithm(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod);

};


#endif // _MULTI_GAME_MANAGER_
