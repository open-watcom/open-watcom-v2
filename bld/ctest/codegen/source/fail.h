#ifndef _FAIL_H
#define _FAIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned errors;
extern unsigned main_terminated;

extern void fail( unsigned line );
#define _fail   (fail(__LINE__))

unsigned errors;
unsigned main_terminated;

void fail( unsigned line )
{
    ++errors;
    printf( "failure on line %u\n", line );
    if( main_terminated || errors > 5 ) {
        abort();
    }
}

#define _PASS \
    main_terminated = 1; \
    return( errors != 0 );

#define _PASS_EXIT \
    main_terminated = 1; \
    exit( errors != 0 );

#define ALWAYS_PASS     \
int main()              \
{                       \
    _PASS;              \
}

// macro __u will automatically generate a unique name
#define __up(a,b)       a##b
#define __ue(a,b)       __up(a,b)
#define __u             __ue(__u,__LINE__)

#endif
