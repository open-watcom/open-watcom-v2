#include <wcskip.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

char *text[ 20 ] = {
    "wcskip_list.h",    "Defines", "for", "the", "WATCOM",
    "Container", "skip_list", "Table", "Class", "Ian",
    "WCValskip_listTable", "Skip_list", "table", "for", "values,",
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
    WCValSkipList<str_data> skip_list;
    str_data temp = text[ 0 ];

    skip_list.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };
    cout.flush();
}


void test2() {
    WCPtrSkipList<str_data> skip_list;
    skip_list.exceptions( WCExcept::not_empty );

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };
}


void test3() {
    WCValSkipListSet<str_data> skip_list;
    str_data temp = text[ 0 ];

    skip_list.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };
}


void test4() {
    WCPtrSkipListSet<str_data> skip_list;

    skip_list.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };
}



void test5() {
    WCValSkipListDict<str_data,int> skip_list;
    str_data temp = text[ 0 ];

    skip_list.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp, i );
    };
}


void test6() {
    WCPtrSkipListDict<str_data,int> skip_list;
    str_data temp = text[ 0 ];

    skip_list.exceptions( WCExcept::not_empty );
    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ], &i );
    };
}
