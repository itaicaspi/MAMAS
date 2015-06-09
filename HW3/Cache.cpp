/***************************************
 * Project:    MAMAS HW3
 * Author:     Itai Caspi & Yotam Eshel
 * 
 * File name:  Cache.cpp
 ***************************************/

#include "Cache.h"

Cache::Cache(AddrType numWays,AddrType dataSize,AddrType lineSize) :
	numWays(numWays), dataSize(dataSize), lineSize(lineSize) {

	// resize all data structures to fit requested cache parameters
	sets = dataSize / (lineSize * numWays);
	ways.resize(numWays);
	for (vector<Way>::iterator way = ways.begin(); way != ways.end(); ++way) {
		way->valid.resize(sets, false);
		way->tags.resize(sets);
		// since we don't actually need to deal with data, we don't need the
		// data array
	}

}


bool Cache::lookup(AddrType addr) {

	// break address to fields
	int lineOffset = addr & (lineSize - 1);
	int set = (addr / lineSize) & (sets - 1);
	int tag = addr / (lineSize * sets);

	// go over all the ways and look for a hit
	for (vector<Way>::iterator way = ways.begin(); way != ways.end(); ++way) {
		if (way->valid[set] && way->tags[set] == tag) {
			// hit
			return true;
		}
	}

	// miss
	return false;
}


int Cache::LRU() {
	// TODO: implement LRU
	return 0;
}
