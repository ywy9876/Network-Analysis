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

#include "MyGraph.cpp"

class MyER : public MyGraph {

public:

	map <string, char> E_ER; // key composed by origin+destination words

	MyER(const MyGraph& g) {


		//G is not initialized
		//E is not initialized
		n = g.n;
		m = g.m;
		indexNode = g.indexNode; // makes a copy
		nodeIndex = g.nodeIndex; //makes a copy
		Nodes = g.Nodes;  // makes a copy

		default_random_engine gen (seed);
		// a random number between 0 and number of nodes -1 because of the index of vector, no matter the value
		uniform_int_distribution<int> dist(0, n-1);


		cout << " Initialized ER " << endl;

		int edgesCreated = 0;
		while (edgesCreated < m) {
			//cout << " while " << edgesCreated << " < " << m << endl;
			int originIndex = dist(gen);
			int destinationIndex = dist(gen);
			// cannot have loop
			if (originIndex != destinationIndex) {
				string origin = indexNode[originIndex];
				string destination = indexNode[destinationIndex];
				bool valid = true;
				map <string, char>::iterator it;
				it = E_ER.find(origin+destination);
				//cout << "origin:" << origin << " dest:" << destination << endl;
				if (it != E_ER.end()) continue;
				it = E_ER.find(destination+origin);
				if (it != E_ER.end()) continue;
				// we know that it's not a loop nor multiedge
				if (valid) {
					//cout << "adding " << destination << "-" << origin << endl;
					G[origin].neighbours.push_back(destination); // add b to adjacency list
					G[destination].neighbours.push_back(origin);

					E_ER[origin+destination] = '1';
					E_ER[destination+origin] = '1';

					E.push_back(std::make_pair(origin,destination));
					E.push_back(std::make_pair(destination, origin));
					//E_ER.push_back(make_pair(origin, destination)); // add to the edges vector
					++edgesCreated;
				}
			}
		}
		// update m
		m = edgesCreated;


	}


	void printER (){
		cout << "Print ER.G" << endl;
		for (auto itr = G.begin(); itr != G.end(); ++itr) {
			cout << itr->first << ": ";
			for (string neighbour : itr->second.neighbours)
				cout << neighbour << '\t';
			cout << endl;
		}
	}

	void printNodeIndex(){
		std::map<std::string, int>::iterator it = nodeIndex.begin();
		while(it != nodeIndex.end())
		{
			std::cout<<it->first<<" :: "<<it->second<<std::endl;
			it++;
		}
	}

	void printE_ER(){
		map <string, char>::iterator it2 = E_ER.begin();
		while(it2 != E_ER.end())
		{
			cout<<it2->first<<" :: "<<it2->second<< endl;
			it2++;
		}
	}

};
