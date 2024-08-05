#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "first_pass.h"
#include "second_pass.h"
#include "text_parser.h"
#include "file_writer.h"
#include "free_memory.h"
#include "gui_analyzer.h"
#include "ins_analyzer.h"


/* Second pass */
void secondPass(char *fname , Label **labelTable, int *labelCount, MemoryWords **memoryRecords, int *memoryRecCount, int *entryExist) {
    bool containsLabel;
    int lineNum = 0,wordNum = 0, entryPos;
    char line[MAX_LINE + 2];
    char **brokenLine = NULL;
    FILE *fsource;
    /* open file stream */
    fsource = fopen(fname,"r");
    /* read from the file line by line */
    while (fgets(line,MAX_LINE + 2,fsource)!= NULL) {
        /* increase line count by 1 */
        lineNum++;
        /* allocate memory for broken line */
        brokenLine = malloc(MAX_WORDS * sizeof(char*));
        CHECK_MALLOC(brokenLine)
        /* break line and get number of words */
        wordNum = breakLine(line,brokenLine,lineNum);
        /* if the line isn't empty */
        if(wordNum > 0) {
            /*If the first word is a label we skip it*/
            removeChar(brokenLine[0], ':');
            containsLabel = (isAlreadyLabel(brokenLine[0],*labelTable,*labelCount) > 0) ? 1 : 0;
            brokenLine = (brokenLine + containsLabel);

            if(!strcmp(brokenLine[0],".entry")) {
                /* if it's an .entry line -> check if the 2nd word is already a label */
                entryPos = isAlreadyLabel(brokenLine[1], *labelTable, *labelCount);
                /* if it is a label, its index on the label table is (entryPos - 1) because isAlreadyLabel returns number of row */
                entryPos = (entryPos) ? entryPos - 1 : -1;
                if (entryPos < 0) {
                    /* it is not a label already */
                    IS_ERROR = TRUE;
                    printf("The label '%s' of the .entry guidance doesn't exist in the source file, at line %d\n",brokenLine[1],lineNum);
                }
                else if ((*labelTable + entryPos)->isExternal) {
                    /* it is a label, we do not allow external labels to be in a .entry line */
                    IS_ERROR = TRUE;
                    printf("The label '%s' of the .entry guidance is also defined as an external label, at line %d\n",brokenLine[1],lineNum);
                }
                else {
                    /* it is a label and not external, set flags */
                    *entryExist = 1;
                    (*labelTable + entryPos)->isAlsoEntry = TRUE;
                }
            }

        }

	    /* free last line */
	if(wordNum > 0) {
        freeBrokenLine(brokenLine, wordNum, containsLabel);
    }

        /* reset flag */
        containsLabel = 0;
    }
	fclose(fsource);
    /* sorting the records list - instructions records first, and then guidance records */
    qsort(*memoryRecords, *memoryRecCount, sizeof(MemoryWords), compareAddress);
    /* complete all the instruction records, where needed */
    completeInstructionRecords(memoryRecords,memoryRecCount,labelTable,labelCount);
}

bool isLabelExternal(char *label, Label **labelTable, int labelCounter) {
    int i;
    bool isExternal;
    for(i = 0; i < labelCounter; i++)
        /* go through all the labels */
        if(!strcmp(label,(*labelTable + i)->labelName))
            /* find the label we look for and check if it is external */
            isExternal = (*labelTable + i)->isExternal;
    /* return answer */
    return isExternal;
}

int getLabelAddress(Label **labelTable, int numOfLabel) {
    /* get the label */
    Label label = *(*labelTable + numOfLabel - 1);
    /* return its address */
    return label.labelAddress;
}

void completeInstructionRecords(MemoryWords **memoryRecords,int *memoryRecCount,Label **labelTable,int *labelCount) {
    int i;
    MemoryWords curRecord;

    for (i = 0; i < *memoryRecCount; i++) {
        /* go through all the sorted records, only the instructions. store current record */
        curRecord = *(*memoryRecords + i);
        if(curRecord.isInstruction) {
            if (!(curRecord.ins->isSetSource) && curRecord.ins->LSource > 0)
                /* if we have source operand words, build their code */
                buildCode(&curRecord, "source", i, labelTable, labelCount);
            if (!(curRecord.ins->isSetDest) && curRecord.ins->LDest > 0)
                /* if we have destination operand words, build their code */
                buildCode(&curRecord, "dest", i, labelTable, labelCount);
        }
    }
}

void buildCode(MemoryWords *curRecord, char *type,int i, Label **labelTable, int *labelCount) {
    char *operand;
    int opType,numOfLabel,address,are;
    const char *delimeter = ".";
    /* action == 0 - source, action == -1 - dest */
    int action = (strcmp(type,"source"));
    /* get operand */
    if(action)
	    operand = curRecord->ins->OPDest;
    else
        operand = curRecord->ins->OPSource;

    /* get its type */
    opType = getOperandType(operand, i + 1, labelTable, *labelCount);
    if (opType == 1) {
        /* it is a label */
        if ((numOfLabel = isAlreadyLabel(operand, *labelTable, *labelCount))) {
            /* get its row number in label table, determine A.R.E value by checking if label is external */
            are = (isLabelExternal(operand, labelTable, *labelCount)) ? 1 : 2;
            /* if label is internal set A.R.E to 0, else(external) get its address from the table */
            address = (are == 1) ? 0 : getLabelAddress(labelTable, numOfLabel);
            /* build source code and set flag */
            if(action) {
                *(curRecord->ins->DCode) = (address << 2) | are;
                curRecord->ins->isSetDest = TRUE;
            } else {
                *(curRecord->ins->SCode) = (address << 2) | are;
                curRecord->ins->isSetSource = TRUE;
            }
        } else {
            /* no label was found, print error */
            printf("Label '%s' wasn't found, at line %d\n", operand, curRecord->ins->lineNum);
            IS_ERROR = TRUE;
        }
    }
    if (opType == 2) {
        /* it is a struct, extract label name from operand */
        operand = strtok(operand, delimeter);


        if ((numOfLabel = isAlreadyLabel(operand, *labelTable, *labelCount))) {
            /* get its row number in label table, determine A.R.E value by checking if label is external */
            are = (isLabelExternal(operand, labelTable, *labelCount)) ? 1 : 2;
            /* if it is internal, get its address from the table, else(external) set address to 0 */
            address = (are == 1) ? 0 : getLabelAddress(labelTable, numOfLabel);
            /* build source code and set flag */
            if(action) {
                *curRecord->ins->DCode = (address << 2) | are;
                curRecord->ins->isSetDest = TRUE;
            } else {
                *curRecord->ins->SCode = (address << 2) | are;
                curRecord->ins->isSetSource = TRUE;
            }
        } else {
            /* no label was found, print error */
            printf("Label '%s' wasn't found at line %d\n", operand, curRecord->ins->lineNum);
            IS_ERROR = TRUE;
        }
    }
}
