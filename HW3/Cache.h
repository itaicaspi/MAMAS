
#ifndef CACHE_H_
#define CACHE_H_

#include <stdbool.h>


//AddrType represent memory address.
typedef unsigned int AddrType;

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

private:
	// add private members here

};

#endif /* CACHE_H_ */
