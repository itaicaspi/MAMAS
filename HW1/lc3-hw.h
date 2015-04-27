/*
 * LC3-hw.h
 */

#ifndef LC3_HW_H_
#define LC3_HW_H_



struct Signals
{
	// For you to define
	// Including latches between phases in the pipeline


	struct EX_Signals {
		bool RegDst;
		bool ALUOp1;
		bool ALUOp0;
		bool ALUSrc;
	};

	struct MEM_WB_Signals {
		bool Branch;
		bool MemRead;
		bool MemWrite;
		bool RegWrite;
		bool MemToReg;
		bool PCSrc;
	};


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
		short result;
		short data;
		unsigned short NewPC;
		struct MEM_WB_Signals MEM_WB;
	} EX_MEM_latches;


	struct ControlSignals {
		struct EX_Signals EX;
		struct MEM_WB_Signals MEM_WB;
	};

};

enum implementedIns {ADD, ADDI, AND, ANDI, LD, ST, BR};

const struct ControlSignals insSignals[] {
	{1, 0, 0, 0}, {0, 0, 0, 1, 0, 1}, // ADD - ALUOp same as LD and ST
	{1, 0, 0, 1}, {0, 0, 0, 1, 0, 1}, // ADDI - ALUOp same as LD and ST
	{1, 1, 0, 0}, {0, 0, 0, 1, 0, 1}, // AND
	{1, 1, 0, 1}, {0, 0, 0, 1, 0, 1}, // ANDI
	{0, 0, 0, 1}, {0, 1, 0, 1, 1, 1}, // LD
	{0, 0, 0, 1}, {0, 0, 1, 0, 0, 1}, // ST
	{0, 0, 1, 0}, {1, 0, 0, 0, 0, 0}  // BR
};

#endif
