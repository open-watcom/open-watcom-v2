#include "fail.h"

struct B { 
    int a,b; 
    B(int a,int b) : a(a),b(b) 
    { 
    } 
}; 
 
void f(int x,B &r,int *c)
{ 
    if( r.a != x ) fail(__LINE__);
    if( r.b != x ) fail(__LINE__);
    if( *c != x ) fail(__LINE__);
} 
 
void fa(int x,B &b,int c,...) 
{ 
    f(x,b,&c); 
} 
 
void fb(int x,B b,int c,...) 
{ 
    f(x,b,&c); 
} 
 
void fp(int x,B *b,int c,...) 
{ 
    f(x,*b,&c); 
} 
 
int main( void )
{ 
    int i = 1;

    f( i, B(i,i), &i );
    ++i;
    fa( i, B(i,i), i );
    ++i;
    fb( i, B(i,i), i );
    ++i;

    B b(i,i);

    f(i,b,&i);
    fa(i,b,i);
    fp(i,&b,i);
    fb(i,b,i);

    _PASS;
} 
