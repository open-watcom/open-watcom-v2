struct _X1 {
    struct A { virtual void fn()=0; };
    struct B : virtual A { virtual void fn(); };
    struct C : virtual B { virtual void fn(); };
    struct D : virtual C { virtual void fn(); };
    struct E : virtual C {};
    struct F : E, D { F(){} };
};


void while_loop( int x )
{
    int i = 0;

    goto ENTER_WHILE;
    while( i < x ) {
ENTER_WHILE:
	++i;
    }
}

void do_while_loop( int x )
{
    int i = 0;

    goto ENTER_DO_WHILE;
    do {
ENTER_DO_WHILE:
	++i;
    } while( i < x );
}

void for_loop( int x )
{
    int i = 0;

    goto ENTER_FOR;
    for( ; i < x; ) {
ENTER_FOR:
	++i;
    }
}

struct S {
    int x;
    mutable int y;
    int mutable z;
    int w;
    mutable int by : 1;
    int mutable bz : 1;
    int bq : 1;
};

void foo( S const *p )
{
    p->x = 1;
    p->y = 1;
    p->z = 1;
    p->w = 1;
    p->by = 1;
    p->bz = 1;
    p->bq = 1;
}

mutable int x;

mutable void bar( int mutable y )
{
    mutable char c;
}

struct Q {
    static mutable q;
    mutable const int *p;
    mutable int * const r;
    mutable const int b : 1;
    Q();
};

bool quiet;

void x82() {
    quiet = true;
    quiet = false;
    ++quiet;
    quiet++;
    --quiet;
    quiet--;
    quiet+=1;
    quiet-=1;
    quiet+=true;
    quiet-=false;
}

extern "C" int __cdecl x95;
extern "C" int x96;
int __cdecl x97;
int x98()
{
    return x95+x96;
}

struct X103;

bool x105,y105,z105;

bool x107( int b, char *p, int X103::*mp )
{
    x105 = b;
    y105 = p;
    z105 = mp;
    x105 += true;
    x105 += 1;
    x105 += b;
    y105 += p;
    z105 += mp;
    return x105 && y105 && z105;
}

const int x120_x0 = 0;
const int x120_x1 = 1;
const int x120_x2 = 2;
void x120_x3();

void x125() {
    if( x120_x0 );
    if( x120_x1 );
    if( x120_x2 );
    if( x120_x3 );
    if( !x120_x0 );
    if( !x120_x1 );
    if( !x120_x2 );
    if( !x120_x3 );
    if( !!x120_x0 );
    if( !!x120_x1 );
    if( !!x120_x2 );
    if( !!x120_x3 );
    if( x120_x0 || x120_x0 );
    if( x120_x1 || x120_x1 );
    if( x120_x2 || x120_x2 );
    if( x120_x3 || x120_x3 );
    if( x120_x0 && x120_x0 );
    if( x120_x1 && x120_x1 );
    if( x120_x2 && x120_x2 );
    if( x120_x3 && x120_x3 );
    if( !x120_x0 || x120_x0 );
    if( !x120_x1 || x120_x1 );
    if( !x120_x2 || x120_x2 );
    if( !x120_x3 || x120_x3 );
    if( !x120_x0 && x120_x0 );
    if( !x120_x1 && x120_x1 );
    if( !x120_x2 && x120_x2 );
    if( !x120_x3 && x120_x3 );
}

#pragma pack ( pop )
#pragma pack ( pop );
#pragma pack ( pop /
#pragma pack ( push +
#pragma pack ( push , *
#pragma pack ( push , 3 -
#pragma pack ( push , 3 )
#pragma pack ( push , __WATCOMC__ $
#pragma pack
#pragma pack;
#pragma pack()
#pragma pack();
#pragma pack(3)
#pragma pack(3);
#pragma pack(__WATCOMC__)
#pragma pack(__WATCOMC__);
