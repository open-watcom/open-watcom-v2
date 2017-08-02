// 3.8.3.5 ANSI C standard
#define str(s) # s
#define xstr(s) str(s)
#define debug(s,t) printf("x" # s "= %d, x" # t "= %s", x ## s, x ##t )
#define INCFILE(n) st ## n
#define glue(a,b) a ## b
#define xglue(a,b) glue(a,b)
#define HIGHLOW "hello"
#define LOW LOW ", world"

// OK, a little obscure but it tests a feature
#include xstr(INCFILE(dio).h)
#include xstr(INCFILE(dlib).h)
#include xstr(INCFILE(ring).h)

#include "fail.h"

char *gen[] = {
xstr(debug(1,2);),
xstr(fputs(str(strncmp("abc\0d", "abc", '\4') /* this goes away */ == 0) str(: @\n), s );),
xstr(glue(HIGH,LOW);),
xstr(xglue(HIGH,LOW)),
 NULL
};

char *check[] = {
 "printf(\"x\" \"1\" \"= %d, x\" \"2\" \"= %s\", x1, x2 );",
 "fputs(\"strncmp(\\\"abc\\\\0d\\\", \\\"abc\\\", '\\\\4') == 0\" \": @\\n\", s );",
 "\"hello\";",
 "\"hello\" \", world\"",
 NULL
};

int main()
{
    char **c;
    char **v;

    c = gen;
    v = check;
    for(;;) {
	if( *c == NULL && *v == NULL ) break;
	if( *c == NULL ) {
	    fail(__LINE__);
	    break;
	}
	if( *v == NULL ) {
	    fail(__LINE__);
	    break;
	}
	if( strcmp( *c, *v ) != 0 ) {
	    fail(__LINE__);
	}
	++c;
	++v;
    }
    _PASS;
}
