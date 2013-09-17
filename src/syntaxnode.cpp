#include "syntaxnode.h"
#include "context.h"
#include "build.h"

using namespace DScript;

AbstractSyntaxNode::AbstractSyntaxNode() : varname(NULL), m_type(ESN_ERROR)
{
}

AbstractSyntaxNode:: ~AbstractSyntaxNode()
{
    if(NULL != varname)
    {
        delete []varname;
    }
}

void AbstractSyntaxNode::setVarName(const char* str)
{
    int len = strlen(str) + 1;
    varname = new char[len];

    if(NULL != varname)
    {
        memset(varname, '\0', len);
        strcpy(varname, str);
    }
}

void AbstractSyntaxNode::build(Context* context)
{
    for(size_t i = 0; i < getChildNum(); ++i)
    {
        buildAChild(i, context);
    }
}

void AbstractSyntaxNode::display()
{
    displayChilds();
}

void AbstractSyntaxNode::execute(Context*)
{

}

DObject AbstractSyntaxNode::value()
{
    return DObject();
}

void AbstractSyntaxNode::addChild(AbstractSyntaxNode* pChild)
{
    m_childs.push_back(pChild);
}

void AbstractSyntaxNode::displayChilds()
{
    CHILD_LIST_TYPE::iterator it = m_childs.begin();
    for(; it != m_childs.end(); ++it)
    {
        (*it)->display();
    }
}

const char* AbstractSyntaxNode::getVarName()
{
    return varname;
}

size_t AbstractSyntaxNode::getChildNum()
{
    return m_childs.size();
}

ESYNATXNODE_TYPE AbstractSyntaxNode::getType()
{
    return m_type;
}

void AbstractSyntaxNode::displayAChild(size_t index)
{
    if(index < m_childs.size())
    {
        m_childs[index]->display();
    }
}

void AbstractSyntaxNode::buildAChild(size_t index, Context* context)
{
    if(index < m_childs.size())
    {
        m_childs[index]->build(context);
    }
}

void AbstractSyntaxNode::executeAChild(size_t index, Context* context)
{
    if(index < m_childs.size())
    {
        m_childs[index]->execute(context);
    }
}

AbstractSyntaxNode*    AbstractSyntaxNode::getAChild(size_t index)
{
    if(index < m_childs.size())
    {
        return m_childs[index];
    }

    return NULL;
}

void AbstractSyntaxNode::setType(ESYNATXNODE_TYPE type)
{
    m_type = type;
}

void AbstractSyntaxNode::setChildsList(CHILD_LIST_TYPE& newchilds)
{
    m_childs.clear();

    CHILD_LIST_TYPE::iterator it = newchilds.begin();
    for(; it != newchilds.end(); ++it)
    {
        m_childs.push_back(*it);
    }
}

VarDefineSyntaxNode::VarDefineSyntaxNode()
{
    setType(ESN_VAR_DEFINE);
    m_isGlobalVardefine = true;
    m_index = -1;
}

void VarDefineSyntaxNode::display()
{
    printf("%s, ", getVarName());
    displayAChild(0);
    printf("\n");
}

void VarDefineSyntaxNode::build(Context* context)
{
    m_isGlobalVardefine = !(gBuildData.isInFunc());

    if(m_isGlobalVardefine)
    {
        executeAChild(0, context);
    }
    
    if (!m_isGlobalVardefine)
    {
        m_index = gBuildData.insertLocalBuildData(getVarName());
    }
    else
    {
        m_index = gBuildData.insertGlobalBuildData(getVarName());

        context->insertGlobalObject(m_index, context->getEax());

        context->insertNameIndexPair(getVarName(), m_index);
    }

    AbstractSyntaxNode::build(context);
}

void VarDefineSyntaxNode::execute(Context* context)
{
    executeAChild(0, context);
    assert(m_index != -1);

    if (m_isGlobalVardefine)
    {
        context->insertGlobalObject(m_index, context->getEax());
    }
    else
    {
        context->insertLocalObject(m_index, context->getEax());
    }
}

ReturnSyntaxNode::ReturnSyntaxNode()
{
    setType(ESN_RETURN);
}

