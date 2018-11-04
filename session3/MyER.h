#include <iostream>
#include <string>
#include <vector>
#include <queue>
#include <map>
#include <iterator>
#include <dirent.h>
#include <cstring>
#include <memory>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <random>
#include <chrono>
#include <omp.h>

//#include <utility>

using namespace std;


#ifndef MYER_H_
#define MYER_H_

#include "MyGraph.h"

class MyER : public MyGraph {

public:

	map <string, char> E_ER; // key composed by origin+destination words

	MyER(const MyGraph& g, int newseed);
	void printER ();
	void printNodeIndex();
	void printE_ER();

};


#endif /* MYER_H_ */
