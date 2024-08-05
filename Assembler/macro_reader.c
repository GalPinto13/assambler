#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "text_parser.h"
#include "free_memory.h"
#include "macro_reader.h"


char *readMacro (char *fname) {
    /* initialize counters */
	int newPos = 0,i = 0;
    /* initialize flag */
	bool IS_MACRO = FALSE;
    /* a pointer to the head of the linked list */
	struct Macro *head = NULL;
    /* file holders */
	FILE *fsource,*ftarget;
    /* line holder */
	char buf[MAX_LINE];
    /* memory allocation and checks */
	char *word = malloc(sizeof(char) * MAX_LINE);
	char *mName = malloc(sizeof(char) * MAX_LINE);
	char *tname = malloc(sizeof(char) * MAX_LINE);
    CHECK_MALLOC(word)
    CHECK_MALLOC(mName)
    CHECK_MALLOC(tname)

    /* put together file names and extensions */
	strcpy(tname, fname);
	strcat(fname,".as");
	strcat(tname, ".am");

    /* open files */
	fsource = fopen(fname, "r");
	ftarget = fopen(tname, "w");

    /* get line by line from the file */
	while (fgets(buf, MAX_LINE, fsource) != NULL) {
        /* if we are in a macro */
		if (IS_MACRO) {
            /* set the content */
		    setMacroContent(head, buf, fsource);
            /* end of macro reset flag */
		    IS_MACRO = FALSE;
		}

        /* get the first word */
        newPos = getFirstWord(i, buf, word);

        if(!strcmp(word, "macro")) {
            /* the word is 'macro' */
            /* get its name */
            getFirstWord(newPos, buf, mName);
            /* add macro node to list */
            addMacroNode(&head, mName);
            /* set flag */
            IS_MACRO = TRUE;
        } else {
            /* the word is not macro */
            if(isMacro(head, word))
                /* the word is a name of a macro that was already set, get its content and put it in the file */
                fputs(getMacro(head, word)->macroContent,ftarget);
            else
                /* no macro or macro name */
            	if(!isEndMacro(buf))
                    /* if its not an 'endmacro' line, copy the line content to the file */
            		fputs(buf, ftarget);
        }
        /* reset index */
        i = 0;
	}

    /* close streams */
	fclose(fsource);
	fclose(ftarget);


    /* free memory */
	free(word);
	free(mName);
	if(head == NULL) {
        /* no macros -> no new file */
		remove(tname);
		free(tname);
		return fname;
	}
	strcpy(fname, tname);
	free(tname);
	freeMacro(head);
	return fname;
}

bool isMacro(struct Macro *head, char *mName) {
    struct Macro *temp = head;
    while(temp != NULL) {
        /* iterate through linked list */
        if(!strcmp(mName, temp->macroName))
            /* if a match was found, return true */
            return TRUE;
        temp = temp->next;
    }
    return FALSE;
}

struct Macro* getMacro(struct Macro *head, char *mName) {
    struct Macro *temp = head;
    while(temp != NULL) {
        /* iterate through linked list */
        if(!strcmp(mName,temp->macroName))
            /* if a match was found, return the node */
            return temp;
        temp = temp->next;
    }
    return temp;
}

void addMacroNode(struct Macro **head,char *mName){
    struct Macro *lastNode;
    /* allocate memory for new node */
    struct Macro *newNode = malloc(sizeof (struct Macro));
    newNode->macroName = malloc(sizeof (char) * strlen(mName)+1);
    CHECK_MALLOC(newNode)
    CHECK_MALLOC(newNode->macroName)

    /* copy its name */
    strcpy(newNode->macroName, mName);
	newNode->next = NULL;
    if (*head == NULL)
        /* the list is empty */
        *head = newNode;
    else {
        /* list isn't empty */
        lastNode = *head;
        while (lastNode->next != NULL)
            /* go to last node */
            lastNode = lastNode->next;
        /* set its next node to be the new node */
        lastNode->next = newNode;
    }
}

void setMacroContent(struct Macro *head, char buf[], FILE *fsource) {
    char *tmp;
    int lineNum = 1;
    struct Macro *temp = head;

    /* get to the last macro */
    while(temp->next != NULL)
        temp = temp->next;

    /* allocate memory for its content */
    temp->macroContent =  malloc(sizeof (char) * (lineNum * MAX_LINE));
    CHECK_MALLOC(temp->macroContent)

    if(!isEndMacro(buf)) {
        /* if we are not at the end of the macro */
        /* copy line content */
        strcpy(temp->macroContent, buf);
        /* read from file while we are not at the end of the macro or end of the file */
        while (fgets(buf, MAX_LINE, fsource) != NULL && !isEndMacro(buf)) {
            /* increase counter */
        	lineNum++;
            /* reallocate memory for content holder */
            tmp = realloc(temp->macroContent, sizeof (char) * (lineNum * MAX_LINE));
            if (tmp == NULL)
                printf("Not enough space\n");
            else
                /* copy line */
                strcat(tmp, buf);
            /* and put it in the node */
            temp->macroContent = tmp;
        }
	
    }
    /* if we got to the end of macro, jump to next line */
    fgets(buf, MAX_LINE, fsource);
}



bool isEndMacro(char line[]) {
    int i = 0;
    bool answer;

    /* allocate memory for a word to be read */
    char *word = malloc(sizeof(char) * MAX_LINE);
    CHECK_MALLOC(word);

    /* get the first word */
    getFirstWord(i, line, word);

    /* if it is "endmacro" return true */
    answer = (!strcmp(word, "endmacro"));
    free(word);
    return answer;
}