
#include "AlgorithmRegistration.h"
#include "MultiGameManager.h"
#include <iostream>


AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod) 
{
	//std::cout << "Creating AlgorithmRegistration in ctor from MACRO" << std::endl;
	MultiGameManager::getGameManager().registerAlgorithm(factoryMethod);
}

AlgorithmRegistration::~AlgorithmRegistration()
{
	//std::cout << "DELETING AlgorithmRegistration" << std::endl;
}


