#include "dump.h"


class A {
    public:
	int ai;
	char ac;
};
class B : public A {
    public:
	int bi;
	char bc;
};

void f( int ) BAD;
void f( int B:: *pbi ) GOOD;
void f( char ) BAD;
void f( char B:: *pbc ) GOOD;

void g( int A:: *pai, char A:: *pac )
{
    f( pai );
    f( pac );
}
int main( void ) {
    g( 0, 0 );
    CHECK_GOOD( 34 );
    return errors != 0;
}
