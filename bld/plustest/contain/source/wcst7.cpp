#include <wcskip.h>
#include <iostream.h>
#include "strdata.h"

char *text[ 20 ] = {
    "wcskip_list.h",    "Defines", "for", "the", "WATCOM",
    "Container", "skip_list", "Table", "Class", "Ian",
    "WCValskip_listTable", "Skip_list", "table", "for", "values,",
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
    WCPtrSkipList<str_data> skip_list;
    str_data temp = text[ 0 ];

    skip_list.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( new str_data( str_text[ i ] ) );
    };

    skip_list.forAll( print_ptr, 0 );
    cout << "\n";

    skip_list.clearAndDestroy();

    if( skip_list.entries() != 0 ) cout << "Should have 0 entries\n";
    skip_list.forAll( print_ptr, 0 );
    cout << "\n";
    cout.flush();
}


void test4() {
    WCPtrSkipListSet<str_data> skip_list;
    str_data temp = text[ 0 ];

    skip_list.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	if( i != 13 ) {
	    skip_list.insert( new str_data( str_text[ i ] ) );
	}
    };

    skip_list.forAll( print_ptr, 0 );
    cout << "\n";

    skip_list.clearAndDestroy();

    if( skip_list.entries() != 0 ) cout << "Should have 0 entries\n";
    skip_list.forAll( print_ptr, 0 );
    cout << "\n";
    cout.flush();
}


void test6() {
    WCPtrSkipListDict<str_data,str_data> skip_list;
    str_data temp = text[ 0 ];

    skip_list.clearAndDestroy();

    for( int i = 0; i < 20; i++ ) {
	if( i != 13 ) {
	    skip_list.insert( new str_data( str_text[ i ] ), new str_data( str_text[ i ] ) );
	}
    };

    skip_list.forAll( print_dict_ptr, 0 );
    cout << "\n";

    skip_list.clearAndDestroy();

    if( skip_list.entries() != 0 ) cout << "Should have 0 entries\n";
    skip_list.forAll( print_dict_ptr, 0 );
    cout << "\n";
    cout.flush();
}
