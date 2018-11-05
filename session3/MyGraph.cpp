#include "MyGraph.h"
//#include "mydegree.h"



class mydegree
{

public:
	string Node;
	int degree;

	mydegree(string& Node, int mydegree){
		this->Node = Node;
		this->degree = mydegree;
	}



	//pending compare by degree of each node! -> read E and count degree
	//  maybe; G_ER[origin].neighbours.size()
};


struct CompareNode
{

	bool operator()(mydegree & n1, mydegree & n2)
		{
			//cout << " comparing " << n1.myNode << " and " << n2.myNode << " " << endl;
			if (&n1){
				if (&n2){
					return n1.degree < n2.degree;
				}
			}
			return false;
		}


};

struct CompareNodeDecr
{

	bool operator()(mydegree & n1, mydegree & n2)
		{
			//cout << " comparing " << n1.myNode << " and " << n2.myNode << " " << endl;
			if (&n1){
				if (&n2){
					return n1.degree > n2.degree;
				}
			}
			return false;
		}


};


//class MyGraph {
//
//public:
//	Graph G; // a dictionary which the key is the name of node (e.g a word) and the value is a adjacency list [b, c, etc.]
//	Edges E; // all the edges
//	vector<string> Nodes;// stores the words in the order of incoming
//	map<string, int> nodeIndex;// stores the correspondences between words and indices
//	map<int, string> indexNode;// stores the correspondences between indices and words
//	int n;
//	int m; // n for number of nodes, m for number of edges
//	double closeness_centrality;

MyGraph::MyGraph(){
		n=0;
		m=0;
		closeness_centrality = 0;
		visitednodes = 0;

	}

