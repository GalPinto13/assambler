#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "first_pass.h"
#include "free_memory.h"
#include "text_parser.h"
#include "gui_analyzer.h"
#include "ins_analyzer.h"

/* First pass */
void firstPass(char *fname, MemoryWords **memoryRecords, int *memoryRecCount, Label **labelTable, int *labelCount, int *IC, int *DC, int *externExist) {
    bool validLabel = FALSE,containsLabel, isInst, isGui;
    char line[MAX_LINE + 2];
    char **brokenLine = NULL;
    int lineNum = 1,wordNum;
    FILE *fsource;
    /* open read stream */
    fsource = fopen(fname,"r");
    /* reset counters */
    *IC = 0;
    *DC = 0;
    /* reading from the source file line by line until the end */
    while (fgets(line,MAX_LINE + 2,fsource)!= NULL) {
        /* if the line is too long, read until the end of it */
        if (longLine(line, lineNum))
            while (fgets(line,MAX_LINE + 2,fsource)!= NULL && strlen(line) > MAX_LINE);
          
    
        /* the line  length is valid perform process the line */
        else {
            /* allocating memory for broken line */
            brokenLine = malloc(MAX_WORDS * sizeof(char *));
            CHECK_MALLOC(brokenLine)
            /* break line and store number of words */
            wordNum = breakLine(line, brokenLine, lineNum);
            if (wordNum > 0) {
                /* Checking if the first word is a label and if it's a valid one, and it's not been declared yet */
                containsLabel = (wordNum > 0) ? isLabel(brokenLine[0]) : FALSE;
                if (containsLabel) {
                    if(!isAlreadyLabel(brokenLine[0], *labelTable, *labelCount))
                        validLabel = isValidLabel(lineNum, brokenLine[0], *labelTable, labelCount);
                    else {
                        printf("Label '%s' is already declared, at line %d\n", brokenLine[0], lineNum);
                        IS_ERROR = TRUE;
                    }
                }

                if ((isGui = isGuidanceDataStorage(brokenLine[containsLabel]))) {
                    /* First word is a guidance word */
                    if (isValidGuiLine(lineNum, brokenLine + containsLabel, wordNum - containsLabel)) {
                        /* It is a Valid Data Guidance Line */
                        if (containsLabel && validLabel) {
                            /* if there is a label, and it is valid, insert is to the table */
                            insertLabel(brokenLine[0], *DC, *IC, labelTable, labelCount, isGui, FALSE);
                        }
                        if (!containsLabel || validLabel) {
                            /* Add Valid Guidance line to the memory only if its Label is valid or doesn't have a Label at all*/
                            addMemoryWords(lineNum, wordNum - containsLabel, brokenLine, containsLabel, DC, IC,
                                           memoryRecords,
                                           memoryRecCount, isGui, FALSE, labelTable, *labelCount);
                        }
                    }
                }
                else if (!strcmp(brokenLine[containsLabel], ".extern") ||
                           !strcmp(brokenLine[containsLabel], ".entry")) {
                    /* it is an .entry or .extern label line */
                    if (wordNum - containsLabel != 2 || isSavedWord(brokenLine[containsLabel + 1]) ||
                        brokenLine[containsLabel + 1] == NULL) {
                        /* validation of the .extern/.entry line failed */
                        IS_ERROR = TRUE;
                        printf("Invalid .extern / .entry guidance appeared, at line %d\n", lineNum);
                    }
                    else {
                        /* passed first validation */
                        if (!strcmp(brokenLine[containsLabel], ".extern")) {
                            /*validate the extern label isn't already a label */
                            if (isAlreadyLabel(brokenLine[containsLabel +1],*labelTable,*labelCount)) {
                                printf("The extern label %s is already defined as label in this file, at line %d\n", brokenLine[containsLabel +1], lineNum);
                                /* is error? */
                            }
                            else {
                                /* it is an .extern, update flag and label table */
                                *externExist = 1;
                                /* insert label */
                                insertLabel(brokenLine[containsLabel + 1], 0, 0, labelTable, labelCount, FALSE, FALSE);
                            }
                        }
                    }
                }
                else if ((isInst = isInstruction(brokenLine[containsLabel]))) {
                    /* the line is an instruction */
                    if (isValidInsLine(lineNum, containsLabel, brokenLine[containsLabel], brokenLine,
                                       wordNum - containsLabel - isInst, labelTable, *labelCount)) {
                        /* It is a Valid instruction line */
                        if (containsLabel && validLabel)
                            /* if there is a label, insert it to the table */
                            insertLabel(brokenLine[0], *DC, *IC, labelTable, labelCount, FALSE, isInst);
                        if (!containsLabel || validLabel)
                            /* if label is valid or doesn't exist, add instruction to the memory */
                            addMemoryWords(lineNum, wordNum, brokenLine, containsLabel,
                                           DC, IC, memoryRecords, memoryRecCount, FALSE, isInst, labelTable, *labelCount);
                    }
                } else {
                    /* there are words in the line but first word is not a valid or not valid command */
                    if (!isInstruction(brokenLine[containsLabel]) && !isGuidanceDataStorage(brokenLine[containsLabel]))
                        printf("Word '%s' is not a valid command, at line %d\n",brokenLine[containsLabel], lineNum);
                    IS_ERROR = TRUE;
                }
            }
        }
        /* increase line counter only if wordNum Greater or Equal to Zero*/
            lineNum++;
        /* free last line and reset flag */
        isInst = FALSE;
        isGui = FALSE;
       containsLabel = FALSE;
        /* Free brokenLine in case words were found */
        if (wordNum > 0)
            freeBrokenLine(brokenLine, wordNum, containsLabel);

        wordNum = 0;
    }
    /* close stream */
    fclose(fsource);

    /* update the addresses of the data records and data labels */
    updateDataRecords(memoryRecords, *memoryRecCount,*IC);
    updateDataLabels(labelTable,*labelCount,*IC);
}

