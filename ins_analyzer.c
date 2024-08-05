#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "text_parser.h"
#include "ins_analyzer.h"
#include "first_pass.h"

bool isValidInsLine(int lineNum, bool containsLabel, char *opName, char **brokenLine, int numOfOperands, Label **labelTable, int labelCount) {
    int i, allowedOperands = -1, opCode = -1,operandType;
    for(i=0; i < INSTRUCTIONS_NUMBER;i++){
        /* go through all the instructions */
        if(!strcmp(opName, opTable[i].opName)) {
            /* find the instruction and get its code and allowed operands */
            opCode = i;
            allowedOperands = opTable[i].allowedOperands;
        }
    }

    if(opCode == -1) {
        /* if no instruction was found */
        printf("'%s' is not an instruction, at line %d\n", opName,lineNum);
        IS_ERROR = TRUE;
        return FALSE;
    }

    if(allowedOperands != numOfOperands) {
        /* only the correct number of allowed operands is accepted */
        printf("Incompatible number of operands for instruction '%s', at line %d\n", opName,lineNum);
        IS_ERROR = TRUE;
        return FALSE;
    }

    if(numOfOperands == 1) {
        /* only 1 operand, get its type */
        operandType = getOperandType(brokenLine[containsLabel+1],lineNum, labelTable, labelCount);
        if((operandType == 0 && opTable[opCode].destMethods != 4) && operandType != -1) {
            /* check that the type matches the allowed types for instruction's operand */
            printf("Incompatible type of operand for instruction '%s' at line %d\n", opName,lineNum);
            IS_ERROR = TRUE;
        }

    }
    if(numOfOperands == 2) {
        /* 2 operands, start with the first one, get its type */
        operandType = getOperandType(brokenLine[containsLabel+1],lineNum, labelTable, labelCount);
        if(((operandType == 0 || operandType == 4) && opTable[opCode].sourceMethods != 4) && operandType != -1) {
            /* check that the type matches the allowed types for instruction's operand */
            printf("Incompatible type of operand for instruction '%s', at line %d\n", opName,lineNum);
            IS_ERROR = TRUE;
        }
        /* get the second operand's type */
        operandType = getOperandType(brokenLine[containsLabel+2],lineNum, labelTable, labelCount);
        if((operandType == 0 && opTable[opCode].destMethods != 4) && operandType != -1) {
            /* check that the type matches the allowed types for instruction's operand */
            printf("Incompatible type of operand for instruction '%s', at line %d\n", opName,lineNum);
            IS_ERROR = TRUE;
        }
    }

    /* return false if there is an error */
    return (!IS_ERROR);
}

int getOperandType(char *operand,int lineNum, Label **labelTable, int labelCount) {
    int i = 0,dotIndex = -1;
    char *tmp;
    bool error = FALSE;
    /* starts with '#' -> direct */
    if(operand[0] == '#') {
        /* is number valid? */
        if(!isValidNum((operand + 1), "ins")) {
            /* we don't accept invalid numbers */
            printf("Incompatible format for number operand, at line %d\n", lineNum);
            IS_ERROR = TRUE;
            return -1;
        }
        /* opType 0 = direct */
        return 0;
    }

    /* starts with 'r' -> register */
    if(operand[0] == 'r') {
        i = 1;
        if(isValidNum((operand + i),"ins")) {
            if (atoi((operand + i)) >= 0 && atoi((operand + i)) <= (REGISTERS_NUMBER - 1))
                /* it is a register */
                return 3;
            else
                /* can be a label -> opType 1 */
                return 1;
        }
    }

    /* not a register or direct, we look for a dot */
    for(i = 0; i < strlen(operand); i++)
        if(operand[i] == '.')
            /* dot is found. if a dot was already found before, dotIndex = -2, else keep dot index */
            dotIndex = (dotIndex == -1) ? i : -2;

    if(dotIndex == -2) {
        /* 2 dots were found */
        printf("There is more than one dot at the operand, at line %d\n", lineNum);
        IS_ERROR = TRUE;
        return -1;
    }

    if(dotIndex > -1) {
        /* found only 1 dot */
        if(dotIndex == 0) {
            /* we don't allow dot at the beginning of an operand */
            printf("There is a dot in the beginning of the operand '%s', at line %d\n", operand, lineNum);
            IS_ERROR = TRUE;
            error = TRUE;
        }
        if(atoi((operand+dotIndex+1)) != 1 && atoi((operand+dotIndex+1)) != 2) {
            /* we allow only 1/2 after the dot in a struct operand */
            printf("After a dot in a struct operand, only 1 or 2 is allowed, at line %d\n", lineNum);
            IS_ERROR = TRUE;
            error = TRUE;
        }
        /* extract label name */
        tmp = malloc (sizeof(char) * MAX_LABEL);
        CHECK_MALLOC(tmp)
        strcpy(tmp, operand);
        tmp = strtok(tmp, ".");
        /* remove dot */
        removeChar(tmp, '.');
        /* check if valid */
        if(!isValidLabel(lineNum, tmp, *labelTable, &labelCount))
            error = TRUE;
        free(tmp);
        if(error)
            return -1;
        /* a struct operand -> opType 2 */

        return 2;
    }
    /* no dots were found, ruled out all other types and errors -> opType 1 -> label */
    if(!isValidLabel(lineNum, operand, *labelTable, &labelCount))
        return -1;
    return 1;
}

