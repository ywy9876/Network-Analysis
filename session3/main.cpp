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






void calculate_distance(const WGraph& G, int s, vector<double>& d, vector<int>& p) {

	
}




int main() {


//	MyGraph g = MyGraph("./datarepo/1.txt");
	MyGraph g = MyGraph("./datarepo/Basque_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Arabic_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Catalan_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Chinese_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/Czech_syntactic_dependency_network.txt");
//	MyGraph g = MyGraph("./datarepo/English_syntactic_dependency_network.txt");

	cout << "Real graph" << endl;
	//g.print();

	auto start = std::chrono::high_resolution_clock::now();
	g.calculate_closeness_v1();
	auto finish = std::chrono::high_resolution_clock::now();
	std::chrono::duration<double> elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;


	cout << " Creating an ER " << endl;
	MyER er = MyER(g);
	//cout << "m: " << er.m << endl;
	//er.printER();
	//cout << " Now the print method" << endl;
	//er.print();

	start = std::chrono::high_resolution_clock::now();
	er.calculate_closeness_v1();
	finish = std::chrono::high_resolution_clock::now();
	elapsed = finish - start;
	cout << "Time spent in calculating closeness: " << elapsed.count() << "s" << endl;


}

