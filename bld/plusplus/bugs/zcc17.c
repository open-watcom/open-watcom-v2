typedef unsigned size_t;
struct C {
    void *operator new( size_t );
    void *operator new( size_t, int );
    C();
};

extern void foo( C * );

void main( void )
{
    C *p;

    /* broken for AT&T, Borland, and Zortech (GNU can't even parse it) */
    /* (they both forget to check for a NULL return from the specialized new) */
    p = new (-3) C;
    foo( p );
}
