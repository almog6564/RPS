
#include "AlgorithmRegistration.h"

// AlgorithmRegistration.cpp
AlgorithmRegistration::AlgorithmRegistration(std::function<std::unique_ptr<PlayerAlgorithm>()> factoryMethod) 
{
	factoryMethod();
}