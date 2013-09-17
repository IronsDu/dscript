#ifndef _BUILD_H
#define _BUILD_H

#include <string.h>
#include "dscript.h"
#include "strmap.h"

class BuildData
{
public:
    BuildData();

    int         insertGlobalBuildData(const char* name);        //插入一个全局符号,并返回其索引
    int         insertLocalBuildData(const char* name);         //插入一个局部符号,并返回其索引

    int         getGlobalBuildIndex(const char* name);          //查询一个符号在全局表中的索引
    int         getLocalBuildIndex(const char* name);           //查询一个符号在局部表中的索引

    int         getCurrentLocalListSize();                      //获取当前局部表的大小

    bool        isInFunc();                                     //当前build是否处于函数内的解析
    void        setInFunc(bool);

    void        clearGlobalList();
    void        clearLocalList();

private:

    STR_MAP_TYPE            m_globalNameList;                   //全局符号表
    STR_MAP_TYPE            m_localNameList;                    //当前局部符号表

    int                     m_globalListSize;
    int                     m_localListSize;
    bool                    m_isInFunc;
};

extern BuildData gBuildData;

#endif
