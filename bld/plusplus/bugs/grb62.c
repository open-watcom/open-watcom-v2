void bar( char ** );
void foo()
{
    char a[40];
    char b[40];

    // would be nice if we can eliminate the use of a static array here (AFS)
    char *p[2] = { a, b };
    bar( p );
}
