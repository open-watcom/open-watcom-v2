// ztc bug

void f( int,   const int * );
void f( float, int * );

void g( int a, int *b )
{
    f( a, b );          // this is ambiguous
// function 1
// int   -> int           = exact
// int * -> const int *   = trivial
// function 2
// int   -> float         = std conv
// int * -> int *         = exact
//
// ztc resolves to function 2
}
