#include <wchash.h>
#include <iostream.h>

void print_val( int s, void * ) {
    cout << s << " ";
}

void print_ptr( int *s, void * ) {
    cout << *s << " ";
}

unsigned hash_fn( const int &i ) {
    return( i );
}

void test1();
void test2();

int main() {
    test1();
    test2();
    cout.flush();
    return 0;
};


void test1() {
    WCValHashTable<int> hash( hash_fn, 3 );
    int temp;

    for( int i = 0; i < 20; i++ ) {
	temp = 0;
	hash.insert( temp );
	if( hash.occurrencesOf( temp ) != i + 1 ) cout << i << "wrong\n";
    };
    hash.forAll( print_val, 0 );
    cout << "\nremoving 0's\n";
    hash.removeAll( temp );
    hash.forAll( print_val, 0 );
    cout << "\n";
}


void test2() {
    WCPtrHashTable<int> hash( hash_fn, 3 );
    int array[ 20 ];

    for( int i = 0; i < 20; i++ ) {
	array[ i ] = 0;
	hash.insert( &array[ i ] );
	if( hash.occurrencesOf( &array[ i ] ) != i + 1 ) cout << i << "wrong\n";
    };
    hash.forAll( print_ptr, 0 );
    cout << "\nremoving 0's\n";
    hash.removeAll( &array[ 0 ] );
    hash.forAll( print_ptr, 0 );
    cout << "\n";
}
