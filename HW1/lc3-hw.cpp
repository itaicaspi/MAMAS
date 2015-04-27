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
	   //Fetch(ins, signals);
	   //Decode(ins, signals);
	   Exec(signals);
	   WbMem(signals);
	   signals = tempSignals;
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
	short val2;
	if (signals.controls.ALUSrc == 0) {
		val2 = signals.ID_EX_latches.data2;
	} else {
		val2 = signals.ID_EX_latches.Imm;
	}

	// TODO: for ALU operation - perform operation
	if (signals.controls.ALUOp1 == 0 && signals.controls.ALUOp0 == 0) {
		tempSignals.EX_MEM_latches.result = signals.ID_EX_latches.data1 + val2;
	} else if (signals.controls.ALUOp1 == 1 && signals.controls.ALUOp0 == 0) {
		tempSignals.EX_MEM_latches.result = signals.ID_EX_latches.data1 & val2;
	} else if (signals.controls.ALUOp1 == 0 && signals.controls.ALUOp0 == 1) {
		// TODO: branch subtract
	}

	// update flags for ADD and AND
	// TODO: for memory access - calculate effective address
	// TODO: for branch - calculate target address and branch condition
	if (signals.controls.RegDst == 0) {
		tempSignals.EX_MEM_latches.Rd_Rt = signals.ID_EX_latches.Rt;
	} else {
		tempSignals.EX_MEM_latches.Rd_Rt = signals.ID_EX_latches.Rd;
	}
}

void LC3::WbMem(struct Signals &signals)
{
	// TODO: for STORE or LOAD - write or read from memory
	// TODO: for branch - decide if to jump and update pc accordingly
	// TODO: store the result from ALU or memory to the destination register

	if (signals.controls.MemRead) {
		if (signals.controls.MemToReg) {
			= signals.EX_MEM_latches.result;
		} else {
			 = mem[signals.EX_MEM_latches.data];
		}

	} else {
		mem[signals.EX_MEM_latches.result] = signals.EX_MEM_latches.data;
	}

	if (signals.controls.PCSrc) {
		pc += 2;
	} else {
		pc = signals.EX_MEM_latches.NewPC;
	}


}
