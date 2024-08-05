#ifndef ASSEMBLER_MACRO_READER_H
#define ASSEMBLER_MACRO_READER_H

/**
 * Pre-process file
 * @param fname file name to pre-process
 * @return file name to be processed after
 */
char *readMacro (char *fname);

/**
 * Checks if a string is a name of a Macro
 * @param head linked list of Macros to go through
 * @param mName macro name to check
 * @return true if it is a macro
 */
bool isMacro(struct Macro *head, char *mName);

/**
 * Get a Macro node by its name
 * @param head linked list of Macros search
 * @param mName the name of the wanted macro
 * @return a pointer the the Macro node
 */
struct Macro* getMacro(struct Macro *head, char *mName);

/**
 * Adds a new Macro node the linked list
 * @param head pointer to the head of the list
 * @param mName new macro's name
 */
void addMacroNode(struct Macro **head,char *mName);

/**
 * Sets the content of a Macro node
 * @param head pointer to the head of a linked list
 * @param buf the read line
 * @param fsource the file to read from
 */
void setMacroContent(struct Macro *head, char buf[],FILE *fsource);

/**
 * Checks if a line is an 'endmacro' line
 * @param line a line to check
 * @return true if it is an 'endmacro' line
 */
bool isEndMacro(char line[]);

#endif
