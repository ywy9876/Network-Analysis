#include "johnson.h"


int read_graph2(std::string file_name){

	std::string line;
	std::vector<Edge> edge_vect;
	int n = 0;
	std::cout << "testj2 ini" << std::endl;
	std::ifstream myfile(file_name);
	cout << "opening file" << endl;
	if (myfile.is_open()) {
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
		return testj2(n,edge_vect);

	}
	return 0;
}


int testj2(int n,std::vector<Edge> edge_vect ){

	  using namespace boost;
	  typedef adjacency_list<vecS, vecS, directedS, no_property,
		property< edge_weight_t, int, property< edge_weight2_t, int > > > Graph;
	  const int V = n;
	  typedef std::pair < int, int >Edge;
	  const std::size_t EE = edge_vect.size();
	  Edge edge_array[EE] = {};
	  for (int jj=0; jj < edge_vect.size(); jj++){
		  edge_array[jj] = edge_vect[jj];
	  }
	  const std::size_t E = sizeof(edge_array) / sizeof(Edge);
	  Graph g(edge_array, edge_array + E, V);
	  property_map < Graph, edge_weight_t >::type w = get(edge_weight, g);
	  cout << " E = " << E << endl;
	  int weights[E] = {};
	  std::fill_n(weights, E, 1);
	  int *wp = weights;
	  graph_traits < Graph >::edge_iterator e, e_end;
	  for (boost::tie(e, e_end) = edges(g); e != e_end; ++e)
	  	w[*e] = *wp++;
	  std::vector < int >d(V, (std::numeric_limits < int >::max)());
	  int** D = new int*[V];
	  for (int jj=0; jj<V; jj++){
		  D[jj]= new int[V];
	  }
	  auto start = std::chrono::high_resolution_clock::now();
	  johnson_all_pairs_shortest_paths(g, D, distance_map(&d[0]));
auto finish = std::chrono::high_resolution_clock::now();
std::chrono::duration<double> elapsed = finish - start;
cout << "Time spent in calculating johnson all pairs dijkstra: " << elapsed.count() << "s" << endl;
	  std::cout << "       ";
	  for (int k = 0; k < V; ++k)
		std::cout << std::setw(5) << k;
	  std::cout << std::endl;
	  for (int i = 0; i < V; ++i) {
		std::cout << std::setw(3) << i << " -> ";
		for (int j = 0; j < V; ++j) {
		  if (D[i][j] == (std::numeric_limits<int>::max)())
			std::cout << std::setw(5) << "inf";
		  else
			std::cout << std::setw(5) << D[i][j];
		}
		std::cout << std::endl;
	  }
	  for (int jj=0; jj<V; jj++){
		  delete [] D[jj];
	  }
	  delete [] D;
	  return 0;
	}



