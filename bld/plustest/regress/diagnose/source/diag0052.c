void __cdecl __fortran a( void );
void __export __export __loadds b( void );
void __based(void) __based(void) *c;
int far far d;
int far near e;
int __based(void) near f;
int __based(r) __based(void) g;
int far (near h)[3];
int (far (__based(void) (i[2]))[3]);
int far j[2];
int (far k)[3];

int (*l)( int a, int a, int b, int b );
int m( int a, int a, int b, int b );
int n( int a, int a, int b, int b )
{
    return( 0 );
}

typedef int F( int, int );

F foo;

int o( int x, int y )
{
    return x + y;
}

F far p;

int near p( int, int )
{
    return 0;
}

struct B1 {
    __segment s;
    struct B2 {
	char __based(s) *p;
    };
};

enum { E1 = 1 };
char __based(E1) *bp1;
char __based((__segment)E1) *bp2;
struct B3 {
    int : 1;
} not_integral;
int bpi, *bpp;
char __based(not_integral) *bp3;
char __based(bpi) *bp4;
char __based(bpp) *bp5;
