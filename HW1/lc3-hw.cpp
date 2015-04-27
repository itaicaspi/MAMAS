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
	short val2, result;

	if (signals.ID_EX_latches.EX.ALUSrc == 0) {
		val2 = signals.ID_EX_latches.data2;
	} else {
		val2 = signals.ID_EX_latches.Imm;
	}

	// Calculate jump address
	tempSignals.EX_MEM_latches.NewPC = signals.ID_EX_latches.NewPC +
									   signals.ID_EX_latches.Imm << 1;

	// Perform ALU operations
	if (signals.ID_EX_latches.EX.ALUOp1 == 0 && signals.ID_EX_latches.EX.ALUOp0 == 0) {
		result = signals.ID_EX_latches.data1 + val2;
		updateFlags(result);
	} else if (signals.ID_EX_latches.EX.ALUOp1 == 1 && signals.ID_EX_latches.EX.ALUOp0 == 0) {
		result = signals.ID_EX_latches.data1 & val2;
		updateFlags(result);
	} else if (signals.ID_EX_latches.EX.ALUOp1 == 0 && signals.ID_EX_latches.EX.ALUOp0 == 1) {
		// TODO: branch subtract
	}

	// Update the flags
	tempSignals.EX_MEM_latches.result = result;

	// Pass the register latches to next level
	if (signals.ID_EX_latches.EX.RegDst == 0) {
		tempSignals.EX_MEM_latches.Rd_Rt = signals.ID_EX_latches.Rt;
	} else {
		tempSignals.EX_MEM_latches.Rd_Rt = signals.ID_EX_latches.Rd;
	}

	// Pass the signals to the next phase
	tempSignals.EX_MEM_latches.MEM_WB = signals.ID_EX_latches.MEM_WB;
}

void LC3::WbMem(struct Signals &signals)
{

	short writeData, readData;

	if (signals.EX_MEM_latches.MEM_WB.MemRead) {
		// Read from memory
		readData = mem[signals.EX_MEM_latches.data];
	} else if (signals.EX_MEM_latches.MEM_WB.MemWrite) {
		// Write to memory
		mem[signals.EX_MEM_latches.result] = signals.EX_MEM_latches.data;
	}

	if (signals.EX_MEM_latches.MEM_WB.MemToReg) {
		// Write data is from memory
		writeData = signals.EX_MEM_latches.result;
	} else {
		// Write data is from ALU result
		writeData = mem[signals.EX_MEM_latches.data];
	}

	// Writing to RF can be done in parallel to reading in DECODE
	if (signals.EX_MEM_latches.MEM_WB.RegWrite) {
		regs[signals.EX_MEM_latches.Rd_Rt] = writeData;
	}

	// Branch decision
	if (signals.EX_MEM_latches.MEM_WB.Branch &&
		(signals.EX_MEM_latches.Rd_Rt == P_FLAG + N_FLAG + Z_FLAG ||
		signals.EX_MEM_latches.Rd_Rt == flags)) {
		// branch
		tempSignals.EX_MEM_latches.MEM_WB.PCSrc = 0;
		// TODO: clean all the command that were already loaded to the pipe
	} else {
		tempSignals.EX_MEM_latches.MEM_WB.PCSrc = 1;
	}


	// Branch or regular execution
	if (tempSignals.EX_MEM_latches.MEM_WB.PCSrc) {
		pc += 2;
	} else {
		pc = signals.EX_MEM_latches.NewPC;
	}


}

void LC3::updateFlags(short val)
{
	if (val > 0) flags = P_FLAG;
	else if (val == 0) flags = Z_FLAG;
	else flags = N_FLAG;
}


// TODO: need to create a temp copy of flags
// TODO: deal with hazards