int createIns(Instruction *ins, char **brokenLine, int numOfOperands, int lineNum, Label **labelTable, int labelCount) {
    int i,opType;
    char *operand;

    /* go through all instructions and get the opCode */
    for(i=0; i < INSTRUCTIONS_NUMBER;i++)
        if(!strcmp(brokenLine[0], opTable[i].opName))
            ins->op = i;

    /* set line number */
    ins->lineNum = lineNum;
    /* reset counters */
    ins->LSource = 0;
    ins->LDest = 0;
    ins->L = 0;
    /* set flags */
    ins->isSetSource = FALSE;
    ins->isSetDest = FALSE;

    if(numOfOperands > 0) {
        /* we have at least 1 operand */
        for (i = 0; i < numOfOperands; i++) {
            /* go through all the operands */
            operand = *(brokenLine + i + 1);
            /* get its type */
            opType = getOperandType(operand, lineNum, labelTable, labelCount);
            switch (numOfOperands) {
                /* treat different situations differently */
                case 1:
                    /* only 1 operand */
                    if (opType == 2) {
                        /* it is a struct, allocate memory */
                        ins->DCode = malloc(sizeof(int) * 2);
                        CHECK_MALLOC(ins->DCode)
                        *(ins->DCode) = 0;
                        /* set 2nd word's code */
                        *(ins->DCode + 1) = generateInsStructWord(operand);
                        /* increase count by 2 */
                        ins->LDest = ins->LDest + 2;
                    }
                    else {
                        /* allocate memory */
                        ins->DCode = malloc(sizeof(int));
                        CHECK_MALLOC(ins->DCode)
                        if(opType != 1) {
                            if(opType == 0)
                                /* set direct word code */
                                *(ins->DCode) = generateInsDirWord(operand);
                            else
                                /* set new register word code */
                                *(ins->DCode) = generateInsRegWord("new", operand, ins, 2);
                            ins->isSetDest = TRUE;
                        } else {
                            /* for label -> keep word code empty */
                            *(ins->DCode) = 0;
                        }
                        ins->LDest++;
                    }
                    /* allocate memory and copy operand */
                    ins->OPDest = malloc(sizeof (char) * (strlen(operand) + 1));
                    CHECK_MALLOC(ins->OPDest)
                    ins->OPSource = NULL;
                    strcpy(ins->OPDest,operand);
                    ins->td = opType;
                    ins->ts = 0;
                    break;
                case 2:
                    /* we deal with 2 operands */
                    if (i == 0) {
                        /* first thing first, we deal with the first operand */
                        if (opType == 2) {
                            /* it is a struct, allocate memory */
                            ins->SCode = malloc(sizeof(int) * 2);;
                            CHECK_MALLOC(ins->SCode)
                            /* generate 2nd word for struct and insert that */
                            *(ins->SCode + 1) = generateInsStructWord(operand);
                            /* increase count by 2 */
                            ins->LSource = ins->LSource + 2;
                        }
                        else {
                            /* allocate memory */
                            ins->SCode = malloc(sizeof(int));
                            CHECK_MALLOC(ins->SCode)
                            if(opType == 0) {
                                *(ins->SCode) = generateInsDirWord(operand);
                                ins->isSetSource = TRUE;
                            }
                            if(opType == 3) {
                                *(ins->SCode) = generateInsRegWord("new", operand, ins, 1);
                                ins->isSetSource = TRUE;
                            }
                            ins->LSource++;
                        }
                        /* allocate memory and copy operand */
                        ins->OPSource = malloc(sizeof (char) * (strlen(operand) +1));
                        CHECK_MALLOC(ins->OPSource)
                        strcpy(ins->OPSource,operand);
                        ins->ts = opType;
                    } else {
                        /* now we deal with the 2nd operand */
                        if (opType == 2) {
                            /* it is a struct, memory allocation */
                            ins->DCode = malloc(sizeof(int) * 2);
                            CHECK_MALLOC(ins->DCode)
                            /* generate struct's 2nd word */
                            *(ins->DCode + 1) = generateInsStructWord(operand);
                            ins->LDest = ins->LDest + 2;
                        }
                        else {
                            /* memory allocation */
                            ins->DCode = malloc(sizeof(int));
                            CHECK_MALLOC(ins->DCode)
                            if(opType == 0) {
                                /* it is direct, generate the direct word */
                                *(ins->DCode) = generateInsDirWord(operand);
                                ins->isSetDest = TRUE;
                            }
                            if(opType == 3) {
                                /* it is a register */
                                if(ins->ts == 3) {
                                    /* the first operand was also a register, modify the word to add the 2nd operand */
                                    *(ins->SCode) = generateInsRegWord("mod", operand, ins, 2);
					                ins->LDest--;
                                } else {
                                    /* first operand wasn't register, create new word for it */
                                    *(ins->DCode) = generateInsRegWord("new", operand, ins, 2);
                                    ins->isSetDest = TRUE;
                                }
                            }
                            ins->LDest++;
                        }
                        /* copy operand */
                        ins->OPDest = malloc(sizeof (char) * (strlen(operand) + 1));
                        CHECK_MALLOC(ins->OPDest)
                        strcpy(ins->OPDest,operand);
                        ins->td = opType;
                    }
                    break;
            }
        }
    } else {
        /* no operands, no methods */
        ins->OPSource = NULL;
        ins->OPDest = NULL;
        ins->ts = 0;
        ins->td = 0;
    }
    /* are for ins is always 0 */
    ins->are = 0;
    /* generate first word */
    ins->ICode = generateInsFirstWord(ins);
    /* aggregate the count */
    ins->L = ins->LSource + ins->LDest + 1;
    /* return sum of count -> num of words for instruction in the memory */
    return ins->L;
}

