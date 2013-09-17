#ifndef _TOKEN_H
#define _TOKEN_H

namespace DScript
{
    enum EToken
    {
        ETOKEN_EOF,
        ETOKEN_NUM,                 //'0~9'

        ETOKEN_PLUS,                //'+'
        ETOKEN_PP,                  //'++'
        ETOKEN_MINUS,               //'-'
        ETOKEN_MM,                  //'--'
        ETOKEN_MUL,                 //'*'
        ETOKEN_DIV,                 //'/'
        ETOKEN_A,                   //'='
        ETOKEN_G,                   //'>'
        ETOKEN_L,                   //'<'
        ETOKEN_E,                   //'=='
        ETOKEN_NE,                  //'!='
        ETOKEN_LE,                  //'<='
        ETOKEN_GE,                  //'>='
        ETOKEN_PE,                  //'+='
        ETOKEN_ME,                  //'-='
        ETOKEN_OR,                  //'||'
        ETOKEN_AND,                 //'&&'

        ETOKEN_FUNC,                //function
        ETOKEN_IF,                  //if
        ETOKEN_ELSE,                //else
        ETOKEN_ELIF,                //else if
        ETOKEN_BREAK,               //break
        ETOKEN_RETURN,              //return
        ETOKEN_FOR,                 //for
        ETOKEN_WHILE,               //while
        ETOKEN_NAME,                //varname
    };

    EToken getToken(const char* &input);

    EToken getKeyToken(const char* start, const char* end);

#ifndef NULL
#define NULL 0
#endif

    EToken checkToken(const char* &input, EToken token, const char* msg = NULL);

    bool checkCurrentToken(EToken, const char* msg = NULL, const char* source = NULL);

    const char* getCurrentTokenStr();
    int getCurrentTokenNum();
    EToken  getCurrentToken();
}

#endif
