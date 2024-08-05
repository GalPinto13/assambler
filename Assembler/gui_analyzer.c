#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "text_parser.h"
#include "gui_analyzer.h"


bool isGuidanceDataStorage(char *word) {
    /* string comparison */
    return (!strcmp(word,".data") || !strcmp(word,".struct") || !strcmp(word,".string"));
}

int createGui(Guidance *gui,char **brokenLine,int numOfWords) {
    int i;
    /* is it a data guidance */
    if(!strcmp(brokenLine[0],".data") && numOfWords >= 2) {
        /* set data guidance flags */
        gui->isData = TRUE;
        gui->isStruct = FALSE;
        gui->isString = FALSE;

        /* allocate memory for data storage */
        gui->datalist = malloc(sizeof (int) * numOfWords - 1);
        CHECK_MALLOC(gui->datalist)

        for(i = 0;i < numOfWords - 1; i++)
            /* store data */
            *(gui->datalist + i) = numToCode(atoi(brokenLine[i+1]));

        /* store its size */
        gui->dataArraySize = numOfWords - 1;
        /* return size */
        return gui->dataArraySize;
    }

    /* is it a string guidance */
    else if (!strcmp(brokenLine[0],".string")) {
        /* remove quotation */
        removeChar(brokenLine[1],'"');
        /* set string guidance flags */
        gui->isData = FALSE;
        gui->isStruct = FALSE;
        gui->isString = TRUE;

        /* memory allocation for string */
        gui->string = malloc(sizeof (char) * strlen(brokenLine[1]) + 1);
        CHECK_MALLOC(gui->string)

        /* copy string */
        strcpy(gui->string,brokenLine[1]);

        /* return size */
        return strlen(gui->string) + 1;
    }

    /* is it a struct guidance */
    else if(!strcmp(brokenLine[0],".struct")) {
        /* set struct guidance flags */
        gui->isData = FALSE;
        gui->isStruct = TRUE;
        gui->isString = FALSE;

        /* memory allocation for data part of struct */
        gui->datalist = malloc(sizeof (int));
        CHECK_MALLOC(gui->datalist)
        /* insert data part to guidance */
        *(gui->datalist) = numToCode(atoi(brokenLine[1]));

        /* remove quotation for string part of struct */
        removeChar(brokenLine[2],'"');
        /* memory allocation for string part of struct */
        gui->string = malloc(sizeof (char) * strlen(brokenLine[2]) + 1);
        CHECK_MALLOC(gui->string)
        /* copy string to guidance */
        strcpy(gui->string,brokenLine[2]);

        /* return size of guidance */
        return strlen(gui->string) + 2;
    }
    /* if no guidance detected, the size is 0 */
    return 0;
}

bool isValidGuiLine(int lineNum,char **brokenLine, int numOfWords) {
    /* set flag */
    bool validLine = FALSE;

    /* if it is a data guidance */
    if(!strcmp(brokenLine[0], ".data")) {
        /* validate data guidance line */
        validLine = isValidGuiData(&brokenLine[1], numOfWords - 1);
        if(!validLine) {
            printf("Invalid data guidance, at line %d\n",lineNum);
            IS_ERROR = TRUE;
        }
    }
    /* if it is a string guidance */
    else if(!strcmp(brokenLine[0],".string")) {
        /* validate string guidance line */
        validLine = isValidGuiString(&brokenLine[1], numOfWords - 1);
        if(!validLine) {
            printf("Invalid string guidance, at line %d\n",lineNum);
            IS_ERROR = TRUE;
        }
    }
    /* if it is a struct guidance */
    else if(!strcmp(brokenLine[0],".struct")) {
        /* validate struct guidance line */
        validLine = isValidGuiStruct(&brokenLine[1],numOfWords -1);
        if(!validLine) {
            printf("Invalid struct guidance, at line %d\n",lineNum);
            IS_ERROR = TRUE;
        }
    }
    /* return answer */
    return validLine;
}

bool isValidGuiStruct(char **brokenLine, int numOfWords) {
    /* 2 words, a number, and a valid string */
    return (numOfWords == 2) && isValidNum(brokenLine[0], "gui") && isValidString(brokenLine[1]);
}

bool isValidGuiString(char **brokenLine, int numOfWords) {
    /* 1 word, a valid string */
    return (numOfWords == 1) && isValidString(brokenLine[0]);
}

bool isValidGuiData(char **brokenLine, int numOfWords) {
    int i;
    for (i = 0; i < numOfWords; i++)
        /* iterate through all numbers */
        if(!isValidNum(brokenLine[i],"gui"))
            /* if any invalid, return false */
            return FALSE;
    return TRUE;
}

void insertGuiToRec(MemoryWords *memoryRecord,int *memoryRecCount,Guidance *gui,int DC) {
    /* check that we didn't go over the memory limit */
    if(DC + START_ADDRESS > END_ADDRESS) {
        printf("Error at inserting guidance to memory record, no more space left in the memory\n");
        IS_ERROR = TRUE;
    } else {
	    /* insert guidance */
	    memoryRecord->gui = gui;
	    /* set flags */
	    memoryRecord->isGuidance = TRUE;
	    memoryRecord->isInstruction = FALSE;
	    /* set address */
	    (memoryRecord)->address = DC + START_ADDRESS;
	    /* increase counter */
	    ++(*memoryRecCount);
    }
}

void updateDataLabels(Label **labelTable, int symCounter, int IC) {
    int i;
    for(i = 0; i < symCounter; i++)
        /* go through all the labels */
        if((*labelTable + i)->isGuidance)
            /* if it is a guidance label, set new address */
            (*labelTable + i)->labelAddress += IC;
}

void updateDataRecords(MemoryWords **memoryRecords,int memoryRecCount,int IC) {
    int i;
    for(i = 0; i < memoryRecCount; i++)
        /* go through all the memory records */
        if((*memoryRecords + i)->isGuidance)
            /* if it is a guidance record, set new address */
            (*memoryRecords + i)->address += IC;
}
