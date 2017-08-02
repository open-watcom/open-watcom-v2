/*
FYI, here is how I debugged the DLL load problem...

In Watcom debugger...

(1) wd <exe> or wdw <exe>
(2) menu Break/On Image Load...
(3) enter tomdll.dll
(4) F5 or Run/Go
(5) debugger should break on the image load
(6) menu Code/Images
(7) right click on tomdll.dll line
(8) access the modules or functions to set a breakpoint
(9) you may have to right-click to include more modules/functions
    if the debugger is excluding them from view
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

void log_it( char *m ) {
    FILE *fp = fopen( "tomexe.out", "a" );
    assert( fp != 0 );
    fputs( m, fp );
    fclose( fp );
}

#define WORKING_SIZE (4 * 1024)

char *workMem;

void __dll_initialize( void ) {
    log_it("In __dll_initialize\n");
    workMem = (char*)malloc( WORKING_SIZE );
    assert( workMem != 0 );
}

void __dll_terminate( void ) {
    log_it( "In __dll_terminate\n");
    free( workMem );
    workMem = 0;
}

struct __ {
    __() {
	__dll_initialize();
    }
    ~__() {
	__dll_terminate();
    }
};
__ run_init_term;		// gbl ctor+dtor

extern "C" void __export dll_entry( void ) {
    log_it( "hi from entry\n" );
    assert( workMem != 0 );
    *workMem = '@';
}

struct teststatic {
    teststatic(char *);
    ~teststatic();
    char *st;
    int sz;
};

teststatic::teststatic(char *in) {
    log_it( "In Constructor\n");
    unsigned n = strlen( in );
    st = new char[n+1];
    assert( st != 0 );
    sz = n;
    memcpy( st, in, n+1 );
    log_it( st );
}

teststatic::~teststatic() {
    log_it( "In destructor\n");
    log_it( st );
    delete [] st;
}

teststatic tomfirst("This is the first string\n");
teststatic tomsecont("This is the second string\n");
