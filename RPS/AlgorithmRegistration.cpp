
#include "AlgorithmRegistration.h"
#include "MultiGameManager.h"
#include <iostream>


AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod) 
{
	MultiGameManager::getGameManager().registerAlgorithm(factoryMethod);
}

AlgorithmRegistration::~AlgorithmRegistration()
{
}