void ReturnSyntaxNode::display()
{
    printf("\n");
    displayChilds();
}

void ReturnSyntaxNode::execute(Context* context)
{
    executeAChild(0, context);
    context->setExecuteState(ES_RETURN);
}

BlockSyntaxNode::BlockSyntaxNode()
{
    setType(ESN_BLOCK);
}

void BlockSyntaxNode::display()
{
    printf("[[\n");
    for(size_t i = 0; i < getChildNum(); ++i)
    {
        displayAChild(i);
    }
    printf("]]\n");
}

void BlockSyntaxNode::execute(Context* context)
{
    for(size_t i = 0; i < getChildNum(); ++i)
    {
        executeAChild(i, context);
        if(ES_RETURN == context->getExecuteState() || ES_BREAK == context->getExecuteState())
        {
            break;
        }
    }
}

BreakSyntaxNode::BreakSyntaxNode()
{
    setType(ESN_BREAK);
}

void BreakSyntaxNode::display()
{
    printf("break\n");
}

void BreakSyntaxNode::execute(Context* context)
{
    if (context->getBreakBlockNum() > 0)
    {
        context->setExecuteState(ES_BREAK);
    }
    else
    {
        printf("break\n");
    }
}

WhileBlockSyntaxNode::WhileBlockSyntaxNode()
{
    m_bHasBlock = false;
    setType(ESN_WHILE);
}

void WhileBlockSyntaxNode::setHasBlock()
{
    m_bHasBlock = true;
}

void WhileBlockSyntaxNode::display()
{
    displayAChild(0);
    printf("\n");
    displayAChild(1);
    printf("\n");
}

void WhileBlockSyntaxNode::execute(Context* context)
{
    DObject& eax = context->getEax();

    if(m_bHasBlock)
    {
        context->incBreakBlockNum();
    }

    while(true)
    {
        executeAChild(0, context);
        if(eax.data.int_data != 0)
        {
            executeAChild(1, context);
        }
        else
        {
            break;
        }

        if(ES_RETURN == context->getExecuteState() || ES_BREAK == context->getExecuteState())
        {
            break;
        }
    }

    if (ES_BREAK == context->getExecuteState())
    {
        context->setExecuteState(ES_RUN);
    }

    if(m_bHasBlock)
    {
        context->decBreakBlockNum();
    }
}


IfblockSyntaxNode::IfblockSyntaxNode()
{
    setType(ESN_IF);
}

void IfblockSyntaxNode::display()
{
    size_t childsNum = getChildNum();
    childsNum = (childsNum % 2) == 0 ? childsNum : (childsNum-1);
    size_t i = 0;
    for(; i < childsNum;)
    {
        displayAChild(i++);
        printf("\n");
        displayAChild(i++);
    }

    if (getChildNum() % 2 == 1)
    {
        displayAChild(i);
    }
}

void IfblockSyntaxNode::execute(Context* context)
{
    DObject& eax = context->getEax();

    size_t childsNum = getChildNum();
    for(size_t i = 0; i < childsNum; ++i)
    {
        executeAChild(i, context);
        if(eax.data.int_data != 0)
        {
            executeAChild(++i, context);
            break;
        }
        else
        {
            ++i;
        }
    }
}

CallfuncSyntaxNode::CallfuncSyntaxNode() : m_funcIndex(-1)
{
    setType(ESN_CALLFUNC);
}

void CallfuncSyntaxNode::display()
{
    printf("%s", getVarName());
    for(size_t i = 0; i < getChildNum(); ++i)
    {
        displayAChild(i);
        if(i != (getChildNum()-1))
        {
            printf("\n");
        }
    }
    printf("\n");
}

void CallfuncSyntaxNode::build(Context* context)
{
    m_funcIndex = gBuildData.getGlobalBuildIndex(getVarName());
    assert(-1 != m_funcIndex);

    AbstractSyntaxNode::build(context);
}

void CallfuncSyntaxNode::execute(Context* context)
{
    DObject* object = context->findGlobalObject(m_funcIndex);
    if(NULL != object)
    {
        _pushArg(context);
        context->executeScriptFun(*object);
    }
    else
    {
        printf("%s\n", getVarName());
    }
}

