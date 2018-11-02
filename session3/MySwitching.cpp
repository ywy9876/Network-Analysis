#include "MySwitching.h"

//class MySwitching : public MyGraph {
//
//public:
//
//	int Q = 0;
//	int QE = 0;
//	map <string, char> actualEdges; // char to save space


	MySwitching::MySwitching(const MyGraph& g, uniform_int_distribution<int>& dist, default_random_engine& gen, int Q){

		//G is not initialized
		E = g.E;//makes a copy
		n = g.n;
		m = g.m;
		indexNode = g.indexNode; // makes a copy
		nodeIndex = g.nodeIndex; //makes a copy
		Nodes = g.Nodes;  // makes a copy


		int m = E.size();
		//cout << "IN creating Switching, E.size()=" << m << endl;
		QE = Q * m;
		this->Q = Q;
		int count = 0;
		int notValid = 0;
		int edge1, edge2;
		string u, v, s, t;

		for (auto edge: E) {
			actualEdges[edge.first+edge.second] = '1';
			actualEdges[edge.second+edge.first] = '1';
		}

		//cout << "finished updating edges " << endl;
		while (count < QE) {
			++count;
			edge1 = dist(gen);
			edge2 = dist(gen);
			// we simply dont want it choose the same edge
			while (edge1 == edge2)
				edge2 = dist(gen);

			//cout << " finished randomly picking one edge " << edge2 << endl;

			u = E[edge1].first;
			v = E[edge1].second;
			s = E[edge2].first;
			t = E[edge2].second;
			cout << u << " " << v  << " " << s << " " << t << endl;
			// first, check if the switching produces a loop
			if (u == t or s == v) {
				++notValid;
				continue;
			}
			// v = t means no change, and also u = s
			if (v == t or u == s) continue;
			// now check if u-t already exist
			auto itr = actualEdges.find(u+t);
			if (itr != actualEdges.end()) {
				++notValid;
				continue;
			}
			itr = actualEdges.find(t+u);
			if (itr != actualEdges.end()) {
				++notValid;
				continue;
			}
			// now check if s-v already exist
			itr = actualEdges.find(s+v);
			if (itr != actualEdges.end()) {
				++notValid;
				continue;
			}
			itr = actualEdges.find(v+s);
			if (itr != actualEdges.end()) {
				++notValid;
				continue;
			}
			// all constraints satisfied, modify the vector E and actualEdges
			// erase u+v, v+u, s+t, t+s from the map actualEdges
			actualEdges.erase(u+v);
			//actualEdges.erase(v+u);
			actualEdges.erase(s+t);
			//actualEdges.erase(t+s);
			// add u+t, t+u, s+v, v+s
			actualEdges[u+t] = '1';
			//actualEdges[t+u] = '1';
			actualEdges[s+v] = '1';
			//actualEdges[v+s] = '1';
			// modify the vector E, uv -> ut, st -> sv
			E[edge1] = make_pair(u, t);
			E[edge2] = make_pair(s, v);
		}

		cout << " now adding edgges to G " << endl;
		for (auto edge: E) {
			G[edge.first].neighbours.push_back(edge.second);
			G[edge.second].neighbours.push_back(edge.first);
		}
		cout << "Print Switching model" << endl;
		for (auto itr = G.begin(); itr != G.end(); ++itr) {
			cout << itr->first << ": ";
			for (string neighbour : itr->second.neighbours)
				cout << neighbour << '\t';
			cout << endl;
		}
		for (auto edge: E) {
			cout << edge.first << " -> " << edge.second << endl;
		}

		cout << "Edges: " << E.size() << endl;
		cout << "QE: " << QE << endl;
		cout << "failed switching: " << notValid << endl;
	}


//};