MyGraph::MyGraph(const string file_name) {
		try {

			string line;
			ifstream myfile(file_name);
			if (myfile.is_open()) {
				// initializing the needed variables
				bool firstLine = true;
				int sameNode = 0;
				n = 0;
				m = 0;
				closeness_centrality = 0;
				visitednodes = 0;

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
								// if there isn't already a edge a->b
								if (find((it->second).neighbours.begin(), (it->second).neighbours.end(), b) == (it->second).neighbours.end()) {
									G[a].neighbours.push_back(b); // add b to adjacency list
									E.push_back(make_pair(a, b)); // add to the edges vector
									E.push_back(make_pair(b, a));
									++m;
								}
							}
							else {
								// since word a does not present in the dictionary, we add it with the edge
								G[a].neighbours.push_back(b);
								G[a].visited = 0;

								E.push_back(make_pair(a, b));
								E.push_back(make_pair(b, a));
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
								G[b].visited = 0;
								E.push_back(make_pair(b, a));
								E.push_back(make_pair(a, b));
								Nodes.push_back(b);
								nodeIndex[b] = n;
								indexNode[n] = b;
								++n;
								++m;
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



void MyGraph::print(){

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



	void MyGraph::dijkstra( int s, vector<double>& d) {
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




	void MyGraph::calculate_closeness_v1() {
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
			G[Nodes[idx]].visited = 1;
			visitednodes++;

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

	double MyGraph::closeness_batch (vector<string>& Nodes) {
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
		int visitedTotal = 0;

		omp_set_num_threads(8);
		#pragma omp parallel for firstprivate(count) reduction (+: C, visitedTotal)
		for (int idx = 0; idx < Nodes.size(); ++idx){

			// compute all dij
			vector<double>& d = G[Nodes[idx]].distances;
			dijkstra(idx, d);
			G[Nodes[idx]].visited = 1;


			// compute Ci
			double Ci = 0;
			for ( int i = 0; i < d.size() ; i++){
				if (i ==  idx) continue;
				double invdij = 1.0/double(d[i]);
				Ci += invdij;
			}
			Ci = Ci /(n - 1);
			C += Ci;
			visitedTotal+=1;
		}
		C = C / n;
		visitednodes+=visitedTotal;
		return C;

	}



	double MyGraph::closeness_batch_before_k1 (vector<string>& Nodes, int * foundk1) {
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
		 *  if k=1 are found
		 *  	it updates Ci but sets foundk1=1
		 *  	it stops computing distances and updaating ci
		 *  	then caller can handle that
		 *
		*/

		double C = 0;
		int count = 0;
		int visitedTotal=0;


		omp_set_num_threads(8);
		#pragma omp parallel for firstprivate(count) reduction (+: C,visitedTotal)
		for (int idx = 0; idx < Nodes.size(); ++idx){

			if (*foundk1==1)
				continue;

			//verify node degree
			int degree = G[Nodes[idx]].neighbours.size();
			if (degree < 2){
				*foundk1=1;
				continue;
			}

			// compute all dij
			vector<double>& d = G[Nodes[idx]].distances;
			dijkstra(idx, d);
			G[Nodes[idx]].visited = 1;


			// compute Ci
			double Ci = 0;
			for ( int i = 0; i < d.size() ; i++){
				if (i ==  idx) continue;
				double invdij = 1.0/double(d[i]);
				Ci += invdij;
			}
			Ci = Ci /(n - 1);
			C += Ci;
			visitedTotal++;
		}
		C = C / n;
		visitednodes+=visitedTotal;

		return C;

	}

	double MyGraph::closeness_batch_when_k1 (vector<string>& Nodes) {
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
		int visitedTotal =0;

		omp_set_num_threads(8);
		#pragma omp parallel for firstprivate(count) reduction (+: C,visitedTotal)
		for (int idx = 0; idx < Nodes.size(); ++idx){

			// compute all dij
			vector<double>& d = G[Nodes[idx]].distances;
			// all d contain only one element,
			// before computing dijkstra, make sure this element is already "visited
			int skip=0;
			for (int jj=0; jj<d.size(); jj++){
				string nodestring = G[Nodes[jj]].neighbours[jj];
				if (G[nodestring].visited == 0)
					skip = 1;
			}

			if (skip==1)
				continue;

			dijkstra(idx, d);
			G[Nodes[idx]].visited = 1;


			// compute Ci
			double Ci = 0;
			for ( int i = 0; i < d.size() ; i++){
				if (i ==  idx) continue;
				double invdij = 1.0/double(d[i]);
				Ci += invdij;
			}
			Ci = Ci /(n - 1);
			C += Ci;
			visitedTotal++;
		}
		C = C / n;
		visitednodes+=visitedTotal;
		return C;

	}




	void MyGraph::sort_Nodes (string sort_type){

		cout << "              SORT: sort_type " << sort_type << " , " <<  endl;
		if (sort_type == "incr")
			cout << "              " << "sort_type == incr" << endl;
		else if (sort_type == "decr")
			cout << "              " << "sort_type == decr" << endl;
		else if (sort_type.compare("incr"))
			cout << "              " << "sort_type.compare(incr)" << endl;
		else if (sort_type.compare("decr"))
			cout << "              " << "sort_type.compare(decr)" << endl;
		else
			cout << "              " << "sort_type == no working" << endl;

		if (sort_type == "shuffle")
			random_shuffle (Nodes.begin(), Nodes.end());
		else if (sort_type == "incr") {
			vector<mydegree> ds;
			for (int i=0; i< n; i++){
				//ds[i] = mydegree(Nodes[i]);
				ds.push_back(mydegree(Nodes[i],get_degree(Nodes[i])));
			}
			cout << " ds size " << ds.size() << endl;
	//		for (int i=0; i < ds.size(); i++)
	//			cout << "| ds[" << i << "]=" << ds[i].myNode << endl;
			sort(ds.begin(), ds.end()-1, CompareNode() );
			// copy back to Nodes
			//cout << Nodes.size() << " " << ds.size() << " comparing Nodes and ds sizes"<< endl;
			for (int i=0; i < Nodes.size(); i++){
				Nodes[i] = ds[i].Node;
				// get sorting output
				//cout << p.Node << " d=" << p.degree << endl;
			}


		}
		else if (sort_type == "decr"){
			vector<mydegree> ds;
			for (int i=0; i< n; i++){
				//ds[i] = mydegree(Nodes[i]);
				ds.push_back(mydegree(Nodes[i],get_degree(Nodes[i])));
			}
			sort(ds.begin(), ds.end()-1, CompareNodeDecr() );
			// copy back to Nodes
			for (int i=0; i < Nodes.size(); i++){
				Nodes[i] = ds[i].Node;
			}
		}
		// else // do nothing


	}

	void MyGraph::print_nodes_vector(){
		std::cout << "myvector contains:";
		  for (std::vector<string>::iterator it=Nodes.begin(); it!=Nodes.begin()+4; ++it)
			std::cout << ' ' << *it;
		  std::cout << '\n';
	}

	void MyGraph::calculate_closeness_v2_bounded(double xAH, string sorttype) {
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

		// sorttype: "shuffle", "incr", "decr", other
		sort_Nodes(sorttype);
		cout << "after sorting ";
		print_nodes_vector();

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
			if (i < 1 ){
				for (int idx = 0; idx < 3; ++idx)
					cout << " node of which we compute dij's:  " <<  Nodesbatch[idx] << endl;

			}
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


	void MyGraph::calculate_closeness_v2_bounded_k1(double xAH, string sorttype) {
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

		// sorttype: "shuffle", "incr", "decr", other
		sort_Nodes(sorttype);
		cout << "after sorting ";
		print_nodes_vector();

		// Chop Nodes into batches of size...4?8?16?
		int nodebatch_size = min(n,1000);

		// Boolean deecistion variables
		int nh_over_ah  = 0;
		int nh_under_ah = 0;

		double C=0;
		double Cmax=0;
		double Cmin=0;
		int M=0;

		//for second loop (for when k=1)
		int lasti=0;
		int dok1loop=0;
		int foundk1=0;



		cout << "now the first loop:" << n - nodebatch_size + 1 <<  " n=" << n << " nodebatch_size=" << nodebatch_size  << " Nodes.size()=" << Nodes.size() << " slice(" << 0 << "," << 0+nodebatch_size-1 << ")" << endl;
		// for Nodebatch:
		for (int i=0; i < n  - nodebatch_size + 1 && foundk1==0; i+=nodebatch_size-1){
			// preparate NodeBatch

			vector<string> Nodesbatch = slice(Nodes,i, i+nodebatch_size-1);
			//vector<string>& NodesbatchRef = Nodesbatch;
			cout << " i=" << i << " n=" << n << " nodebatch_size=" << nodebatch_size << " slice(" << i << "," << i+nodebatch_size-1 << ")" << endl;

			// 		compute Cbi/N;
			// 		Cmin += Cbi/N;
			if (i < 1 ){
				for (int idx = 0; idx < Nodesbatch.size() && idx < 3; ++idx)
					cout << " node of which we compute dij's:  " <<  Nodesbatch[idx] << endl;
			}

			Cmin += closeness_batch_before_k1( Nodesbatch, &foundk1);

			// 		M += nodebatch_size;
			M += nodebatch_size;
			// 		Cmax = Cmin + 1 - M/N;
			Cmax = Cmin + 1.0 - double(M)/double(n);
			// 		C = Cmin
			C = Cmin;
			//
			cout << " Cmin=" << Cmin << " Cmax=" << Cmax << " foundk1= "<< foundk1 <<  endl;


			// foundk1, handle this situation, separate nodes ith k=1 from batch, then compute next steps
			if (foundk1==0){
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

			} else {
				cout << "found node with degree 1! : changing to new loop" << endl;
				int newbatchsize = 0;

				// skipping the nodes with k>1 in this batch?
//				for (int j=0; j< Nodesbatch.size(); j++){
//					if (G[Nodesbatch[j]].neighbours.size()>=2)
//						newbatchsize++;
//					else {
//						// we save the position of first node with degree k=1
//						lasti=j;
//					}
//				}
				M += newbatchsize;
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
				} else {
					//we then do the loop with k=1
					dok1loop=1;
					break;
				}
			}
		}

		if (foundk1==1){
			// k=1 loop
			// prepare a new vector of Nodes, where we will be removing each node once it is visited
			vector<string> NodesK1 = slice(Nodes,lasti, Nodes.size());

			// for Nodebatch:
			while ( visitednodes < G.size()){
				for (int i=lasti; i < n - nodebatch_size + 1; i+=nodebatch_size){
					// preparate NodeBatch
					vector<string> Nodesbatch = slice(NodesK1,i, i+nodebatch_size);
					vector<string>& NodesbatchRef = Nodesbatch;

					// 		compute Cbi/N;
					// 		Cmin += Cbi/N;
					if (i < 1 ){
						for (int idx = 0; idx < 3; ++idx)
							cout << " node with k=1 of which we compute dij's:  " <<  Nodesbatch[idx] << endl;
					}
					Cmin += closeness_batch_when_k1( Nodesbatch);

					//remove nodes from NodesK1 , very ineficient..AVOID
					// repeat the loop in a while loop while visitednodes < G.size()

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

				}
			}
		}
		closeness_centrality = C;
	}


	void MyGraph::calculate_closeness_v3(){

		johnson john = johnson(n, transform_to_edge_vect());
		john.johnson_all_pairs_dijkstra();
		//john.printDMatrix();

		int V = john.V;
		int ** D = john.D;

		double C = 0;
		int count = 0;
		//cout << "Closeness centrality: " << endl;
		for (int idx = 0; idx < Nodes.size(); ++idx){
			// compute all dij
			int* d = D[idx];
			// compute Ci
			double Ci = 0;
			for ( int i = 0; i < V ; i++){
				if (i ==  idx) continue;
				else if (d[i]== std::numeric_limits<int>::infinity()) continue;
				double invdij = 1.0/double(d[i]);
				Ci += invdij;
			}
			Ci = Ci /(n - 1);
			C += Ci;
			G[Nodes[idx]].visited = 1;
			visitednodes++;
		}
		C = C / n;
		closeness_centrality = C;
		john.freeMem();
		cout << "closeness centrality CWS: " << endl;
		cout << C << endl;
	}

	vector<Edge> MyGraph::transform_to_edge_vect(){

		vector<Edge> edge_vect;
		for (auto p : E){
			string a = p.first;
			string b = p.second;
			edge_vect.push_back(Edge(nodeIndex[b],nodeIndex[a]));
			edge_vect.push_back(Edge(nodeIndex[a],nodeIndex[b]));
		}

		//cout << "Debugging edge_vect" << endl;
//		for (auto p : edge_vect)
//			cout << indexNode[p.first] << "(" << p.first << ") - " << indexNode[p.second] << "(" << p.second << ")" << endl;

		return edge_vect;

	}

	int MyGraph::get_degree(string nodename){
		map <string, Node >::const_iterator it;
		it = G.find(nodename);
		if (it != G.end())
			return G[nodename].neighbours.size();
		else
			return 0;
	}
