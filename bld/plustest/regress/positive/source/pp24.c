#include <iostream.h>
#include "fail.h"


#define BIT_VECT( b1, b2, b3, b4, b5, b6, b7, b8 ) \
    b8 + b7*2 + b6*4 + b5*8 + b4*16 + b3*32 + b2*64 +b1*128

#define max_count 	5
#define forever 	for (;;)
#define max(A, B)	((A) > (B) ? (A) : (B))

#define square(x)  	x * x
#define dprint(e,s)	if( strcmp(#e,s) != 0 ) fail(__LINE__);

#define paste(a, b, c)	a ## b  ## c
#define paste2(a, b)	a ## b

#define const2 		"i"
#define const3 		@
#define xstr(z)		#z
#define str(z)		xstr(z)

#define	BUG2_C


struct B {
    virtual void foo();
    virtual void fo1();
    virtual void fo2();
};
struct D : B {
    virtual void foo();
};

void foo( B *p ) {
    p->fo1();

}


main()
{

int 	x = 15, y = 30;
int 	i = 0, result;
char 	buffer[100];


#ifdef BUG2_C

    if( strlen( __FILE__ ) < 4 ) fail(__LINE__);

 forever {
     i++;
     if (i >= max_count) break;
     strcpy (buffer, paste2("value of ", paste(const2, "=", " ")));
     if( strcmp( buffer, "value of i= " ) != 0 ) fail(__LINE__);
 }
 result = square (max(square(x), square(y)));
 if( result != (30*30*30*30) ) fail(__LINE__);
 dprint(result, "result" );

 result = BIT_VECT( 1, 1, 1, 1, 1, 1, 1, 1);
 if( result != 0x0ff ) fail(__LINE__);
 dprint(result, "result" );

 if (strcmp( str( const3 ), "@")) fail(__LINE__);
#else
fail(__LINE__);
#endif

  _PASS;
}
