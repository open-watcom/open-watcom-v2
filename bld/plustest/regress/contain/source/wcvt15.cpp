#include <wcvector.h>
#include <iostream.h>
#include <strdata.h>
#include "testexpt.h"

char *text[ 20 ] = {
    "zero", "one", "two", "three", "four",
    "five", "six", "seven", "eight", "nine",
    "ten", "eleven", "twelve", "thirteen", "fourteen",
    "fifteen", "sixteen", "seventeen", "eighteen", "nineteen"
};

str_data str_text[ 20 ];

void fill_str_text() {
    for( int i = 0; i < 20; i++ ) {
	str_text[ i ] = text[ i ];
    }
};

void test2();
void test4();
void test6();

int main( void ) {
    fill_str_text();
    test2();
    test4();
    test6();
    return 0;
}

void test2() {
    WCPtrVector<str_data> vect;
    vect.exceptions( WCExcept::not_empty );

    vect.clearAndDestroy();

    for( int i = 0; i < 20; i += 2 ) {
	vect[ i ] = new str_data( str_text[ i ] );
    };

    vect.clearAndDestroy();
};
    
void test4() {
    WCPtrOrderedVector<str_data> vect( 20 );
    vect.exceptions( WCExcept::not_empty );

    vect.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	vect.insert( new str_data( str_text[ i ] ) );
    };
    for( i = 0; i < 20; i++ ) {
	cout << *vect[ i ] << " ";
    };
    cout << "\n";

    vect.clearAndDestroy();
};
    
void test6() {
    WCPtrSortedVector<str_data> vect( 20 );
    vect.exceptions( WCExcept::not_empty );

    vect.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	vect.insert( new str_data( str_text[ i ] ) );
    };
    for( i = 0; i < 20; i++ ) {
	cout << *vect[ i ] << " ";
    };
    cout << "\n";

    vect.clearAndDestroy();
};
    
