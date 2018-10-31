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
	while (edgesCreated < m) {
		int originIndex = dist(gen);
		int destinationIndex = dist(gen);
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
	cout << "Print G" << endl;
	for (auto itr = G_ER.begin(); itr != G_ER.end(); ++itr) { 
		cout << itr->first << ": ";  
		for (string neighbour : itr->second.neighbours)
			cout << neighbour << '\t'; 
		cout << endl;
	} 
	cout << "m: " << edgesCreated << endl;
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
			create_ER(indexNode, n, m);
			// printCreatedGraph(G,E,Nodes, indexNode, nodeIndex, sameNode);

		}
		else cout << "Could not open the file, please check the path name";


	}
	catch (const char* msg) {
		cout << msg << endl;
		exit(1);
	}
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


void calculate_closeness(Graph& G, Edges& E, vector<string>& Nodes, map<string, int>& nodeIndex,
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
	cout << "Closeness centrality: " << endl;
	for (auto node: Nodes){
		//cout << " node " << node << endl;

		// compute all dij
		vector<double> d;
		vector<int> p;

		dijkstra(G, nodeIndex[node], d, nodeIndex, indexNode, n);

		// compute Ci
		double Ci = 0;
		//cout << " dij's :"<< endl;
		for (int i =0; i< d.size() && i !=  nodeIndex[node]; i++){
			//cout << d[i] << endl;
			double invdij = 1.0/double(d[i]);
			Ci+=invdij;
		}
		Ci = Ci /(n - 1);
		//cout << "Ci=" << Ci << endl;

		// update C
		C+=Ci;
		//cout << "update C=" << C << endl;


	}
	// finally set C
	C = C / n;

	cout << "closeness centrality CWS: " << endl;
	cout << C << endl;

}


int main() {

	Graph G; // a dictionary which the key is the name of node (e.g a word) and the value is a adjacency list [b, c, etc.]
	Edges E; // all the edges
	vector <string> Nodes; // stores the words in the order of incoming
	map<string, int> nodeIndex; // stores the correspondences between words and indices
	map<int, string> indexNode; // stores the correspondences between indices and words
	int n, m;

	const auto& directory_path = string("./data/");
	const auto& files = get_directory_files(directory_path);
	for (const auto& file : files) {
		cout << directory_path+file << endl;
		create_graph(directory_path+file,G,E,Nodes,nodeIndex,indexNode,n,m);
	}


	printCreatedGraph( G, E, Nodes, nodeIndex, indexNode,n, m, 0);

	calculate_closeness(G,E,Nodes,nodeIndex, indexNode,n,m);


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

}

