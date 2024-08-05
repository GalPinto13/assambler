#ifndef ASSEMBLER_SECOND_PASS_H
#define ASSEMBLER_SECOND_PASS_H

/**
 * Second pass over the file
 * looks for entry and extern labels
 * and fills the gaps in instruction records ->
 *      the labels addresses from the label table
 * @param fname file name
 * @param labelTable pointer to the labels table
 * @param labelCount counter of labels
 * @param memoryRecords pointer to the memory records table
 * @param memoryRecCount counter of the memory records
 * @param DC data counter
 * @param IC instruction counter
 * @param entryExist flag if entry exist
 */
void secondPass(char *fname , Label **labelTable, int *labelCount, MemoryWords **memoryRecords, int *memoryRecCount, int *entryExist);

/**
 * Checks if a label is external
 * @param label a label to check
 * @param labelTable pointer to the label table
 * @param labelCounter counter of the labels at the label table
 * @return true if the label is external
 */
bool isLabelExternal(char *label, Label **labelTable, int labelCounter);

/**
 * Get the address of a label from the labels table
 * @param labelTable pointer to the labels table
 * @param numOfLabel number of the label to get its address
 * @return label's address
 */
int getLabelAddress(Label **labelTable, int numOfLabel);

/**
 * Fill all the gaps in the instructions memory records
 * all the labels addresses from the label table
 * to the designated locations in the memory
 * @param memoryRecords memory records table
 * @param memoryRecCount counter of memory records
 * @param labelTable label table
 * @param labelCount counter of labels
 */
void completeInstructionRecords(MemoryWords **memoryRecords,int *memoryRecCount,Label **labelTable,int *labelCount);

/**
 * Builds the codes for the operands using label's address from label table
 * @param curRecord pointer to the current record
 * @param operand the operand that we process
 * @param type type of operand - source/destination
 * @param i the counter of the record
 * @param labelTable label table
 * @param labelCount counter of labels
 */
void buildCode(MemoryWords *curRecord, char *type,int i, Label **labelTable, int *labelCount);

#endif
