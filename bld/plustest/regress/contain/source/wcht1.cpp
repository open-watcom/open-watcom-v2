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
    WCValHashTable<str_data> hash( str_data::hash_fn, 10 );
    str_data temp;
    str_data temp2;

    cout << "# of elements" << hash.entries() << "\n";
    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( hash.find( temp, temp2 ) ) cout << "Found in empty list!" << temp << '\n';
    if( hash.contains( temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !hash.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
	if( !hash.contains( temp ) ) cout << "Did not find" << temp << '\n';
    };

    hash = hash;
    hash.forAll( print_val, 0 );
    cout << "\n";

    temp = "for";
    if( hash.occurrencesOf( temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( hash.occurrencesOf( temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( hash.occurrencesOf( temp ) != 0 ) cout << "Not 0 hello\n";

    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";

    cout << "Clearing list\n";
    hash.clear();

    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    hash.forAll( print_val, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";
}


void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn, 10 );
    str_data temp;

    cout << "# of elements" << hash.entries() << "\n";
    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    if( hash.find( &str_text[ 0 ] ) ) cout << "Found in empty list!" << temp << '\n';
    if( hash.contains( &str_text[ 0 ] ) ) cout << "Found in empty list!" << temp << '\n';
    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	if( !hash.find( &str_text[ i ] ) ) cout << "Did not find" << temp << '\n';
	if( !hash.contains( &str_text[ i ] ) ) cout << "Did not find" << temp << '\n';
    };

    hash = hash;
    hash.forAll( print_ptr, 0 );
    cout << "\n";

    temp = "for";
    if( hash.occurrencesOf( &temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( hash.occurrencesOf( &temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( hash.occurrencesOf( &temp ) != 0 ) cout << "Not 0 hello\n";

    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";

    cout << "Clearing list\n";
    hash.clear();

    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    hash.forAll( print_ptr, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";
}


void test3() {
    WCValHashSet<str_data> hash( str_data::hash_fn, 10 );
    str_data temp;
    str_data temp2;

    cout << "test3\n# of elements" << hash.entries() << "\n";
    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( hash.find( temp, temp2 ) ) cout << "Found in empty list!" << temp << '\n';
    if( hash.contains( temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( i == 13 ) {
	    if( hash.insert( temp ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !hash.insert( temp ) ) cout << "Insert failed\n";
	}
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !hash.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
	if( !hash.contains( temp ) ) cout << "Did not find" << temp << '\n';
    };

    hash = hash;
    hash.forAll( print_val, 0 );
    cout << "\n";

    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";

    cout << "Clearing list\n";
    hash.clear();

    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    hash.forAll( print_val, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";
}


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn, 10 );
    str_data temp;
    str_data temp2;

    cout << "test4\n# of elements" << hash.entries() << "\n";
    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( hash.find( &temp ) ) cout << "Found in empty list!" << temp << '\n';
    if( hash.contains( &temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	if( i == 13 ) {
	    if( hash.insert( &str_text[ i ] ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !hash.insert( &str_text[ i ] ) ) cout << "Insert failed\n";
	}
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !hash.find( &temp ) ) cout << "Did not find" << temp << '\n';
	if( !hash.contains( &temp ) ) cout << "Did not find" << temp << '\n';
    };

    hash = hash;
    hash.forAll( print_ptr, 0 );
    cout << "\n";

    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";

    cout << "Clearing list\n";
    hash.clear();

    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    hash.forAll( print_ptr, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";
}


void test5() {
    WCValHashDict<str_data, int> hash( str_data::hash_fn, 10 );
    str_data temp;
    str_data temp2;
    int inttemp;

    cout << "test5\n# of elements" << hash.entries() << "\n";
    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( hash.find( temp, inttemp ) ) cout << "Found in empty list!" << temp << '\n';
    if( hash.contains( temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( i == 13 ) {
	    if( hash.insert( temp, i ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !hash.insert( temp, i ) ) cout << "Insert failed\n";
	}
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !hash.find( temp, inttemp ) ) cout << "Did not find" << temp << '\n';
	if( !hash.contains( temp ) ) cout << "Did not find" << temp << '\n';
    };

    hash = hash;
    hash.forAll( print_dict_val, 0 );

    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";

    cout << "Clearing list\n";
    hash.clear();

    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    hash.forAll( print_dict_val, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";
}


void test6() {
    WCPtrHashDict<str_data, int> hash( str_data::hash_fn, 10 );
    str_data temp;
    str_data temp2;
    int int_array[ 20 ];
    

    cout << "test6\n# of elements" << hash.entries() << "\n";
    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( hash.find( &temp ) ) cout << "Found in empty list!" << temp << '\n';
    if( hash.contains( &temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	if( i == 13 ) {
	    if( hash.insert( &str_text[ i ], &int_array[ i ] ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !hash.insert( &str_text[ i ], &int_array[ i ] ) ) cout << "Insert failed\n";
	}
	if( hash.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !hash.find( &temp ) ) cout << "Did not find" << temp << '\n';
	if( !hash.contains( &temp ) ) cout << "Did not find" << temp << '\n';
    };

    hash = hash;
    hash.forAll( print_dict_ptr, 0 );

    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";

    cout << "Clearing list\n";
    hash.clear();

    if( !hash.isEmpty() ) cout << "NOT Empty!\n";
    hash.forAll( print_dict_ptr, 0 );
    cout << "\nNumber of buckets:" << hash.buckets() << "\n";
    cout << "# of elements" << hash.entries() << "\n";
}
