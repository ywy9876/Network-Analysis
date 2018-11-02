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


#include "/home/pau/boost_1_68_0/boost/config.hpp"
#include <fstream>
#include <iostream>
#include <vector>
#include <iomanip>
#include "/home/pau/boost_1_68_0/boost/property_map/property_map.hpp"
#include "/home/pau/boost_1_68_0/boost/graph/adjacency_list.hpp"
#include "/home/pau/boost_1_68_0/boost/graph/graphviz.hpp"
#include "/home/pau/boost_1_68_0/boost/graph/johnson_all_pairs_shortest.hpp"

#include "johnson.h"



int testj2(std::string file_name){


	std::string line;
	std::vector<Edge> edge_vect;
	int n = 0;
	std::cout << "testj2 ini" << std::endl;
	std::ifstream myfile(file_name);
	cout << "opening file" << endl;
	if (myfile.is_open()) {
		cout << " file openned" << endl;

		// initializing the needed variables
		bool firstLine = true;
		int sameNode = 0;
		int m = 0;
		Graph2 G;
		Edges2 E2;
		std::vector<std::string> Nodes;// stores the words in the order of incoming
		std::map<std::string, int> nodeIndex;// stores the correspondences between words and indices
		std::map<int, std::string> indexNode;
		int linenum = 0;

		cout << " start to read lines" << endl;
		// read all lines of the file
		while (std::getline(myfile, line)) {
			if (!firstLine) {
				std::istringstream iss(line);
				std::string a, b;
				// if the line contains only two elements
				if (!(iss >> a >> b)) {
					"Not exact two elements in one line";
					return 1;
				}
				// avoiding loop
				if (a != b) {

					//cout << a << " " << b << endl;
					std::map <std::string, Node2>::iterator it;
					it = G.find(a);
					// if the dictionary already contains word a
					if (it != G.end()) {
						// if there isn't already an edge a->b
						if (find((it->second).neighbours.begin(), (it->second).neighbours.end(), b) == (it->second).neighbours.end()) {
							G[a].neighbours.push_back(b); // add b to adjacency list
							E2.push_back(make_pair(a, b)); // add to the edges vector
							++m;
							edge_vect.push_back(Edge(nodeIndex[a],nodeIndex[b]));
							linenum++;
						}
					}
					else {
						// since word a does not present in the dictionary, we add it with the edge
						G[a].neighbours.push_back(b);
						E2.push_back(make_pair(a, b));
						Nodes.push_back(a);
						nodeIndex[a] =n;
						indexNode[n] = a;
						++n;
						++m;

						edge_vect.push_back(Edge(nodeIndex[a],nodeIndex[b]));
						linenum++;
					}

					it = G.find(b);
					// if the node did not show up yet
					if (it == G.end()) {
						// since word b does not present in the dictionary, we add it with the edge
						G[b].neighbours.push_back(a);
						Nodes.push_back(b);
						nodeIndex[b] = n;
						indexNode[n] = b;
						++n;
						edge_vect.push_back(Edge(nodeIndex[b],nodeIndex[a]));
						linenum++;
					}
					// if the dictionary already contains word b
					else {
						// if there isn't already an edge b->a
						if (find( G[b].neighbours.begin(),  G[b].neighbours.end(), a) ==  G[b].neighbours.end()) {
							 G[b].neighbours.push_back(a);
							 edge_vect.push_back(Edge(nodeIndex[b],nodeIndex[a]));
						}
					}



				}
				else ++sameNode;

			}
			else firstLine = false;

		}
		 myfile.close();
		 cout << " Graph has been read E=" << edge_vect.size()  << " samenode=" << sameNode << endl;

	//---------------------------------------------------



	  using namespace boost;
	  typedef adjacency_list<vecS, vecS, directedS, no_property,
		property< edge_weight_t, int, property< edge_weight2_t, int > > > Graph;
	  const int V = n;
	  // basque n: 12207, m: 25541 ,
	  typedef std::pair < int, int >Edge;
	  const std::size_t EE = edge_vect.size();
//	  Edge edge_array[] =
//	    { Edge(0, 1), Edge(0, 2), Edge(0, 3), Edge(0, 4), Edge(0, 5),
//	    Edge(1, 2), Edge(1, 5), Edge(1, 3), Edge(2, 4), Edge(2, 5),
//	    Edge(3, 2), Edge(4, 3), Edge(4, 1), Edge(5, 4)
//	  };
	  Edge edge_array[EE] = {};
	  for (int jj=0; jj < edge_vect.size(); jj++){
		  edge_array[jj] = edge_vect[jj];
	  }
	  //std::fill_n(edge_array,EE,Edge(1,5));

	  const std::size_t E = sizeof(edge_array) / sizeof(Edge);

	  cout << " edge_array  has been created E=" << E << endl;


//	#if defined(BOOST_MSVC) && BOOST_MSVC <= 1300
//	  // VC++ can't handle the iterator constructor
//	  Graph g(V);
//	  for (std::size_t j = 0; j < E; ++j)
//		add_edge(edge_array[j].first, edge_array[j].second, g);
//	#else
	  Graph g(edge_array, edge_array + E, V);
//	#endif
//
	  property_map < Graph, edge_weight_t >::type w = get(edge_weight, g);
	  cout << " E = " << E << endl;
	  int weights[E] = {};
	  // set all weights to 1
	  std::fill_n(weights, E, 1);
//	  for (int jj=0; jj < E; ++jj){
//		  cout << "weights[" << jj << "]=" << weights[jj] << endl;
//	  }
	  int *wp = weights;

	  graph_traits < Graph >::edge_iterator e, e_end;
	  for (boost::tie(e, e_end) = edges(g); e != e_end; ++e)
	  	w[*e] = *wp++;

	  std::vector < int >d(V, (std::numeric_limits < int >::max)());

	  int** D = new int*[V];
	  for (int jj=0; jj<V; jj++){
		  D[jj]= new int[V];
	  }


	  //int D[V][V];
	  auto start = std::chrono::high_resolution_clock::now();

	  johnson_all_pairs_shortest_paths(g, D, distance_map(&d[0]));

	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating johnson all pairs dijkstra: " << elapsed.count() << "s" << endl;

	  std::cout << D[2][2] << endl;



	  std::cout << "       ";
	  for (int k = 0; k < 20; ++k)
		std::cout << std::setw(5) << k;

	  std::cout << std::endl;
	  for (int i = 0; i < 20; ++i) {
		std::cout << std::setw(3) << i << " -> ";
		for (int j = 0; j < 20; ++j) {
		  if (D[i][j] == (std::numeric_limits<int>::max)())
			std::cout << std::setw(5) << "inf";
		  else
			std::cout << std::setw(5) << D[i][j];
		}
		std::cout << std::endl;
	  }

//	  std::ofstream fout("figs/johnson-eg.dot");
//	  fout << "digraph A {\n"
//		<< "  rankdir=LR\n"
//		<< "size=\"5,3\"\n"
//		<< "ratio=\"fill\"\n"
//		<< "edge[style=\"bold\"]\n" << "node[shape=\"circle\"]\n";
//
//	  graph_traits < Graph >::edge_iterator ei, ei_end;
//	  for (boost::tie(ei, ei_end) = edges(g); ei != ei_end; ++ei)
//		fout << source(*ei, g) << " -> " << target(*ei, g)
//		  << "[label=" << get(edge_weight, g)[*ei] << "]\n";
//
//	  fout << "}\n";



	  for (int jj=0; jj<V; jj++){
		  delete [] D[jj];
	  }
	  delete [] D;


	  return 0;




	}


}

