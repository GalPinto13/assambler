#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include "free_memory.h"

void freeBrokenLine(char **brokenLine, int wordNum, bool containsLabel) {
    int i = 0;
    /* if we skipped the label word, bring it back */
    brokenLine = (brokenLine - containsLabel);

    while(i < wordNum) {
        /* free all words in the line */
        free(*(brokenLine + i));
        i++;
    }
	free(brokenLine);
}

void freeLabelTable(Label *labelTable, int labelNum) {
    int i = 0;
    while(i < labelNum) {
        /* free all label names in the table */
        free((labelTable + i)->labelName);
        i++;
    }
}

void freeMemRec(MemoryWords *memoryRecords, int memoryRecCount) {
    int i = 0;
    while(i < memoryRecCount) {
        /* if the record is an instruction, use freeInstruction */
        if((memoryRecords+i)->isInstruction)
            freeInstruction((memoryRecords+i)->ins);
        /* if the record is a guidance, use freeGuidance */
        if((memoryRecords+i)->isGuidance)
            freeGuidance((memoryRecords+i)->gui);
        i++;
    }
}

void freeInstruction(Instruction *ins) {
    /* if a source operand is set, free it */
    if(ins->OPSource != NULL)
        free(ins->OPSource);
    /* if a source code is set, free it */
    if(ins->LSource > 0)
        free(ins->SCode);
    /* if a destination operand is set, free it */
    if(ins->OPDest != NULL)
        free(ins->OPDest);
    /* if a destination code is set, free it */
    if(ins->LDest > 0)
        free(ins->DCode);
    /* free pointer */
    free(ins);
}

void freeGuidance(Guidance *gui) {
    /* if it is a data guidance, free data */
    if(gui->isData)
        free(gui->datalist);
    /* if it is a string guidance, free string */
    if(gui->isString)
        free(gui->string);
    /* if it is a struct guidance, free data and string */
    if(gui->isStruct) {
        free(gui->datalist);
        free(gui->string);
    }
    /* free pointer */
    free(gui);
}

void freeMacro(struct Macro *head) {
    struct Macro *tmp;
    while(head != NULL) {
        /* go through the list */
        tmp = head;
        /* free everything */
        free(tmp->macroContent);
        free(tmp->macroName);
        /* move to next */
        head = head->next;
        /* free pointer */
        free(tmp);
    }
}
