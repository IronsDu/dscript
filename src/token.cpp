#include "token.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <locale>
#include "strhelp.h"

namespace DScript
{
    const int TOKEN_LEN = 21;

    char tokenstr[TOKEN_LEN];
    int tokennum;
    EToken currenttoken;

    const char* getCurrentTokenStr()
    {
        return tokenstr;
    }

    int    getCurrentTokenNum()
    {
        return tokennum;
    }

    EToken getCurrentToken()
    {
        return currenttoken;
    }

    EToken getToken(const char* &input)
    {
        char ch = *input;

        if('\0' == ch)
        {
            currenttoken = ETOKEN_EOF;
            input++;
        }
        else if('+' == ch)
        {
            char next = *(input+1);
            if ('=' == next)
            {
                currenttoken = ETOKEN_PE;
                input += 2;
            }
            else if('+' == next)
            {
                currenttoken = ETOKEN_PP;
                input += 2;
            }
            else
            {
                currenttoken = ETOKEN_PLUS;
                input++;
            }
        }
        else if('-' == ch)
        {
            char next = *(input+1);
            if ('=' == next)
            {
                currenttoken = ETOKEN_ME;
                input += 2;
            }
            else if('-' == next)
            {
                currenttoken = ETOKEN_MM;
                input += 2;
            }
            else
            {
                currenttoken = ETOKEN_MINUS;
                input++;
            }
        }
        else if('*' == ch)
        {
            currenttoken = ETOKEN_MUL;
            input++;
        }
        else if('/' == ch)
        {
            currenttoken = ETOKEN_DIV;
            input++;
        }
        else if('=' == ch)
        {
            if ('=' == *(input+1))
            {
                currenttoken = ETOKEN_E;
                input += 2;
            }
            else
            {
                currenttoken = ETOKEN_A;
                input++;
            }
        }
        else if('>' == ch)
        {
            if ('=' == *(input+1))
            {
                currenttoken = ETOKEN_GE;
                input += 2;
            }
            else
            {
                currenttoken = ETOKEN_G;
                input++;
            }
        }
        else if('<' == ch)
        {
            if ('=' == *(input+1))
            {
                currenttoken = ETOKEN_LE;
                input += 2;
            }
            else
            {
                currenttoken = ETOKEN_L;
                input++;
            }
        }
        else if('!' == ch)
        {
            if ('=' == *(input+1))
            {
                currenttoken = ETOKEN_NE;
                input += 2;
            }
        }
        else if('|' == ch)
        {
            if ('|' == *(input+1))
            {
                currenttoken = ETOKEN_OR;
                input += 2;
            }
        }
        else if ('&' == ch)
        {
            if ('&' == *(input+1))
            {
                currenttoken = ETOKEN_AND;
                input += 2;
            }
        }
        else if(isalpha(ch))
        {
            const char* nameend = skipChar(input);
            currenttoken =  getKeyToken(input, nameend);
            input = nameend;

            return currenttoken;
        }
        else if(isdigit(ch))
        {
            const char* numend = skipNum(input);
            tokennum = getNumFromStr(input, numend);
            input = numend;

            return currenttoken = ETOKEN_NUM;
        }
        else
        {
            currenttoken = (EToken)ch;
            input++;
        }

        return currenttoken;
    }

    struct keytoken
    {
        const char* key_name;
        EToken    token;
    };

    static keytoken keywords[] =
    {
        {"function", ETOKEN_FUNC},
        {"if", ETOKEN_IF},
        {"else", ETOKEN_ELSE},
        {"elif", ETOKEN_ELIF},
        {"for", ETOKEN_FOR},
        {"while", ETOKEN_WHILE},
        {"break", ETOKEN_BREAK},
        {"return", ETOKEN_RETURN},
    };

    EToken getKeyToken(const char* str)
    {
        for(size_t i = 0; i < sizeof(keywords) / sizeof(keytoken); ++i)
        {
            if(strcmp(keywords[i].key_name, str) == 0)
            {
                return keywords[i].token;
            }
        }

        return ETOKEN_NAME;
    }


    EToken getKeyToken(const char* start, const char* end)
    {
        memset(tokenstr, 0, sizeof(tokenstr));

        if((size_t)(end - start) < sizeof(tokenstr))
        {
            auxCopyStr(tokenstr, start, end);

            return getKeyToken(tokenstr);
        }

        return ETOKEN_NAME;
    }

    EToken checkToken(const char* &input, EToken token, const char* msg)
    {
        const char* oldinput = input;
        EToken currenttoken = getToken(input);

        if(currenttoken != token)
        {
            if(NULL != msg)
            {
                printf("%s\n", msg);
            }
            printf("near: %s \n按任意键盘退出\n", oldinput);
            getchar();
            exit(0);
        }

        return currenttoken;
    }

    bool checkCurrentToken(EToken token, const char* msg /* = NULL */, const char* source /*= NULL */)
    {
        if(currenttoken != token && NULL != msg && NULL != source)
        {
            printf("%s\n", msg);
            printf("near:%s", source);
        }

        return currenttoken == token;
    }
}