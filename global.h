#ifndef ASSEMBLER_GLOBAL_H
#define ASSEMBLER_GLOBAL_H

/* Max words in a line */
#define MAX_WORDS 50
/* Max chars in a line */
#define MAX_LINE 81
/* Max chars for a label */
#define MAX_LABEL 31
/* Memory words start address */
#define START_ADDRESS 100
/* Memory words end address */
#define END_ADDRESS 255
/* Number of bits in a memory word -- 256 and larger will cause problem because the size of an int is 64 bits * 4 = 256 */
#define NUMBER_OF_BITS 10
/* Number of instructions */
#define INSTRUCTIONS_NUMBER 16
/* Number of registers */
#define REGISTERS_NUMBER 8
/* Max number allowed for operand of instruction */
#define MAX_NUMBER_INS 127
/* Min number allowed for operand of instruction */
#define MIN_NUMBER_INS -128
/* Max number allowed for guidance */
#define MAX_NUMBER_GUI 511
/* Min number allowed for guidance */
#define MIN_NUMBER_GUI -512

#define OPEN_STREAM(file,fname,size,original,extension,mode) \
    FILE *file;\
    /* allocated memory and copy file name */\
    char *fname = malloc(sizeof (char) * size);\
    CHECK_MALLOC(fname)\
    strcpy(fname,original);\
    /* open stream */\
    file = fopen(strcat(fname,extension),mode);


/* Returns error if memory wasn't allocated successfully */
#define CHECK_MALLOC(pointer)\
    if(pointer == NULL) {\
        printf("ERROR: Unable to allocate memory\n");\
        IS_ERROR = TRUE;\
        exit(TRUE);\
    }

/* CHECK_FILE_RETURN check the validity of the file, CHECK_AMT_OF_FILES check that files were entered */

#define CHECK_FILE_RETURN(file,fileName)\
    if(!file) {\
        printf("ERROR: Unable to read file %s\n", fileName);\
        return;\
    }

#define CHECK_AMT_OF_FILES(num)\
    if(num <= 1) {\
        printf("ERROR: No file name entered\n");\
        return 0;\
    }


/* Print to object file, set temp vars, print their content and free them */
/* if we don't do that, for each call of the function we will lose the pointer to the memory allocated
 * and therefore, will lose bytes in the memory */
#define PRINT_TO_OBJECT_FILE(val1, val2, tmp1, tmp2, objFile)\
	    tmp = val1;\
        tmp2 = val2;\
        fprintf(objFile, "%s\t%s\n", tmp, tmp2);\
        free(tmp);\
        free(tmp2);

/* booleans */
typedef enum booleans {
    FALSE = 0, TRUE = 1
} bool;

/* number of operands */
typedef enum operands {
    ONE=1, TWO=2, NONE=0
} Operands;

/* operation at opTable */
typedef struct operations {
    /* its name */
    char *opName;
    /* its code */
    int opcode;
    /* allowed operands number */
    Operands allowedOperands;
    /* allowed methods for source */
    int sourceMethods;
    /* allowed methods for destination */
    int destMethods;
} Operations;

/* Macro node */
struct Macro {
    /* its name */
    char *macroName;
    /* its content */
    char *macroContent;
    /* pointer to next node */
    struct Macro *next;
};

/* label */
typedef struct label {
    /* its name */
    char *labelName;
    /* word number @ memory */
    int labelAddress;
    /* flags */
    bool isGuidance;
    bool isInstruction;
    bool isAlsoEntry;
    bool isExternal;
} Label;

typedef struct instruction {
    /* line number */
    int lineNum;
    /* first word:
     * op - opcode
     * ts - source method number
     * td - destination method number
     * are - A.R.E
     */
    int op;
    int ts;
    int td;
    int are;
    /* LSource - memory words needed for source */
    int LSource;
    /* LDest - memory words needed for destination */
    int LDest;
    /* flags */
    bool isSetSource;
    bool isSetDest;
    /* binary code for source */
    int *SCode;
    /* binary code for destination */
    int *DCode;
    /* instruction binary code */
    int ICode;
    /* total words for the instruction */
    int L;
    /* source operand */
    char *OPSource;
    /* destination operand */
    char *OPDest;
} Instruction;

typedef struct guidance {
    /* flags */
	bool isEntry;
	bool isExternal;
	bool isString;
	bool isData;
	bool isStruct;
    /* its label */
    char *internalLabel;
    /* string if string */
    char *string;
    /* data if data */
    int *datalist;
    int dataArraySize;
    /* label address */
    int internalLabelAddress;
} Guidance;

typedef struct memoryWords {
    /* pointers to data */
    Instruction *ins;
    Guidance *gui;
    /* flags */
    bool isInstruction;
    bool isGuidance;
    /* address in memory */
    int address;
} MemoryWords;

/* global data holders to be used */
extern char RegNames[8][3];
extern Operations opTable[];
extern const char Base32[32];
extern bool IS_ERROR;

#endif