void CallfuncSyntaxNode::_pushArg(Context* context)
{
    DObject& eax = context->getEax();

    context->clearArgv();
    for(size_t i = 0; i < getChildNum(); ++i)
    {
        executeAChild(i, context);
        if (OT_ERROR == eax.type)
        {
            break;
        }
        context->pushArg(eax);
    }
}

FuncDefineSyntaxNode::FuncDefineSyntaxNode()
{
    setType(ESN_FUNC_DEFINE);
    m_localObjectSize = 0;
}

int FuncDefineSyntaxNode::getLocalObjectSize()
{
    return m_localObjectSize;
}

void FuncDefineSyntaxNode::display()
{
    printf("%s\n", getVarName());
    displayAChild(0);
    printf("\n");
    displayAChild(1);
    printf("\n");
}

void FuncDefineSyntaxNode::build(Context* context)
{
    DObject v = value();
    int index = gBuildData.insertGlobalBuildData(getVarName());

    context->insertGlobalObject(index, v);

    context->insertNameIndexPair(getVarName(), index);

    gBuildData.setInFunc(true);

    AbstractSyntaxNode::build(context);

    gBuildData.setInFunc(false);

    m_localObjectSize = gBuildData.getCurrentLocalListSize();
    gBuildData.clearLocalList();
}

DObject    FuncDefineSyntaxNode::value()
{
    DObject ojbect;
    ojbect.type = OT_PFN;
    ojbect.data.ptr_data = (void*)this;
    return ojbect;
}

void FuncDefineSyntaxNode::execute(Context* context)
{
    executeAChild(0, context);
    executeAChild(1, context);
    context->clearArgv();
}

FuncArgListSyntaxNode::FuncArgListSyntaxNode()
{
    setType(ESN_FUNC_ARG);
}

FuncArgListSyntaxNode::~FuncArgListSyntaxNode()
{
    for(ARGLIST_TYPE::iterator it = arglist.begin(); it != arglist.end(); ++it)
    {
        delete *it;
    }
}

void FuncArgListSyntaxNode::addArg(const char* str)
{
    int len = strlen(str) + 1;
    char* temp = new char[len];

    if(NULL != temp)
    {
        memset(temp, '\0', len);
        strcpy(temp, str);
        arglist.push_back(temp);
    }
}

void FuncArgListSyntaxNode::display()
{
    if(arglist.size() > 0)
    {
        char* back = arglist.back();
        for(ARGLIST_TYPE::iterator it = arglist.begin(); it != arglist.end(); ++it)
        {
            printf("%s", *it);
            if(back != *it)
            {
                printf(",");
            }
        }
    }

    printf(")\n");
}

void FuncArgListSyntaxNode::build(Context *context)
{
    for(ARGLIST_TYPE::iterator it = arglist.begin(); it != arglist.end(); ++it)
    {
        gBuildData.insertLocalBuildData(*it);
    }
}

void FuncArgListSyntaxNode::execute(Context* context)
{
    if(arglist.size() > 0)
    {
        DObject arg;
        int i = 0;

        if(context->getArgvSize() != arglist.size())
        {
            printf("error \n");
        }
        else
        {
            for(ARGLIST_TYPE::iterator it = arglist.begin(); it != arglist.end(); ++it, ++i)
            {
                if(context->getArg(i, arg))
                {
                    context->insertLocalObject(i, arg);
                }
                else
                {
                    break;
                }
            }
        }
    }
}

OpSyntaxNode::OpSyntaxNode()
{
    setType(ESN_OP);
}

void OpSyntaxNode::setOPToken(EToken token)
{
    m_opToken = token;
}

EToken OpSyntaxNode::getOPToken()
{
    return m_opToken;
}

void OpSyntaxNode::executeOP(DObject left, AbstractSyntaxNode* node, Context* context)
{
    DObject& eax = context->getEax();

    assert(NULL != node);
    node->execute(context);

    if(left.type == eax.type)
    {
        _executeOP(left, eax);
        eax = left;
    }
    else
    {
        printf("\n");
    }
}

