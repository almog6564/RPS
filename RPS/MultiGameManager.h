
#ifndef _MULTI_GAME_MANAGER_
#define _MULTI_GAME_MANAGER_

#include "defs.h"
#include "PlayerAlgorithm.h"
#include <vector>
#include <functional>
#include <memory>

class MultiGameManager 
{
	MultiGameManager() {}
	
	~MultiGameManager() {}


	static MultiGameManager gameManager;


public:

	std::vector <std::function<std::unique_ptr<PlayerAlgorithm>()>> factories;

	static MultiGameManager& getGameManager(void)
	{
		return gameManager;
	}

	void registerAlgorithm(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod);

	static void clearFactories(void)
	{
		gameManager.factories.clear();
	}
};


#endif // _MULTI_GAME_MANAGER_
