#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( A __near * ) GOOD;
void f( A __far * ) GOOD;
void f( const A __near * ) GOOD;
void f( const A __far * ) GOOD;
void f( volatile A __near * ) GOOD;
void f( volatile A __far * ) GOOD;
void f( B __near * ) GOOD;
void f( B __far * ) GOOD;
void f( const B __near * ) GOOD;
void f( const B __far * ) GOOD;
void f( volatile B __near * ) GOOD;
void f( volatile B __far * ) GOOD;
void f( C __near * ) GOOD;
void f( C __far * ) GOOD;
void f( const C __near * ) GOOD;
void f( const C __far * ) GOOD;
void f( volatile C __near * ) GOOD;
void f( volatile C __far * ) GOOD;

void gA( A __near *npa,
        A __far *fpa,
        const A __near * cnpa,
        const A __far * cfpa,
        volatile A __near * vnpa,
        volatile A __far * vfpa )
{
    f( npa );
    f( fpa );
    f( cnpa );
    f( cfpa );
    f( vnpa );
    f( vfpa );
}
void gB( B __near *npb,
        B __far *fpb,
        const B __near * cnpb,
        const B __far * cfpb,
        volatile B __near * vnpb,
        volatile B __far * vfpb )
{
    f( npb );
    f( fpb );
    f( cnpb );
    f( cfpb );
    f( vnpb );
    f( vfpb );
}
void gC( C __near *npc,
        C __far *fpc,
        const C __near * cnpc,
        const C __far * cfpc,
        volatile C __near * vnpc,
        volatile C __far * vfpc )
{
    f( npc );
    f( fpc );
    f( cnpc );
    f( cfpc );
    f( vnpc );
    f( vfpc );
}
int main( void ) {
    gA( 0, 0, 0, 0, 0, 0 );
    gB( 0, 0, 0, 0, 0, 0 );
    gC( 0, 0, 0, 0, 0, 0 );
    CHECK_GOOD( 297 );
    return errors != 0;
}