void OpSyntaxNode::_executeOP(DObject& ret1, DObject& ret2)
{
    int& ret1intdata = ret1.data.int_data;
    int& ret2intdata = ret2.data.int_data;

    switch(m_opToken)
    {
    case ETOKEN_A:
        ret1intdata = ret2intdata;
        break;
    case ETOKEN_PLUS:
        ret1intdata += ret2intdata;
        break;
    case ETOKEN_PE:
        ret1intdata = (ret1intdata += ret2intdata);
        break;
    case ETOKEN_MINUS:
        ret1intdata -= ret2intdata;
        break;
    case ETOKEN_ME:
        ret1intdata = (ret1intdata -= ret2intdata);
        break;
    case ETOKEN_MUL:
        ret1intdata *= ret2intdata;
        break;
    case ETOKEN_DIV:
        ret1intdata /= ret2intdata;
        break;
    case ETOKEN_G:
        ret1intdata = (ret1intdata > ret2intdata);
        break;
    case ETOKEN_L:
        ret1intdata = (ret1intdata < ret2intdata);
        break;
    case ETOKEN_E:
        if (ret1.type == ret2.type)
        {
            if (ret1.type == OT_STRING)
            {
                ret1intdata = strcmp((const char*)ret1.data.ptr_data, (const char*)ret2.data.ptr_data) == 0;
            }
            else
            {
                ret1intdata = (ret1intdata == ret2intdata);
            }
        }
        break;
    case ETOKEN_NE:
        if (ret1.type == ret2.type)
        {
            if (ret1.type == OT_STRING)
            {
                ret1intdata = strcmp((const char*)ret1.data.ptr_data, (const char*)ret2.data.ptr_data) != 0;
            }
            else
            {
                ret1intdata = (ret1intdata != ret2intdata);
            }
        }
        break;
    case ETOKEN_LE:
        ret1intdata = (ret1intdata <= ret2intdata);
        break;
    case ETOKEN_GE:
        ret1intdata = (ret1intdata >= ret2intdata);
        break;
    case ETOKEN_OR:
        ret1intdata = (ret1intdata || ret2intdata);
        break;
    case ETOKEN_AND:
        ret1intdata = (ret1intdata && ret2intdata);
        break;
    default:
        break;
    }
}

void OpSyntaxNode::display()
{
    switch(m_opToken)
    {
    case ETOKEN_A:
        printf(" = ");
        break;
    case ETOKEN_PLUS:
        printf(" + ");
        break;
    case ETOKEN_PE:
        printf(" += ");
        break;
    case ETOKEN_PP:
        printf(" ++ ");
        break;
    case ETOKEN_MINUS:
        printf(" - ");
        break;
    case ETOKEN_ME:
        printf(" -= ");
        break;
    case ETOKEN_MM:
        printf(" -- ");
        break;
    case ETOKEN_MUL:
        printf(" * ");
        break;
    case ETOKEN_DIV:
        printf(" / ");
        break;
    case ETOKEN_G:
        printf(" > ");
        break;
    case ETOKEN_L:
        printf(" < ");
        break;
    case ETOKEN_E:
        printf(" == ");
        break;
    case ETOKEN_NE:
        printf(" != ");
        break;
    case ETOKEN_LE:
        printf(" <= ");
        break;
    case ETOKEN_GE:
        printf(" >= ");
        break;
    case ETOKEN_OR:
        printf(" || ");
        break;
    case ETOKEN_AND:
        printf(" && ");
        break;
    default:
        break;
    }
}

LeftOPSyntaxNode::LeftOPSyntaxNode(OpSyntaxNode* op, AbstractSyntaxNode* data) : m_op(op),m_data(data),m_varIndex(-1),m_isLocalRef(true)
{
}

void LeftOPSyntaxNode::build(Context* context)
{
    m_varIndex = gBuildData.getLocalBuildIndex(m_data->getVarName());
    if (-1 == m_varIndex)
    {
        m_varIndex = gBuildData.getGlobalBuildIndex(m_data->getVarName());
        m_isLocalRef = false;
    }
}

