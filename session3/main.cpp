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



//#include "johnson.cpp"
#include "MyER.cpp"
 
using namespace std;

const unsigned int seed = 1234567;
uniform_int_distribution<int> dist;
default_random_engine gen;
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



void write_NH_estimation_result(MyGraph g, vector<double> xNHs, string filename){

	ofstream myfile;
	myfile.open ("results.txt",  ios::out | ios::app );

	cout << "Result: " << filename.substr(10,10)   <<  " xA=" << g.closeness_centrality << " xNH=[";
	myfile << "Result: " << filename.substr(10,10)    <<  " xA=" << g.closeness_centrality << " xNH=[";

	for (std::vector<double>::iterator it = xNHs.begin() ; it != xNHs.end(); ++it){
		cout << *it << ", ";
		myfile << *it << ", ";
	}

	cout << "]" << endl;
	myfile << "]" << endl;

	myfile.close();
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



void write_NH_estimation_partial_result(MyGraph g,double xNH, string filename, int seed, int rng_iteration, chrono::duration<double> timespent){

	ofstream myfile;
	myfile.open ("results.txt",  ios::out | ios::app );

	cout <<  filename.substr(10,10)   <<  " xA=" << g.closeness_centrality \
			<< " xNH=" << xNH << " t:" << timespent.count() \
			<< " i:" << rng_iteration << " seed:" << seed \
			<< " " << filename << endl;
	myfile << filename.substr(10,10)  <<  " xA=" << g.closeness_centrality \
			<< " xNH=" << xNH << " t:" << timespent.count() \
			<< " i:" << rng_iteration << " seed:" \
			<< seed << " " << filename << endl;

	myfile.close();

}



void monteCarlo_estimation_with_ER(string filename){

	MyGraph g = MyGraph(filename);

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

	//with alpha= 0.05 -> T >> 1/alph = 20
	int T = 100;
	vector<double> xNHs;

	// seed and rng  initialization
	default_random_engine gen (seed);
	// a random number between 0 and number of nodes -1 because of the index of vector, no matter the value
	uniform_int_distribution<int> dist(0, g.n-1);

	for (int i=0; i < T ; i++){
		MyER er = MyER(g, dist, gen);

		auto start2 = std::chrono::high_resolution_clock::now();
		er.calculate_closeness_v2_bounded(g.closeness_centrality);
		auto finish2 = std::chrono::high_resolution_clock::now();
		elapsed = finish2 - start2;

		xNHs.push_back(er.closeness_centrality);
		//cout << " xNH_" << i << "=" << er.closeness_centrality << " | " << elapsed.count() << endl;
		write_NH_estimation_partial_result(g, er.closeness_centrality,filename,seed,i,elapsed);
	}

	auto finish3 = std::chrono::high_resolution_clock::now();
	elapsed = finish3 - start;
	cout << "Total time: " << elapsed.count() << "s" << endl;

	write_NH_estimation_result(g,xNHs,filename);
}

void estimate_all_x_with_ER_NH(){

	vector<string> dirfiles = get_directory_files("./datarepo");

	for (std::vector<string>::iterator it = dirfiles.begin() ; it != dirfiles.end(); ++it){
		monteCarlo_estimation_with_ER("./datarepo/"+*it);
	}

}

void example_estimate_all_x_with_ER(){
	estimate_all_x_with_ER_NH();
}

void example_estimate_1_with_ER(){
	monteCarlo_estimation_with_ER("./datarepo/1.txt");
}

void example_estimate_basque_with_ER(){
	monteCarlo_estimation_with_ER("./datarepo/Basque_syntactic_dependency_network.txt");
}

void example_estimate_some_manually_with_ER(){
	monteCarlo_estimation_with_ER("./datarepo/Basque_syntactic_dependency_network.txt");

	MyGraph g = MyGraph("./datarepo/Arabic_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Catalan_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Chinese_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Czech_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/English_syntactic_dependency_network.txt");
}

void example_create_graph_1(){
	MyGraph g = MyGraph("./datarepo/1.txt");

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

}

void example_create_graph_basque(){
	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

}


void example_create_graph_others(){
	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Arabic_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Catalan_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Chinese_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/Czech_syntactic_dependency_network.txt");
	//	MyGraph g = MyGraph("./datarepo/English_syntactic_dependency_network.txt");

	//	cout << "Real graph" << endl;
	//	//g.print();
	//

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

}

void example_johson_all_pairs(){

	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");

	// some inconsistencies in the example
	auto start = std::chrono::high_resolution_clock::now();
	//johnson_allpairs_dijkstra(g);
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

}

void example_ER(){

	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");

	cout << " Creating an ER " << endl;
	MyER er = MyER(g, dist, gen);
	//cout << "m: " << er.m << endl;
	//er.printER();
	//cout << " Now the print method" << endl;
	//er.print();

	auto start = std::chrono::high_resolution_clock::now();
	er.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

}


int main() {

//	example_estimate_all_x_with_ER(); // too long to execute, better a single file approach

	example_estimate_1_with_ER();
//	example_estimate_basque_with_ER();
//	example_estimate_some_manually_with_ER();

//	example_create_graph_1();
//	example_create_graph_basque();
//  example_create_graph_others();

// example_johnson_all_pairs();

// example_ER();


}

