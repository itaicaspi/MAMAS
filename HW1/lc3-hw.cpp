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
	   Fetch(ins, signals);
	   Decode(ins, signals);
	   Exec(signals);
	   WbMem(signals);
	   signals = tempSignals;
   }
}

// you may change the signatures of this fucntions according to your needs. 
void LC3::Fetch(unsigned short &ins, struct Signals &signals)
{
	if (signals.EX_MEM_latches.MEM_WB.PCSrc) {
		tempSignals.IF_ID_latches.NewPC = signals.EX_MEM_latches.NewPC + 2;
	} else {
		tempSignals.IF_ID_latches.NewPC = signals.IF_ID_latches.NewPC + 2;
	}

	tempSignals.IF_ID_latches.ins = ReadMem(pc);

	// If this is a jump - control hazard
	unsigned short opcode = decodeUnsignedField(signals.IF_ID_latches.ins, OPCODE);
	if ((opcode & 0x3) == 0x0) {
		// TODO: Control hazard, need to stall until this command reaches Memory step
	}
}

void LC3::Decode(unsigned short ins, struct Signals &signals)
{
	unsigned short opcode = decodeUnsignedField(signals.IF_ID_latches.ins, OPCODE);
	// TODO: where does the opcode continue to? how does it reach the exec stage?

	// fill ALUSrc, Immediate latchs
	tempSignals.ID_EX_latches.EX.ALUSrc = decodeUnsignedField(signals.IF_ID_latches.ins, IMM);
	// extension from 5bit to 16 done automatically by compiler
	tempSignals.ID_EX_latches.Imm = decodeSignedField(signals.IF_ID_latches.ins, IMM5);

	// Opcode controls
	tempSignals.ID_EX_latches.EX.ALUOp0 = opcode & 0x4;
	tempSignals.ID_EX_latches.EX.ALUOp0 = opcode & 0x8;
	signals.ID_EX_latches.MEM_WB.Branch	= (opcode & 0x3) == 0x0;

	signals.ID_EX_latches.EX.RegDst = 1; // ??

	// Get register values
	unsigned short sr1 = decodeUnsignedField(signals.IF_ID_latches.ins, SR1);
	unsigned short sr2 = decodeUnsignedField(signals.IF_ID_latches.ins, SR2);
	tempSignals.ID_EX_latches.data1 = regs[sr1];
	tempSignals.ID_EX_latches.data2 = regs[sr2];

	// Not very sure about these two..
	tempSignals.ID_EX_latches.Rd = decodeUnsignedField(signals.IF_ID_latches.ins, SR);
	tempSignals.ID_EX_latches.Rt = decodeUnsignedField(signals.IF_ID_latches.ins, SR1);

	// Forward signals, latches
	tempSignals.EX_MEM_latches.NewPC = signals.IF_ID_latches.NewPC;

	tempSignals.EX_MEM_latches.MEM_WB.MemRead = (opcode & 0x11) == 0x2;
	tempSignals.EX_MEM_latches.MEM_WB.MemWrite = (opcode & 0x11) == 0x3;
	signals.EX_MEM_latches.MEM_WB.MemToReg = (opcode & 0x11) == 0x2;
	signals.EX_MEM_latches.MEM_WB.RegWrite = ((opcode & 0x11) == 0x2) ||  (opcode & 0x11) == 0x1;
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

	// store current flags in case they are not updated
	tempSignals.EX_MEM_latches.flags = flags;

	// Perform ALU operations
	if (signals.ID_EX_latches.EX.ALUOp1 == 0 && signals.ID_EX_latches.EX.ALUOp0 == 0) {
		// ADD
		result = signals.ID_EX_latches.data1 + val2;
		tempSignals.EX_MEM_latches.flags = getFlags(result);
	} else if (signals.ID_EX_latches.EX.ALUOp1 == 1 && signals.ID_EX_latches.EX.ALUOp0 == 0) {
		// AND
		result = signals.ID_EX_latches.data1 & val2;
		tempSignals.EX_MEM_latches.flags = getFlags(result);
	}

	// Update the result
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
		// update flags for LD
		tempSignals.EX_MEM_latches.flags = getFlags(readData);
	} else if (signals.EX_MEM_latches.MEM_WB.MemWrite) {
		// Write to memory
		mem[signals.EX_MEM_latches.result] = signals.EX_MEM_latches.data;
	}

	if (signals.EX_MEM_latches.MEM_WB.MemToReg) {
		// Write data is from ALU result
		writeData = signals.EX_MEM_latches.result;
	} else {
		// Write data is from memory
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
		pc = signals.EX_MEM_latches.NewPC;
		// clean pipe but keep flags
		flags = tempSignals.EX_MEM_latches.flags;
		tempSignals = 0;
	} else {
		// save flags and continue execution
		flags = tempSignals.EX_MEM_latches.flags;
		pc += 2;
	}


}

unsigned short LC3::getFlags(short val)
{
	unsigned short tempFlags;
	if (val > 0) tempFlags = P_FLAG;
	else if (val == 0) tempFlags = Z_FLAG;
	else tempFlags = N_FLAG;
	return tempFlags;
}


// TODO: deal with hazards
