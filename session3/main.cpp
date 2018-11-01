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
#include <math.h> 
//#include <utility>
 
using namespace std;


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
const unsigned int seed = 1234567;
//const unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
vector <string> get_directory_files(const string& dir) {
	struct dirent *dirent_ptr;
	shared_ptr<DIR> directory_ptr(opendir(dir.c_str()), [](DIR* dir){ dir && closedir(dir); });
	vector <string> files;
	
	if (!directory_ptr) {
		cout << "Error opening : " << strerror(errno) << dir << endl;
		return files;
	}

	while ((dirent_ptr = readdir(directory_ptr.get())) != nullptr) {
		string file_name = dirent_ptr->d_name;
		if(file_name[0] != '.' and file_name.substr(file_name.size() - 3, 3) == "txt")
			files.push_back(file_name);
	}
	return files;
}

void create_ER(map<int, string>& indexNode, const int n, const int m) {
	default_random_engine gen (seed);
	// a random number between 0 and number of nodes -1 because of the index of vector, no matter the value
	uniform_int_distribution<int> dist(0, n-1);
	Graph G_ER;
	//Edges E_ER;
	map <string, char> E_ER; // key composed by origin+destination words
	int edgesCreated = 0;
	int originIndex, destinationIndex;
	while (edgesCreated < m) {
		originIndex = dist(gen);
		destinationIndex = dist(gen);
		// cannot have loop
		if (originIndex != destinationIndex) {
			string origin = indexNode[originIndex]; 
			string destination = indexNode[destinationIndex];
			bool valid = true;
			map <string, char>::iterator it;
			it = E_ER.find(origin+destination);
			if (it != E_ER.end()) continue;
			it = E_ER.find(destination+origin);
			if (it != E_ER.end()) continue;
			// we know that it's not a loop nor multiedge
			if (valid) {
				G_ER[origin].neighbours.push_back(destination); // add b to adjacency list
				G_ER[destination].neighbours.push_back(origin);
				E_ER[origin+destination] = '1';
				E_ER[destination+origin] = '1';
				//E_ER.push_back(make_pair(origin, destination)); // add to the edges vector	
				++edgesCreated;
			}
		}
	}
	/*
	cout << "Print ER" << endl;
	for (auto itr = G_ER.begin(); itr != G_ER.end(); ++itr) {
		cout << itr->first << ": ";
		for (string neighbour : itr->second.neighbours)
			cout << neighbour << '\t';
		cout << endl;
	}
	cout << "m: " << edgesCreated << endl;
	*/
}

void create_switching(map<int, string>& indexNode, map<string, int>& nodeIndex, Edges E, int Q) {
	int m = E.size();
	int QE = Q * m;
	int count = 0;
	int notValid = 0;
	int edge1, edge2;
	string u, v, s, t;
	map <string, char> actualEdges; // char to save space
	for (auto edge: E) {
		actualEdges[edge.first+edge.second] = '1';
		actualEdges[edge.second+edge.first] = '1';
	}
	default_random_engine gen (seed);
	uniform_int_distribution<int> dist(0, E.size() - 1);
	while (count < QE) {
		++count;
		edge1 = dist(gen);
		edge2 = dist(gen);
		// we simply dont want it choose the same edge
		while (edge1 == edge2) 
			edge2 = dist(gen);
		u = E[edge1].first;
		v = E[edge1].second;
		s = E[edge2].first;
		t = E[edge2].second;
		//cout << u << " " << v  << " " << s << " " << t << endl;
		// first, check it the switching produces a loop
		if (u == t or s == v) {
			++notValid;
			continue;
		}
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
		actualEdges.erase(v+u);
		actualEdges.erase(s+t);
		actualEdges.erase(t+s);	
		// add u+t, t+u, s+v, v+s
		actualEdges[u+t] = '1';
		actualEdges[t+u] = '1';
		actualEdges[s+v] = '1';
		actualEdges[v+s] = '1';
		// modify the vector E, uv -> ut, st -> sv
		E[edge1] = make_pair(u, t);
		E[edge2] = make_pair(s, v);
	}
	Graph G; //create graph using E
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

void printCreatedGraph(Graph& G,
		Edges& E,
		vector<string>& Nodes,
		map<string, int>& nodeIndex,
		map<int, string>& indexNode,
		int n, int m, int sameNode){

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
	cout << "n: " << n << ", m: " << m << " , " << sameNode << endl;
	cout << G.size() << ", " << E.size() << endl;

}

void create_graph(const string file_name,
		Graph& G,
		Edges& E,
		vector<string>& Nodes,
		map<string, int>& nodeIndex,
		map<int, string>& indexNode,
		int& n, int& m) throw() {
	try {
		string line;
		ifstream myfile(file_name);
		if (myfile.is_open()) {
			// initializing the needed variables
			bool firstLine = true;
			int sameNode = 0;
			n = 0;
			m = 0; // n for number of nodes, m for number of edges
//			Graph G; // a dictionary which the key is the name of node (e.g a word) and the value is a adjacency list [b, c, etc.]
//			Edges E; // all the edges
//			vector <string> Nodes; // stores the words in the order of incoming
//			map<string, int> nodeIndex; // stores the correspondences between words and indices
//			map<int, string> indexNode; // stores the correspondences between indices and words

			// read all lines of the file
			while (getline(myfile, line)) {
				if (!firstLine) {
					istringstream iss(line);
					string a, b;
					// if the line contains only two elements
					if (!(iss >> a >> b)) throw "Not exact two elements in one line";
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
							nodeIndex[a] = n;
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
							if (find(G[b].neighbours.begin(), G[b].neighbours.end(), a) == G[b].neighbours.end()) {
								G[b].neighbours.push_back(a);
							}
						}

						
					}
					else ++sameNode;
				}
				else firstLine = false;
			}
			myfile.close();
			cout << "Original:" << endl;
			printCreatedGraph(G,E,Nodes, nodeIndex, indexNode, n, m, sameNode);
			create_ER(indexNode, n, m);
			int Q = max(1, (int) log10(m));
			create_switching(indexNode, nodeIndex, E, Q);

		}
		else cout << "Could not open the file, please check the path name";


	}
	catch (const char* msg) {
		cout << msg << endl;
		exit(1);
	}
}

