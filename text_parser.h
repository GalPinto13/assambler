#ifndef ASSEMBLER_TEXT_PARSER_H
#define ASSEMBLER_TEXT_PARSER_H

/**
 * Get the first word in a row
 * @param pos index of first char
 * @param source source line to check
 * @param target pointer to first word found
 * @return pos, the index of the first char of the word
 */
int getFirstWord(int pos, char source[],char *target);

/**
 * Breaks a line to separate words
 * @param lineText a line to break
 * @param brokenLine pointer to store the words
 * @param lineNum the number of the line in the file
 * @return the number of words in that line
 */
int breakLine(char *lineText, char **brokenLine, int lineNum);

/**
 * Checks if a word is a label
 * @param firstWord a word to check
 * @return true if it is a label
 */
bool isLabel(char *firstWord);

/**
 * removes a char from a string
 * @param str a string to remove char from
 * @param charToRemove a char to remove
 */
void removeChar(char * str, char charToRemove);

/**
 * Checks if a string is valid
 * @param word a string to check
 * @return true if it is a valid string
 */
bool isValidString(char *word);

/**
 * Checks if a string is a number and a valid one - between specific range
 * @param word a string to extract a number to check from
 * @param type a string - "gui"/"ins" to determine type of range to vaildate
 * @return true if it is a valid number
 */
bool isValidNum(char *word, char *type);

/**
 * Checks if a number is within the defined range
 * @param num a number to check
 * @param type a string - "gui"/"ins" to determine type of range to vaildate
 * @return true if it is in range
 */
bool isNumInRange(int num, char *type);

/**
 * Checks if a string is not alphanumeric
 * @param word a string to check
 * @return true if it is NOT alphanumeric
 */
bool notAlNum(char *word);

/**
 * Checks if the line doesn't exceeds the 80 Chars limit
 * @param lineText a line to check
 * @param lineNum the line number
 * @return true if it is exceeding the limit false otherwise
 */
bool longLine(char *lineText, int lineNum);

#endif
