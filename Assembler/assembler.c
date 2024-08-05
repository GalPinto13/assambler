#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include "utils.h"
#include <string.h>
#include "macro_reader.h"
#include "first_pass.h"
#include "second_pass.h"
#include "file_writer.h"
#include "free_memory.h"
#include "assembler.h"

/*
 * Main program - entry point
 * execute with file names as arguments
 * valid file name = XXXX.as
 * for example: test.as
 */
int main (int argc,char *argv[]) {
    int i;
    /* Validate that files were provided */
    CHECK_AMT_OF_FILES(argc)
    /* Iterate through the files and process them */
    for (i = 1; i < argc; i++)
    {
        /* Reset error flag for each file */
        IS_ERROR=FALSE;
        /* Read file */
        read(argv[i]);
    }

    return 0;
}


void read(char *fileName) {
    FILE *file;
    /* allocate memory for temp file name */
    char *tmpFname = malloc(sizeof (char) * (strlen(fileName)+4));
    char *tmpFname2 = malloc(sizeof(char) * (strlen(fileName)+4));
    CHECK_MALLOC(tmpFname)
    CHECK_MALLOC(tmpFname2)
    /* copy the name that was passed as argument to temp file name */
    strcpy(tmpFname,fileName);
    strcpy(tmpFname2,fileName);
    /* open file read mode, with .as extension */
    file = fopen(strcat(tmpFname,".as"), "r");
    CHECK_FILE_RETURN(file,tmpFname)
    fclose(file);

    printf("--- assembling file %s ---\n\n", fileName);

    /* assemble file */
    assemble(tmpFname2);
    /* free memory */
    free(tmpFname);
    free(tmpFname2);
    /* If an error was found during assembling, report that */
    if (IS_ERROR)
        printf("\n--- assembler errors found on file %s ---\n", fileName);
    else
        printf("\n--- assembler run successfully over file %s ---\n", fileName);

}

void assemble(char *fileName) {
    /* Initialize counters */
    int IC=0, DC=0,memoryRecCount = 0,labelNum = 0,externExist = 0, entryExist = 0;
    /* Data holders */
    Label *labelTable;
    MemoryWords *memoryRecords;
    /* file names */
    char *sourceFile;
    char *fname = malloc(sizeof (char) * (strlen(fileName)+4));
    /* store original name */
    strcpy(fname,fileName);
    /* pre-process and get new file name */
    sourceFile = readMacro(fileName);
    /* first pass */
    firstPass(sourceFile, &memoryRecords, &memoryRecCount, &labelTable, &labelNum, &IC, &DC,&externExist);

    /* second pass - if first one didn't fail */
    if(!IS_ERROR)
        secondPass(sourceFile,&labelTable,&labelNum,&memoryRecords,&memoryRecCount,&entryExist);
    /* write object file, entry/extern if exists, if second pass was successful */
    if(!IS_ERROR) {
        writeObjectFile(fname,&memoryRecords,memoryRecCount,IC,DC);
        if(entryExist)
            writeEntryFile(fname, &labelTable, labelNum);
        if (externExist)
            writeExternFile(fname,&labelTable,labelNum,  &memoryRecords, memoryRecCount);
    }

    /* free data holders */
	free(fname);
    if(labelNum) {
        freeLabelTable(labelTable, labelNum);
	    free(labelTable);
    }
    if(memoryRecCount) {
        freeMemRec(memoryRecords,memoryRecCount);
	    free(memoryRecords);
    }
}