int generateInsFirstWord(Instruction *ins) {
    /* first instruction word is from the form of:
     * op-ts-td-are
     * xxxx-xx-xx-00 */
    int code,opCode,td, ts;
    opCode = ins->op << 6;
    ts = ins->ts << 4;
    td = ins->td << 2;
    code = opCode | ts | td | ins->are;
    return code;
}

int generateInsDirWord(char *operand) {
    int num;

    /* convert to code */
    num = numToCode(atoi((operand + 1)));

    /* 2 LSB are 0 -> A.R.E = 00 */
    num = num << 2;

    return num;
}

int generateInsRegWord(char *action, char *operand, Instruction *ins, int opNum) {
    int num;
    /* convert string to integer */
    num = atoi((operand + 1));
    /* 2 LSB are 0 */
    num = num << 2;
	if(opNum == 1)
        /* only 1 operand -> make this a source one */
		num = num << 4;
    if(!strcmp(action, "mod")) {
        /* we are modifying an existing code */
        num = (*(ins->SCode)) | num;
        ins->isSetDest = TRUE;
    }
    return num;
}

int generateInsStructWord(char *operand) {
    int i,num;
    for(i = 0; operand[i+1] != '\n' && operand[i+1] != EOF && operand[i+1] != '\0' && operand[i+1] != '\r'; i++)
        /* find the dot in the operand */
        if(operand[i] == '.')
            /* convert the number after the dot to int */
            num = atoi((operand + i + 1));
    /* 2 LSB are 0 */
    num = num << 2;

    return num;
}

void insertInsToRec(MemoryWords *memoryRecord,int *memoryRecCount,Instruction *ins,int IC) {
    /* check that we didn't go over the memory limit */
    if(IC + START_ADDRESS > END_ADDRESS) {
        printf("Error at inserting guidance to memory record, no more space left in the memory\n");
        IS_ERROR = TRUE;
    } else {
	    /* insert instruction */
	    memoryRecord->ins = ins;
	    /* set flags */
	    memoryRecord->isInstruction = TRUE;
	    memoryRecord->isGuidance = FALSE;
	    /* set address */
	    (memoryRecord)->address = IC + START_ADDRESS;
	    /* increase counter */
	    ++(*memoryRecCount);
    }
}
