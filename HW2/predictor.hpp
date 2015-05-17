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
	// You can add your own stuff here
};
