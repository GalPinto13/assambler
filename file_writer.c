#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "file_writer.h"

void writeObjectFile(char *fileName, MemoryWords **memoryRecords, int memoryRecCount, int IC, int DC) {
    int i = 0,j;
	char *tmp;
	char *tmp2;
    /* open file stream */
    OPEN_STREAM(objFile,objName, strlen(fileName)+4, fileName, ".ob", "w")
	tmp = binaryToBase32(IC);
	tmp2= binaryToBase32(DC);
    /* print to file instruction count and data count @ base32 */
    fprintf(objFile, "%s \t %s\n", tmp+1, tmp2+1);
    free(tmp);
    free(tmp2);
    while (i < memoryRecCount && (*memoryRecords + i)->isInstruction){
        /* go through all instructions */
        /* print the address and code, converted to base32 */
	    PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address), binaryToBase32((*memoryRecords + i)->ins->ICode), tmp, tmp2, objFile);

        for (j = 0;j < (*memoryRecords + i)->ins->LSource ;j++) {
            /* go through all source words if exists, print the address and code, converted to base32 */
	        PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address  +1 + j), binaryToBase32((*memoryRecords + i)->ins->SCode[j]), tmp, tmp2, objFile);
        }

        for (j = 0;j < (*memoryRecords + i)->ins->LDest;j++) {
            /* go through all destination words if exists, print the address and code, converted to base32 */
            PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address  + 1 + j + (*memoryRecords + i)->ins->LSource), binaryToBase32((*memoryRecords + i)->ins->DCode[j]), tmp, tmp2, objFile);
        }
        i++;
    }
    while (i < memoryRecCount){
        /* go through all the rest of the records */
        if((*memoryRecords + i)->gui->isData) {
            /* if it is a data guidance */
            for (j = 0; j < (*memoryRecords + i)->gui->dataArraySize; j++) {
                /* go through all data list if exists, print the address and code, converted to base32 */
	            PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address + j), binaryToBase32((*memoryRecords + i)->gui->datalist[j]), tmp, tmp2, objFile);

            }
        }
        else if ((*memoryRecords + i)->gui->isString) {
            /* if it is a string guidance */
            for (j = 0; j <= strlen((*memoryRecords + i)->gui->string); j++) {
                /* print the address and code of string, char by char, converted to base32 */
	            PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address + j), binaryToBase32((int)(*memoryRecords + i)->gui->string[j]), tmp, tmp2, objFile);
            }
        }
        else if ((*memoryRecords + i)->gui->isStruct){
            /* if it is a struct guidance */
            /* print the address and code of the int value stored in the struct, converted to base32 */
	            PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address), binaryToBase32((*memoryRecords + i)->gui->datalist[0]), tmp, tmp2, objFile);

            for (j = 0; j <= strlen((*memoryRecords + i)->gui->string); j++) {
                /* print the address and code of the string value stored in the struct, converted to base32 */
	            PRINT_TO_OBJECT_FILE(binaryToBase32((*memoryRecords + i)->address + j + 1), binaryToBase32((int)(*memoryRecords + i)->gui->string[j]), tmp, tmp2, objFile);
            }
        }
        i++;
    }
    /* free memory and close file stream */
    fclose(objFile);
    free(objName);
}

void writeEntryFile(char *fileName,Label **labelTable, int labelCounter) {
    int i;
	char *tmp;
    /* open file stream */
    OPEN_STREAM(entFile,entName, strlen(fileName)+5, fileName, ".ent", "w")

    for (i = 0; i <labelCounter;i++) {
        /* go through all labels */
        if ((*labelTable + i)->isAlsoEntry) {
            tmp = binaryToBase32((*labelTable + i)->labelAddress);
            /* print entry labels names and addresses, converted to base32 */
            fprintf(entFile, "%s\t%s\n", (*labelTable + i)->labelName, tmp);
            free(tmp);
        }
    }
    /* close file stream and free memory */
    fclose(entFile);
    free(entName);
}

void writeExternFile(char *fileName, Label **labelTable, int labelCounter, MemoryWords **memoryRecords, int memoryRecCount) {
    int i = 0,j;

	char *tmp;
    /* allocate memory for label */
    char *extLabel = malloc(sizeof (char) * MAX_LABEL +1);

    /* open file stream */
    OPEN_STREAM(extFile,extName, strlen(fileName)+5, fileName, ".ext", "w")

    CHECK_MALLOC(extLabel)


    /* sorting the label array based on whether they're external labels */
    qsort (*labelTable, labelCounter, sizeof(Label), compareIsExt);
 
    while (i < labelCounter && (*labelTable + i)->isExternal) {
        /* go through all external labels in the sorted table */
        for(j = 0; j < memoryRecCount && (*memoryRecords + j)->isInstruction; j++){
            /* go through all the instructions records in the sorted record table */
            if((*memoryRecords + j)->ins->ts == 1 || (*memoryRecords + j)->ins->ts == 2) {
                /* if there is a label in the source, copy the operand */
                strcpy(extLabel, (*memoryRecords + j)->ins->OPSource);
                /* if it is a struct, extract the label from operand */
                if (!(strcmp((*labelTable + i)->labelName, extLabel))) {
                    tmp = binaryToBase32((*memoryRecords + j)->address + 1);
                    /* if it matches a label name, print its address and code, converted to base32 */
                    fprintf(extFile, "%s\t%s\n", (*labelTable + i)->labelName, tmp);
                    free(tmp);
                }
            }
            if((*memoryRecords + j)->ins->td == 1 || (*memoryRecords + j)->ins->td == 2) {
                /* if there is a label in the destination, copy the operand */
                strcpy(extLabel,(*memoryRecords + j)->ins->OPDest);
                if (!(strcmp((*labelTable + i)->labelName, extLabel))) {
                    /* if it matches a label name, print its address and code, converted to base32 */
                    tmp = binaryToBase32((*memoryRecords + j)->address + 1 + (*memoryRecords + j)->ins->LSource);
                    fprintf(extFile, "%s\t%s\n", (*labelTable + i)->labelName, tmp);
                    free(tmp);
                }
            }
        }
        i++;
    }
    /* close stream and free memory */
    fclose(extFile);
    free(extLabel);
    free(extName);
}

/* compare methods for the qsort function */
int compareIsExt (const void *a, const void * b) {
    Label *s1 = (Label *)a;
    Label *s2 = (Label *)b;
    return -(s1->isExternal - s2->isExternal);
}

int compareAddress (const void *a, const void * b) {
    MemoryWords *m1 = (MemoryWords *)a;
    MemoryWords *m2 = (MemoryWords *)b;
    return (m1->address - m2->address);
}
