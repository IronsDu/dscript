#include "parser.h"
#include <map>

#include "module.h"
#include "syntaxnode.h"
#include "strhelp.h"

using namespace DScript;

typedef std::map<int , AbstractStateHandler*> STATEHANDLER_MAP_TYPE;
static STATEHANDLER_MAP_TYPE gStateHandlerList;

AbstractStateHandler* DScript::getStateHandler(EState state)
{
    STATEHANDLER_MAP_TYPE::iterator it = gStateHandlerList.find(state);
    if(it != gStateHandlerList.end())
    {
        return it->second;
    }

    return NULL;
}

void DScript::addStateHandler(EState state, AbstractStateHandler* handler)
{
    gStateHandlerList[state] = handler;
}

AbstractSyntaxNode* DScript::executeState(EState state, Module* module)
{
    AbstractStateHandler* handler = getStateHandler(state);
    AbstractSyntaxNode* node = NULL;

    if(NULL != handler)
    {
        node = handler->proc(module);
        module->addCurrentSyntaxNodeChild(node);
    }

    return node;
}

class InitStateHandler : public AbstractStateHandler
{
private:

    virtual    AbstractSyntaxNode*    proc(Module* module)
    {
        const char* &input = module->getParserPos();

        module->pushCurrentSyntaxNode(new InstanceSyntaxNode);

        while(true)
        {
            input = skipSpace(input);

            EToken token = getToken(input);

            if(token == ETOKEN_EOF)
            {
                break;
            }
            else if(token == ETOKEN_NAME)
            {
                executeState(ESTATE_VARDEFINE, module);
            }
            else if(token == ETOKEN_FUNC)
            {
                executeState(ESTATE_FUNC_DEFINE, module);
            }
            else
            {
                module->onParserError();
                break;
            }
        }

        return module->popCurrentSyntaxNode();
    }
};

class BlockDefineStateHandler : public AbstractStateHandler
{
private:

    virtual    AbstractSyntaxNode*    proc(Module* module)
    {
        const char* &input = module->getParserPos();

        BlockSyntaxNode* node = new BlockSyntaxNode;
        module->pushCurrentSyntaxNode(node);

        input = skipSpace(input);

        EToken token = getToken(input);

        while(true)
        {
            token = getCurrentToken();

            if(ETOKEN_IF == token)
            {
                executeState(ESTATE_IF_BLOCK, module);
            }
            else if(ETOKEN_WHILE == token)
            {
                executeState(ESTATE_WHILE_BLOCK, module);
            }
            else if(EToken('{') == token)
            {
                getToken(input);
                module->addCurrentSyntaxNodeChild(proc(module));
                checkCurrentToken(EToken('}'), "", input);
                break;
            }
            else if(ETOKEN_NAME == token || ETOKEN_MM == token || ETOKEN_PP == token)
            {
                executeState(ESTATE_CALCULATE_DEFINE, module);
                checkCurrentToken(EToken(';'), "", input);
                input = skipSpace(input);
                getToken(input);
            }
            else if (ETOKEN_BREAK == token)
            {
                getToken(input);
                input = skipSpace(input);
                module->addCurrentSyntaxNodeChild(new BreakSyntaxNode);
                getToken(input);
            }
            else if(ETOKEN_RETURN == token)
            {
                executeState(ESTATE_RETURN_BLOCK, module);
            }
            else
            {
                break;
            }
        }

        input = skipSpace(input);

        return module->popCurrentSyntaxNode();
    }
};

class CalculateStateHandler : public AbstractStateHandler
{
private:

