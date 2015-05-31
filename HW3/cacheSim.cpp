
#include "MemHier.h"
#include <fstream>
#include <vector>
#include <iostream>
#include <sstream>
#include <string>

using namespace std;

// split string by the delimiter ',' and return a vector that contain the result.
vector<string> tokenize(string str) {
vector<string> vec;
stringstream ss(str);
string s;

while (getline(ss, s, ',')) {
 vec.push_back(s);
}
return vec;
}
// convert string to AddrType
AddrType strAsNumber(string str)
{
	return(AddrType)atoi(str.data());
}


int main (int argc ,char** argv)
{
    // checking number of argument
	if(argc!=4)
	{
		cerr<<"illegal number of arguments";
		return 0;
	}
	// read definitions file
	ifstream defintionStream;
	defintionStream.open(argv[1]);
	string defintionStr;
	defintionStream>>defintionStr;
	defintionStream.close();

	// construct caches from the  definitions file
	vector<string> cacheDef=tokenize(defintionStr);
	if(cacheDef.size()!=6)
	{
		cerr<<"illegal definition file/definition file doesn't exist";
		return 0;
	}
	AddrType lineSize=strAsNumber(cacheDef[0]);
	bool inclusive=(cacheDef[1].compare("TRUE")==0); //the cache inclusive if the token value equal "TRUE"
	AddrType L1numWays=strAsNumber(cacheDef[2]);
	AddrType L1Data=strAsNumber(cacheDef[3]);
	AddrType L2numWays=strAsNumber(cacheDef[4]);
	AddrType L2Data=strAsNumber(cacheDef[5]);

	Cache L1(L1numWays,L1Data,lineSize);
	Cache L2(L2numWays,L2Data,lineSize);
	MemHier memHier=MemHier(&L1,&L2,inclusive);

	// memory accesses sequence read from file
	ifstream simStream;
	simStream.open(argv[2]);
	string line;
	ofstream writeStream;
	writeStream.open(argv[3]);

	 while (getline(simStream,line))
	 {
		 vector<string> access=tokenize(line);
		 if(access.size()!=2)
		 {
			 cerr<<"illegal accesses file";
			 break;
		 }
		 bool toWrite=access[1].compare("W")==0||access[1].compare("w")==0;
		 //access memory hierarchy
		 CacheStatus stat=memHier.accessMem(strAsNumber(access[0]),toWrite);
		 switch(stat) {
		 case L1HIT:  writeStream<<access[0]<<",L1HIT"<<endl; break;
		 case L2HIT:  writeStream<<access[0]<<",L2HIT"<<endl; break;
		 case MEMACC: writeStream<<access[0]<<",MEMACC"<<endl; break;
		 }
	 }
   writeStream.close();
   simStream.close();

return 0;
}
