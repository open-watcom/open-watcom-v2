#include <wcvector.h>
#include <iostream.h>

void test1();
void test2();

int main(){
    test1();
    test2();
    cout.flush();
    return 0;
}

void test1() {
    int i;
    WCValOrderedVector<long> * 	vect = new WCValOrderedVector<long>( 0 );

    (*vect)[ 0 ] = 17;

    vect->insert( 25 );
    vect->insert( 0 );
    vect->insert( 25 );
    vect->insert( 0 );
    vect->insert( 1234567 );

    for( i = 0; i < vect->entries(); i++ ){
	cout << ( *vect )[ i ] << ' ';
    }
    cout << "\n";
    delete vect;
}

void test2() {
    int i;

    long	a = 25;
    long 	b = 50;
    long	c = 0;
    long 	d = 1234;
    WCPtrOrderedVector<long> 	vect( 7 );

    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &a );
    vect.insert( &c );
    vect.insert( &d );

    vect.resize( 5 );

    for( i = 0; i < 5; i++ ){
	cout << *vect[ i ] << ' ';
    }
    cout << "\n";
}
