#include "fail.h"
#include <string.h>

#if defined(__AXP__)
#define UNALIGN __unaligned
#else
#define UNALIGN
#endif

#pragma pack(1)
struct S {
    char c;
    short s;
    long l;
    double d;
};
#pragma pack()

char usec( char UNALIGN *p ) {
    char v = *p;
    (*p)++;
    return v;
}
short uses( short UNALIGN *p ) {
    short v = *p;
    (*p)++;
    return v;
}
long usel( long UNALIGN *p ) {
    long v = *p;
    (*p)++;
    return v;
}
double used( double UNALIGN *p ) {
    double v = *p;
    (*p)++;
    return v;
}

int main() {
    S *p = new S;
    memset( p, 0, sizeof( *p ) );
    if( usec( &(p->c) ) != 0 ) fail(__LINE__);
    if( uses( &(p->s) ) != 0 ) fail(__LINE__);
    if( usel( &(p->l) ) != 0 ) fail(__LINE__);
    if( used( &(p->d) ) != 0 ) fail(__LINE__);
    if( usec( &(p->c) ) != 1 ) fail(__LINE__);
    if( uses( &(p->s) ) != 1 ) fail(__LINE__);
    if( usel( &(p->l) ) != 1 ) fail(__LINE__);
    if( used( &(p->d) ) != 1 ) fail(__LINE__);
    delete p;
    _PASS;
}
