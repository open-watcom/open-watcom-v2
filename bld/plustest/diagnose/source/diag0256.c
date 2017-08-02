void f1( int *const *const volatile *      *const volatile ){};
void f2( int volatile *const *const volatile *volatile *  ){};
void f3( int volatile *const volatile *      volatile *const *const volatile ){};
void f4( int volatile *const *const volatile *      * ) {};
void f5( int volatile *const *const volatile *      * ) {};
void f6( int const volatile * const  volatile* volatile *const volatile *const  volatile* ){};

void main()
{
#if __WATCOM_REVISION__ >= 8
    int *const *      volatile *      *      volatile x1;
    int *const *const volatile *      *const volatile y1 = x1;

    int volatile *const *      volatile *volatile * volatile x2;
    int volatile *const *const volatile *volatile * y2 = x2;

    int volatile *const          *      volatile *      *      volatile x3;
    int volatile *const volatile *      volatile *const *const volatile y3 = x3;

    int volatile *const *      volatile *      * x4;
    int volatile *const *const volatile *      * y4 = x4;

    int volatile *const *      volatile *      * volatile x5;
    int volatile *const *const volatile *      * y5 = x5;

    int *****x6;
    int const volatile * const  volatile* volatile *const volatile *const  volatile* y6 = x6;

    f1( x1 );
    f2( x2 );
    f3( x3 );
    f4( x4 );
    f5( x5 );
    f6( x6 );
#else
    #error at least one
#endif
}
