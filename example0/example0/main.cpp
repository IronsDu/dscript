#include <stdio.h>
#include <stdlib.h>

#include "module.h"
#include "context.h"
#include "dscript.h"

using namespace DScript;

void print(Context* context)
{
    DObject arg;
    if(context->getArg(0, arg))
    {
        switch(arg.type)
        {
        case OT_NUM:
            printf("%d", arg.data.int_data);
            break;
        case OT_STRING:
            printf("%s", (char*)arg.data.ptr_data);
            break;
        default:
            context->onExecuteError();
        }
    }
}

void mygetc(Context* context)
{
    DObject& eax = context->getEax();

    int ch = getchar();
    eax.data.int_data = ch;
    eax.type = OT_NUM;
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        fprintf( stderr, "please usage script filename and entry function.\n");
    }
    else
    {
        Module m;
        m.registerNativeFunction("print", print);
        m.registerNativeFunction("mygetc", mygetc);

        m.parser(argv[1]);

        Context* context = m.CreateContext();
        context->executeScriptFun(argv[2]);

        getchar();
    }

    return 0;
}
