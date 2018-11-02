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

#include "MyGraph.h"

#ifndef MYSWITCHING_H_
#define MYSWITCHING_H_


class MySwitching : public MyGraph {

public:

	int Q = 0;
	int QE = 0;
	map <string, char> actualEdges; // char to save space


	MySwitching(const MyGraph& g, uniform_int_distribution<int>& dist, default_random_engine& gen, int Q);


};
#endif /* MYSWITCHING_H_ */
