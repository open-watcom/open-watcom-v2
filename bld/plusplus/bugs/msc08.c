int x;

void bar( void )
{
    extern void i( double );
}

double i( double )              /* cannot redefine the return type! */
{
    x = __LINE__;
    return x;
}
