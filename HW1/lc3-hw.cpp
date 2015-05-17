#include "lc3.h"
/////// For you to implement

//you cannot change the signature of this function.
void LC3::Run(int steps)
{
	struct Signals signals;
	//set the content of the struct to 0's.
	memset(&signals, 0, sizeof(signals));

	// initialize pc to 0 and pipe to stall
	tempSignals.IF_ID_latches.NewPC = 0;
	signals.IF_ID_latches.ins = RES_OPCODE << 12;
	for (int i=0; i<steps; i++) {
		//each iteration is equal to one clock cycle.
		Fetch(signals);
		Decode(signals);
		Exec(signals);
		WbMem(signals);
		signals = tempSignals;
	}
}

// you may change the signatures of this fucntions according to your needs. 
void LC3::Fetch(struct Signals &signals)
{
	if (signals.stall != 0) {
		tempSignals.IF_ID_latches.ins = RES_OPCODE << 12;
		tempSignals.stall--;
	} else {
		unsigned short pc_select = signals.EX_MEM_latches.MEM_WB.PCSrc ?
				signals.EX_MEM_latches.NewPC :
				tempSignals.IF_ID_latches.NewPC;
		tempSignals.IF_ID_latches.ins = ReadMem(pc_select);
		tempSignals.IF_ID_latches.NewPC = pc_select + 2;

		// If this is a jump - control hazard
		unsigned short opcode = decodeUnsignedField(tempSignals.IF_ID_latches.ins, OPCODE);
		if (opcode == BR_OPCODE) {
			tempSignals.stall = 3;
		}

	}
}

