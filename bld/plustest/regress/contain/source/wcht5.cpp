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
    int i, j;

    WCValHashTable<str_data> hash( str_data::hash_fn, 2 );
    str_data temp;
    str_data temp2;

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    }

    hash.forAll( print_val, 0 );
    cout << "\n";

    for( i = 0; i < 20; i += 3 ) {
	cout.flush();
	hash.resize( i );
	if( i == 0 ) {
	    if( hash.buckets() != 2 ) cout << "resize 0 changed hash\n";
	} else {
	    for( j = 0; j < 20; j++ ) {
		temp = text[ j ];
		if( !hash.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
		if( !hash.contains( temp ) ) cout << "Did not find" << temp << '\n';
	    }
	    hash.forAll( print_val, 0 );
	    cout << "\n";
	}
    }

    cout.flush();
}


void test2() {
    int i, j;

    WCPtrHashTable<str_data> hash( str_data::hash_fn, 1 );

    for( i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    }

    hash.forAll( print_ptr, 0 );
    cout << "\n!! Note: did not find 13, 2 should alterningly occur !!\n";

    for( i = 2; i < 20; i += 3 ) {
	cout.flush();
	hash.resize( i );
	for( j = 0; j < 20; j++ ) {
	    if( hash.find( &str_text[ j ] ) != &str_text[ j ] ) cout << "Did not find" << j << '\n';
	    if( !hash.contains( &str_text[ j ] ) ) cout << "Did not find" << j << '\n';
	}
	hash.forAll( print_ptr, 0 );
	cout << "\n";
    }

    cout.flush();
}


void test3() {
    int i, j;

    WCValHashSet<str_data> hash( str_data::hash_fn, 2 );
    str_data temp;
    str_data temp2;

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    }

    hash.forAll( print_val, 0 );
    cout << "\n";

    for( i = 0; i < 10; i += 3 ) {
	cout.flush();
	hash.resize( i );
	if( i == 0 ) {
	    if( hash.buckets() != 2 ) cout << "resize 0 changed hash\n";
	} else {
	    for( j = 0; j < 20; j++ ) {
		temp = text[ j ];
		if( !hash.find( temp, temp2 ) ) cout << "Did not find" << temp << '\n';
		if( !hash.contains( temp ) ) cout << "Did not find" << temp << '\n';
	    }
	    hash.forAll( print_val, 0 );
	    cout << "\n";
	}
    }

    cout.flush();
}


void test4() {
    int i, j;

    WCPtrHashSet<str_data> hash( str_data::hash_fn, 2 );
    str_data temp;

    for( i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    }

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    for( i = 0; i < 10; i += 3 ) {
	cout.flush();
	hash.resize( i );
	if( i == 0 ) {
	    if( hash.buckets() != 2 ) cout << "resize 0 changed hash\n";
	} else {
	    for( j = 0; j < 20; j++ ) {
		temp = text[ j ];
		if( !hash.find( &temp ) ) cout << "Did not find" << temp << '\n';
		if( !hash.contains( &temp ) ) cout << "Did not find" << temp << '\n';
	    }
	    hash.forAll( print_ptr, 0 );
	    cout << "\n";
	}
    }

    cout.flush();
}


void test5() {
    int i, j;

    WCValHashDict<str_data,int> hash( str_data::hash_fn, 2 );
    str_data temp;
    str_data temp2;
    int tempint;

    for( i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp, i );
    }

    cout << "\n";
    hash.forAll( print_dict_val, 0 );

    for( i = 0; i < 10; i += 3 ) {
	cout.flush();
	cout << "\n";
	hash.resize( i );
	if( i == 0 ) {
	    if( hash.buckets() != 2 ) cout << "resize 0 changed hash\n";
	} else {
	    for( j = 0; j < 20; j++ ) {
		temp = text[ j ];
		if( !hash.find( temp, tempint ) ) cout << "Did not find" << temp << '\n';
		if( !hash.contains( temp ) ) cout << "Did not find" << temp << '\n';
	    }
	    hash.forAll( print_dict_val, 0 );
	}
    }
    cout.flush();
}


void test6() {
    int i, j;

    WCPtrHashDict<str_data,int> hash( str_data::hash_fn, 2 );
    str_data temp;
    str_data temp2;
    int int_array[ 20 ];

    for( i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	hash.insert( &str_text[ i ], &int_array[ i ] );
    }

    cout << "\n";
    hash.forAll( print_dict_ptr, 0 );

    for( i = 0; i < 10; i += 3 ) {
	cout.flush();
	cout << "\n";
	hash.resize( i );
	if( i == 0 ) {
	    if( hash.buckets() != 2 ) cout << "resize 0 changed hash\n";
	} else {
	    for( j = 0; j < 20; j++ ) {
		temp = text[ j ];
		if( !hash.find( &temp ) ) cout << "Did not find" << temp << '\n';
		if( !hash.contains( &temp ) ) cout << "Did not find" << temp << '\n';
	    }
	    hash.forAll( print_dict_ptr, 0 );
	}
    }
    cout.flush();
}
