#define str(s) # s
#define xstr(s) str(s)
#define debug(s,t) printf("x" # s "= %d, x" # t "= %s", x ## s, x ##t )
#define INCFILE(n) vers ## n
#define glue(a,b) a ## b
#define xglue(a,b) glue(a,b)
#define HIGHLOW "hello"
#define LOW LOW ", world"

debug(1,2);
fputs(str(strncmp("abc\0d", "abc", '\4') /* this goes away */ == 0) str(: @\n), s );
#include xstr(INCFILE(2).h)
glue(HIGH,LOW);
xglue(HIGH,LOW)

#define OBJ_LIKE (1-1)
#define OBJ_LIKE /* white space */ (1-1) /* other white space */
#define FTN_LIKE(a) ( a )
#define FTN_LIKE( a ) ( /* note white space */ \
a /* other stuff
*/ )

#define OBJ_LIKE2 (0)
#define OBJ_LIKE2 (1-1)
#define FTN_LIKE2(b) (a)
#define FTN_LIKE2(a) (a)
