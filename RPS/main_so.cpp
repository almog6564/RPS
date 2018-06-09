
#include "MultiGameManager.h"
#include <iostream>

#ifndef _WIN64
#include <dlfcn.h>
#else
#define RTLD_NOW 0
void* dlopen(const char* a, int b) { (void*)a; (void)b; return (void*)a; }
void dlclose(void* a) { (void*)a; }
#endif



int main(void)
{
	std::cout << "starting main" << std::endl;
	
	printf("loading RSPPlayer_0.so\n");
	void* handle = dlopen("/specific/a/home/cc/students/cs/zeltsman/projects/RPS/RSPPlayer_0.so", RTLD_LAZY /*RTLD_NOW | RTLD_GLOBAL | RTLD_DEEPBIND*/);

	if (!handle) {
		printf("Error with %s\n", dlerror());
		return 0 ;
	}


	printf("finished dlopen v = %p\n", handle);

	printf("game manager size = %d\n", (int) MultiGameManager::getGameManager().factories.size());
	
	printf("before algos\n");

	std::vector <unique_ptr<PlayerAlgorithm>> algos;

	std::vector<unique_ptr<PiecePosition>> vectorToFill;

	algos.push_back(MultiGameManager::getGameManager().factories[0]());
		
	algos[0]->getInitialPositions(1, vectorToFill);


	algos.clear();

	printf("after algos CLEAR\n");


	MultiGameManager::getGameManager().factories.clear();
	printf("after factories CLEAR\n");

	dlclose(handle);

	printf("finished dlclose\n");

	std::cout << "finishing main" << std::endl;

	return 0;
}