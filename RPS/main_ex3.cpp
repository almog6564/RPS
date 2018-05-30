
#include "MultiGameManager.h"
#include <iostream>

int main(void)
{
	std::cout << "starting main" << std::endl;
	auto& method = MultiGameManager::getGameManager().algos[0];

	method();

	std::cout << "finishing main" << std::endl;

	return 0;
}