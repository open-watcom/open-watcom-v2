struct A { int a; };
struct B { int b; };

int f( struct A * a, struct B * b )
{
        return( a == b );	// no warning with -wx, no error
}

int f( struct A * a, int * b )
{
        return( a == b );	// error
}

struct X : A {
};
struct Y : A {
};

int f( X * a, Y * b )
{
        return( a == b );	// error
}
