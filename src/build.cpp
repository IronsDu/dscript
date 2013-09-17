#include "build.h"
#include <assert.h>

BuildData gBuildData;

BuildData::BuildData()
{
    m_globalListSize = 0;
    m_localListSize = 0;
    m_isInFunc = false;
}

int BuildData::insertGlobalBuildData(const char* name)
{
    STR_MAP_TYPE::iterator it = m_globalNameList.find(name);
    assert(it == m_globalNameList.end());

    m_globalNameList[name] = m_globalListSize;

    return m_globalListSize++;
}

int BuildData::insertLocalBuildData(const char* name)
{
    STR_MAP_TYPE::iterator it = m_localNameList.find(name);
    if(it == m_localNameList.end())
    {
        m_localNameList[name] = m_localListSize;
        return m_localListSize++;
    }
    else
    {
        return it->second;
    }
}

int BuildData::getGlobalBuildIndex(const char* name)
{
    STR_MAP_TYPE::iterator it = m_globalNameList.find(name);
    assert(it != m_globalNameList.end());

    return it->second;
}

int BuildData::getLocalBuildIndex(const char* name)
{
    STR_MAP_TYPE::iterator it = m_localNameList.find(name);
    
    if(it != m_localNameList.end())
    {
        return it->second;
    }
    else
    {
        return -1;
    }
}

int BuildData::getCurrentLocalListSize()
{
    return m_localListSize;
}

bool BuildData::isInFunc()
{
    return m_isInFunc;
}

void BuildData::setInFunc(bool bInFunc)
{
    m_isInFunc = bInFunc;
}

void BuildData::clearGlobalList()
{
    m_globalListSize = 0;
    m_globalNameList.clear();
}

void BuildData::clearLocalList()
{
    m_localListSize = 0;
    m_localNameList.clear();
}