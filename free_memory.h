#ifndef ASSEMBLER_FREE_MEMORY_H
#define ASSEMBLER_FREE_MEMORY_H

/**
 * Frees brokenLine pointers to the words, and the pointer to the line
 * @param brokenLine line to free
 * @param wordNum number of words in the line
 * @param containsLabel is there a label in the line
 */
void freeBrokenLine(char **brokenLine, int wordNum, bool containsLabel);

/**
 * Frees the memory for all the labels in the table
 * @param labelTable pointer to label table
 * @param labelNum number of labels in the table
 */
void freeLabelTable(Label *labelTable, int labelNum);

/**
 * Frees the memory records table
 * @param memoryRecords memory records table to free
 * @param memoryRecCount number of records in the table
 */
void freeMemRec(MemoryWords *memoryRecords, int memoryRecCount);

/**
 * Frees an instruction's data pointers and the pointer to the instruction object
 * @param ins instruction to free
 */
void freeInstruction(Instruction *ins);

/**
 * Frees an guidance's data pointers and the pointer to the guidance object
 * @param ins guidance to free
 */
void freeGuidance(Guidance *gui);

/**
 * Free(memory) the linked list of the Macros and its nodes
 * @param head pointer to the head of a linked list
 */
void freeMacro(struct Macro *head);
#endif
