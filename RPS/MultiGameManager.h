
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

	static MultiGameManager gameManager;

public:

	std::vector <std::function<std::unique_ptr<PlayerAlgorithm>()>> algos;

	static MultiGameManager& getGameManager()
	{
		return gameManager;
	}

	void registerAlgorithm(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod);

};


#endif // _MULTI_GAME_MANAGER_
