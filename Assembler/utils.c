#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "utils.h"

/*
 * stores all the register names
 */
char RegNames[8][3] = {"r0", "r1", "r2", "r3", "r4", "r5", "r6", "r7"};

/*
 * table that stores all the operation by their codes,
 * the number of allowed operands
 * a flag the indicates which are allowed for source and which for destination
 */
Operations opTable[] = {{"mov",0,TWO,4,3},
                                    {"cmp",1,TWO,4,4},
                                    {"add",2,TWO,4,3},
                                    {"sub",3,TWO,4,3},
                                    {"not",4,ONE,0,3},
                                    {"clr",5,ONE,0,3},
                                    {"lea",6,TWO,2,3},
                                    {"inc",7,ONE,0,3},
                                    {"dec",8,ONE,0,3},
                                    {"jmp",9,ONE,0,3},
                                    {"bne",10,ONE,0,3},
                                    {"get",11,ONE,0,3},
                                    {"prn",12,ONE,0,4},
                                    {"jsr",13,ONE,0,3},
                                    {"rts",14,NONE,0,0},
                                    {"hlt",15,NONE,0,0},
                                    };

/*
 * stores all the chars used to represent base32 code
 */
const char Base32[32] = {
		'!',
		'@',
		'#',
		'$',
		'%',
		'^',
		'&',
		'*',
		'<',
		'>',
		'a',
		'b',
		'c',
		'd',
		'e',
		'f',
		'g',
		'h',
		'i',
		'j',
		'k',
		'l',
		'm',
		'n',
		'o',
		'p',
		'q',
		'r',
		's',
		't',
		'u',
		'v'};
/*
 * an error flag
 */
bool IS_ERROR = FALSE;

char *binaryToBase32(int binary) {
    /* split bits to groups of 5 and use & operator with a mask(32) */
    int numOfBlocks,leftBlock = 0,remainder,i;
    char *code;
    /* stores last char */
    char tmp[2];

    /* how many bits do we want from the left block */
    remainder = NUMBER_OF_BITS % 5;

    if(remainder > 0) {
        /* it's not a full 5 bits block */
        /* all full 5 bits blocks + 1 */
        numOfBlocks = ((NUMBER_OF_BITS / 5) + 1);
        /* create mask for left block */
        for(i = 0; i < remainder; i++)
            leftBlock += pow(2,i);
    } else {
        /* all blocks are 5 bits */
        numOfBlocks = (NUMBER_OF_BITS / 5);
        /* mask for left block is 11111 */
        leftBlock = 31;
    }

    /* allocate memory for code, each block gets a char + terminator char */
    code = malloc(sizeof (char) * numOfBlocks + 1);
    CHECK_MALLOC(code);

    for(i = numOfBlocks; i > 0; --i) {
        /* go through all the blocks, left to right */
        if(i == numOfBlocks)
            /* if it's the most left block, use mask and get the first char */
            sprintf(code, "%c", Base32[(binary >> ((i-1)*5)) & leftBlock]);
        else {
            /* and then get all the rest of the chars */
            sprintf(tmp, "%c",Base32[(binary >> ((i-1)*5)) & 31]);
            /* and concatenate them to a string */
            strcat(code, tmp);
    	}
    }
    /* and return the code */
    return code;
}

bool isInstruction(char *word) {
    int i;
    for(i=0; i < INSTRUCTIONS_NUMBER;i++){
        /* iterate through instructions/operations at opTable and compare string */
        if(!strcmp(word, opTable[i].opName))
            return TRUE;
    }
    return FALSE;
}

bool isSavedWord(char *word){
    int i;
    /* is the word a register name? */
    for(i=0; i < REGISTERS_NUMBER ;i++){
        if(!strcmp(word, RegNames[i]))
            return TRUE;
    }
    /* is is an instruction? */
    if(isInstruction(word))
        return TRUE;
    /* is it a guidance word? */
    if (!strcmp(word,".data") || !strcmp(word,".string") || !strcmp(word,".struct") || !strcmp(word,".entry") || !strcmp(word,".extern"))
        return TRUE;
    return FALSE;
}

bool isInteger(double val) {
    /* cast */
    int temp = (int) val;
    /* true if casted and original values are equal */
    return (val == temp);
}

int numToCode(int num) {
    if (num < 0) {
        /* if the number is negative, get the absolute value */
        num = -num;
        /* reverse the bits, and add 1 to the LSB */
        num = ((~num) + 1);
    }
    return num;
}