/***************************************
 * Project:    MAMAS HW3
 * Author:     Itai Caspi & Yotam Eshel
 * 
 * File name:  MemHier.cpp
 ***************************************/

#include "MemHier.h"

MemHier::MemHier(Cache* L1,Cache* L2, bool inclusive) :
	L1(L1), L2(L2), inclusive(inclusive) {
	// TODO: inclusive?
}

CacheStatus MemHier::hierLookup(AddrType addr) {
	int data = L1->lookup(addr);
	if (data == -1) { // L1 miss
		data = L2->lookup(addr);
		if (data == -1) { // L2 miss
			return MEMACC;
		}
		return L2HIT;
	}
	return L1HIT;
}

CacheStatus MemHier::accessMem(AddrType addr,bool toWrite) {
	if (!toWrite) {
		// read
		return hierLookup(addr);
	} else {
		// write

	}
	return MEMACC;
}
