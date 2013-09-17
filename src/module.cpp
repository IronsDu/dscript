#include "module.h"
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "parser.h"
#include "syntaxnode.h"
#include "context.h"
#include "build.h"

using namespace DScript;

static char* load_file( const char *filename)
{
    char* buffer = NULL;
    int file_size = 0;

    FILE *fp = NULL;
    fp = fopen(filename, "rb");
    if(0 == fp)
    {
        return NULL;
    }

    fseek( fp, 0, SEEK_END );
    file_size = ftell( fp );
    fseek( fp, 0, SEEK_SET );

    buffer = (char*) malloc(file_size + 1);
    fread(buffer, file_size, 1, fp);
    buffer[file_size] = 0;
    fclose( fp );

    return buffer;
}

Module::Module()
{
    m_parserPos = NULL;
    m_sourceBuffer = NULL;
    m_currentNode = NULL;
    m_defaultContext = new Context;
}

Module::~Module()
{
    delete m_defaultContext;
    free((void*)m_sourceBuffer);
}

void Module::onParserError()
{

}

bool Module::registerNativeFunction(const char* name, native_fn pfn)
{
    if(NULL != m_defaultContext)
    {
        NativeFunctionSyntaxNode* pNativeFunc = new NativeFunctionSyntaxNode(pfn);
        assert(NULL != pNativeFunc);

        DObject pfnOjbect;
        pfnOjbect.type = OT_PFN;
        pfnOjbect.data.ptr_data = pNativeFunc;

        int index = gBuildData.insertGlobalBuildData(name);

        m_defaultContext->insertGlobalObject(index, pfnOjbect);

        m_defaultContext->insertNameIndexPair(name, index);

        return true;
    }
    else
    {
        return false;
    }
}

bool Module::parser(const char* filename)
{
    initAllStateHandler();

    m_sourceBuffer = load_file(filename);
    assert(NULL != m_sourceBuffer);
    if(NULL != m_sourceBuffer)
    {
        m_parserPos = m_sourceBuffer;

        AbstractStateHandler* pHandler = getStateHandler(ESTATE_INIT);
        if(NULL != pHandler)
        {
            AbstractSyntaxNode* root = pHandler->proc(this);

            root->display();
            root->build(m_defaultContext);

            return true;
        }
    }

    return false;
}

const char* & Module::getParserPos()
{
    return m_parserPos;
}

void Module::addCurrentSyntaxNodeChild(AbstractSyntaxNode* node)
{
    assert(NULL != node);
    m_currentNode->addChild(node);
}

void Module::pushCurrentSyntaxNode(AbstractSyntaxNode* node)
{
    m_currentNodeList.push_back(node);
    m_currentNode = node;
}

AbstractSyntaxNode*    Module::popCurrentSyntaxNode()
{
    AbstractSyntaxNode* old = m_currentNode;
    m_currentNodeList.pop_back();
    if(m_currentNodeList.size() >= 1)
    {
        m_currentNode = (m_currentNodeList.back());
    }

    assert(NULL != old);

    return old;
}

Context* Module::CreateContext()
{
    Context* newContext = new Context;
    if(NULL != newContext)
    {
        newContext->copy(m_defaultContext);
    }
    
    return newContext;
}
