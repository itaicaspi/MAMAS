#ifndef MEMHIER_H_
#define MEMHIER_H_
#include "Cache.h"

enum CacheStatus {
	L1HIT=0, //L1 Hit
	L2HIT=1, //L1 Miss,L2 Hit
	MEMACC=2// L1 & L2 Miss
} ;
/*
 * MemHier represent the memory hierarchy for the simulator
 * the hierarchy contain two levels of cache ' L1& L2 and a main memory
 */
class MemHier {
public:
	/*
	 * MemHier: a constructor for the class
	 * @param L1: the L1 Cache of the hierarchy
	 * @param L2: the L2 Cache of the hierarchy
	 * @param inclusive:  true for inclusive hierarchy , false for non-inclusive hierarchy.
	 */
	MemHier(Cache* L1,Cache* L2, bool inclusive);
	/*
	 * CacheStatus accessMem: simulate a memory access
	 * @param addr: the memory address requested
	 * @param toWrite: true for write request, false for read request
	 * return: CacheStatus that represent the memory access result.
	 */
	CacheStatus accessMem(AddrType addr,bool toWrite);

private:
//add private members here
	Cache* L1;
	Cache* L2;
	bool inclusive;
};

#endif
