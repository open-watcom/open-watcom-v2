#include <wclist.h>
#include <iostream.h>
#include "strdata.h"

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
    WCPtrSList<str_data> list;
    str_data temp = text[ 0 ];

    int i;
    for( i = 0; i < 20; i++ ) {
	list.insert( &str_text[ i ] );
    };

    list.forAll( print_ptr, 0 );
    cout << "\n";

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !list.contains( &temp ) ) cout << "Does not contain " << temp << "\n";
    }
}

void test2() {
    WCPtrDList<str_data> list;
    str_data temp = text[ 0 ];

    int i;
    for( i = 0; i < 20; i++ ) {
	list.insert( &str_text[ i ] );
    };

    list.forAll( print_ptr, 0 );
    cout << "\n";

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !list.contains( &temp ) ) cout << "Does not contain " << temp << "\n";
    }
}

void test3() {
    WCValSList<str_data> list;
    str_data temp = text[ 0 ];

    int i;
    for( i = 0; i < 20; i++ ) {
	list.insert( str_text[ i ] );
    };

    list.forAll( print_val, 0 );
    cout << "\n";

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !list.contains( temp ) ) cout << "Does not contain " << temp << "\n";
    }
}

void test4() {
    WCValDList<str_data> list;
    str_data temp = text[ 0 ];

    int i;
    for( i = 0; i < 20; i++ ) {
	list.insert( str_text[ i ] );
    };

    list.forAll( print_val, 0 );
    cout << "\n";

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !list.contains( temp ) ) cout << "Does not contain " << temp << "\n";
    }
}
