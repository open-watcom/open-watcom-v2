#include <wcskip.h>
#include <iostream.h>
#include "strdata.h"

char *text[ 20 ] = {
    "wcskip_list.h",    "Defines", "for", "the", "WATCOM",
    "Container", "skip_list", "Table", "Class", "Ian",
    "WCValskip_listTable", "skip_list", "table", "for", "values,",
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

void test_contain_find_val( str_data s, void *void_skip ){
    WCValSkipList<str_data> *skip_list = (WCValSkipList<str_data> *)void_skip;
    str_data temp;
    if( !skip_list->contains( s ) ) cout << "Does not contain " << s << "\n";
    if( !skip_list->find( s, temp ) ) cout << "Does not find " << s << "\n";
};

void print_ptr( str_data *s, void * ) {
    cout << *s << " ";
}

void print_dict_val( str_data s, int i, void * ) {
    cout << s << ":" << i << " ";
}

void print_dict_ptr( str_data *s, int *i, void * ) {
    cout << *s << ":" << *i << " ";
}

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    fill_str_text();
    test1();
    cout << "\n\n";
    test2();
    cout << "\n\n";
    test3();
    cout << "\n\n";
    test4();
    cout << "\n\n";
    test5();
    cout << "\n\n";
    test6();
    cout << "\n\n";
    cout.flush();
    return 0;
};

    
void test1() {
    WCValSkipList<str_data> skip_list;
    str_data temp = text[ 0 ];

    if( skip_list.remove( temp ) ) cout << "Remove on empty list returned true\n";
    if( skip_list.removeAll( temp ) != 0 ) cout << "RemoveAll on empty list returned != 0\n";

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };

    skip_list.forAll( print_val, 0 );

    temp = "for";
    if( skip_list.removeAll( temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( skip_list.removeAll( temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( skip_list.removeAll( temp ) != 0 ) cout << "Not 0 hello\n";

    temp = "not";
    if( !skip_list.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( !skip_list.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( !skip_list.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    skip_list.forAll( print_val, 0 );
    skip_list.forAll( test_contain_find_val, &skip_list );
    cout << "\n";

    cout.flush();
}


void test2() {
    WCPtrSkipList<str_data> skip_list;
    str_data temp = text[ 0 ];

    if( skip_list.remove( &temp ) ) cout << "Remove on empty list returned true\n";
    if( skip_list.removeAll( &temp ) != 0 ) cout << "RemoveAll on empty list returned != 0\n";

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };

    skip_list.forAll( print_ptr, 0 );

    temp = "for";
    if( skip_list.removeAll( &temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( skip_list.removeAll( &temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( skip_list.removeAll( &temp ) != 0 ) cout << "Not 0 hello\n";

    temp = "not";
    if( skip_list.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( skip_list.remove( &temp ) != &str_text[ 19 ] ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( &temp ) != &str_text[ 16 ] ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    skip_list.forAll( print_ptr, 0 );

    cout.flush();
}


void test3() {
    WCValSkipListSet<str_data> skip_list;
    str_data temp = text[ 0 ];

    if( skip_list.remove( temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };

    skip_list.forAll( print_val, 0 );

    temp = "not";
    if( !skip_list.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( !skip_list.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    skip_list.forAll( print_val, 0 );

    cout.flush();
}


void test4() {
    WCPtrSkipListSet<str_data> skip_list;
    str_data temp = text[ 0 ];

    if( skip_list.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };

    skip_list.forAll( print_ptr, 0 );

    temp = "not";
    if( skip_list.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( skip_list.remove( &temp ) != &str_text[ 16 ] ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    skip_list.forAll( print_ptr, 0 );

    cout.flush();
}


void test5() {
    WCValSkipListDict<str_data,int> skip_list;
    str_data temp = text[ 0 ];

    cout << "\n";
    if( skip_list.remove( temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	skip_list[ str_text[ i ] ] = i;
    };

    skip_list.forAll( print_dict_val, 0 );

    temp = "not";
    if( !skip_list.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( !skip_list.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    skip_list.forAll( print_dict_val, 0 );

    cout.flush();
}


void test6() {
    WCPtrSkipListDict<str_data,int> skip_list;
    str_data temp = text[ 0 ];
    int int_array[ 20 ];

    cout << "\n";
    if( skip_list.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	skip_list[ &str_text[ i ] ] = &int_array[ i ];
    };

    skip_list.forAll( print_dict_ptr, 0 );

    temp = "not";
    if( *skip_list.remove( &temp ) != 17 ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( *skip_list.remove( &temp ) != 19 ) cout << "not able to remove to\n";
    temp = "to";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    skip_list.forAll( print_dict_ptr, 0 );

    cout.flush();
}
