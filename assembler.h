#ifndef ASSEMBLER_ASSEMBLER_H
#define ASSEMBLER_ASSEMBLER_H

/**
 * Take the filename and get the file ready to be read and assembled
 * @param fileName The filename, without it's extension
 */
void read(char *fileName);
/**
 * Processes a single assembly source file, and creates output files.
 * @param fileName The filename, without it's extension
*/
void assemble(char *fileName);

#endif
