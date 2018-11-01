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



#include "MyER.cpp"
 
using namespace std;


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



void monteCarlo_estimation_with_ER(string filename){

	MyGraph g = MyGraph(filename);

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;

	//with alpha= 0.05 -> T >> 1/alph = 20
	int T = 2;
	vector<double> xNHs;

	for (int i=0; i < T ; i++){
		MyER er = MyER(g);

		auto start2 = std::chrono::high_resolution_clock::now();
		er.calculate_closeness_v2_bounded(g.closeness_centrality);
		auto finish2 = std::chrono::high_resolution_clock::now();
		elapsed = finish2 - start2;

		xNHs.push_back(er.closeness_centrality);
		cout << " xNH_" << i << "=" << er.closeness_centrality << " | " << elapsed.count() << endl;
	}

	cout << "Result: xA=" << g.closeness_centrality << "xNH=[";
	for (std::vector<double>::iterator it = xNHs.begin() ; it != xNHs.end(); ++it){
			cout << *it << ", ";
		}
	cout << "]" << endl;

	auto finish3 = std::chrono::high_resolution_clock::now();
	elapsed = finish3 - start;
	cout << "Total time: " << elapsed.count() << "s" << endl;

}

void estimate_all_x_with_ER_NH(){

	vector<string> dirfiles = get_directory_files("./datarepo");

	for (std::vector<string>::iterator it = dirfiles.begin() ; it != dirfiles.end(); ++it){
		monteCarlo_estimation_with_ER(*it);
	}

}


int main() {

//	monteCarlo_estimation_with_ER("./datarepo/1.txt");
	monteCarlo_estimation_with_ER("./datarepo/Basque_syntactic_dependency_network.txt");

//	MyGraph g = MyGraph("./datarepo/1.txt");
//	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Arabic_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Catalan_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Chinese_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Czech_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/English_syntactic_dependency_network.txt");
//
//	cout << "Real graph" << endl;
//	//g.print();
//
//	auto start = std::chrono::high_resolution_clock::now();
//	g.calculate_closeness_v1();
//	auto finish = std::chrono::high_resolution_clock::now();
//	std::chrono::duration<double> elapsed = finish - start;
//	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;
//
//
//	cout << " Creating an ER " << endl;
//	MyER er = MyER(g);
//	//cout << "m: " << er.m << endl;
//	//er.printER();
//	//cout << " Now the print method" << endl;
//	//er.print();
//
//	start = std::chrono::high_resolution_clock::now();
//	er.calculate_closeness_v1();
//	finish = std::chrono::high_resolution_clock::now();
//	elapsed = finish - start;
//	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;


}

