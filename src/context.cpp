#include "context.h"
#include <assert.h>
#include <stdio.h>

#include "syntaxnode.h"
#include "build.h"

using namespace DScript;

Context::Context()
{
    m_executeState = ES_RUN;
    m_breakBlockNum = 0;
    m_currentLocalObjectList = NULL;
}

Context::~Context()
{
    for(std::list<OBJCT_LIST_TYPE*>::iterator it = m_localObjectListStack.begin(); it != m_localObjectListStack.end(); ++it)
    {
        delete (*it);
    }
}

void Context::copy(const Context* context)
{
    m_globalObjectList = context->m_globalObjectList;
    m_funcNameIndexMap = context->m_funcNameIndexMap;
}

void Context::insertLocalObject(int index, const DObject& object)
{
    if(m_currentLocalObjectList->size() <= index)
    {
        (*m_currentLocalObjectList).resize(index + 1);
    }
    (*m_currentLocalObjectList)[index] = object;
}

DObject* Context::findLocalObject(int index)
{
    assert(m_currentLocalObjectList->size() > index);
    return &((*m_currentLocalObjectList)[index]);
}

void Context::insertGlobalObject(int index, const DObject& object)
{
    if(m_globalObjectList.size() <= index)
    {
        m_globalObjectList.resize(index + 1);
    }
    
    m_globalObjectList[index] = object;
}

DObject* Context::findGlobalObject(int index)
{
    assert(m_globalObjectList.size() > index);
    return &m_globalObjectList[index];
}

void Context::pushArg(const DObject& arg)
{
    m_argv.push_back(arg);
}

bool Context::getArg(size_t index, DObject& ret)
{
    if(m_argv.size() > index)
    {
        ret = m_argv[index];

        return true;
    }
    else
    {
        return false;
    }
}

size_t Context::getArgvSize() const
{
    return m_argv.size();
}

void Context::clearArgv()
{
    m_argv.clear();
}

void Context::executeScriptFun(const char* name)
{    
    DObject* object = findGlobalObject(getFuncNameIndex(name));

    if(NULL != object)
    {
        executeScriptFun(*object);
    }
}

void Context::executeScriptFun(const DObject& funcObject)
{
    assert(OT_PFN == funcObject.type);

    {
        m_whileBlockNumv.push_back(m_breakBlockNum);
        m_breakBlockNum = 0;
    }

    AbstractSyntaxNode* node = (AbstractSyntaxNode*)(funcObject.data.ptr_data);

    FuncDefineSyntaxNode* funcDefine = (FuncDefineSyntaxNode*)node;
    newLocalObjectList(funcDefine->getLocalObjectSize());
    node->execute(this);

    destroyCurrentLocalObjectList();

    {
        m_breakBlockNum = m_whileBlockNumv.back();
        m_whileBlockNumv.pop_back();
        m_executeState = ES_RUN;
    }
}

DObject& Context::getEax()
{
    return m_eax;
}

void Context::setExecuteState(EEXECUTE_STATE state)
{
    m_executeState = state;
}

EEXECUTE_STATE Context::getExecuteState() const
{
    return m_executeState;
}

int Context::getBreakBlockNum() const
{
    return m_breakBlockNum;
}

void Context::incBreakBlockNum()
{
    m_breakBlockNum ++;
}

void Context::decBreakBlockNum()
{
    m_breakBlockNum ++;
}

void Context::onExecuteError()
{

}

void Context::insertNameIndexPair(const char* name, int index)
{
    m_funcNameIndexMap[name] = index;
}

void Context::newLocalObjectList(int localObjectListSize)
{
    m_currentLocalObjectList = new OBJCT_LIST_TYPE();

    m_localObjectListStack.push_back(m_currentLocalObjectList);
}

void Context::destroyCurrentLocalObjectList()
{
    delete m_currentLocalObjectList;

    m_localObjectListStack.pop_back();
    if (m_localObjectListStack.size() > 0)
    {
        m_currentLocalObjectList = m_localObjectListStack.back();
    }
    else
    {
        m_currentLocalObjectList = NULL;
    }
}

int Context::getFuncNameIndex(const char* funcName)
{
    //return gBuildData.getGlobalBuildIndex(name)

    STR_MAP_TYPE::iterator it = m_funcNameIndexMap.find(funcName);
    assert(it != m_funcNameIndexMap.end());

    return it->second;
}
