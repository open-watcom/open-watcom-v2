#include <wchash.h>
#include <iostream.h>
#include "strdata.h"

char *text[ 20 ] = {
    "wchash.h",    "Defines", "for", "the", "WATCOM",
    "Container", "Hash", "Table", "Class", "Ian",
    "WCValHashTable", "hash", "table", "for", "values,",
    " values", "do", "not", "need", "to"
};

str_data str_text[ 20 ];

void fill_str_text() {
    for( int i = 0; i < 20; i++ ) {
	str_text[ i ] = text[ i ];
    }
};

void print_ptr( str_data *s, void * ) {
    cout << *s << " ";
}

void print_dict_ptr( str_data *s, str_data *s2, void * ) {
    cout << *s << ":" << *s2 << " ";
}

void test2();
void test4();
void test6();

int main() {
    fill_str_text();
    test2();
    test4();
    test6();
    cout.flush();
    return 0;
};

    
void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	hash.insert( new str_data( str_text[ i ] ) );
    };

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    hash.clearAndDestroy();

    if( hash.entries() != 0 ) cout << "Should have 0 entries\n";
    hash.forAll( print_ptr, 0 );
    cout << "\n";
    cout.flush();
}


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	if( i != 13 ) {
	    hash.insert( new str_data( str_text[ i ] ) );
	}
    };

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    hash.clearAndDestroy();

    if( hash.entries() != 0 ) cout << "Should have 0 entries\n";
    hash.forAll( print_ptr, 0 );
    cout << "\n";
    cout.flush();
}


void test6() {
    WCPtrHashDict<str_data,str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	if( i != 13 ) {
	    hash.insert( new str_data( str_text[ i ] ), new str_data( str_text[ i ] ) );
	}
    };

    hash.forAll( print_dict_ptr, 0 );
    cout << "\n";

    hash.clearAndDestroy();

    if( hash.entries() != 0 ) cout << "Should have 0 entries\n";
    hash.forAll( print_dict_ptr, 0 );
    cout << "\n";
    cout.flush();
}
