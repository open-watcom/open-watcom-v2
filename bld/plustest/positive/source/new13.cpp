#include "fail.h"

int dummy;

struct S {
    S();
};
S::S() {
    if( this == NULL ) fail(__LINE__);
}

struct SD {
    SD();
    ~SD();
};
SD::SD() {
    if( this == NULL ) fail(__LINE__);
}
SD::~SD() {
    dummy = __LINE__;
}

struct T {
    T();
    void *operator new( unsigned );
    void *operator new []( unsigned );
};
T::T() {
    if( this == NULL ) fail(__LINE__);
}
void *T::operator new( unsigned ) {
    return( NULL );
}
void *T::operator new []( unsigned ) {
    return( NULL );
}

struct TD {
    TD();
    ~TD();
    void *operator new( unsigned );
    void *operator new []( unsigned );
};
TD::TD() {
    if( this == NULL ) fail(__LINE__);
}
TD::~TD() {
    dummy = __LINE__;
}
void *TD::operator new( unsigned ) {
    return( NULL );
}
void *TD::operator new []( unsigned ) {
    return( NULL );
}

int return_null;

void *operator new( unsigned size ) {
    if( ! return_null ) {
	return( malloc( size ) );
    }
    return( NULL );
}

int main() {
    ++return_null;
    new (NULL) S;
    new (NULL) S[__LINE__];
    new S;
    new S[__LINE__];
    new SD;
    new SD[__LINE__];
    --return_null;
    if( return_null != 0 ) fail(__LINE__);
    new T;
    new T[__LINE__];
    new TD;
    new TD[__LINE__];
    _PASS;
}