void LeftOPSyntaxNode::execute(Context* context)
{
    assert(-1 != m_varIndex);
    
    DObject* object = NULL;

    if(m_isLocalRef)
    {
        object = context->findLocalObject(m_varIndex);
    }
    else
    {
        object = context->findGlobalObject(m_varIndex);
    }

    assert(NULL != object);
    assert(object->type == OT_NUM);

    DObject& eax = context->getEax();

    eax = *object;

    switch(m_op->getOPToken())
    {
    case ETOKEN_PP:
        ++(object->data.int_data);
        break;
    case ETOKEN_MM:
        --(object->data.int_data);
        break;
    default:
        break;
    }
}

RightOPSyntaxNode::RightOPSyntaxNode(OpSyntaxNode* op, AbstractSyntaxNode* data) : m_op(op),m_data(data),m_varIndex(-1),m_isLocalRef(true)
{

}

void RightOPSyntaxNode::build(Context* context)
{
    m_varIndex = gBuildData.getLocalBuildIndex(m_data->getVarName());
    if (-1 == m_varIndex)
    {
        m_varIndex = gBuildData.getGlobalBuildIndex(m_data->getVarName());
        m_isLocalRef = false;
    }
}

void RightOPSyntaxNode::execute(Context* context)
{
    DObject* object = NULL;

    if(m_isLocalRef)
    {
        object = context->findLocalObject(m_varIndex);
    }
    else
    {
        object = context->findGlobalObject(m_varIndex);
    }

    assert(NULL != object);
    assert(object->type == OT_NUM);

    DObject& eax = context->getEax();

    switch(m_op->getOPToken())
    {
    case ETOKEN_PP:
        ++(object->data.int_data);
        break;
    case ETOKEN_MM:
        --(object->data.int_data);
        break;
    default:
        break;
    }

    eax = *object;
}

CalculateDefineSyntaxNode::CalculateDefineSyntaxNode()
{
    setType(ESN_CALCULATE);
}
void CalculateDefineSyntaxNode::display()
{
    displayChilds();
}

void CalculateDefineSyntaxNode::build(Context* context)
{
    CHILD_LIST_TYPE    temp_childs;

    size_t index = 0;
    size_t childNum = getChildNum();

    while(index < childNum)
    {
        AbstractSyntaxNode* child1 = (AbstractSyntaxNode*)getAChild(index++);
        assert(NULL != child1);
        AbstractSyntaxNode* child2 = NULL;

        if(child1->getType() == ESN_VARREF)
        {
            child2 = (AbstractSyntaxNode*)getAChild(index++);

            if(NULL != child2 && child2->getType() == ESN_OP)
            {
                OpSyntaxNode* child2op = (OpSyntaxNode*)child2;
                if(child2op->getOPToken() == ETOKEN_PP || child2op->getOPToken() == ETOKEN_MM)
                {
                    temp_childs.push_back(new LeftOPSyntaxNode(child2op, child1));
                    child1 = NULL;
                    child2 = NULL;
                }
            }
        }
        else if(child1->getType() == ESN_OP)
        {
            child2 = (AbstractSyntaxNode*)getAChild(index++);

            if(NULL != child2 && child2->getType() == ESN_VARREF)
            {
                OpSyntaxNode* child1op = (OpSyntaxNode*)child1;
                if(child1op->getOPToken() == ETOKEN_PP || child1op->getOPToken() == ETOKEN_MM)
                {
                    temp_childs.push_back(new RightOPSyntaxNode(child1op, child2));
                    child1 = NULL;
                    child2 = NULL;
                }
            }
        }

        if(NULL != child1)
        {
            assert(child1->getType() != ESN_OP);
            temp_childs.push_back(child1);
        }

        if(NULL == child2)
        {
            child2 = (AbstractSyntaxNode*)getAChild(index++);
        }

        if(NULL == child2)
        {
            break;
        }
        else
        {
            assert(child2->getType() == ESN_OP);
            temp_childs.push_back(child2);
        }
    }

    setChildsList(temp_childs);

    AbstractSyntaxNode::build(context);
}

