/*
    solution:
        - if a symbol is injected into file scope by a function
            - allow its use until the local scope is closed
            - mark it as "invisible"
            - everybody except the declaration code has to ignore it

    MSC solves 90% by keeping the extern decls in local scope but it
    cannot detect the problem with changing the return type of a function
    (at the end of this file)
*/
int x;

void foo( void )
{
    extern void i( int );
    extern int k;
}

#if 0
void barf( void )
{
    i(1);       /* should not be visible */
    k = 1;      /* should not be visible */
}
#endif

void i( float )
{
    x = __LINE__;
}

void bar( void )
{
    extern void i( double );
}

#if 0           /* this turns off the "invisible" bit */
void i( double )
{
    x = __LINE__;
}
#endif

void sam( void )
{
    i( (double) 1 );
}

void fubar( void )
{
    extern void vfd( double );
}

double vfd( double )            /* cannot redefine the return type! */
{
    x = __LINE__;
    return x;
}

int f( char * );
void g( void )
{
    extern int f( int );
    f( "asdf" );        /* error: f( int ) hides f( char * ) (p.310) */
}
