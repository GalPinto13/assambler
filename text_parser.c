#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "utils.h"
#include "text_parser.h"


int getFirstWord(int pos, char source[],char *target) {
    int i = 0;
    /* skip blanks and tabs */
    NEXT_NOT_WHITE(source,pos);
    /* copy only alphanumeric chars to *target */
    for(;pos < strlen(source) && isalnum(source[pos]); pos++,i++)
        *(target + i) = source[pos];
    /* end of a string char */
    *(target + i) = '\0';
    /* return the position of the first char of the word, at the original string */
    return pos;
}


void removeChar(char * str, char charToRemove) {
    int i, j;
    int len = strlen(str);
    for (i = 0; i < len; i++) {
        /* go through all the chars */
        if (str[i] == charToRemove) {
            /* remove char if found */
            for (j = i; j < len; j++) {
                str[j] = str[j + 1];
            }
            len--;
            i--;
        }
    }
}

bool isLabel(char *firstWord) {
    /* check if the last char is - ':', if so, the word is a label */
    return (*(firstWord + strlen(firstWord) -1) == ':');
}

bool longLine(char *lineText, int lineNum) {
    if (strlen(lineText) > MAX_LINE) {
        /* if the text is more than allowed */
        printf("Line is too long (more than %d characters), at line %d\n", MAX_LINE, lineNum);
        IS_ERROR  = TRUE;
        return TRUE;
    }
    return FALSE;
}

int breakLine(char *lineText, char **brokenLine, int lineNum) {
    /* initialize counters */
    int lineChar = 0, wordNum = 0, wordChar = 0;
    /* initialize flags */
    bool writing = FALSE, comma = FALSE, colon = FALSE, wroteNum = FALSE, RemComma =FALSE;

    /* if the line is empty or comment line return 0 words */
    if(lineText[0] == '\n' || lineText[0] == '\r' || lineText[0] == '\0' || lineText[lineChar] == ';')
        return wordNum;

    /* memory allocation */
    brokenLine[wordNum] = malloc(MAX_LABEL * sizeof(char));
    CHECK_MALLOC(brokenLine[wordNum]);

    /* while we are not at the end of a line/file/word */
    while (lineText[lineChar] != '\n' && lineText[lineChar] != '\0' && lineText[lineChar] != '\r' && lineText[lineChar] != EOF) {
        /* a word can't be longer than we defined */
        if(wordChar == MAX_LABEL) {
            printf("Word number %d is too long (more than %d characters), at line %d\n",wordNum+1, MAX_LABEL, lineNum);
            IS_ERROR  = TRUE;
            return wordNum;
        }
        /* checks for a colon */
        if(lineText[lineChar] == ':') {
            if(colon) {
                /* if a colon was already found in this line */
                /* we don't allow that */
                printf("More than one colon in the line is not allowed, at line %d\n", lineNum);
                IS_ERROR  = TRUE;
                return wordNum;
            }
            /* set colon flag to true */
            colon = TRUE;
        }
        /* checks for a comma */
        if(lineText[lineChar] == ',') {
            if(comma) {
                /* if a comma was the last char we checked */
                /* we don't allow that */
                printf("More than one consecutive comma found, at line %d\n", lineNum);
                IS_ERROR = TRUE;
                return wordNum;
            }
            /* set comma flag to true */
            comma = TRUE;
        }


        /* checks if we are at the end of a word and not in the middle of a quote */
        if((lineText[lineChar] == ' ' || lineText[lineChar] == '\t' ||
        lineText[lineChar] == ',' || lineText[lineChar] == ':')) {
            if(writing) {
                /* if we're in middle of writing a word*/
                if(lineText[lineChar] == ':') {
                    /* We found a Label - copy the colon if found */
                    brokenLine[wordNum][wordChar] = lineText[lineChar];
                    wordChar++;
                }
                /* reset flags */
                writing = FALSE;
                RemComma = FALSE;
                /* end of a word char */
                brokenLine[wordNum][wordChar] = '\0';
                /* move to next word */
                wordNum++;
                /* reset char counter */
                wordChar=0;
                /* allocate memory for next word */
                if(wordNum >= MAX_WORDS) {
                    /* we don't allow more words than we defined */
                    printf("We allow only %d words in a row, exceeded that limit, at line %d\n", MAX_WORDS, lineNum);
                    IS_ERROR = TRUE;
                    return wordNum;
                }
                else {
                    /* allocate memory for next word */
                    brokenLine[wordNum] = malloc(MAX_LABEL * sizeof(char));
                    CHECK_MALLOC(brokenLine[wordNum]);
                }
            }
            if(lineText[lineChar] == ',') {
                /* if a comma was found in valid place, remember it with RemComma */
                if ((wordNum - colon >= 2))
                    RemComma = TRUE;
                else
                {
                    /* a comma found between invalid words positions */
                    printf("A comma shouldn't appear between word number %d and word number %d, at line %d\n",
                           wordNum, wordNum + 1, lineNum);
                    IS_ERROR = TRUE;
                    return wordNum;
                }
            }

        }
        else
        {    /* comma wasn't found between two words that should be separated by comma */
            if((wordNum -colon >=2) && !RemComma) {
                printf("Comma is missing between word number %d and word number %d, at line %d\n",wordNum, wordNum +1, lineNum);
		        IS_ERROR = TRUE;
                return wordNum;
	        }
            brokenLine[wordNum][wordChar] = lineText[lineChar];
            if(isdigit(brokenLine[wordNum][wordChar]))
                /* is the char a digit - set flag if so */
                wroteNum = TRUE;
            /* move to next char */
            wordChar++;
            /* set flags */
            writing = TRUE;
            comma = FALSE;
        }
        /* move to next char */
        lineChar++;
    }

    if(comma) {
        /* we don't allow comma at the end of a line */
        printf("Comma at the end of the line error, at line %d\n", lineNum);
        IS_ERROR  = TRUE;
        return wordNum;
    }


    if(writing) {
        /* end word */
        brokenLine[wordNum][wordChar] = '\0';
        wordNum++;
    }
    if(wroteNum)
        /* reset flag */
        wroteNum = FALSE;

    return wordNum;
}

