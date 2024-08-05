#ifndef ASSEMBLER_FIRST_PASS_H
#define ASSEMBLER_FIRST_PASS_H

/**
 * First pass over the file
 * validates the file content and show errors if found
 * generates codes for guidance lines
 * store the labels in a label table
 * generates codes for the first word of an instruction,
 * also generates codes for instruction operand that are direct/register/2nd word of a struct operand
 * @param fname file name
 * @param memoryRecords pointer to memory records table
 * @param memoryRecCount counter of memory records
 * @param labelTable pointer to label table
 * @param labelCount counter of labels
 * @param IC instruction counter
 * @param DC data counter
 * @param externExist flag -> extern exist
 */
void firstPass(char *fname, MemoryWords **memoryRecords, int *memoryRecCount, Label **labelTable, int *labelCount, int *IC, int *DC, int *externExist);

/**
 * Checks if a label is a valid label
 * @param lineNum the number of the line that the label is at
 * @param word the label to check
 * @param labelTable label table
 * @param labelCount label counter
 * @return true if it is a valid label
 */
bool isValidLabel(int lineNum,char *word, Label *labelTable, int *labelCount);

/**
 * Inserts label to label table
 * @param label label to insert
 * @param DC data counter
 * @param IC instruction counter
 * @param labelTable label table
 * @param labelCount label counter
 * @param isGui is it guidance flag
 * @param isInst is it instruction flag
 */
void insertLabel(char *label,int DC, int IC, Label **labelTable, int *labelCount, bool isGui,bool isInst);

/**
 * Checks if a string is a name of a label that we inserted to label table
 * @param word label to check
 * @param labelTable label table
 * @param labelCount label counter
 * @return if it is a label, return the row number at the table(first row = 1), if not return 0
 */
int isAlreadyLabel(char *word, Label *labelTable, int labelCount);

/**
 * Adds new memory words to memory record table
 * @param lineNum number of line that we process
 * @param numOfWords number of words in that line
 * @param brokenLine pointer to broken line
 * @param containsLabel contains a label flag
 * @param DC data counter
 * @param IC instruction counter
 * @param memoryRecords memory records table
 * @param memoryRecCount memory records counter
 * @param isGui is it a guidance flag
 * @param isInst is it an instruction flag
 * @param labelTable label table
 * @param labelCount label counter
 */
void addMemoryWords(int lineNum, int numOfWords,char **brokenLine,bool containsLabel,int *DC,int *IC,MemoryWords **memoryRecords, int *memoryRecCount, bool isGui,bool isInst, Label **labelTable, int labelCount);

#endif