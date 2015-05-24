#include "predictor.hpp"
#include <cstdlib>
#include <iostream>
using namespace std;
Predictor::Predictor(bool local_history, bool local_tables, 
		int history_table_size, int history_size,bool using_share) :
		local_history(local_history),
		local_tables(local_tables),
		history_table_size(local_history ? history_table_size : 1), // BHR table size
		history_size(history_size), // BHR history field size
		using_share(using_share)
{
	// resize and initialize history table
	history_table.resize(this->history_table_size);

	// resize tables and initialize all counters to weakly not taken
	int total_counters_tables = local_tables ? this->history_table_size : 1;
	counters_table.resize(total_counters_tables);
	for (int i = 0; i < total_counters_tables; i++) {
		counters_table[i].resize(2 << history_size, 1);
	}

	// This trick works only for 2^n history_table_size..
	// example: history_table_size = 16 --> tag_mask=0b1111
	set_mask = this->history_table_size - 1;
	tag_mask = 0xffffffff ^ set_mask;
	history_mask = ((2 << history_size) >> 1) - 1;
}

bool Predictor::predict(unsigned long instruction_pointer)
{
	unsigned long set = instruction_pointer & set_mask;
	unsigned long tag = (instruction_pointer & tag_mask) / history_table_size;
	unsigned long ip_lsb = instruction_pointer & history_mask;

	// look up first stage
	unsigned int history_entry = 0;
	if (local_history) {
		if (history_table[set].tag == tag) {
			// entry exists
			history_entry = set;
		} else {
			// entry doesn't exist
			if (!local_tables) {
				return getCounter(0, using_share ? ip_lsb : 0) > 1;
			}
			return false; // default prediction - weak not taken
		}
	}
	int history = history_table[history_entry].history;
	if (using_share) history ^= ip_lsb;

	unsigned char counter = getCounter(history_entry, history);
	return (int)counter > 1; // states 2 & 3 are taken
}

void Predictor::update(unsigned long instruction_pointer, bool taken)
{
	unsigned long set = instruction_pointer & set_mask;
	unsigned long tag = (instruction_pointer & tag_mask) / history_table_size;
	unsigned long ip_lsb = instruction_pointer & history_mask;

	// look up first stage
	unsigned int history_entry = set;
	bool entry_exists = true;

	if (local_history) {
		if (history_table[set].tag != tag) {
			entry_exists = false;
		}
	} else {
		// local history => 1 entry in history table
		history_entry = 0;
	}

	if (entry_exists) { // entry exists

		int history = history_table[history_entry].history;

		// update history
		history_table[history_entry].history = ((history << 1) + taken) & history_mask;

		if (using_share) history ^= ip_lsb;

		// update prediction
		setCounter(history_entry, history, taken);

	} else { // entry doesn't exist

		int history = (using_share ? ip_lsb : 0) & history_mask;

		// clear history counters
		if (local_tables) counters_table[set].assign(counters_table[set].size(),1);

		// update prediction
		setCounter(set, history, taken);

		history_table[set].tag = tag;
		history_table[set].history = taken & history_mask;
	}
}

unsigned char Predictor::getCounter(unsigned int table, unsigned int history) {
	table = local_tables ? table : 0;
	return counters_table[table][history];
}

void Predictor::setCounter(unsigned int table, unsigned int history, bool taken) {
	table = local_tables ? table : 0;
	int state = counters_table[table][history];
	if (taken) {
		state += (state == 3) ? 0 : 1;
	} else {
		state -= (state == 0) ? 0 : 1;
	}
	counters_table[table][history] = state;
}


