#include "dump.h"


class A { public: int a; };
class B : public A { public: int b; };
class C : public B { public: int c; };

void f( A near * ) GOOD;
void f( A far * ) GOOD;
void f( const A near * ) GOOD;
void f( const A far * ) GOOD;
void f( volatile A near * ) GOOD;
void f( volatile A far * ) GOOD;
void f( B near * ) GOOD;
void f( B far * ) GOOD;
void f( const B near * ) GOOD;
void f( const B far * ) GOOD;
void f( volatile B near * ) GOOD;
void f( volatile B far * ) GOOD;
void f( C near * ) GOOD;
void f( C far * ) GOOD;
void f( const C near * ) GOOD;
void f( const C far * ) GOOD;
void f( volatile C near * ) GOOD;
void f( volatile C far * ) GOOD;

void gA( A near *npa,
	A far *fpa,
	const A near * cnpa,
	const A far * cfpa,
	volatile A near * vnpa,
	volatile A far * vfpa )
{
    f( npa );
    f( fpa );
    f( cnpa );
    f( cfpa );
    f( vnpa );
    f( vfpa );
}
void gB( B near *npb,
	B far *fpb,
	const B near * cnpb,
	const B far * cfpb,
	volatile B near * vnpb,
	volatile B far * vfpb )
{
    f( npb );
    f( fpb );
    f( cnpb );
    f( cfpb );
    f( vnpb );
    f( vfpb );
}
void gC( C near *npc,
	C far *fpc,
	const C near * cnpc,
	const C far * cfpc,
	volatile C near * vnpc,
	volatile C far * vfpc )
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