    virtual    AbstractSyntaxNode*    proc(Module* module)
    {
        const char* &input = module->getParserPos();

        CalculateDefineSyntaxNode* calcutenode = new CalculateDefineSyntaxNode;
        module->pushCurrentSyntaxNode(calcutenode);

        EToken token = getCurrentToken();

        bool EuqalFirst = true;

        while(true)
        {
            if(ETOKEN_NUM == token)
            {
                ConstNumSyntaxNode* node = new ConstNumSyntaxNode;
                node->setValue(getCurrentTokenNum());
                module->addCurrentSyntaxNodeChild(node);

                input = skipSpace(input);
                getToken(input);
            }
            else if(ETOKEN_NAME == token)
            {
                if('(' == *input)
                {
                    getToken(input);
                    input = skipSpace(input);
                    executeState(ESTATE_CALLFUNC, module);

                    input = skipSpace(input);
                    getToken(input);
                }
                else
                {
                    input = skipSpace(input);
                    getToken(input);

                    if(ETOKEN_A == getCurrentToken() && EuqalFirst)
                    {
                        VarDefineSyntaxNode* vardefine = new VarDefineSyntaxNode;
                        vardefine->setVarName(getCurrentTokenStr());
                        module->addCurrentSyntaxNodeChild(vardefine);

                        module->pushCurrentSyntaxNode(vardefine);
                        input = skipSpace(input);
                        getToken(input);

                        module->addCurrentSyntaxNodeChild(proc(module));

                        module->popCurrentSyntaxNode();
                    }
                    else if((ETOKEN_PE == getCurrentToken() || ETOKEN_ME == getCurrentToken()) && EuqalFirst)
                    {
                        OPESyntaxNode* peNode = new OPESyntaxNode(getCurrentToken());
                        peNode->setVarName(getCurrentTokenStr());
                        module->addCurrentSyntaxNodeChild(peNode);

                        module->pushCurrentSyntaxNode(peNode);
                        input = skipSpace(input);
                        getToken(input);

                        module->addCurrentSyntaxNodeChild(proc(module));

                        module->popCurrentSyntaxNode();
                    }
                    else
                    {
                        VarRefSyntaxNode* node = new VarRefSyntaxNode;
                        node->setVarName(getCurrentTokenStr());
                        module->addCurrentSyntaxNodeChild(node);
                    }
                }
            }
            else if(EToken('(') == token)
            {
                input = skipSpace(input);
                getToken(input);
                module->addCurrentSyntaxNodeChild(proc(module));

                checkCurrentToken(EToken(')'), "", input);

                input = skipSpace(input);
                getToken(input);
            }
            else if(EToken('\"') == token)
            {
                const char* end = endsearch(input, '\"');
                if (end == input || NULL == end)
                {
                    printf("near %s", input);
                    module->onParserError();
                }
                getKeyToken(input, end);
                input = end;
                ConstStrSyntaxNode* node = new ConstStrSyntaxNode;
                node->setVarName(getCurrentTokenStr());
                module->addCurrentSyntaxNodeChild(node);

                checkToken(input, EToken('\"'), "");
                input = skipSpace(input);
                getToken(input);
            }
            else if(ETOKEN_PLUS == token
                ||    ETOKEN_PE == token
                ||    ETOKEN_PP == token
                ||    ETOKEN_MINUS == token
                ||    ETOKEN_ME == token
                ||    ETOKEN_MM == token
                ||    ETOKEN_MUL == token
                ||    ETOKEN_DIV == token
                ||    ETOKEN_G == token
                ||    ETOKEN_L == token
                ||    ETOKEN_E == token
                ||    ETOKEN_NE == token
                ||    ETOKEN_LE == token
                ||    ETOKEN_GE == token
                ||    ETOKEN_OR == token
                ||    ETOKEN_AND == token)
            {
                OpSyntaxNode* node = new OpSyntaxNode;
                node->setOPToken(token);
                module->addCurrentSyntaxNodeChild(node);

                input = skipSpace(input);
                token = getToken(input);
            }
            else
            {
                break;
            }

            token = getCurrentToken();

            EuqalFirst = false;
        }

        return module->popCurrentSyntaxNode();
    }
};

class FuncDefineStateHandler : public AbstractStateHandler
{
private:

