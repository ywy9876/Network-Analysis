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

#include "johnson.h"
//#include "mydegree.h"

#ifndef MYGRAPH_H_
#define MYGRAPH_H_

typedef pair<double, int> WArc; // weighted arc
typedef map<int, vector<WArc> > WGraph; // weighted graph
/*
struct Edge {
  string origin;
  string dest;
};
*/
struct Node {
  vector<string> neighbours;
  vector<double> distances;
};

typedef map<string, Node > Graph; // unweighted graph
typedef vector <pair<string, string> > Edges; // stores all the edges


const int infinit = 100000000;

typedef std::pair < int, int >Edge;

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{
    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;

    std::vector<T> vec(first, last);
    return vec;
}

class MyGraph {

public:
	Graph G; // a dictionary which the key is the name of node (e.g a word) and the value is a adjacency list [b, c, etc.]
	Edges E; // all the edges
	vector<string> Nodes;// stores the words in the order of incoming
	map<string, int> nodeIndex;// stores the correspondences between words and indices
	map<int, string> indexNode;// stores the correspondences between indices and words
	int n;
	int m; // n for number of nodes, m for number of edges
	double closeness_centrality;

	MyGraph();
	MyGraph(const string file_name);
	void print();
	void dijkstra( int s, vector<double>& d);
	void calculate_closeness_v1();
	double closeness_batch (vector<string>& Nodes);
	void sort_Nodes (string sort_type ="shuffle");
	void print_nodes_vector();
	void calculate_closeness_v2_bounded(double xAH=0.5);
	void calculate_closeness_v3();
	vector<Edge> transform_to_edge_vect();



};




#endif /* MYGRAPH_H_ */
