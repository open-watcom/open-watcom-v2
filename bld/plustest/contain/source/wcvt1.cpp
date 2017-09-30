#include <wcvector.h>
#include <stdlib.h>
#include <iostream.h>

static void test1();
static void test2();

int main(){
    test1();
    test2();
    cout.flush();
    return 0;
}

void test1() {
    int i;

    WCValVector<long> 	vect( 5 );

    cout << "length before indexing: " << vect.length() << "\n";
    vect[ 2 ] = 25;
    vect[ 0 ] = 50;
    vect[ 1 ] = vect[ 3 ] = 0;
    vect[ 4 ] = 1234567;
    cout << "length after indexing: " << vect.length() << "\n";

    for( i = 0; i < 5; i++ ){
	cout << vect[ i ] << '\n';
    }
}


void test2() {
    int i;

    long	a = 25;
    long 	b = 50;
    long	c = 0;
    long 	d = 1234;
    WCPtrVector<long> 	vect( 5 );

    cout << "length before indexing: " << vect.length() << "\n";
    vect[ 2 ] = &a;
    vect[ 0 ] = &b;
    vect[ 1 ] = vect[ 3 ] = &c;
    vect[ 4 ] = &d;
    cout << "length after indexing: " << vect.length() << "\n";

    for( i = 0; i < 5; i++ ){
	cout << *vect[ i ] << '\n';
    }
}
