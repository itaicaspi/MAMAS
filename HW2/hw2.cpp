#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <stdlib.h>
#include "predictor.hpp"

using namespace std;

vector<string> &split(const string &s, char delim, vector<string> &elems) {
	stringstream ss(s);
	string item;
	elems.clear();
	while (getline(ss, item, delim)) {
		elems.push_back(item);
	}
	return elems;
}

int main(int argc, char *argv[]) {
	ifstream srcFile;
	char line[256];
	string sub;
	bool local_history = false;
	bool local_tables = false;
	bool using_share= false;
	int history_size, history_table_size;
	vector<string> elems;

	if (argc < 2) {
		cout << "usage: predictor [filename]" << endl;
		return -1;
	}
	try {
		srcFile.open(argv[1]);
		srcFile.getline(line, 256);
		split(line, ' ', elems);
		history_table_size = strtoul(elems[0].c_str(), NULL, 0);
		history_size = strtoul(elems[1].c_str(), NULL, 0);
		if (elems[2] == "local_history")
			local_history = true;
		if (elems[3] == "local_tables")
			local_tables = true;
		if (elems.size() >= 5 && elems[4]=="using_share")
			using_share=true;
	} catch (...) {
		cout << "Error in input file: cannot read config" << endl;
		exit(-1);
	}

	Predictor predictor(local_history, local_tables, 
			    history_table_size, history_size,using_share);
	while (!srcFile.eof()) {
		srcFile.getline(line, 256);
		split(line, ' ', elems);
		if (elems.size() == 0)
			continue;
		if (elems.size() != 2) {
			cout << "Error in input file: bad trace" << endl;
			exit(-1);
		}
		unsigned long pc = strtoul(elems[0].c_str(), NULL, 0);
		bool taken = elems[1] == "T";
		cout << (predictor.predict(pc) ? "T" : "N") << endl;
		predictor.update(pc, taken);
	}

	return 0;
}

