~Q( void )
{
}

operator int()
{
    return( 0 );
}

operator -- ()
{
}

struct S1 {
    operator -- ( void );
};

struct S2 {
    int operator ++( int, int );
};

operator []( int, int )
{
}

struct S3 {
    operator []( int, int );
    operator []( int );
};

operator () ()
{
}

operator = ( int, int )
{
}

struct S4 {
    operator = ( int, int );
    operator = ( int );
};

operator + ( int, int, int )
{
}

struct S5 {
    operator + ( int, int );
    operator + ( int );
};

operator ~ ( int, int, int )
{
}

operator += ( int, int, int )
{
}
