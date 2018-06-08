
#include "MultiGameManager.h"

MultiGameManager MultiGameManager::gameManager;

void MultiGameManager::registerAlgorithm(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod)
{
	//TODO: should warn if id is already registered
	printf("pushing factoryMethod into MultiGameManager %p\n", (void*)this);
	MultiGameManager::getGameManager().factories.push_back(factoryMethod);
	printf("after pushing algos.size = %d \n", (int) factories.size());

}
