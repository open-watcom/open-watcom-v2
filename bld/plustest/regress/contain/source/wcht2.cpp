#include <wchash.h>
#include <iostream.h>
#include "strdata.h"

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
void test3();
void test4();

int main() {
    test1();
    test2();
    test3();
    test4();
    cout.flush();
    return 0;
};


void test1() {
    WCValHashTable<int> hash( hash_fn, 5 );
    int temp;
    int temp2;

    int i;
    for( i = 0; i < 50; i++ ) {
	temp = i;
	hash.insert( temp );
    };
    for( i = 0; i < 50; i++ ) {
	temp = i;
	if( !hash.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
    };
    hash.forAll( print_val, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
}


void test2() {
    WCPtrHashTable<int> hash( hash_fn, 5 );
    int array[ 50 ];

    int i;
    for( i = 0; i < 50; i++ ) {
	array[ i ] = i;
	hash.insert( &array[ i ] );
    };
    for( i = 0; i < 50; i++ ) {
	if( !hash.find( &array[ i ] ) ) cout << "Did not find" << i << '\n';
    };
    hash.forAll( print_ptr, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
}


void test3() {
    WCValHashSet<int> hash( hash_fn, 5 );
    int temp;
    int temp2;

    cout << "test3\n";
    int i;
    for( i = 0; i < 50; i++ ) {
	temp = i;
	hash.insert( temp );
    };
    for( i = 0; i < 50; i++ ) {
	temp = i;
	if( !hash.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
    };
    hash.forAll( print_val, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
}


void test4() {
    WCPtrHashSet<int> hash( hash_fn, 5 );
    int array[ 50 ];

    int i;
    for( i = 0; i < 50; i++ ) {
	array[ i ] = i;
	hash.insert( &array[ i ] );
    };
    for( i = 0; i < 50; i++ ) {
	if( !hash.find( &array[ i ] ) ) cout << "Did not find" << i << '\n';
    };
    hash.forAll( print_ptr, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
}