    AbstractSyntaxNode*    proc(Module* module)
    {
        const char* &input = module->getParserPos();

        FuncDefineSyntaxNode* funcdefinenode = new FuncDefineSyntaxNode;
        module->pushCurrentSyntaxNode(funcdefinenode);

        input = skipSpace(input);
        checkToken(input, ETOKEN_NAME, "");

        funcdefinenode->setVarName(getCurrentTokenStr());
        checkToken(input,EToken('('), "函数参数申明应该以(开始");

        FuncArgListSyntaxNode* arglistnode = new FuncArgListSyntaxNode;
        module->addCurrentSyntaxNodeChild(arglistnode);

        input = skipSpace(input);
        EToken token = getToken(input);

        while(EToken(')') != token)
        {
            if(token == ETOKEN_NAME)
            {
                arglistnode->addArg(getCurrentTokenStr());

                input = skipSpace(input);
                token = getToken(input);
            }
            else
            {
                printf("%s", input);
            }

            if(token == EToken(','))
            {
                input = skipSpace(input);
                token = getToken(input);
            }
        }

        checkCurrentToken(EToken(')'), "", input);
        getToken(input);

        executeState(ESTATE_BLOCK_DEFINE, module);

        return module->popCurrentSyntaxNode();
    }
};

class CallfuncStateHandler : public AbstractStateHandler
{
private:

    AbstractSyntaxNode*        proc(Module* module)
    {
        const char* &input = module->getParserPos();

        CallfuncSyntaxNode* callfuncNode = new CallfuncSyntaxNode;
        module->pushCurrentSyntaxNode(callfuncNode);

        callfuncNode->setVarName(getCurrentTokenStr());

        AbstractStateHandler* calculateHandler = getStateHandler(ESTATE_CALCULATE_DEFINE);

        EToken token = getToken(input);

        while(EToken(')') != token)
        {
            module->addCurrentSyntaxNodeChild(calculateHandler->proc(module));

            token = getCurrentToken();
            if(token == EToken(','))
            {
                input = skipSpace(input);
                token = getToken(input);
                if (token == EToken(')'))
                {
                    printf("near%s", input);
                    module->onParserError();
                }
            }
            else
            {
                break;
            }
        }

        checkCurrentToken(EToken(')'), "", input);

        return module->popCurrentSyntaxNode();
    }
};

class IfblockStateHandler : public AbstractStateHandler
{
private:
    AbstractSyntaxNode*        proc(Module* module)
    {
        const char* &input = module->getParserPos();

        module->pushCurrentSyntaxNode(new IfblockSyntaxNode);

        AbstractStateHandler* calculateHandler = getStateHandler(ESTATE_CALCULATE_DEFINE);

        do
        {
            EToken currenttoken = getCurrentToken();

            if((ETOKEN_IF == currenttoken) || (ETOKEN_ELIF == currenttoken) || (ETOKEN_ELSE == currenttoken))
            {
                if(ETOKEN_ELSE != currenttoken)
                {
                    checkToken(input, EToken('('), "");
                    input = skipSpace(input);
                    getToken(input);

                    module->addCurrentSyntaxNodeChild(calculateHandler->proc(module));
                    checkCurrentToken(EToken(')'), "", input);
                }

                input = skipSpace(input);
                EToken token = getToken(input);

                if(EToken('{') == token)
                {
                    executeState(ESTATE_BLOCK_DEFINE, module);

                    input = skipSpace(input);
                    getToken(input);
                }
                else if(ETOKEN_NAME == token)
                {
                    module->addCurrentSyntaxNodeChild(calculateHandler->proc(module));
                    checkCurrentToken(EToken(';'), "", input);

                    input = skipSpace(input);
                    getToken(input);
                }
                else if (ETOKEN_RETURN == token)
                {
                    executeState(ESTATE_RETURN_BLOCK, module);
                }
                else
                {
                    checkCurrentToken(EToken('{'), "", input);
                }

                if(ETOKEN_ELSE == currenttoken)
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }while(true);

        return module->popCurrentSyntaxNode();
    }
};

class VardefineStateHandler : public AbstractStateHandler
{
private:

