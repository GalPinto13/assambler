#include <stdlib.h>
#include <stdio.h>
#include <string.h>


int main() {
    int i,j;
    char *a = "S.1";
    const char separator = '.';
    char * const sep_at = strchr(strdup(a), separator);
    i = 1;
    j = (i == 1) ? 1 : 1;
    printf("yossi");
    /*
    if(sep_at != NULL)
    {
        *sep_at = *(sep_at + 1);
        *(sep_at + 1) = '\0';
        i = atoi(sep_at);
        printf("first part: '%s'\nsecond part: '%s'\nnum: %d\n", a, sep_at, i);
    }
    printf("something\n");*/
    return 0;
}