bool isValidString(char *word) {
    int i;
    for (i=0; i < strlen(word) && isprint((int) word[i]); i++);
    /* The string is valid if its string starts and ends with quotations and all its values are printable */
    return (*word == '"') && (*(word + strlen(word) -1) == '"') && (word[i] == 0);
}


bool isValidNum(char *word, char *type)
{
    int i;
    double num;
    for (i = 0; *(word+i) != '\0'; i++) {
        /* iterate through the word */
        if(i == 0 && (*(word) == '-' || *(word) == '+'))
            /* if we have a sign +/-, it is ok, move on */
            i++;
        if (!isdigit(*(word + i)))
            /* check that all the rest are digits */
            return FALSE;
    }
    /* convert it to a double */
    num = atof(word);
    if (!isNumInRange(num,type) || !isInteger(num))
        /* if number is not in defined range or is not an integer, it is not valid */
        return FALSE;
    return TRUE;
}

bool isNumInRange(int num, char *type) {
    /* we don't allow numbers that are not in the defined range */
    /* ranges for guidance and instruction are difference therefore we differentiate between the types */
    if(!strcmp("gui", type)) {
        if (num > MAX_NUMBER_GUI ||  num < MIN_NUMBER_GUI) {
            printf("Number is not in allowed range %d <= x <= %d, the number: %d\n", MIN_NUMBER_GUI, MAX_NUMBER_GUI, num);
            IS_ERROR = TRUE;
            return FALSE;
        }
    } else if(!strcmp("ins",type)) {
        if (num > MAX_NUMBER_INS ||  num < MIN_NUMBER_INS) {
            printf("Number is not in allowed range %d <= x <= %d, the number: %d\n", MIN_NUMBER_INS, MAX_NUMBER_INS, num);
            IS_ERROR = TRUE; 
            return FALSE;
        }
    }
    return TRUE;
}


bool notAlNum(char *word) {
    int i;
    for(i = 0; i < strlen(word); i++)
        /* iterate through the word */
        if(!isalnum(*(word + i)))
            /* if any char isn't alphanumeric char, return true -> it is not alphanumeric */
            return TRUE;
    return FALSE;
}