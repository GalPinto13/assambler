#ifndef ASSEMBLER_GUI_ANALYZER_H
#define ASSEMBLER_GUI_ANALYZER_H

/**
 * Checks if a string is a saved guidance word
 * @param word a string to check
 * @return true if it is a guidance word
 */
bool isGuidanceDataStorage(char *word);

/**
 * Creates a new Guidance object
 * @param gui a pointer to a guidance object
 * @param brokenLine a broken line to process
 * @param numOfWords number of words at the line
 * @return number of words needed in the memory for the guidance
 */
int createGui(Guidance *gui,char **brokenLine,int numOfWords);

/**
 * Checks if a line is a valid guidance line
 * @param lineNum the number of the line in the file
 * @param brokenLine a broken line to process
 * @param numOfWords number of words at the line
 * @return true if it is a valid guidance line
 */
bool isValidGuiLine(int lineNum,char **brokenLine, int numOfWords);

/**
 * Checks if a line is valid guidance struct
 * @param brokenLine a broken line to process
 * @param numOfWords number of words at the line
 * @return true if it is valid guidance struct
 */
bool isValidGuiStruct(char **brokenLine, int numOfWords);

/**
 * Checks if a line is valid guidance string
 * @param brokenLine a broken line to process
 * @param numOfWords number of words at the line
 * @return true if it is valid guidance string
 */
bool isValidGuiString(char **brokenLine, int numOfWords);

/**
 * Checks if a line is valid guidance data
 * @param brokenLine a broken line to process
 * @param numOfWords number of words at the line
 * @return true if it is valid guidance data
 */
bool isValidGuiData(char **brokenLine, int numOfWords);

/**
 * Inserts the guidance to memory record
 * @param memoryRecord pointer to the memory records
 * @param memoryRecCount counter of memory records
 * @param gui a guidance to insert
 * @param DC data counter
 */
void insertGuiToRec(MemoryWords *memoryRecord,int *memoryRecCount,Guidance *gui,int DC);

/**
 * Update address for data labels
 * @param labelTable pointer to the labels table
 * @param symCounter labels counter
 * @param IC instruction counter
 */
void updateDataLabels(Label **labelTable, int symCounter, int IC);

/**
 * Update address for data records
 * @param memoryRecords pointer to memory records
 * @param memoryRecCount memory records counter
 * @param IC instruction counter
 */
void updateDataRecords(MemoryWords **memoryRecords,int memoryRecCount,int IC);

#endif
