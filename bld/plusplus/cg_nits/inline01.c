/*
    poor code quality affecting inline expansions

    Borland, MS C7.0, and MetaWare all generate direct stores to the auto struct
    Zortech and WATCOM use pointer stores
*/
typedef struct P {
    int x,y;
} P;
P *foo( void )
{
    P d,*p;

    p = &d;
    (*p).x = 1;
    (*p).y = 2;
    return p;
}
