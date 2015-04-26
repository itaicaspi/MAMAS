#include "lc3.h"

/////// For you to implement

//you cannot change the signature of this function.
void LC3::Run(int steps)
{
   struct Signals signals;
   //set the content of the struct to 0's. 
   memset(&signals, 0, sizeof(signals));
	for (int i=0; i<steps; i++) {
	//each iteration is equal to one clock cycle.
	}
}

// you may change the signatures of this fucntions according to your needs. 
void LC3::Fetch(unsigned short &ins, struct Signals &signals)
{
}

void LC3::Decode(unsigned short ins, struct Signals &signals)
{
}

void LC3::Exec(struct Signals &signals)
{
}

void LC3::WbMem(struct Signals &signals)
{
}