void addMemoryWords(int lineNum, int numOfWords,char **brokenLine,bool containsLabel,int *DC,int *IC,
                    MemoryWords **memoryRecords, int *memoryRecCount, bool isGui,bool isInst, Label **labelTable, int labelCount) {
    int L;

    /* allocate memory for objects instruction/guidance */
    Guidance *gui = malloc(sizeof(Guidance));
    Instruction *ins = malloc(sizeof(Instruction));
    CHECK_MALLOC(gui)
    CHECK_MALLOC(ins)

    /* if it is the first memory record, allocate memory for it */
    if(*memoryRecCount == 0)
        *memoryRecords = malloc(sizeof (MemoryWords));
    else
        /* or re allocate memory if already allocated */
        *memoryRecords = realloc(*memoryRecords,sizeof (MemoryWords) * ((*memoryRecCount) + 1));
    CHECK_MALLOC(memoryRecords)

    /* Skip first word in case it's a label */
    brokenLine = (brokenLine + containsLabel);

    if(isGui) {
        /* it is a valid guidance line -> create gui and insert it to records */
        L = createGui(gui,brokenLine,numOfWords);
        insertGuiToRec((*memoryRecords+ *memoryRecCount),memoryRecCount,gui,*DC);
        *DC += L;
        /* Instruction pointer not was not in use, freeing */
        free(ins);
    }
    else if(isInst) {
        /* it is a valid instruction line -> create ins and insert it to records */
        L = createIns(ins, brokenLine, numOfWords - containsLabel - isInst, lineNum, labelTable, labelCount);
        insertInsToRec((*memoryRecords + *memoryRecCount), memoryRecCount, ins, *IC);
        *IC += L;
        /* Guidance pointer not was not in use, freeing */
        free(gui);
    }

}

bool isValidLabel(int lineNum,char *word, Label *labelTable, int *labelCount) {
    /* remove the colon from the word */
    removeChar(word,':');
    if(notAlNum(word) || isSavedWord(word) || isdigit(word[0])) {
        /* we do not allow:
         * 1. chars that aren't alphanumeric
         * 2. usage of saved words
         */
        printf("Usage of invalid label '%s' appeared, at line %d\n", word, lineNum);
        IS_ERROR = TRUE;
        return FALSE;
    }
    return TRUE;
}

int isAlreadyLabel(char *word, Label *labelTable, int labelCount) {
    int i,ans = 0;
    char *tmp = malloc(sizeof(char) * MAX_LABEL);
    CHECK_MALLOC(tmp)
    strcpy(tmp, word);
    removeChar(tmp, ':');
    for(i = 0; i < labelCount; i++)
        /* go through all the labels */
        if (!strcmp(tmp, (labelTable + i)->labelName))
            /* if a label was found, return its row number at the labels table */
            ans = i + 1;
    free(tmp);
    return ans;
}


void insertLabel(char *label,int DC, int IC, Label **labelTable, int *labelCount, bool isGui,bool isInst) {
    /* allocate or reallocate memory, depends on the counter */
    if(*labelCount == 0)
        *labelTable = malloc(sizeof (Label));
    else
        *labelTable = (Label*)realloc(*labelTable, sizeof(Label) * ((*labelCount) + 1));
    CHECK_MALLOC(labelTable)

    /* allocate memory for the label name */
    (*labelTable + *labelCount)->labelName = malloc(sizeof (char) * MAX_LABEL);
    CHECK_MALLOC((*labelTable + *labelCount)->labelName)

    /* copy the name */
    strcpy((*labelTable + *labelCount)->labelName,label);
    if (isGui) {
        /* it is a guidance, set flags and address */
        (*labelTable + *labelCount)->isInstruction = FALSE;
        (*labelTable + *labelCount)->isGuidance = TRUE;
        (*labelTable + *labelCount)->isExternal = FALSE;
        (*labelTable + *labelCount)->isAlsoEntry = FALSE;
        (*labelTable + *labelCount)->labelAddress = DC + START_ADDRESS;
    }
    else if (isInst) {
        /* it is an instruction, set flags and address */
        (*labelTable + *labelCount)->isGuidance = FALSE;
        (*labelTable + *labelCount)->isInstruction = TRUE;
        (*labelTable + *labelCount)->isExternal = FALSE;
        (*labelTable + *labelCount)->isAlsoEntry = FALSE;
        (*labelTable + *labelCount)->labelAddress = IC + START_ADDRESS;
    }
    else {
        /* it isn't an instruction or a guidance -> it is external */
        (*labelTable + *labelCount)->isGuidance = FALSE;
        (*labelTable + *labelCount)->isInstruction = FALSE;
        (*labelTable + *labelCount)->isExternal = TRUE;
        (*labelTable + *labelCount)->isAlsoEntry = FALSE;
        (*labelTable + *labelCount)->labelAddress = 0;
    }
    /* increase counter */
    ++(*labelCount);
}
