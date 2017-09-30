/* Check range comparison warnings - _Bool can only be 0 or 1. */
int cmp1( _Bool b )
{
    return( b > 1 );
}

int cmp2( _Bool b )
{
    return( b < 0 );
}

int cmp3( _Bool b )
{
    return( b < 2 );
}

int cmp4( _Bool b )
{
    return( b == 7 );
}

/* _Bool must behave like any other integer when comparing against pointers. */
int pcmp( int *pi, _Bool b )
{
    return( pi == b );
}
