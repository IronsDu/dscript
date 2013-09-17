#ifndef _DSCRIPT_H
#define _DSCRIPT_H

//DScript内置类型
enum EOJBECT_TYPE
{
    OT_ERROR,
    OT_NUM,
    OT_STRING,
    OT_PFN,
};

//DScript中脚本变量的类型定义
struct DObject
{
    DObject()
    {
        type = OT_ERROR;
        data.int_data = 0;
    }

    EOJBECT_TYPE    type;   //变量类型
    union
    {
        int     int_data;
        void*   ptr_data;
    }data;                  //变量值
};

#endif
