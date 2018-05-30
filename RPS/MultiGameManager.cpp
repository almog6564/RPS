
#include "MultiGameManager.h"


MultiGameManager MultiGameManager::gameManager;


void MultiGameManager::registerAlgorithm(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod)
{
	//TODO: should warn if id is already registered
	algos.push_back(factoryMethod);
}
