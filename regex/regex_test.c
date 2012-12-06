#include<stdio.h>
#include<regex.h>

int main(int argc,char** argv)
{
    int status,i;
    int cflags = REG_EXTENDED;
    regmatch_t pmatch[1];
    const size_t nmatch = 1;
    regex_t reg;
    const char* pattern = "hi.*regex";
    char * buf = "wawo hi,I love regex good";

    regcomp(&reg,pattern,cflags);
    status = regexec(&reg,buf,nmatch,pmatch,0);
    if (status == 0)
    {
        printf("Match:");
        for(i=pmatch[0].rm_so;i<pmatch[0].rm_eo;i++)
            putchar(buf[i]);
        printf("\n");
    }
    regfree(&reg);
    return 0;
}
