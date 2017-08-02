// test far truncation with PODS, non-PODS
//
//


struct PODS {
    int x;
};

struct NONPODS {
    int x;
    NONPODS();
    NONPODS( NONPODS const & );
    NONPODS& operator= ( NONPODS const & );
};


PODS __near p1;
PODS __far  p2;

NONPODS __near n1;
NONPODS __far  n2;

extern void poo( PODS const __near & );
extern void poo( NONPODS const __near & );

extern void goo( PODS  __near & );
extern void goo( NONPODS  __near & );

extern void boo( PODS );
extern void boo( NONPODS );

void foo( PODS __near & np
        , PODS __far  & fp
        , NONPODS __near & nn
        , NONPODS __far & fn )
{
    np = np;
    fp = fp;
    np = fp;
    fp = np;

    nn = nn;
    fn = fn;
    nn = fn;
    fn = nn;

    np.~PODS();
    fp.~PODS();

    nn.~NONPODS();
    fn.~NONPODS();

    PODS pa = np;
    PODS pb = fp;

    NONPODS na = nn;
    NONPODS nb = fn;

    poo( nn );
    poo( fn );

    poo( np );
    poo( fp );

    goo( nn );
    goo( fn );

    goo( np );
    goo( fp );

    boo( nn );
    boo( fn );

    boo( np );
    boo( fp );
}



// #pragma on ( dump_exec_ic )
