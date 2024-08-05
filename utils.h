#ifndef ASSEMBLER_UTILS_H
#define ASSEMBLER_UTILS_H

/* include global headers */
#include "global.h"

/* move to next not white char on a string */
#define NEXT_NOT_WHITE(string, index) \
        for (;string[(index)] && (string[(index)] == '\t' || string[(index)] == ' '); (++(index)))\
        ;

/**
 * Converts defined NUMBER_OF_BITS of an int to Base32 code string
 * @param binary int to convert
 * @return 2 chars string of Base32 code
 */
char *binaryToBase32(int binary);

/**
 * Checks if a word is a saved word - register name/instruction name
 * @param word a word to check
 * @return true if the word is a saved word
 */
bool isSavedWord(char *word);

/**
 * Checks if a number is an integer
 * @param val a number to check
 * @return true if it is an integer
 */
bool isInteger(double val);

/**
 * Checks if a string is a name of an instruction
 * @param word a string to check
 * @return true if it is an instruction
 */
bool isInstruction(char *word);

/** Takes an int and convert it to its 2's complement, binary code represented by an int
 * @param num a number to convert
 * @return an int representing 2's complement value in binary of a number
 */
int numToCode(int num);

#endif
