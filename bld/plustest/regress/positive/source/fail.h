#ifndef _FAIL_H
#define _FAIL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

extern unsigned errors;
extern unsigned main_terminated;
extern int _CD_sig;
extern int _CD_count;

extern void fail( unsigned line );
#define _fail	(fail(__LINE__))

#ifndef __FAILEX_H

unsigned errors;
unsigned main_terminated;

int _CD_sig;
int _CD_count;

void fail( unsigned line )
{
    ++errors;
    printf( "failure on line %u\n", line );
    if( main_terminated || errors > 5 ) {
        abort();
    }
}

#ifdef __cplusplus

struct _CHECK_CD {
    ~_CHECK_CD() {
	if( _CD_count != 0 ) fail(__LINE__);
    }
};
_CHECK_CD __check_cd;

struct __ZAP {
    char *p;
    __ZAP() {
	#if defined(__I86__)
	    char buff[512];
	#else
	    char buff[2048];
	#endif
	memset( buff, ~0, sizeof(buff) );
	p = buff;
    }
} __zap_stack;

#endif

#endif

#ifdef __cplusplus

struct _CD {
    int sig;
    int check_sig;
    void ok() const {
	// verify object was not destructed
	if( sig <= 0 ) fail(__LINE__);
	// verify sig is valid
	if( sig > _CD_sig ) fail(__LINE__);
	// verify check is valid
	if( check_sig != ~sig ) fail(__LINE__);
    }
    _CD() : sig(++_CD_sig),check_sig(~sig) {
	++_CD_count;
    }
    _CD( _CD const &s ) {
	s.ok();
	sig = ++_CD_sig;
	check_sig = ~sig;
	++_CD_count;
    }
    ~_CD() {
	ok();
	--_CD_count;
	if( _CD_count < 0 ) fail(__LINE__);
	sig = -1;
    }
    _CD & operator =( _CD const &s ) {
	s.ok();
	ok();
	return *this;
    }
};

#endif


#define _PASS \
    if( ! errors ) { \
        puts( "PASS " __FILE__ ); \
	fflush( stdout ); \
    } \
    main_terminated = 1; \
    return( errors != 0 );

#define _PASS_EXIT \
    if( ! errors ) { \
        puts( "PASS " __FILE__ ); \
	fflush( stdout ); \
    } \
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
