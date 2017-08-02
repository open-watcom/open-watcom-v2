#include "fail.h"

#if __WATCOM_REVISION__ >= 8
struct Integer {
    Integer(int val) { _value = val; }
    ~Integer() { _value = -1; }
    int _value;
};

int Bug(const Integer &x = 0)
{
    return x._value;
}

int main()
{
    if( Bug(1) != 1 ) fail(__LINE__);
    if( Bug() != 0 ) fail(__LINE__);
    _PASS;
}
#else
ALWAYS_PASS
#endif
