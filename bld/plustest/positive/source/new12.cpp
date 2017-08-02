#include "fail.h"

int dummy;

struct D {
    int __u[35];
    void operator delete( void *p );
    void operator delete []( void *p );
};
struct DS {
    int __u[35];
    void operator delete( void *p, unsigned );
    void operator delete []( void *p, unsigned );
};

struct S00 {
    int __u[35];
} *s00[2];

struct S01 : D {
} *s01[2];

struct S02 : DS {
} *s02[2];

struct S10 {
    int __u[35];
    ~S10() {}
} *s10[2];

struct S11 : D {
    ~S11() {}
} *s11[2];

struct S12 : DS {
    ~S12() {}
} *s12[2];

struct S20 {
    int __u[35];
    ~S20() {}
} *s20[2];

struct S21 : D {
    ~S21() {}
} *s21[2];

struct S22 : DS {
    ~S22() {}
} *s22[2];

struct S30 {
    int __u[35];
    virtual ~S30() {}
} *s30[2];

struct S31 : D {
    virtual ~S31() {}
} *s31[2];

struct S32 : DS {
    virtual ~S32() {}
} *s32[2];

struct S40 {
    int __u[35];
    ~S40() {--dummy;}
} *s40[2];

struct S41 : D {
    ~S41() {--dummy;}
} *s41[2];

struct S42 : DS {
    ~S42() {--dummy;}
} *s42[2];

struct S50 {
    int __u[35];
    virtual ~S50() {--dummy;}
} *s50[2];

struct S51 : D {
    virtual ~S51() {--dummy;}
} *s51[2];

struct S52 : DS {
    virtual ~S52() {--dummy;}
} *s52[2];

template <class T>
    void foo( T **p ) {
	delete p[0];
	delete [] p[1];
    }

void D::operator delete( void *p )
{
    if( p != NULL ) fail(__LINE__);
}

void D::operator delete []( void *p )
{
    if( p != NULL ) fail(__LINE__);
}

void DS::operator delete( void *p, unsigned )
{
    if( p != NULL ) fail(__LINE__);
}

void DS::operator delete []( void *p, unsigned )
{
    if( p != NULL ) fail(__LINE__);
}

int main() {
    foo( s00 );
    foo( s01 );
#if __WATCOMC__ > 1060
    foo( s02 );
#endif

    foo( s10 );
    foo( s11 );
#if __WATCOMC__ > 1060
    foo( s12 );
#endif

    foo( s20 );
    foo( s21 );
#if __WATCOMC__ > 1060
    foo( s22 );
#endif

    foo( s30 );
    foo( s31 );
    foo( s32 );

    foo( s40 );
    foo( s41 );
#if __WATCOMC__ > 1060
    foo( s42 );
#endif

    foo( s50 );
    foo( s51 );
    foo( s52 );
    _PASS;
}
