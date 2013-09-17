#ifndef _PARSER_H
#define _PARSER_H

namespace DScript
{
    //解析器解析状态
    enum EState
    {
        ESTATE_INIT,                //start parse script file
        ESTATE_VARDEFINE,           //define var
        ESTATE_FUNC_DEFINE,         //define function

        ESTATE_BLOCK_DEFINE,        //define {}
        ESTATE_CALCULATE_DEFINE,    //calulate exp
        ESTATE_IF_BLOCK,            //if block
        ESTATE_FOR_BLOCK,           //for block
        ESTATE_WHILE_BLOCK,         //while block
        ESTATE_RETURN_BLOCK,        //return statement;
        ESTATE_CALLFUNC,            //call function
    };

    class AbstractSyntaxNode;
    class Module;

    class AbstractStateHandler
    {
    public:
        virtual AbstractSyntaxNode* proc(Module*) = 0;
    };

    AbstractStateHandler* getStateHandler(EState state);
    void addStateHandler(EState state, AbstractStateHandler* handler);

    AbstractSyntaxNode* executeState(EState state, Module*);
    void initAllStateHandler();
}


#endif
