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

#include "MyGraph.h"
#include "MyER.h"
#include "MySwitching.h"
#include "johnson.h"

 


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






void write_NH_estimation_partial_result(MyGraph g,double xNH, string filename, int seed, int rng_iteration, chrono::duration<double> timespent, string htype){

	ofstream myfile;
	myfile.open ("results.txt",  ios::out | ios::app );

	cout <<  filename.substr(10,10)   <<  " xA=" << g.closeness_centrality \
			<< " xNH=" << xNH << " t:" << timespent.count() \
			<< " htype: " << htype \
			<< " i:" << rng_iteration << " seed:" << seed \
			<< " " << filename << endl;
	myfile << filename.substr(10,10)  <<  " xA=" << g.closeness_centrality \
			<< " xNH=" << xNH << " t:" << timespent.count() \
			<< " htype: " << htype \
			<< " i:" << rng_iteration << " seed:" \
			<< seed << " " << filename << endl;

	myfile.close();

}



void monteCarlo_estimation(string filename, string htype="ER"){

	MyGraph g = MyGraph(filename);
	g.print();

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	//g.closeness_centrality = 0.269719; // only for Basque lang
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;



	//with alpha= 0.05 -> T >> 1/alph = 20
	int T = 40;
	vector<double> xNHs;

	// seed and rng  initialization
	default_random_engine gen (seed);
	// a random number between 0 and number of nodes -1 because of the index of vector, no matter the value
	uniform_int_distribution<int> dist(0, g.E.size() - 1);
	if (htype=="ER")
		uniform_int_distribution<int> dist(0, g.n-1);


	for (int i=0; i < T ; i++){
		if (htype=="ER"){
			MyER er = MyER(g, dist, gen);
			auto start2 = std::chrono::high_resolution_clock::now();
			er.calculate_closeness_v2_bounded(g.closeness_centrality);
			auto finish2 = std::chrono::high_resolution_clock::now();
			elapsed = finish2 - start2;

			xNHs.push_back(er.closeness_centrality);
			//cout << " xNH_" << i << "=" << er.closeness_centrality << " | " << elapsed.count() << endl;
			write_NH_estimation_partial_result(g, er.closeness_centrality,filename,seed,i,elapsed,htype);

		} else{
			MySwitching sw = MySwitching(g, dist, gen, log(g.E.size()) + 0 );

			cout << "Generated Switching model network " << i << endl;


			auto start2 = std::chrono::high_resolution_clock::now();
			sw.calculate_closeness_v2_bounded(g.closeness_centrality);
			auto finish2 = std::chrono::high_resolution_clock::now();
			elapsed = finish2 - start2;

			xNHs.push_back(sw.closeness_centrality);
			//cout << " xNH_" << i << "=" << er.closeness_centrality << " | " << elapsed.count() << endl;
			write_NH_estimation_partial_result(g, sw.closeness_centrality,filename,seed,i,elapsed,htype);

		}


	}

	auto finish3 = std::chrono::high_resolution_clock::now();
	elapsed = finish3 - start;
	cout << "Total time: " << elapsed.count() << "s" << endl;

	write_NH_estimation_result(g,xNHs,filename);
}

void monteCarlo_estimation_with_ER(string filename){
	return monteCarlo_estimation(filename,"ER");
}

void monteCarlo_estimation_with_SW(string filename){
	return monteCarlo_estimation(filename,"SW");
}



//-----------------------_EXAMPLES_-----------------------------------------


void compute_all_x(){

	vector<string> dirfiles = get_directory_files("./datarepo");

	for (std::vector<string>::iterator it = dirfiles.begin() ; it != dirfiles.end(); ++it){
		cout << "./datarepo/"+*it << endl;
		read_graph2("./datarepo/"+*it);
	}

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

void example_estimate_basque_with_SW(){

	//monteCarlo_estimation_with_SW("./datarepo/Basque_syntactic_dependency_network.txt");
	monteCarlo_estimation_with_SW("./datarepo/Arabic_syntactic_dependency_network.txt");

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

	testj2(g.n, g.transform_to_edge_vect());

}

void example_create_graph_basque(){
	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");

	//g.print();

	testj2(g.n, g.transform_to_edge_vect());

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


void example_SW(){

	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");

	default_random_engine gen (seed);
	uniform_int_distribution<int> dist(0, g.E.size() - 1);

	cout << " Creating an SW " << endl;
	MySwitching sw = MySwitching(g, dist, gen,log(g.E.size()) + 10 );
	cout << "m: " << sw.m << endl;
	sw.print();

	auto start = std::chrono::high_resolution_clock::now();
	sw.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;
}

int main() {

//	example_estimate_all_x_with_ER(); // too long to execute, better a single file approach

//	example_estimate_1_with_ER();
//	example_estimate_basque_with_ER();
//	example_SW();
	//testj();
//	example_estimate_basque_with_SW();

//	example_estimate_some_manually_with_ER();

	example_create_graph_1();
	example_create_graph_basque();
//	example_create_graph_basque();
//  example_create_graph_others();

// example_johnson_all_pairs();

// example_ER();

	//read_graph2("./datarepo/Basque_syntactic_dependency_network.txt");
//	read_graph2("./datarepo/1.txt");

//	compute_all_x();

}