void CalculateDefineSyntaxNode::execute(Context* context)
{
    size_t    index = 0;
    DObject& eax = context->getEax();

    executeAChild(index++, context);
    size_t childNum = getChildNum();

    while(index < childNum)
    {
        OpSyntaxNode* op = (OpSyntaxNode*)getAChild(index++);
        assert(NULL != op);

        op->executeOP(eax, getAChild(index++), context);
    }
}

VarRefSyntaxNode::VarRefSyntaxNode()
{
    setType(ESN_VARREF);
    m_isLocalRef = true;
    m_index = -1;
}

void VarRefSyntaxNode::display()
{
    printf("%s", getVarName());
}

void VarRefSyntaxNode::build(Context* context)
{
    m_index = gBuildData.getLocalBuildIndex(getVarName());
    if (-1 == m_index)
    {
        m_index = gBuildData.getGlobalBuildIndex(getVarName());
        m_isLocalRef = false;
    }
}

void VarRefSyntaxNode::execute(Context* context)
{
    DObject& eax = context->getEax();

    eax.data.int_data = 0;
    eax.type = OT_ERROR;

    DObject* object = NULL;

    if (m_isLocalRef)
    {
        object = context->findLocalObject(m_index);
    }
    else
    {
        object = context->findGlobalObject(m_index);
    }

    if(NULL != object)
    {
        eax = *object;
    }
}

ConstStrSyntaxNode::ConstStrSyntaxNode()
{
    setType(ESN_CONST_STR);
}

void ConstStrSyntaxNode::display()
{
    printf("%s", getVarName());
}

void ConstStrSyntaxNode::execute(Context* context)
{
    DObject& eax = context->getEax();

    eax.type = OT_STRING;
    eax.data.ptr_data = (void*)getVarName();
}

ConstNumSyntaxNode::ConstNumSyntaxNode()
{
    this->m_value = 0;
    setType(ESN_CONST_NUM);
}

void ConstNumSyntaxNode::setValue(int avalue)
{
    this->m_value = avalue;
}

void ConstNumSyntaxNode::display()
{
    printf("%d", m_value);
}

void ConstNumSyntaxNode::execute(Context* context)
{
    DObject& eax = context->getEax();

    eax.type = OT_NUM;
    eax.data.int_data = m_value;
}

OPESyntaxNode::OPESyntaxNode(EToken token) : m_optoken(token), m_isLocalRef(true), m_index(-1)
{
    setType(token == ETOKEN_ME ? ESN_ME : ESN_PE);
}

void OPESyntaxNode::build(Context* context)
{
    m_index = gBuildData.getLocalBuildIndex(getVarName());
    if (-1 == m_index)
    {
        m_index = gBuildData.getGlobalBuildIndex(getVarName());
        m_isLocalRef = false;
    }
}

void OPESyntaxNode::execute(Context* context)
{
    DObject& eax = context->getEax();
    executeAChild(0, context);

    DObject* refObject = NULL;

    if(m_isLocalRef)
    {
        refObject = context->findLocalObject(m_index);
    }
    else
    {
        refObject =context->findGlobalObject(m_index);
    }
    
    assert(NULL != refObject);

    if (NULL != refObject)
    {
        if (ETOKEN_PE == m_optoken)
        {
            refObject->data.int_data += eax.data.int_data;
        }
        else if (ETOKEN_ME == m_optoken)
        {
            refObject->data.int_data -= eax.data.int_data;
        }
        else
        {
            return;
        }

        eax = *refObject;
    }
}

InstanceSyntaxNode::InstanceSyntaxNode()
{
    setType(ESN_ROOT);
}

void InstanceSyntaxNode::display()
{
    displayChilds();
}

NativeFunctionSyntaxNode::NativeFunctionSyntaxNode(native_fn pfn) : m_pfn(pfn)
{
    setType(ESN_NATIVE_FUNC);
}

void NativeFunctionSyntaxNode::execute(Context* context)
{
    assert(NULL != m_pfn);
    m_pfn(context);
    context->clearArgv();
}
