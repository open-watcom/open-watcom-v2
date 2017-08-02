void foo( void )
{
    void (*fp)( int, int );
    int(a);
}

struct T {
    double d;
    T(int);
};
int a,b,c[10];

void expr( void )
{
    (int) a, (T)(int) b, c[ sizeof(T) ];
}

void decl( void )
{
    int a, T( int b ), c[ sizeof(T) ];
}

void expr_decl( void )
{
    int(a),T(int(b)),c[sizeof(T)];	/* should match errors for decl()! */
}
/* test is not processed correctly by MS C7.0, MetaWare, Borland, or Zortech */