    AbstractSyntaxNode*        proc(Module* module)
    {
        const char* &input = module->getParserPos();

        VarDefineSyntaxNode* vardefine = new VarDefineSyntaxNode;
        vardefine->setVarName(getCurrentTokenStr());
        module->pushCurrentSyntaxNode(vardefine);

        input = skipSpace(input);
        checkToken(input, ETOKEN_A, "");
        input = skipSpace(input);

        getToken(input);
        executeState(ESTATE_CALCULATE_DEFINE, module);

        checkCurrentToken((EToken)';', "", input);

        return module->popCurrentSyntaxNode();
    }
};

class WhileBlockStateHandler : public AbstractStateHandler
{
private:

    AbstractSyntaxNode*        proc(Module* module)
    {
        const char* &input = module->getParserPos();

        WhileBlockSyntaxNode* pWhileNode = new WhileBlockSyntaxNode;
        module->pushCurrentSyntaxNode(pWhileNode);

        checkToken(input, EToken('('), "");
        input = skipSpace(input);
        getToken(input);
        AbstractStateHandler* calculateHandler = getStateHandler(ESTATE_CALCULATE_DEFINE);
        module->addCurrentSyntaxNodeChild(calculateHandler->proc(module));
        checkCurrentToken(EToken(')'), "", input);

        input = skipSpace(input);
        EToken token = getToken(input);

        if(EToken('{') == token)
        {
            executeState(ESTATE_BLOCK_DEFINE, module);
            input = skipSpace(input);
            getToken(input);

            pWhileNode->setHasBlock();
        }
        else if(ETOKEN_NAME == token)
        {
            module->addCurrentSyntaxNodeChild(calculateHandler->proc(module));
            checkCurrentToken(EToken(';'), "", input);
            input = skipSpace(input);
            getToken(input);
        }
        else if (ETOKEN_RETURN == token)
        {
            executeState(ESTATE_RETURN_BLOCK, module);
        }
        else
        {
            checkCurrentToken(EToken('{'), "", input);
        }

        return module->popCurrentSyntaxNode();
    }
};

class ReturnStateHandler : public AbstractStateHandler
{
private:
    AbstractSyntaxNode*    proc(Module* module)
    {
        const char* &input = module->getParserPos();

        module->pushCurrentSyntaxNode(new ReturnSyntaxNode);
        input = skipSpace(input);
        getToken(input);
        AbstractStateHandler* calculateHandler = getStateHandler(ESTATE_CALCULATE_DEFINE);
        module->addCurrentSyntaxNodeChild(calculateHandler->proc(module));
        checkCurrentToken(EToken(';'), "", input);
        input = skipSpace(input);
        getToken(input);
        return module->popCurrentSyntaxNode();
    }
};

void DScript::initAllStateHandler()
{
    static bool INIT = false;

    if(!INIT)
    {
        addStateHandler(ESTATE_INIT, new InitStateHandler);
        addStateHandler(ESTATE_BLOCK_DEFINE, new BlockDefineStateHandler);
        addStateHandler(ESTATE_CALCULATE_DEFINE, new CalculateStateHandler);
        addStateHandler(ESTATE_FUNC_DEFINE, new FuncDefineStateHandler);
        addStateHandler(ESTATE_CALLFUNC, new CallfuncStateHandler);
        addStateHandler(ESTATE_IF_BLOCK, new IfblockStateHandler);
        addStateHandler(ESTATE_VARDEFINE, new VardefineStateHandler);
        addStateHandler(ESTATE_WHILE_BLOCK, new WhileBlockStateHandler);
        addStateHandler(ESTATE_RETURN_BLOCK, new ReturnStateHandler);

        INIT = true;
    }
}
