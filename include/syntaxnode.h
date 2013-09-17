#ifndef _SYNTAXNODE_H
#define _SYNTAXNODE_H

#include <assert.h>
#include <stdio.h>
#include <vector>
#include <list>

#include "token.h"
#include "dscript.h"

namespace DScript
{
    //语法树节点类型
    enum ESYNATXNODE_TYPE
    {
        ESN_ERROR,
        ESN_VAR_DEFINE,
        ESN_PE,
        ESN_ME,
        ESN_OP,
        ESN_CALCULATE,
        ESN_CONST_STR,
        ESN_CONST_NUM,
        ESN_VARREF,
        ESN_CALLFUNC,
        ESN_FUNC_DEFINE,
        ESN_FUNC_ARG,
        ESN_NATIVE_FUNC,
        ESN_BLOCK,
        ESN_IF,
        ESN_WHILE,
        ESN_RETURN,
        ESN_BREAK,
        ESN_ROOT,
    };

    class Context;

    //语法树节点基类
    class AbstractSyntaxNode
    {
    protected:
        typedef    std::vector<AbstractSyntaxNode*> CHILD_LIST_TYPE;

    public:
        AbstractSyntaxNode();

        virtual                ~AbstractSyntaxNode();

        void                    setVarName(const char* str);

        virtual void            build(Context* context);
        virtual void            display();
        virtual void            execute(Context*);

        virtual DObject         value();

        void                    addChild(AbstractSyntaxNode* pChild);

        void                    displayChilds();

        const char*             getVarName();

        size_t                  getChildNum();

        ESYNATXNODE_TYPE        getType();

    protected:
        void                    displayAChild(size_t index);

        void                    buildAChild(size_t index, Context* context);

        void                    executeAChild(size_t index, Context* context);

        AbstractSyntaxNode*     getAChild(size_t index);

        void                    setType(ESYNATXNODE_TYPE type);

        void                    setChildsList(CHILD_LIST_TYPE& newchilds);

    private:
        CHILD_LIST_TYPE         m_childs;
        char*                   varname;        //TODO::可以提到另外的类中

        ESYNATXNODE_TYPE        m_type;
    };

    class VarDefineSyntaxNode : public AbstractSyntaxNode
    {
    public:
        VarDefineSyntaxNode();

    private:
        void                display();

        void                build(Context* context);

        void                execute(Context* context);

    private:
        bool                m_isGlobalVardefine;
        int                 m_index;
    };

    class ReturnSyntaxNode : public AbstractSyntaxNode
    {
    public:
        ReturnSyntaxNode();

    private:
        void                display();

        virtual void        execute(Context* context);
    };

    class BlockSyntaxNode : public AbstractSyntaxNode
    {
    public:
        BlockSyntaxNode();

    private:
        void                display();

        virtual void        execute(Context* context);
    };

    class BreakSyntaxNode : public AbstractSyntaxNode
    {
    public:
        BreakSyntaxNode();

    private:
        void                display();

        virtual void        execute(Context* context);
    };

    class WhileBlockSyntaxNode : public AbstractSyntaxNode
    {
    public:
        WhileBlockSyntaxNode();

        void                setHasBlock();
    private:
        void                display();

        void                execute(Context* context);

    private:
        bool                m_bHasBlock;
    };

    class    IfblockSyntaxNode : public AbstractSyntaxNode
    {
    public:
        IfblockSyntaxNode();

    private:
        void                display();
        void                execute(Context* context);
    };

    class CallfuncSyntaxNode : public AbstractSyntaxNode
    {
    public:
        CallfuncSyntaxNode();

    private:
        void                display();
        void                build(Context* context);

        void                execute(Context* context);

        void                _pushArg(Context* context);

    private:
        int                 m_funcIndex;
    };

    class    FuncDefineSyntaxNode : public AbstractSyntaxNode
    {
    public:
        FuncDefineSyntaxNode();

        int                 getLocalObjectSize();

    private:
        void                display();

        void                build(Context* context);

        virtual DObject     value();

        virtual void        execute(Context* context);

    private:
        int                 m_localObjectSize;
    };

    class FuncArgListSyntaxNode : public AbstractSyntaxNode
    {
        typedef std::list<char*> ARGLIST_TYPE;

    public:
        FuncArgListSyntaxNode();
        ~FuncArgListSyntaxNode();

        void                addArg(const char* str);

    private:
        void                display();

        void                execute(Context* context);
        void                build(Context* context);
    private:
        ARGLIST_TYPE        arglist;
    };

    class OpSyntaxNode : public AbstractSyntaxNode
    {
    public:
        OpSyntaxNode();

        void                setOPToken(EToken token);

        EToken              getOPToken();

        void                executeOP(DObject left, AbstractSyntaxNode* node, Context* context);

    private:
        void                _executeOP(DObject& ret1, DObject& ret2);

        void                display();

    private:
        EToken              m_opToken;
    };

    class LeftOPSyntaxNode : public AbstractSyntaxNode
    {
    public:
        LeftOPSyntaxNode(OpSyntaxNode* op, AbstractSyntaxNode* data);

    private:
        void                    execute(Context* context);
        void                    build(Context* context);
    private:
        OpSyntaxNode*           m_op;
        AbstractSyntaxNode*     m_data;

        int                     m_varIndex;
        bool                    m_isLocalRef;
    };

    class RightOPSyntaxNode : public AbstractSyntaxNode
    {
    public:
        RightOPSyntaxNode(OpSyntaxNode* op, AbstractSyntaxNode* data);

    private:
        void                execute(Context* context);
        void                build(Context* context);

    private:
        OpSyntaxNode*           m_op;
        AbstractSyntaxNode*     m_data;

        int                     m_varIndex;
        bool                    m_isLocalRef;
    };

    class CalculateDefineSyntaxNode : public AbstractSyntaxNode
    {
    public:
        CalculateDefineSyntaxNode();

    private:
        void                display();

        void                build(Context* context);

        virtual void        execute(Context* context);
    };

    class VarRefSyntaxNode : public AbstractSyntaxNode
    {
    public:
        VarRefSyntaxNode();

    private:
        void                display();

        virtual void        execute(Context* context);
        void                build(Context* context);

        bool                m_isLocalRef;
        int                 m_index;
    };

    class ConstStrSyntaxNode : public AbstractSyntaxNode
    {
    public:
        ConstStrSyntaxNode();

    private:
        void                display();

        virtual void        execute(Context* context);
    };

    class ConstNumSyntaxNode : public AbstractSyntaxNode
    {
    public:
        ConstNumSyntaxNode();

        void                setValue(int avalue);

    private:
        void                display();

        virtual void        execute(Context* context);

    private:
        int                 m_value;
    };

    class OPESyntaxNode : public AbstractSyntaxNode
    {
    public:
        OPESyntaxNode(EToken token);

    private:
        void                execute(Context* context);
        void                build(Context* context);

    private:
        EToken              m_optoken;

        bool                m_isLocalRef;
        int                 m_index;
    };

    class InstanceSyntaxNode : public AbstractSyntaxNode
    {
    public:
        InstanceSyntaxNode();

    private:
        virtual void        display();
    };

    typedef void (*native_fn)(Context*);

    class NativeFunctionSyntaxNode : public AbstractSyntaxNode
    {
    public:
        NativeFunctionSyntaxNode(native_fn pfn);

    private:
        void                execute(Context* context);

    private:
        native_fn           m_pfn;
    };
}

#endif
