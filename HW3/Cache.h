
#ifndef CACHE_H_
#define CACHE_H_

using namespace std;

#include <iostream>

//AddrType represent memory address.
typedef unsigned int AddrType;
typedef struct {
	bool valid;
	AddrType tag;
	unsigned int lru;
} TagArrayEntry;
typedef TagArrayEntry* Way;

/*
 * Cache: this class represent a  cache memory in the memory hierarchy simulator
 * all Cache instances are accessed with 32 bit address.
 * the replacement policy for a Cache instance is LRU.
 */
class Cache {
public:
/*
 * Cache: a constructor for the Cache class.
 * @param numWays: the Cache is numWays-set associative
 * @param dataSize: the Size of the cache data (in bytes)
 * @param lineSize: the size of a cacheline
 */
	Cache(AddrType numWays,AddrType dataSize,AddrType lineSize);

	bool lookup(AddrType addr);
	unsigned int fetchLine(AddrType addr, bool* eviction = NULL);
	void evict(AddrType addr);

private:
	// add private members here
	AddrType numWays;
	AddrType dataSize;
	AddrType lineSize;
	unsigned int sets;

	Way* ways;

	void updateLRU(unsigned int setAccessed, unsigned wayAccessed);
	void getAddrFields(AddrType addr, unsigned int& set, unsigned int& tag);
};

#endif /* CACHE_H_ */
