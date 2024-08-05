#ifndef ASSEMBLER_INS_ANALYZER_H
#define ASSEMBLER_INS_ANALYZER_H

/**
 * Checks if a line is a valid instruction line
 * @param lineNum the number of the line at the source file
 * @param containsLabel does the line contains a label
 * @param opName string containing the name of the operation
 * @param brokenLine the line to process
 * @param numOfOperands number of operands to instruction
 * @return true if it is a valid instruction line
 * @param labelTable label table
 * @param labelCount label counter
 */
bool isValidInsLine(int lineNum,bool containsLabel, char *opName, char **brokenLine, int numOfOperands, Label **labelTable, int labelCount);

/**
 * Check which type is the operand
 * 0 - direct - raw data
 * 1 - label - a label
 * 2 - struct - a label of a struct and its sub-record number
 * 3 - register - r0, r1, ... , r7
 * @param operand operand to check
 * @param lineNum the number of the line at the file
 * @return the operand type
 * @param labelTable label table
 * @param labelCount label counter
 */
int getOperandType(char *operand,int lineNum, Label **labelTable, int labelCount);

/**
 * Creates a new instruction obejct
 * @param ins pointer to the instruction object
 * @param brokenLine the line to process
 * @param numOfOperands number of operands for instruction
 * @param lineNum the number of the line at the file
 * @return the number of memory words for the instruction
 * @param labelTable label table
 * @param labelCount label counter
 */
int createIns(Instruction *ins,char **brokenLine, int numOfOperands,int lineNum, Label **labelTable, int labelCount);

/**
 * Generate the first word of an instruction
 * @param ins the instruction's pointer
 * @return integer holding first word's binary code of an instruction
 */
int generateInsFirstWord(Instruction *ins);

/**
 * Generate a memory word of a direct operand
 * @param operand operand to process
 * @return integer holding the binary code
 */
int generateInsDirWord(char *operand);

/**
 * Generate a memory word, or modify one, holding the register codes
 * @param action new memory word / modify existing word
 * @param operand operand to process
 * @param ins instruction points
 * @param numOfOperands number of operands for the instruction
 * @return integer holding the binary code
 */
int generateInsRegWord(char *action, char *operand, Instruction *ins, int numOfOperands);

/**
 * Generate a memory word for struct's sub-record number
 * @param operand operand to process
 * @return integer holding the binary code
 */
int generateInsStructWord(char *operand);

/**
 * Inserts the instruction to memory record
 * @param memoryRecord pointer to the memory records
 * @param memoryRecCount counter of memory records
 * @param ins an instruction to insert
 * @param IC instruction counter
 */
void insertInsToRec(MemoryWords *memoryRecord,int *memoryRecCount,Instruction *ins,int IC);

#endif
