// MAIN -- mainline for MEMBFUN example

#include <stdio.h>

// Class S illustrates how to declare both static and non-static member
// functions with a pragma which supplies the hardware instructions for
// the function.  The functions are made inline, although there is no
// need to do so.  The examples are for an INTEL 386 (or higher) architecture.

// This pragma will supply the instructions for non-static member "foo" in
// class S.  The compiled call will have three parameters: the first is the
// "this" pointer and the next two will be the supplied arguments.
//
#pragma aux foo_asm             \
    = "add eax,edx"             \
      "add eax,ebx"             \
    parm[ eax][ edx][ ebx ]     \
    value [eax]                 \
    ;

// This pragma will supply the instructions for static member "bar" in
// class S.
//
#pragma aux bar_asm             \
    = "add eax,edx"             \
    parm [eax] [edx ]           \
    value [ eax ]               \
    ;

// Declare class S with foo and bar as above.  The functions goo and woo are
// given as equivalent C++ functions coded entirely in C++.
//
struct S {
    int __pragma("foo_asm") foo( int, int );
    static int __pragma("bar_asm") bar( char, char );
    int goo( int, int );
    static int woo( char, char );
};

int S::goo( int a, int b )
{
    return (int)this + a + b;
}

int S::woo( char a, char b )
{
    return a + b;
}

int value1;
int value2;
int value3;
int value4;

void use_asm( S *p )
{
    value1 = p->foo( 1, 2 );
    value2 = p->bar( 'a', 'b' );
}

//
// following should generate equivalent code to above
//
void use_cpp( S *p )
{
    value3 = p->goo( 1, 2 );
    value4 = p->woo( 'a', 'b' );
}

//
// call the assembler and equivalent C++ functions.
//
int test_s()
{
    S s_val;
    use_asm( &s_val );
    use_cpp( &s_val );
    int retn = 0;
    if( value1 != value3 ) {
        puts( "FAILED test_s: value1 != value2\n" );
        retn = 1;
    }
    if( value2 != value4 ) {
        puts( "FAILED test_s: value2 != value4\n" );
        retn = 1;
    }
    return retn;
}



//
// The following illustrates how to implement member functions with
// alternative names.
//

#pragma aux t_foo_asm "FOO";
#pragma aux t_bar_asm "BAR";
struct T {
    int __pragma("t_foo_asm") foo( int, int );
    static int __pragma("t_bar_asm") bar( char, char );
};

int T::foo( int v1, int v2 )
{
    return v1 * v2 + (int)this;
}

int T::bar( char v1, char v2 )
{
    return v1 * v2;
}

extern "C" {    // functions in CFUN.C
    int CfunFoo( void*, int, int );
    int CfunBar( char, char );
};

int test_t()
{
    T t_val;
    value1 = t_val.foo( 1, 2 );
    value2 = t_val.bar( 'a', 'b' );
    value3 = CfunFoo( (void*)&t_val, 1, 2 );
    value4 = CfunBar( 'a', 'b' );
    int retn = 0;
    if( value1 != value3 ) {
        puts( "FAILED test_t: value1 != value2\n" );
        retn = 1;
    }
    if( value2 != value4 ) {
        puts( "FAILED test_t: value2 != value4\n" );
        retn = 1;
    }
    return retn;
}


//
// mainline
//
int main()
{
    int retn = 0;
    retn |= test_s();
    retn |= test_t();
    if( 0 == retn ) {
        puts( "Passed\n" );
    }
    return retn;
}
