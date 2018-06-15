
#include "AlgorithmRegistration.h"
#include "MultiGameManager.h"
#include <iostream>


AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod) 
{
	//printf("Creating AlgorithmRegistration\n");
	MultiGameManager::getGameManager().registerAlgorithm(factoryMethod);
}

AlgorithmRegistration::~AlgorithmRegistration()
{
}


