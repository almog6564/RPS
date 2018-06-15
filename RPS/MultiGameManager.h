
#ifndef _MULTI_GAME_MANAGER_
#define _MULTI_GAME_MANAGER_

#include "defs.h"
#include "PlayerAlgorithm.h"
#include <vector>
#include <functional>
#include <memory>

typedef std::function<std::unique_ptr<PlayerAlgorithm>()> PlayerAlgorithmFactory;

class MultiGameManager 
{
	MultiGameManager() {}
	
	~MultiGameManager() {}


	static MultiGameManager gameManager;


public:

	std::vector <PlayerAlgorithmFactory> factories;

	static MultiGameManager& getGameManager(void)
	{
		return gameManager;
	}

	void registerAlgorithm(PlayerAlgorithmFactory factoryMethod);

	static void clearFactories(void)
	{
		gameManager.factories.clear();
	}
};


#endif // _MULTI_GAME_MANAGER_
