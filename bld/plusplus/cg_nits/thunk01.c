// WCCP thunk01.c -ox -d1
// generates:
//
//      call final
//      pop si
//      ret
//
// how about?
//
//      pop si
//      jmp final
//
// (only if thunk and final are the same memory model!)
//
extern char *final( char *this, int a, int b );
char *thunk( char *this, int a, int b )
{
    unsigned *cast;

    cast = (void *)this;
    this -= *cast;
    this += 6;
    return( final( this, a, b ) );
}

char (*a[2])( char *this, int a, int b ) = {
    thunk,
    thunk,
};

void foo( void )
{
    char buff[4];
    a[1]( buff, 1, 2 );
}
