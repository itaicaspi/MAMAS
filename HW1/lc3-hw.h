/*
 * LC3-hw.h
 */

#ifndef LC3_HW_H_
#define LC3_HW_H_

/*
 * SIGNALS
 */
struct EX_Signals {
	bool RegDst;
	bool ALUOp1;
	bool ALUOp0;
	bool ALUSrc; // 1 = Immediate; 0=data2
};

struct MEM_WB_Signals {
	bool Branch;
	bool MemRead;
	bool MemWrite;
	bool RegWrite;
	bool MemToReg;
	bool PCSrc;
};


struct ControlSignals {
	struct EX_Signals EX;
	struct MEM_WB_Signals MEM_WB;
};


enum implementedIns {ADD, ADDI, AND, ANDI, LD, ST, BR};

const struct ControlSignals insSignals[] {
	{1, 0, 0, 0}, {0, 0, 0, 1, 0}, // ADD - ALUOp same as LD and ST
	{1, 0, 0, 1}, {0, 0, 0, 1, 0}, // ADDI - ALUOp same as LD and ST
	{1, 1, 0, 0}, {0, 0, 0, 1, 0}, // AND
	{1, 1, 0, 1}, {0, 0, 0, 1, 0}, // ANDI
	{0, 0, 0, 1}, {0, 1, 0, 1, 1}, // LD
	{0, 0, 0, 1}, {0, 0, 1, 0, 0}, // ST
	{0, 0, 1, 0}, {1, 0, 0, 0, 0}  // BR
};


struct Signals
{

	/*
	 * LATCHES
	 */
	struct IF_ID_Latches {
		unsigned short NewPC;
		unsigned short ins;
	} IF_ID_latches;

	struct ID_EX_Latches {
		short Imm;
		short Rt;
		short Rd;
		short data1;
		short data2;
		unsigned short NewPC;
		struct EX_Signals EX;
		struct MEM_WB_Signals MEM_WB;
	} ID_EX_latches;

	struct EX_MEM_Latches {
		short Rd_Rt;
		unsigned short flags;
		short result;
		short data;
		unsigned short NewPC;
		struct MEM_WB_Signals MEM_WB;
	} EX_MEM_latches;


};

#endif
