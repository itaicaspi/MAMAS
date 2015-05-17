#include "predictor.hpp"

Predictor::Predictor(bool local_history, bool local_tables, 
		  int history_table_size, int history_size,bool using_share)
{
}

bool Predictor::predict(unsigned long instruction_pointer)
{
	return false;	
}

void Predictor::update(unsigned long instruction_pointer, 
		       bool taken)
{
	
}

