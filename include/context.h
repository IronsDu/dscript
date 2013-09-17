#ifndef _CONTEXT_H
#define _CONTEXT_H

#include <list>
#include <vector>
#include "dscript.h"
#include "strmap.h"

namespace DScript
{
enum EEXECUTE_STATE
{
    ES_RUN,
    ES_BREAK,
    ES_RETURN,
};

class Context
{
public:
    Context();
    ~Context();

    void            copy(const Context*);                    //copy参数Context

    void            insertLocalObject(int index, const DObject& object);    //插入一个值到局部变量表的index位置

    DObject*        findLocalObject(int index);                //查找局部变量表中index位置的值

    void            insertGlobalObject(int index, const DObject& object);    //插入一个值到全局变量表的index位置
    DObject*        findGlobalObject(int index);                //查找全局变量表中index位置的值

    void            insertNameIndex(const char* name, int index);        //插入一个<符号和索引>
    int             getNameIndex(const char* name);

    void            pushArg(const DObject&);                //调用函数时,压入参数
    bool            getArg(size_t index, DObject&);                //获取实参列表中index位置的参数值
    size_t          getArgvSize() const;                    //获取实参列表的大小
    void            clearArgv();                        //销毁实参列表

    void            executeScriptFun(const char* funcName);            //执行名为funcName的脚本函数
    void            executeScriptFun(const DObject& funcObject);        //执行funcObject所指向的脚本函数的语法树节点

    DObject&        getEax();                        //获取当前执行的最后求值

    void            setExecuteState(EEXECUTE_STATE state);            //设置当前环境的执行状态
    EEXECUTE_STATE  getExecuteState() const;

    int             getBreakBlockNum() const;
    void            incBreakBlockNum();
    void            decBreakBlockNum();

    void            insertNameIndexPair(const char* name, int index);

public:
    virtual void    onExecuteError();

private:

    void            newLocalObjectList(int localObjectListSize);    //调用函数时,新建局部变量表,其大小为localObjectListSize
    void            destroyCurrentLocalObjectList();                //销毁当前局部变量表,并设置上一个局部变量表为当前局部变量表
    int             getFuncNameIndex(const char* funcName);         //查询funcName所对应的函数在全局变量表中的索引

private:

    typedef std::vector<DObject> OBJCT_LIST_TYPE;

    std::list<OBJCT_LIST_TYPE* >        m_localObjectListStack;     //局部变量表栈

    OBJCT_LIST_TYPE*                    m_currentLocalObjectList;   //当前局部变量表
    OBJCT_LIST_TYPE                     m_globalObjectList;         //全局变量表

    EEXECUTE_STATE                      m_executeState;             //当前环境的执行状态
    DObject                             m_eax;                      //当前的最终求值

    std::vector<int>                    m_whileBlockNumv;           //当前函数执行所含有的while语句个数
    int                                 m_breakBlockNum;            //当前函数执行所含有的break语句个数

    typedef std::vector<DObject> ARGV_TYPE;
    ARGV_TYPE                           m_argv;                     //实参列表

    //TODO::可以将此map转移到Module类中,因为同一个Module所产生的Context,此map是相等的
    STR_MAP_TYPE                        m_funcNameIndexMap;         //<符号和索引>表,用于根据函数名查询其函数在全局变量表中的索引
};
}

#endif
