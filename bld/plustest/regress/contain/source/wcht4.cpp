#include <wchash.h>
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
    test2();
    test3();
    test4();
    test5();
    test6();
    cout.flush();
    return 0;
};

    
void test1() {
    WCValHashTable<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    if( hash.remove( temp ) ) cout << "Remove on empty list returned true\n";
    if( hash.removeAll( temp ) != 0 ) cout << "RemoveAll on empty list returned != 0\n";

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };

    hash.forAll( print_val, 0 );
    cout << "\n";

    temp = "for";
    if( hash.removeAll( temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( hash.removeAll( temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( hash.removeAll( temp ) != 0 ) cout << "Not 0 hello\n";

    temp = "not";
    if( !hash.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( !hash.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( !hash.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( hash.remove( temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << hash.entries() << "\n";

    hash.forAll( print_val, 0 );
    cout << "\n";

    cout.flush();
}


void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    if( hash.remove( &temp ) ) cout << "Remove on empty list returned true\n";
    if( hash.removeAll( &temp ) != 0 ) cout << "RemoveAll on empty list returned != 0\n";

    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    temp = "for";
    if( hash.removeAll( &temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( hash.removeAll( &temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( hash.removeAll( &temp ) != 0 ) cout << "Not 0 hello\n";

    temp = "not";
    if( hash.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( &temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( hash.remove( &temp ) != &str_text[ 16 ] ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( &temp ) != &str_text[ 19 ] ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( &temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( hash.remove( &temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << hash.entries() << "\n";

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    cout.flush();
}


void test3() {
    WCValHashSet<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    if( hash.remove( temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };

    hash.forAll( print_val, 0 );
    cout << "\n";

    temp = "not";
    if( !hash.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( !hash.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( hash.remove( temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << hash.entries() << "\n";

    hash.forAll( print_val, 0 );
    cout << "\n";

    cout.flush();
}


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    if( hash.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    temp = "not";
    if( hash.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( &temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( hash.remove( &temp ) != &str_text[ 16 ] ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( &temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( hash.remove( &temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << hash.entries() << "\n";

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    cout.flush();
}


void test5() {
    WCValHashDict<str_data,int> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];

    cout << "\n";
    if( hash.remove( temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	hash[ str_text[ i ] ] = i;
    };

    hash.forAll( print_dict_val, 0 );

    temp = "not";
    if( !hash.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( !hash.remove( temp ) ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( hash.remove( temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << hash.entries() << "\n";

    hash.forAll( print_dict_val, 0 );

    cout.flush();
}


void test6() {
    WCPtrHashDict<str_data,int> hash( str_data::hash_fn, 4 );
    str_data temp = text[ 0 ];
    int int_array[ 20 ];

    cout << "\n";
    if( hash.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	hash[ &str_text[ i ] ] = &int_array[ i ];
    };

    hash.forAll( print_dict_ptr, 0 );

    temp = "not";
    if( *hash.remove( &temp ) != 17 ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( &temp ) ) cout << "should not be able to remove not\n";
    temp = "to";
    if( *hash.remove( &temp ) != 19 ) cout << "not able to remove to\n";
    temp = "to";
    if( hash.remove( &temp ) ) cout << "should not be able to remove to\n";
    temp = "garbage";
    if( hash.remove( &temp ) ) cout << "should not be able to remove garbage\n";

    cout << "\n# of elements" << hash.entries() << "\n";

    hash.forAll( print_dict_ptr, 0 );

    cout.flush();
}
