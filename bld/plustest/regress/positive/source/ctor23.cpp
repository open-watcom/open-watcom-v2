#include "fail.h"
#include <stdarg.h>  
  
struct a {  
    int b;  
    a(int);
};  
  
a::a(int b) : b(b)
{  
}  
  
int f(int x,...)  
{  
    a v(__LINE__);
    va_list p;  

    va_start( p, x );
    v = va_arg( p, a );
    va_end( p );

    return v.b;  
}  
 
int main()
{ 
   if( f( 1, a(__LINE__) ) != __LINE__ ) fail(__LINE__);
   if( f( 1, a(__LINE__) ) != __LINE__ ) fail(__LINE__);
   if( f( 1, a(__LINE__) ) != __LINE__ ) fail(__LINE__);
   if( f( 1, a(__LINE__) ) != __LINE__ ) fail(__LINE__);
   _PASS;
} 
