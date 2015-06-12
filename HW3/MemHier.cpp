/***************************************
 * Project:    MAMAS HW3
 * Author:     Itai Caspi & Yotam Eshel
 * 
 * File name:  MemHier.cpp
 ***************************************/

#include "MemHier.h"

MemHier::MemHier(Cache* L1,Cache* L2, bool inclusive) :
	L1(L1), L2(L2), inclusive(inclusive) {
}

CacheStatus MemHier::accessMem(AddrType addr,bool toWrite) {
	////////////////////////////
	// Check in L1

	bool found = L1->lookup(addr);
	if (found) {
		// since we operate in Write through we access L2 as well (and the
		// memory too but it's not relevant for the simulation)
		if (toWrite) {
			L2->lookup(addr);
		}

		return L1HIT;
	}

	// if not found, and this is a read - allocate the line
	if (!toWrite) {
		L1->fetchLine(addr);
	}

	/////////////////////////////
	// Check in L2

	found = L2->lookup(addr);
	if (found) {
		return L2HIT;
	}

	// if not found, and this is a read - allocate the line
	if (!toWrite) {
		bool eviction;
		AddrType evicted = L2->fetchLine(addr, &eviction);
		if (inclusive && eviction) {
			L1->evict(evicted);
			L1->fetchLine(addr);
		}
	}

	/////////////////////////////
	// Found in main memory

	return MEMACC;
}
