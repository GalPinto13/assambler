#ifndef ASSEMBLER_FILE_WRITER_H
#define ASSEMBLER_FILE_WRITER_H

/**
 * Writes an object output file
 * @param fileName file name
 * @param memoryRecords pointer to the memory records
 * @param memoryRecCount counter of the memory records
 * @param IC instruction counter
 * @param DC data counter
 */
void writeObjectFile(char *fileName, MemoryWords **memoryRecords, int memoryRecCount, int IC, int DC);

/**
 * Writes an entry output file
 * @param fileName file name
 * @param labelTable pointer to the label table
 * @param labelCounter labels counter
 */
void writeEntryFile(char *fileName,Label **labelTable, int labelCounter);

/**
 * Writes an extern file
 * @param fileName file name
 * @param labelTable pointer to the label table
 * @param labelCounter labels counter
 * @param memoryRecords pointer of the memory records
 * @param memoryRecCount memory records counter
 */
void writeExternFile(char *fileName, Label **labelTable, int labelCounter, MemoryWords **memoryRecords, int memoryRecCount);

/**
 * Comparison function for qsort
 * Comparing the value of "isExternal" parameter of a memory record object
 * @param a label
 * @param b label
 * @return true if the "isExternal" value of both labels is different
 */
int compareIsExt (const void *a, const void * b);

/**
 * Comparison function for qsort
 * Comparing addresses of memory records
 * @param a memory record
 * @param b memory record
 * @return true if the address value is different
 */
int compareAddress (const void *a, const void * b);

#endif