#include <wcskip.h>
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
    WCValSkipList<str_data> skip_list;
    str_data temp;
    str_data temp2;

    cout << "# of elements" << skip_list.entries() << "\n";
    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( skip_list.find( temp, temp2 ) ) cout << "Found in empty list!" << temp << '\n';
    if( skip_list.contains( temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
	if( skip_list.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !skip_list.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
	if( !skip_list.contains( temp ) ) cout << "Did not find" << temp << '\n';
    };

    skip_list.forAll( print_val, 0 );

    temp = "for";
    if( skip_list.occurrencesOf( temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( skip_list.occurrencesOf( temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( skip_list.occurrencesOf( temp ) != 0 ) cout << "Not 0 hello\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    cout << "Clearing list\n";
    skip_list.clear();

    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    skip_list.forAll( print_val, 0 );
    cout << "# of elements" << skip_list.entries() << "\n";
}


void test2() {
    WCPtrSkipList<str_data> skip_list;
    str_data temp;

    cout << "# of elements" << skip_list.entries() << "\n";
    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    if( skip_list.find( &str_text[ 0 ] ) ) cout << "Found in empty list!" << temp << '\n';
    if( skip_list.contains( &str_text[ 0 ] ) ) cout << "Found in empty list!" << temp << '\n';
    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
	if( skip_list.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	if( !skip_list.find( &str_text[ i ] ) ) cout << "Did not find " << str_text[ i ] << ':' << i << '\n';
	if( !skip_list.contains( &str_text[ i ] ) ) cout << "Does not contain " << str_text[ i ] << ':' << i << '\n';
    };

    skip_list.forAll( print_ptr, 0 );

    temp = "for";
    if( skip_list.occurrencesOf( &temp ) != 2 ) cout << "Not 2 for's\n";
    temp = "WATCOM";
    if( skip_list.occurrencesOf( &temp ) != 1 ) cout << "Not 1 WATCOM\n";
    temp = "hello";
    if( skip_list.occurrencesOf( &temp ) != 0 ) cout << "Not 0 hello\n";

    cout << "\n# of elements" << skip_list.entries() << "\n";

    cout << "Clearing list\n";
    skip_list.clear();

    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    skip_list.forAll( print_ptr, 0 );
    cout << "# of elements" << skip_list.entries() << "\n";
}


void test3() {
    WCValSkipListSet<str_data> skip_list;
    str_data temp;
    str_data temp2;

    cout << "test3\n# of elements" << skip_list.entries() << "\n";
    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( skip_list.find( temp, temp2 ) ) cout << "Found in empty list!" << temp << '\n';
    if( skip_list.contains( temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( i == 13 ) {
	    if( skip_list.insert( temp ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !skip_list.insert( temp ) ) cout << "Insert failed\n";
	}
	if( skip_list.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !skip_list.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
	if( !skip_list.contains( temp ) ) cout << "Did not find" << temp << '\n';
    };

    skip_list.forAll( print_val, 0 );

    cout << "\n# of elements" << skip_list.entries() << "\n";

    cout << "Clearing list\n";
    skip_list.clear();

    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    skip_list.forAll( print_val, 0 );
    cout << "# of elements" << skip_list.entries() << "\n";
}


void test4() {
    WCPtrSkipListSet<str_data> skip_list;
    str_data temp;
    str_data temp2;

    cout << "test4\n# of elements" << skip_list.entries() << "\n";
    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( skip_list.find( &temp ) ) cout << "Found in empty list!" << temp << '\n';
    if( skip_list.contains( &temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	if( i == 13 ) {
	    if( skip_list.insert( &str_text[ i ] ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !skip_list.insert( &str_text[ i ] ) ) cout << "Insert failed\n";
	}
	if( skip_list.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !skip_list.find( &temp ) ) cout << "Did not find" << temp << '\n';
	if( !skip_list.contains( &temp ) ) cout << "Did not find" << temp << '\n';
    };

    skip_list.forAll( print_ptr, 0 );

    cout << "\n# of elements" << skip_list.entries() << "\n";

    cout << "Clearing list\n";
    skip_list.clear();

    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    skip_list.forAll( print_ptr, 0 );
    cout << "# of elements" << skip_list.entries() << "\n";
}


void test5() {
    WCValSkipListDict<str_data, int> skip_list;
    str_data temp;
    str_data temp2;
    int inttemp;

    cout << "test5\n# of elements" << skip_list.entries() << "\n";
    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( skip_list.find( temp, inttemp ) ) cout << "Found in empty list!" << temp << '\n';
    if( skip_list.contains( temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( i == 13 ) {
	    if( skip_list.insert( temp, i ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !skip_list.insert( temp, i ) ) cout << "Insert failed\n";
	}
	if( skip_list.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !skip_list.find( temp, inttemp ) ) cout << "Did not find" << temp << '\n';
	if( !skip_list.contains( temp ) ) cout << "Did not find" << temp << '\n';
    };

    skip_list.forAll( print_dict_val, 0 );

    cout << "\n# of elements" << skip_list.entries() << "\n";

    cout << "Clearing list\n";
    skip_list.clear();

    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    skip_list.forAll( print_dict_val, 0 );
    cout << "# of elements" << skip_list.entries() << "\n";
}


void test6() {
    WCPtrSkipListDict<str_data, int> skip_list;
    str_data temp;
    str_data temp2;
    int int_array[ 20 ];
    

    cout << "test6\n# of elements" << skip_list.entries() << "\n";
    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    temp = "WATCOM";
    if( skip_list.find( &temp ) ) cout << "Found in empty list!" << temp << '\n';
    if( skip_list.contains( &temp ) ) cout << "Found in empty list!" << temp << '\n';

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	if( i == 13 ) {
	    if( skip_list.insert( &str_text[ i ], &int_array[ i ] ) ) cout << "Inserted non-unique value!!\n";
	} else {
	    if( !skip_list.insert( &str_text[ i ], &int_array[ i ] ) ) cout << "Insert failed\n";
	}
	if( skip_list.isEmpty() ) cout << i << " IS Empty!\n";
    };

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	if( !skip_list.find( &temp ) ) cout << "Did not find" << temp << '\n';
	if( !skip_list.contains( &temp ) ) cout << "Did not find" << temp << '\n';
    };

    skip_list.forAll( print_dict_ptr, 0 );

    cout << "\n# of elements" << skip_list.entries() << "\n";

    cout << "Clearing list\n";
    skip_list.clear();

    if( !skip_list.isEmpty() ) cout << "NOT Empty!\n";
    skip_list.forAll( print_dict_ptr, 0 );
    cout << "# of elements" << skip_list.entries() << "\n";
}
