struct S {
    int memberfn();
    int test( int t, int f )
    {
	return memberfn ? t : f;
    }
};

struct X9 {
    int bad1;
    int bad2[5];
    int *ok1;
};

void x15(const X9 * y)
{
    y->bad1 = 0;
    y->bad2[2] = 0;
    y->ok1[2] = 0;
}

// This one is low priority and probably needs some study but it
// looks like we should allow it.
// Borland:	yes
// WATCOM C:	yes
// CFRONT 2.1:  yes
// GNU C++:     yes
// MSC:		no
// MetaWare:	no

void foo( const double mat[4][4] );

void ack()
{
    double my_mat[4][4];

    foo(my_mat);
}
#define paste( x, y )  x##y
double d = paste( 0189,45.125 );
#ifdef COMOUT
08
#endif
08

