#if !defined( ARCH ) || ( ARCH != 386 )
    #error system dependent test for 386
#else
#pragma warning 933 11
#pragma aux goes_byebye aborts;
#pragma aux also_goes_byebye aborts;

void x4( int, int );
void __pragma("goes_byebye") x4( int, int );

void x6( int, int );
void x6( int, int );

void __pragma("goes_byebye") x10( int, int );
void __pragma("goes_byebye") x10( int, int );

void __pragma("goes_byebye") x13( int, int );
void __pragma("also_goes_byebye") x13( int, int );

struct X15 {
    void x16( int, int );
    void x19( int, int );
    void __pragma("goes_byebye") x22( int, int );
    void __pragma("goes_byebye") x25( int, int );
};

void __pragma("goes_byebye") X15::x16( int, int )
{
}

void X15::x19( int, int )
{
}

void __pragma("goes_byebye") X15::x22( int, int )
{
}

void __pragma("also_goes_byebye") X15::x25( int, int )
{
}

void x39()
{
    void far *p;

    p = unsigned(p);    // column should be 17 not 18 in diagnostic
}

struct x46 {
    int a;
};;;;;;;;;;;;;

#include <stddef.h>

void * operator new( double );
void * operator new( char );
void * operator new( int );
void * operator new( unsigned, char );
#ifdef __386__
void * operator new( unsigned long, char );
#else
void * operator new( unsigned short, char );
#endif
void * operator new( size_t, double );

#pragma aux indep_3r3s "*";

extern "C" int (*__pragma("indep_3r3s") x65)() = 0;
extern "C" int * __pascal x66 = 0;
extern "C" int __pragma("indep_3r3s") x67()
{
    return -3;
}
extern "C" int __cdecl x71()
{
    return -3;
}

void x76() {
    void _far *p1;
    __segment seg;
    void __based( seg ) *p2;

    p2 = p1;                            // no conversion allowed; error
    p2 = (void __based(seg) *) p1;      // warns about truncation
}

void x85() {
    __segment seg1;
    __segment seg2;
    void __based( seg1 ) *p1;
    void __based( seg2 ) *p2;
    
    p2 = p1;                            // OK
}


#endif
