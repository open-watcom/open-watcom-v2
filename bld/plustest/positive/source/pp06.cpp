#include "fail.h"

void recursive_parm( int v )
{
    if( v != 1 && v != 2 ) fail(__LINE__);
}

int recursive_var;

#define intercept(f)  ( f )

#define recursive_parm(s)     intercept(recursive_parm(s))

#define recursive_var       intercept(recursive_var)

int main()
{
    int s = 1;
    
    recursive_parm(s);
    s = 2;
    recursive_parm(s);
    if( recursive_var != 0 ) fail(__LINE__);
    ++recursive_var;
    if( recursive_var != 1 ) fail(__LINE__);
    _PASS;
}
