
#include "MultiGameManager.h"
#include <typeinfo>

MultiGameManager MultiGameManager::gameManager;

void MultiGameManager::registerAlgorithm(PlayerAlgorithmFactory factoryMethod)
{
	MultiGameManager::getGameManager().factories.push_back(factoryMethod);
}
