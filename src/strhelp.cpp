#include "strhelp.h"
#include <locale>
#include <string.h>
#include <stdlib.h>

const char* skipSpace(const char* input)
{
    while(true)
    {
        char ch = *input;

        if(ch != '\t' && ch != '\n' && ch != ' ' &&  ch != '\r')
        {
            break;
        }

        input++;
    }

    return input;
}

const char* skipChar(const char* input, char skipchar)
{
    while(true)
    {
        if(*input != skipchar)
        {
            break;
        }

        input++;
    }

    return input;
}

const char* endsearch(const char* input, char endchar)
{
    while(true)
    {
        char ch = *input;
        if(ch == endchar)
        {
            break;
        }
        else if (ch == '\0')
        {
            return NULL;
        }

        input++;
    }

    return input;
}

const char* skipChar(const char* input)
{
    while(true)
    {
        if(!isalpha(*input))
        {
            break;
        }

        input++;
    }

    return input;
}

const char* skipNum(const char* input)
{
    while(true)
    {
        char ch = *input;
        if(isalpha(ch) || !isalnum(ch))
        {
            break;
        }

        input++;
    }

    return input;
}

void auxCopyStr(char* dest, const char* head, const char* end)
{
    if(NULL != dest)
    {
        for(char* p = dest; head < end; p++)
        {
            *p = *head++;
        }
    }
}

int getNumFromStr(const char* head, const char* end)
{
    char str[1024];
    memset(str, 0, sizeof(str));
    auxCopyStr(str, head, end);

    return atoi(str);
}
