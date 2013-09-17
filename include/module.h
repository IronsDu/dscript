#ifndef _MODULE_H
#define _MODULE_H

#include <list>

namespace DScript
{
    //TODO::重要变更
    //当把变量表从map储存结构改为vector后,registerNativeFunction函数的调用必须在parser之前.

    class AbstractSyntaxNode;
    class Context;

    typedef void (*native_fn)(Context*);

    //代码模块
    class Module
    {
    public:
        Module();
        ~Module();

    public:
        virtual void        onParserError();

    public:
        bool                parser(const char* filename);
        const char*&        getParserPos();                                             //获取当前解析位置
        void                addCurrentSyntaxNodeChild(AbstractSyntaxNode* node);        //添加一个子节点到当前节点
        void                pushCurrentSyntaxNode(AbstractSyntaxNode* node);            //保存node节点,并设置node为当前节点
        AbstractSyntaxNode* popCurrentSyntaxNode();                                     //还原上一个当前节点

        Context*            CreateContext();                                            //创建一个执行环境
        
        bool                registerNativeFunction(const char* name, native_fn pfn);    //注册本地函数pfn,其在脚本中名为name


    private:
        const char*         m_parserPos;                        //当前解析位置
        char*               m_sourceBuffer;                     //源码缓冲区

        AbstractSyntaxNode* m_currentNode;                      //当前语法树节点

        std::list<AbstractSyntaxNode*>    m_currentNodeList;    //当前语法树节点列表

        Context*            m_defaultContext;                   //默认执行环境,用于解析脚本时保存所产生全局变量表
                                                                //创建新的执行环境,均从默认执行环境进行拷贝
    };
}

#endif
