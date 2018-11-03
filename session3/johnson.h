//=======================================================================
// Copyright 2001 Jeremy G. Siek, Andrew Lumsdaine, Lie-Quan Lee,
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//=======================================================================
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
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
//#include <utility>
using namespace std;


#include <boost/config.hpp>
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include <boost/property_map/property_map.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/graphviz.hpp>
#include <boost/graph/johnson_all_pairs_shortest.hpp>
#ifndef JOHNSON_H_
#define JOHNSON_H_



struct Node2 {
  std::vector<std::string> neighbours;
  std::vector<double> distances;
};

typedef std::map<std::string, Node2 > Graph2; // unweighted graph
typedef std::vector <std::pair<std::string, std::string> > Edges2; // stores all the edges


typedef std::pair < int, int >Edge;

class johnson {

public:
	int** D;
	int V;
	std::size_t EE;
	std::vector<Edge> edge_vect;
	Edge* edge_array;



	johnson(int n, std::vector<Edge> edge_vect);
	//void read_graph2(std::string file_name);
	void johnson_all_pairs_dijkstra();
	void printDMatrix();
	void printEdgeArray();
	void freeMem ();
};


#endif /* JOHNSON_H_ */
