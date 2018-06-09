
#include "MultiGameManager.h"
#include <typeinfo>

MultiGameManager MultiGameManager::gameManager;

void MultiGameManager::registerAlgorithm(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod)
{
	auto& facs = MultiGameManager::getGameManager().factories;

	for (auto& factory : facs)
	{
		if (typeid(factoryMethod).name() == typeid(factory).name())
		{
			printf("Factory \"%s\" already found, not inserting..", typeid(factoryMethod).name());
			return;
		}
	}
	MultiGameManager::getGameManager().factories.push_back(factoryMethod);
}
