#include <wcvector.h>
#include <iostream.h>
#include <stdlib.h>

const int NUM_ELEMS = 2000;

static int nums[ NUM_ELEMS ];

void init_nums() {
    for( int i = 0; i < NUM_ELEMS; i++ ) {
	nums[ i ] = rand();
    }
}

void test1();
void test2();

int main() {
    init_nums();
    test1();
    test2();
    return 0;
}

void test1() {
    WCValSortedVector<int> vect( NUM_ELEMS );

    int i;
    for( i = 0; i < NUM_ELEMS; i++ ) {
	vect.insert( nums[ i ] );
    }
    for( i = 0; i < NUM_ELEMS; i++ ) {
	if( !vect.contains( nums[ i ] ) ) {
	    cout << "Did not find " << nums[ i ] << "\n";
	}
    }
}


void test2() {
    WCPtrSortedVector<int> vect( NUM_ELEMS );

    int i;
    for( i = 0; i < NUM_ELEMS; i++ ) {
	vect.insert( &nums[ i ] );
    }
    for( i = 0; i < NUM_ELEMS; i++ ) {
	if( !vect.contains( &nums[ i ] ) ) {
	    cout << "Did not find " << nums[ i ] << "\n";
	}
    }
}
