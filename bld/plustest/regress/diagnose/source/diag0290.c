// we didn't generate an error until 97/08/20

extern void f( int a, char *b );    // { a=a; b=b; }
extern void f( int a );             // { a=a; }

void *pf = f;

void foo()
{
    void *pf;
    pf = f;
}
