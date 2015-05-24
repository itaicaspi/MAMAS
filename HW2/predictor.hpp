#include <vector>

using namespace std;

struct First_Level {
	unsigned int tag;
	unsigned int history;
};

class Predictor {
public:
	Predictor(bool local_history, bool local_tables, 
		  int history_table_size, int history_size,bool using_share);

	// Returns whether the branch is predicted to be taken
	bool predict(unsigned long instruction_pointer);

	// Updates the predictor state according to the actual 
	// result
	void update(unsigned long instruction_pointer, bool taken);

private:
	bool local_history;
	bool local_tables;
	int history_table_size;
	int history_size;
	bool using_share;

	vector<First_Level> history_table;
	vector<vector<unsigned char> > counters_table;

	unsigned long set_mask;
	unsigned long tag_mask;
	unsigned long history_mask;

	unsigned char getCounter(unsigned int table, unsigned int history);
	void setCounter(unsigned int table, unsigned int history, bool taken);
};
