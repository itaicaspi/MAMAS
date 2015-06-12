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

	ways = new Way[numWays];

	for (unsigned int i = 0; i < numWays; i++) {
		ways[i] = new TagArrayEntry[sets];

		for (unsigned int j = 0; j < sets; j++) {
			ways[i][j].valid = false;
			ways[i][j].lru = i;
		}
		// since we don't actually need to deal with data, we don't need the
		// data array
	}
}


bool Cache::lookup(AddrType addr) {

	// break address to fields
	unsigned int set, tag;
	getAddrFields(addr, set, tag);

	// go over all the ways and look for a hit
	for (unsigned int i = 0; i < numWays; i++) {
		if (ways[i][set].valid && ways[i][set].tag == tag) {
			updateLRU(set, i);
			// hit
			return true;
		}
	}

	// miss
	return false;
}

// called when line is not present
// returns evicted line
unsigned int Cache::fetchLine(AddrType addr, bool* eviction) {
	// break address to fields
	unsigned int set, tag;
	getAddrFields(addr, set, tag);

	// find the right way (the one with lru = 0)
	int lru = 0;
	for (unsigned int i = 0; i < numWays; i++) {
		if (!ways[i][set].valid) {
			lru = i;
			if (eviction) *eviction = false;
			break;
		} else if (ways[i][set].lru == 0) {
			lru = i;
			if (eviction) *eviction = true;
		}
	}

	// keep address of first byte of evicted line
	AddrType evicted = 0;
	if (eviction && *eviction) {
		evicted = ways[lru][set].tag * (lineSize * sets) + set * lineSize;
	}

	ways[lru][set].tag = tag;
	ways[lru][set].valid = true;
	updateLRU(set, lru);

	return evicted;
}

void Cache::updateLRU(unsigned int setAccessed, unsigned wayAccessed) {
	unsigned int x = ways[wayAccessed][setAccessed].lru;
	for (unsigned int i = 0; i < numWays; i++) {
		if (ways[i][setAccessed].lru > x) {
			ways[i][setAccessed].lru--;
		}
	}
	ways[wayAccessed][setAccessed].lru = numWays - 1;
}

void Cache::evict(AddrType addr) {
	unsigned int set, tag;
	getAddrFields(addr, set, tag);
	for (unsigned int i = 0; i < numWays; i++) {
		if (ways[i][set].tag == tag) {
			ways[i][set].valid = false;
		}
	}
}

void Cache::getAddrFields(AddrType addr, unsigned int& set, unsigned int& tag) {
	set = (addr / lineSize) & (sets - 1);
	tag = addr / (lineSize * sets);
}
