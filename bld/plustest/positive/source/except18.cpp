#include "fail.h"
#include <iostream.h>
#include <fstream.h>
#include <string.h>

int noMore;

void *operator new( size_t amt )
{
    if( noMore ) {
        throw 'x';
    }
    return malloc( amt );
}
void *operator new[]( size_t amt )
{
    if( noMore ) {
        throw 'x';
    }
    return malloc( amt );
}

struct S {
    char *p;
    S( char *s ) {
        size_t len = strlen( s );
        p = new char[len+1];
        strcpy( p, s );
    }
    ~S() {
        delete [] p;
    }
    S( S const &s ) {
        size_t len = strlen( s.p );
        p = new char[len+1];
        strcpy( p, s.p );
    }
    void operator =( S const &s ) {
        if( this != &s ) {
            delete [] p;
            size_t len = strlen( s.p );
            p = new char[len+1];
            strcpy( p, s.p );
        }
    }
    void operator +=( char *n ) {
        char *op = p;
        size_t len = strlen( n ) + strlen( op );
        p = new char[len+1];
        strcpy( p, op );
        strcat( p, n );
        delete [] op;
    }
};

int sam() {
    static int s = 0;
    ifstream in_file;

    in_file.exceptions( ios::eofbit );
    in_file.exceptions( ios::failbit );
    try {
        ++s;
        in_file.open( "not_here.$$$" );
        ++s;
    } catch( ... ) {
        --s;
    }
    if( s != 0 ) fail(__LINE__);
    try {
        ++s;
        in_file.close();
        ++s;
    } catch( ... ) {
        --s;
    }
    if( s != 0 ) fail(__LINE__);
    ++noMore;
    try {
        ++s;
        S x( "asdf" );
        x += "qwer";
        ++s;
    } catch( ... ) {
        --s;
    }
    static int infinite_loop_check;
    if( infinite_loop_check++ > 1 ) abort();
    if( s != 0 ) fail(__LINE__);
    delete ( new char[10] );
    return -1;
}

int main() {
    int x = 0;
    try {
        sam();
        fail(__LINE__);
    } catch( ... ) {
        ++x;
    }
    if( x != 1 ) fail(__LINE__);
    _PASS;
}
