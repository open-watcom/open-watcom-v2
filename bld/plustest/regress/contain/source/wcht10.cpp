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

int main() {
    fill_str_text();
    test1();
    test2();
    test3();
    test4();
    cout.flush();
    return 0;
};

    
void test1() {
    WCValHashTable<str_data> hash( str_data::hash_fn );
    str_data temp;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };
    hash.forAll( print_val, 0 );
}


void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn );
    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };
    cout << "\n";
    hash.forAll( print_ptr, 0 );
}


void test3() {
    WCValHashSet<str_data> hash( str_data::hash_fn );
    str_data temp;
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( i == 13 ) {
	    if( hash.insert( temp ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !hash.insert( temp ) ) cout << "Insert failed\n";
	}
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };
    cout << "\n";
    hash.forAll( print_val, 0 );
}


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn );
    for( int i = 0; i < 20; i++ ) {
	if( i == 13 ) {
	    if( hash.insert( &str_text[ i ] ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !hash.insert( &str_text[ i ] ) ) cout << "Insert failed\n";
	}
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };
    cout << "\n";
    hash.forAll( print_ptr, 0 );
}
