typedef double T;
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
    int(a),T(int(b)),c[sizeof(T)];      /* should match errors for decl()! */
}
