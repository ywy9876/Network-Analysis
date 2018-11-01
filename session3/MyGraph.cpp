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

#include "mydegree.cpp"

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

	MyGraph(){
		n=0;
		m=0;

	}

	MyGraph(const string file_name) {
		try {

			string line;
			ifstream myfile(file_name);
			if (myfile.is_open()) {
				// initializing the needed variables
				bool firstLine = true;
				int sameNode = 0;
				n = 0;
				m = 0;

				// read all lines of the file
				while (getline(myfile, line)) {
					if (!firstLine) {
						istringstream iss(line);
						string a, b;
						// if the line contains only two elements
						if (!(iss >> a >> b)) {
							"Not exact two elements in one line";
							return;
						}
						// avoiding loop
						if (a != b) {
							//cout << a << " " << b << endl;
							map <string, Node>::iterator it;
							it = G.find(a);
							// if the dictionary already contains word a
							if (it != G.end()) {
								// if there's already a edge a->b
								if (find((it->second).neighbours.begin(), (it->second).neighbours.end(), b) == (it->second).neighbours.end()) {
									G[a].neighbours.push_back(b); // add b to adjacency list
									E.push_back(make_pair(a, b)); // add to the edges vector
									++m;
								}
							}
							else {
								// since word a does not present in the dictionary, we add it with the edge
								G[a].neighbours.push_back(b);
								E.push_back(make_pair(a, b));
								Nodes.push_back(a);
								nodeIndex[a] =n;
								indexNode[n] = a;
								++n;
								++m;
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
							}
							// if the dictionary already contains word b
							else {
								// if there's already a edge b->a
								if (find( G[b].neighbours.begin(),  G[b].neighbours.end(), a) ==  G[b].neighbours.end()) {
									 G[b].neighbours.push_back(a);
								}
							}


						}
						else ++sameNode;
					}
					else firstLine = false;
				}
				myfile.close();
				// printCreatedGraph(G,E,Nodes, indexNode, nodeIndex, sameNode);

			}
			else cout << "Could not open the file, please check the path name";


		}
		catch (const char* msg) {
			cout << msg << endl;

		}
	}



	void print(){

		int sameNode = 0;

		cout << "Print G" << endl;
		for (auto itr = G.begin(); itr != G.end(); ++itr)
		{
			cout << itr->first << ": ";
			for (string neighbour : itr->second.neighbours)
				cout << neighbour << '\t';
			cout << endl;
		}
		cout << "Print E" << endl;
		for (auto p : E)
			cout << p.first << '\t' << p.second << endl;
		cout << "Print Nodes" << endl;
		for (auto node : Nodes)
			cout << node << endl;
		cout << "Print nodeIndex" << endl;
		for (auto itr = nodeIndex.begin(); itr != nodeIndex.end(); ++itr)
			cout << itr->first << "\t" << itr->second << endl;
		cout << "Print indexNode" << endl;
		for (auto itr = indexNode.begin(); itr != indexNode.end(); ++itr)
			cout << itr->first << "\t" << itr->second << endl;
		cout << "G.size()" << G.size() << ", " << "E.size()" << E.size() << endl;
		cout << "n: " << n << ", m: " << m << " , " << sameNode << endl;

	}



	void dijkstra( int s, vector<double>& d) {
		d = vector<double>(n, infinit); d[s] = 0;
		vector<bool> S(n, false);
		priority_queue<WArc, vector<WArc>, greater<WArc> > Q;
		Q.push(WArc(0, s));
		while (not Q.empty()) {
			int u = Q.top().second; Q.pop();
			if (not S[u]) {
				S[u] = true;
				map <string, Node >::const_iterator it;
				it = G.find(indexNode[u]);
			    if (it != G.end()) {
					for (string v_node : (it->second).neighbours) {
						int v = nodeIndex[v_node];
						if (d[v] > d[u] + 1) {
							d[v] = d[u] + 1;
							Q.push(WArc(d[v], v));
						}
					}
				}
			}
		}
	}


	void calculate_closeness_v1() {
		/*
		 *  MEAN CLOSENESS CENTRALITY
		 *  C = 1/N * SUM{i=1,N} Ci
		 *
		 *  Ci = 1 / (N-1) * SUM{j=1,N:i!=j}1/dij
		 *
		 *  dii=0
		 *
		 *
		 */

		double C = 0;
		int count = 0;
		cout << "Closeness centrality: " << endl;
		omp_set_num_threads(omp_get_max_threads()) ;
		omp_set_num_threads(8) ;  // 8 175s 32 176s  64 170s,  128 179s,
		#pragma omp parallel for firstprivate(count) reduction (+: C)
		for (int idx = 0; idx < Nodes.size(); ++idx){
			//cout << "OMP_NUM_THREADS=" << omp_get_num_threads() << endl;
			//cout << "thread " << omp_get_thread_num() << " calculating for node " << Nodes[idx] << " iteration " << idx << endl;
			//cout << " node " << node << endl;
			//if (nodeIndex[node] % 1000 == 0 )

			// compute all dij
			vector<double>& d = G[Nodes[idx]].distances;

			//cout << " node of which we compute dij's:  " <<  nodeIndex[node] << endl;
			dijkstra( idx, d);

			// compute Ci
			double Ci = 0;
			for ( int i = 0; i < d.size() ; i++){
				if (i ==  idx) continue;
				//cout << indexNode[idx] << " -> " << indexNode[i]  << " " << d[i] << endl;
				double invdij = 1.0/double(d[i]);
				Ci += invdij;
			}
			Ci = Ci /(n - 1);
			//cout << "Ci=" << Ci << endl;

			// update C
			C += Ci;
			//cout << "update C=" << C << endl;
			//#pragma omp atomic
			//we can also just count num vertices processes by one thread without sharing this counter
			// because atomic kills performance usually
			count += 1;
			if (count % 1000 == 0 && 0 == omp_get_thread_num())
				cout << count << endl;

		}
		// finally set C
		C = C / n;

		closeness_centrality = C;
		cout << "closeness centrality CWS: " << endl;
		cout << C << endl;

	}

	double closeness_batch (vector<string>& Nodes) {
		/*
		 *  MEAN CLOSENESS CENTRALITY
		 *  C = 1/N * SUM{i=1,N} Ci
		 *
		 *  Ci = 1 / (N-1) * SUM{j=1,N:i!=j}1/dij
		 *
		 *  Nodes contains a subset of all the Nodes of the graph
		 *  -> this allows to compute with parallelization
		 *
		 *  returns C_batch
		 *
		*/

		double C = 0;
		int count = 0;

		omp_set_num_threads(50);
		#pragma omp parallel for firstprivate(count) reduction (+: C)
		for (int idx = 0; idx < Nodes.size(); ++idx){

			// compute all dij
			vector<double>& d = G[Nodes[idx]].distances;

			//cout << " node of which we compute dij's:  " <<  nodeIndex[node] << endl;
			dijkstra(idx, d);

			// compute Ci
			double Ci = 0;
			for ( int i = 0; i < d.size() ; i++){
				if (i ==  idx) continue;
				double invdij = 1.0/double(d[i]);
				Ci += invdij;
			}
			Ci = Ci /(n - 1);
			C += Ci;
		}
		C = C / n;

		return C;

	}






	void sort_Nodes (string sort_type ="shuffle"){

		if (sort_type == "shuffle")
			random_shuffle (Nodes.begin(), Nodes.end());
		else if (sort_type == "incr") {
			vector<mydegree> ds;
			for (int i=0; i< n; i++){
				//ds[i] = mydegree(Nodes[i]);
				ds.push_back(mydegree(Nodes[i]));
			}
			cout << " ds size " << ds.size() << endl;
	//		for (int i=0; i < ds.size(); i++)
	//			cout << "| ds[" << i << "]=" << ds[i].myNode << endl;
			sort(ds.begin(), ds.end()-1, CompareNode() );
		}
	//	else if (sort_type == "decr")
	//	{
	//		vector<degree> ds;
	//		for (int i=0; i< Nodes.size(); i++){
	//			ds[i] = degree(Nodes[i],G,E,nodeIndex,indexNode,n,m);
	//		}
	//		sort(ds.begin(), ds.end());
	//	}
	}

	void print_nodes_vector(){
		std::cout << "myvector contains:";
		  for (std::vector<string>::iterator it=Nodes.begin(); it!=Nodes.begin()+4; ++it)
			std::cout << ' ' << *it;
		  std::cout << '\n';
	}

	void calculate_closeness_v2_bounded(double xAH=0.5) {
		/*
		 *  MEAN CLOSENESS CENTRALITY
		 *  C = 1/N * SUM{i=1,N} Ci
		 *
		 *  Ci = 1 / (N-1) * SUM{j=1,N:i!=j}1/dij
		 *
		 *  dii=0
		 *
		 * bounded computation by xNHmin and xNHmax
		 * parallelization by batches (of randomized nodes!)
		 * after each batch we recompute xNHmin and xNHmax and compare with alternative hypothesis
		 */




		//	sort Nodes -> orign, random, increasing, decreasing
//		print_nodes_vector();
		sort_Nodes( "rand");
//		cout << "after sorting ";
//		print_nodes_vector();

		// Chop Nodes into batches of size...4?8?16?
		int nodebatch_size = 1000;


		// Boolean deecistion variables
		int nh_over_ah  = 0;
		int nh_under_ah = 0;


		 double C=0;
		 double Cmax=0;
		 double Cmin=0;
		 int M=0;

		// for Nodebatch:
		for (int i=0; i < n - nodebatch_size + 1; i+=nodebatch_size){
			// preparate NodeBatch
			vector<string> Nodesbatch = slice(Nodes,i, i+nodebatch_size);
			vector<string>& NodesbatchRef = Nodesbatch;

			// 		compute Cbi/N;
			// 		Cmin += Cbi/N;
			Cmin += closeness_batch( Nodesbatch);

			// 		M += nodebatch_size;
			M += nodebatch_size;
			// 		Cmax = Cmin + 1 - M/N;
			Cmax = Cmin + 1.0 - double(M)/double(n);
			// 		C = Cmin
			C = Cmin;
			//
			if ( Cmin >= xAH){
				nh_over_ah = 1;
				//cout << " xNH over xAH? " << nh_over_ah << endl;
				break;
			}else if ( Cmax <= xAH ){
				nh_under_ah = 1;
				C = Cmax;
				//cout << " xNH under xAH? " << nh_under_ah << endl;
				break;
			}

//			cout << " i: " << i << "-" << i + nodebatch_size-1 \
//				 << " Nodes: " << nodeIndex[Nodesbatch[0]] << "-" << nodeIndex[Nodesbatch[nodebatch_size-1]] \
//			     << " Cmin:" << Cmin << " Cmax:" << Cmax \
//				 << " xAH:" << xAH << " over xAH?" << nh_over_ah << " under xAH?" << nh_under_ah << endl;
		}


		closeness_centrality = C;

		// print C,nh_under_ah, nh_over_ah;
//		cout << endl;
//		cout << " xAH: " << xAH << endl;
//		cout << " partial C: " << C << endl;
//		cout << " Cmin: " << Cmin << endl;
//		cout << " Cmax: " << Cmax << endl;
//		cout << " over xAH?" << nh_over_ah << " under xAH?" << nh_under_ah << endl;
	}


};