void LC3::Decode(struct Signals &signals)
{
	unsigned short opcode = decodeUnsignedField(signals.IF_ID_latches.ins, OPCODE);

	// fill ALUSrc, Immediate latchs
	bool imm_opcode = decodeUnsignedField(signals.IF_ID_latches.ins, IMM);

	// Get register values
	unsigned short sr1 = decodeUnsignedField(signals.IF_ID_latches.ins, SR1);
	unsigned short sr2 = decodeUnsignedField(signals.IF_ID_latches.ins, SR2);
	unsigned short sr  = decodeUnsignedField(signals.IF_ID_latches.ins, SR);

	tempSignals.ID_EX_latches.data1 = regs[sr1];
	tempSignals.ID_EX_latches.data2 = regs[sr2];

	tempSignals.ID_EX_latches.Rd = decodeUnsignedField(signals.IF_ID_latches.ins, DR);
	tempSignals.ID_EX_latches.Rt = sr1;

	// extension from 5bit to 16 done automatically by compiler
	int ins_num = 0;
	switch(opcode) {
	case ADD_OPCODE:
		if (imm_opcode) {
			ins_num = ADDI;
		} else {
			ins_num = ADD;
		}
		ins_num = fixDataHazard(sr1, sr2, signals) ? STALL : ins_num;
		tempSignals.ID_EX_latches.Imm = decodeSignedField(signals.IF_ID_latches.ins, IMM5);
		break;
	case AND_OPCODE:
		if (imm_opcode) {
			ins_num = ANDI;
		} else {
			ins_num = AND;
		}
		ins_num = fixDataHazard(sr1, sr2, signals) ? STALL : ins_num;
		tempSignals.ID_EX_latches.Imm = decodeSignedField(signals.IF_ID_latches.ins, IMM5);
		break;
	case LD_OPCODE:
		tempSignals.ID_EX_latches.Imm = decodeSignedField(signals.IF_ID_latches.ins, PCOFFSET9);
		ins_num = LD;
		break;
	case ST_OPCODE:
		tempSignals.ID_EX_latches.Imm = decodeSignedField(signals.IF_ID_latches.ins, PCOFFSET9);
		ins_num = fixDataHazard(sr, sr, signals) ? STALL : ST;
		tempSignals.ID_EX_latches.data2 = regs[sr];
		break;
	case BR_OPCODE:
		tempSignals.ID_EX_latches.Imm = decodeSignedField(signals.IF_ID_latches.ins, PCOFFSET9);
		ins_num = BR;
		break;
	default:
		ins_num = STALL;
		break;
	}

	/*
	 * signals order:
	 * 	RegDst
	 *	ALUOp1
	 * 	ALUOp0
	 * 	ALUSrc // 1=immediate 0=data2
	 * 	UpdateSignals
	 * 	Branch
	 *	MemRead
	 *	MemWrite
	 *	RegWrite
	 *	MemToReg
	 * 	PCSrc
	 */
	struct ControlSignals insSignals[] =
	{{1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0}, // ADD - ALUOp same as LD and ST
			{1, 0, 0, 1, 1, 0, 0, 0, 1, 0, 0}, // ADDI - ALUOp same as LD and ST
			{1, 1, 0, 0, 1, 0, 0, 0, 1, 0, 0}, // AND
			{1, 1, 0, 1, 1, 0, 0, 0, 1, 0, 0}, // ANDI
			{1, 0, 0, 1, 1, 0, 1, 0, 1, 1, 0}, // LD
			{1, 0, 0, 1, 1, 0, 0, 1, 0, 0, 0}, // ST
			{1, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0}, // BR
			{0, 0, 0, 0, 0, 0, 0, 0, 0, 0}}; // STALL

	tempSignals.ID_EX_latches.EX = insSignals[ins_num].EX;
	tempSignals.ID_EX_latches.MEM_WB = insSignals[ins_num].MEM_WB;


	// Forward signals, latches
	tempSignals.ID_EX_latches.NewPC = signals.IF_ID_latches.NewPC;
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
	tempSignals.EX_MEM_latches.NewPC = signals.ID_EX_latches.NewPC + signals.ID_EX_latches.Imm;

	// Perform ALU operations
	if (signals.ID_EX_latches.MEM_WB.MemRead || signals.ID_EX_latches.MEM_WB.MemWrite) {
		result = signals.ID_EX_latches.NewPC + val2;
	} else if (signals.ID_EX_latches.EX.ALUOp1 == 0 && signals.ID_EX_latches.EX.ALUOp0 == 0) {
		// ADD
		result = signals.ID_EX_latches.data1 + val2;
		if (signals.ID_EX_latches.EX.UpdateSignals) updateFlags(result);
	} else if (signals.ID_EX_latches.EX.ALUOp1 == 1 && signals.ID_EX_latches.EX.ALUOp0 == 0) {
		// AND
		result = signals.ID_EX_latches.data1 & val2;
		if (signals.ID_EX_latches.EX.UpdateSignals) updateFlags(result);
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
	tempSignals.EX_MEM_latches.data = signals.ID_EX_latches.data2;
}

void LC3::WbMem(struct Signals &signals)
{

	short writeData, readData;

	if (signals.EX_MEM_latches.MEM_WB.MemRead) {
		// Read from memory
		readData = ReadMem(signals.EX_MEM_latches.result);
		// update flags for LD
		updateFlags(readData);
	} else if (signals.EX_MEM_latches.MEM_WB.MemWrite) {
		// Write to memory
		WriteMem(signals.EX_MEM_latches.result, signals.EX_MEM_latches.data);
	}

	if (signals.EX_MEM_latches.MEM_WB.MemToReg) {
		// Write data is from memory
		writeData = readData;
	} else {
		// Write data is from ALU result
		writeData = signals.EX_MEM_latches.result;
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
		tempSignals.EX_MEM_latches.MEM_WB.PCSrc = 1;
	} else {
		tempSignals.EX_MEM_latches.MEM_WB.PCSrc = 0;
	}
	pc = signals.EX_MEM_latches.NewPC;

}

void LC3::updateFlags(short val)
{
	if (val > 0) flags = P_FLAG;
	else if (val == 0) flags = Z_FLAG;
	else flags = N_FLAG;
}

bool LC3::fixDataHazard(unsigned short reg1, unsigned short reg2, struct Signals &signals)
{
	// check for data hazards
	unsigned short dest = signals.ID_EX_latches.EX.RegDst ?
			signals.ID_EX_latches.Rd : signals.ID_EX_latches.Rt;

	if (signals.ID_EX_latches.MEM_WB.RegWrite && (reg1 == dest || reg2 == dest)) {
		// data hazard 1 stages away - requires 2 stages stalling
		tempSignals.IF_ID_latches.ins = RES_OPCODE << 12;
		tempSignals.IF_ID_latches.NewPC -= 4;
		return true;
	} else if (signals.EX_MEM_latches.MEM_WB.RegWrite &&
			(reg1 == signals.EX_MEM_latches.Rd_Rt || reg2 == signals.EX_MEM_latches.Rd_Rt)) {
		// data hazard 2 stage away - requires 1 stage stalling
		tempSignals.IF_ID_latches.ins = signals.IF_ID_latches.ins;
		tempSignals.IF_ID_latches.NewPC -= 2;
		return true;
	}
	return false;
}