void calculate_distance(const WGraph& G, int s, vector<double>& d, vector<int>& p) {

	
}



void dijkstra(const Graph& G, int s, vector<double>& d, map<string, int> nodeIndex, map<int, string> indexNode, int n) {
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


void calculate_closeness_v1(Graph& G, Edges& E, vector<string>& Nodes, map<string, int>& nodeIndex,
		map<int, string>& indexNode, int n, int m) {
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
		dijkstra(G, idx, d, nodeIndex, indexNode, n);

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

	cout << "closeness centrality CWS: " << endl;
	cout << C << endl;

}

double closeness_batch (Graph& G, Edges& E, vector<string>& Nodes, map<string, int>& nodeIndex,
		map<int, string>& indexNode, int n, int m) {
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
		dijkstra(G, idx, d, nodeIndex, indexNode, n);

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

template<typename T>
std::vector<T> slice(std::vector<T> const &v, int m, int n)
{
    auto first = v.cbegin() + m;
    auto last = v.cbegin() + n + 1;

    std::vector<T> vec(first, last);
    return vec;
}
//
//bool getNodeDegree(const vector<string>&  v1, const vector<string>&  v2){
//
//	return true;
//
//}
//
void sort_Nodes (Graph& G, Edges& E, vector<string>& Nodes, map<string, int>& nodeIndex,
		map<int, string>& indexNode, int n, int m, string sort_type ="shuffle"){

	if (sort_type == "shuffle")
		return random_shuffle (Nodes.begin(), Nodes.end());
//	else if (sort_type == "incr")
//		return sort (Nodes.begin(), Nodes.end(), getNodeDegree);
//	else if (sort_type == "decr")
//		return sort (Nodes.begin(), Nodes.end(),getNodeDegree);

}

void calculate_closeness_v2_bounded(Graph& G, Edges& E, vector<string>& Nodes, map<string, int>& nodeIndex,
		map<int, string>& indexNode, int n, int m) {
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


	//initialize the alternative hypothesis measure value
	double xAH = 0.6;

	//	sort Nodes -> orign, random, increasing, decreasing
//	std::cout << "myvector contains:";
//	  for (std::vector<string>::iterator it=Nodes.begin(); it!=Nodes.begin()+4; ++it)
//	    std::cout << ' ' << *it;
//	  std::cout << '\n';

	sort_Nodes(G,E, Nodes, nodeIndex,
			indexNode, n,m);
//	std::cout << "myvector contains:";
//	  for (std::vector<string>::iterator it=Nodes.begin(); it!=Nodes.begin()+4; ++it)
//	    std::cout << ' ' << *it;
//	  std::cout << '\n';

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
		Cmin += closeness_batch(G,E, NodesbatchRef, nodeIndex,
					indexNode, n,m);

		// 		M += nodebatch_size;
		M += nodebatch_size;
		// 		Cmax = Cmin + 1 - M/N;
		Cmax = Cmin + 1.0 - double(M)/double(n);
		// 		C = Cmin
		C = Cmin;
		//
		if ( Cmin >= xAH){
			nh_over_ah = 1;
			cout << " xNH over xAH? " << nh_over_ah << endl;
			break;
		}else if ( Cmax <= xAH ){
			nh_under_ah = 1;
			cout << " xNH under xAH? " << nh_under_ah << endl;
			break;
		}

		cout << " i: " << i << "-" << i + nodebatch_size-1 \
			 << " Nodes: " << nodeIndex[Nodesbatch[0]] << "-" << nodeIndex[Nodesbatch[nodebatch_size-1]] \
		     << " Cmin:" << Cmin << " Cmax:" << Cmax \
			 << " xAH:" << xAH << " over xAH?" << nh_over_ah << " under xAH?" << nh_under_ah << endl;
	}
	cout << endl;
	// print C,nh_under_ah, nh_over_ah;
	cout << " xAH: " << xAH << endl;
	cout << " partial C: " << C << endl;
	cout << " Cmin: " << Cmin << endl;
	cout << " Cmax: " << Cmax << endl;
	cout << " over xAH?" << nh_over_ah << " under xAH?" << nh_under_ah << endl;
}

int main() {

	Graph G; // a dictionary which the key is the name of node (e.g a word) and the value is a adjacency list [b, c, etc.]
	Edges E; // all the edges
	vector <string> Nodes; // stores the words in the order of incoming
	map<string, int> nodeIndex; // stores the correspondences between words and indices
	map<int, string> indexNode; // stores the correspondences between indices and words
	int n, m;

//	const auto& directory_path = string("./data/");
//	const auto& files = get_directory_files(directory_path);
//	for (const auto& file : files) {
//		cout << directory_path+file << endl;
//		create_graph(directory_path+file,G,E,Nodes,nodeIndex,indexNode,n,m);
//	}

//	create_graph("./datarepo/1.txt",G,E,Nodes,nodeIndex,indexNode,n,m);
//	create_graph("./datarepo/Arabic_syntactic_dependency_network.txt",G,E,Nodes,nodeIndex,indexNode,n,m);
	create_graph("./datarepo/Basque_syntactic_dependency_network.txt",G,E,Nodes,nodeIndex,indexNode,n,m);
//	create_graph("./datarepo/Catalan_syntactic_dependency_network.txt",G,E,Nodes,nodeIndex,indexNode,n,m);
//	create_graph("./datarepo/Chinese_syntactic_dependency_network.txt",G,E,Nodes,nodeIndex,indexNode,n,m);
//	create_graph("./datarepo/Czech_syntactic_dependency_network.txt",G,E,Nodes,nodeIndex,indexNode,n,m);


	//cout << "Real graph" << endl;
	//printCreatedGraph( G, E, Nodes, nodeIndex, indexNode,n, m, 0);


	auto start = std::chrono::high_resolution_clock::now();
	calculate_closeness_v2_bounded(G,E,Nodes,nodeIndex, indexNode,n,m);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;
	/*
    int  c;
	string u, v, x, y;
		while (cin >> n >> m) {
		Graph G;
		map<string, int> nodeIndex;
		map<int, string> indexNode;
		int cont = 0;
		for (int i = 0; i < m; ++i) {
			cin >> u >> v >> c;
			auto it = nodeIndex.find(u);
			if (it == nodeIndex.end()) {
				nodeIndex[u] = cont;
				indexNode[cont] = u;
				++cont;
			}
			it = nodeIndex.find(v);
			if (it == nodeIndex.end()) {
				nodeIndex[v] = cont;
				indexNode[cont] = v;
				++cont;
			}
			G[u].neighbours.push_back(v);
			G[v].neighbours.push_back(u);
		}
		vector<double> d;
		//vector<int> p;
		cin >> x >> y;
		dijkstra(G, nodeIndex[x], d, nodeIndex, indexNode, n);
		cout << x << "\n";
		for (unsigned int j = 0; j < d.size(); ++j) {

			cout << "node " << indexNode[j] << " and cost " << d[j] << "\n";
		}
		cout << "\n";
	}
	*/
}

