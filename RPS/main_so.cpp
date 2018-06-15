
#include "MultiGameManager.h"
#include <iostream>
#include <sys/types.h>
#include <cerrno>
#include <cstring>
using namespace std;

#ifndef _WIN64
#include <dirent.h>
#include <dlfcn.h>
#else
#define RTLD_LAZY 0
void* dlopen(const char* a, int b) { (void*)a; (void)b; return (void*)a; }
void dlclose(void* a) { (void*)a; }
std::string dlerror() { return ""; }
#endif

bool ends_with(std::string const & value, std::string const & ending)
{
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

int getdir(string dir, vector<string>& soFileNames)
{
	DIR *dp;
	struct dirent *dirp;
	if ((dp = opendir(dir.c_str())) == NULL) {
		cout << "Error(" << strerror(errno) << ") opening " << dir << endl;
		return -1;
	}

	while ((dirp = readdir(dp)) != NULL) 
	{
		string fname(dirp->d_name);
		
		if (ends_with(fname, ".so"))
		{
			soFileNames.push_back(fname);
			printf("Found: %s\n", fname.c_str());
		}
	}
	closedir(dp);
	return 0;
}


int main_so(void)
{
	string path = ".";

	vector<void*> soHandles;
	vector<string> soFileNames;
	std::vector <unique_ptr<PlayerAlgorithm>> algos;

	int soCnt;
	
	if (getdir(path, soFileNames) < 0) 
	{
		return 0;
	}

	soCnt = soFileNames.size();
	if (soCnt == 0)
	{
		cout << "Found 0 shared libraries in the path provided, exiting" << endl;
	}

	for (auto& soFname : soFileNames)
	{
		void* handle = dlopen((path + "/" + soFname).c_str(), RTLD_LAZY);

		if (!handle) {
			printf("Error with %s\n", dlerror());
			return 0;
		}

		printf("Adding [%s] handle 0x%p to soHandles\n", soFname.c_str(), handle);
		soHandles.push_back(handle);
	}


	for (auto& factory : MultiGameManager::getGameManager().factories)
	{
		algos.push_back(factory());
	}
	


	algos.clear();

	MultiGameManager::clearFactories();

	for (auto& so : soHandles)
	{
		dlclose(so);
	}


	return 0;
}