// 95/09/13 -- J.W.Welch        -- change name of string class

#include "fail.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *choices[] = {
    "aaaaa",
    "BBBBBBBBBBB",
    "cccCCCC",
    "xxcCCCC",
    "xx",
    "",
    NULL
};

struct StringClass {
    size_t len;
    char *mem;
    StringClass( char *p = "", size_t keep = 0 ) {
        mem = strdup(p);
        len = strlen(p)+1;
        if( keep && keep < len ) {
            mem[keep] = '\0';
            len = keep;
        }
    }
    ~StringClass(void) {
        free(mem);
    }
};

int should_be;

void Test_Print( StringClass *p, unsigned line )
{
    for( char **curr = choices; *curr != NULL; ++curr ) {
        if( strcmp( p->mem, *curr ) == 0 ) break;
    }
    if( *curr == NULL ) {
        fail( line );
    }
    if(( curr - choices ) != should_be ) {
        fail( line );
    }
}

int main()
{
    StringClass v( choices[0] );
    should_be = 0;
    Test_Print( &v, __LINE__ );
    should_be = 1;
    Test_Print( &StringClass( choices[1] ), __LINE__ );
    should_be = 2;
    Test_Print( &StringClass( choices[2] ), __LINE__ );
    should_be = 4;
    Test_Print( &StringClass( choices[3], 2 ), __LINE__ );
    should_be = 5;
    Test_Print( &StringClass(), __LINE__ );
    _PASS;
}
