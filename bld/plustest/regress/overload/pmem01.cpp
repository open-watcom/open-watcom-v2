#include "dump.h"


class A {
    public:
	int ai;
	char ac;
};

void f( int ) BAD;
void f( int near * ) BAD;
void f( int far * ) BAD;
void f( int A:: *pai ) GOOD;
void f( char ) BAD;
void f( char near * ) BAD;
void f( char far * ) BAD;
void f( char A:: *pac ) GOOD;

void g( int A:: *pai, char A:: *pac )
{
    f( pai );
    f( pac );
}
int main( void ) {
    g( 0, 0 );
    CHECK_GOOD( 30 );
    return errors != 0;
}
