#include <wchash.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

char *text[ 20 ] = {
    "wchash.h",    "Defines", "for", "the", "WATCOM",
    "Container", "Hash", "Table", "Class", "Ian",
    "WCValHashTable", "hash", "table", "for", "values,",
    " values", "to", "not", "need", "to"
};

str_data str_text[ 20 ];

void fill_str_text() {
    for( int i = 0; i < 20; i++ ) {
	str_text[ i ] = text[ i ];
    }
};

void print_val( str_data s, void * ) {
    cout << s << " ";
}

void print_ptr( str_data *s, void * ) {
    cout << *s << " ";
}

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    fill_str_text();
    test_except( test1(), not_empty, "test1" );
    test_except( test2(), not_empty, "test2" );
    test_except( test3(), not_empty, "test3" );
    test_except( test4(), not_empty, "test4" );
    test_except( test5(), not_empty, "test5" );
    test_except( test6(), not_empty, "test6" );
    cout.flush();
    return 0;
};

    
void test1() {
    WCValHashTable<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };
    cout.flush();
}


void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn, 4 );
    hash.exceptions( WCExcept::not_empty );

    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };
}


void test3() {
    WCValHashSet<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };
}


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn, 4 );

    hash.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };
}



void test5() {
    WCValHashDict<str_data,int> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp, i );
    };
}


void test6() {
    WCPtrHashDict<str_data,int> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    hash.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ], &i );
    };
}
