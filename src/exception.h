#pragma once
#include <cstringt.h>

namespace serial_notifier
{

class Exception
{
public:
    Exception()
    {}

    Exception(const CString & what) :
        _what(what)
    {}

    inline CString what()
    {
        return _what;
    }
private:

    CString _what;
};

}
