??=include "fail.h"
#include <stdio.h>
??=include <string.h>
// On Unix, we can't use backslashes here
#ifdef __UNIX__
#include <sys/stat.h>
??=include <sys/time.h>
#else
#include <sys\stat.h>
??=include <sys??/utime.h>
#endif

typedef struct S {
    char *with;
    char *without;
} S;

S verify[] = {
    { "????=#??",	"??##??" },
    { "?????!|??",	"???||??" },
    { "??????-~??",	"????~~??" },
    { "1?2??3???0",	"1?2??3???0" },
    { "#??=",		"##" },
    { "[??(",		"[[" },
    { "??/??/\\",	"\\\\" },
    { "]??)",		"]]" },
    { "^??'",		"^^" },
    { "{??<",		"{{" },
    { "|??!",		"||" },
    { "}??>",		"}}" },
    { "~??-",		"~~" },
    { NULL,		NULL },
};


// C++ comment (terminates on the next line) ??/
last part of previous comment

/* C comment (terminates on the next line) *??/
/

int main()
{
    S *p;

    for( p = verify; p->with != NULL; ++p ) {
	if( strcmp( p->with, p->without ) != 0 ) {
	    fail( __LINE__ );
	}
    }
    _PASS;
}
