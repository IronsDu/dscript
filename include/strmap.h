#ifndef _STRMAP_H
#define _STRMAP_H

#include <string.h>
#include <map>
#include <functional>

//字符串比较的函数对象
class StrComp : public std::binary_function<const char*,const char*, bool>
{
public:
    result_type  operator()( const first_argument_type& left,
        const second_argument_type& right ) const
    {
        return strcmp(left, right) > 0;
    }
};

typedef std::map<const char* , int, StrComp> STR_MAP_TYPE;

#endif
