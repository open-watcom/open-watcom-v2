#include <iostream.h>
#include <wchiter.h>


#include "strdata.h"

#define test_except( code, except, string )			\
    try{							\
	code;							\
	cout << string << " EXCEPTION NOT THROWN\n";		\
    } catch( WCIterExcept::failure oops ) {			\
        WCIterExcept::wciter_state cause = oops.cause();	\
        if( cause & WCIterExcept::except ) {			\
            cout << string << " (supposed to happen)\n";	\
        } else {						\
	    cout << "wrong except\n";				\
	}							\
    }

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

void test4();
void test5();
void test6();

int main() {
    fill_str_text();
    test4();
    test5();
    test6();
    cout << "\n";
    cout.flush();
    return 0;
};


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn );
    str_data temp;

    cout << "\ntest4\n";

    WCPtrHashSetIter<str_data> hash_iter;

    if( ++hash_iter ) cout << "++iter should have failed";
    hash_iter.current();
    if( hash_iter.container() != 0 ) cout << "container should return 0\n";

    hash_iter.reset();
    if( hash_iter() ) cout << "iter() should have failed";

    hash_iter.exceptions( WCIterExcept::check_all );

    test_except( ++hash_iter, undef_iter, "++1" );
    test_except( hash_iter.current(), undef_item, "current1" );
    test_except( hash_iter.container(), undef_iter, "container1" );

    hash_iter.reset();
    test_except( hash_iter(), undef_iter, "()1" );

    hash_iter.reset( hash );

    test_except( hash_iter.current(), undef_item, "current2" );
    if( hash_iter.container() != &hash ) cout << "container != &hash\n";

    if( ++hash_iter ) cout << "++iter should return false\n";
    test_except( ++hash_iter, undef_iter, "++2" );
    hash_iter.reset();
    if( hash_iter() ) cout << "iter() should return false\n";
    test_except( hash_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };
    hash.forAll( print_ptr, 0 );
    cout << "\n";

    hash_iter.reset();
    test_except( hash_iter.current(), undef_item, "current3" );
    while( ++hash_iter ) {
	cout << *hash_iter.current() << " ";
    }
    cout << "\n";

    hash_iter.reset();
    while( hash_iter() ) {
	cout << *hash_iter.current() << " ";
    }
}


void test5() {
    WCValHashDict<str_data,int> hash( str_data::hash_fn );
    str_data temp;
    int int_array[ 20 ];

    cout << "\ntest5\n";

    WCValHashDictIter<str_data,int> hash_iter;

    if( ++hash_iter ) cout << "++iter should have failed";
    str_data temp2;
    if( hash_iter.key() != temp2 ) cout << "key should be default value\n";
    hash_iter.value();
    if( hash_iter.container() != 0 ) cout << "container should return 0\n";

    hash_iter.reset();
    if( hash_iter() ) cout << "iter() should have failed";

    hash_iter.exceptions( WCIterExcept::check_all );

    test_except( ++hash_iter, undef_iter, "++1" );
    test_except( hash_iter.key(), undef_item, "key 1" );
    test_except( hash_iter.value(), undef_item, "value 1" );
    test_except( hash_iter.container(), undef_iter, "container1" );

    hash_iter.reset();
    test_except( hash_iter(), undef_iter, "()1" );

    hash_iter.reset( hash );

    test_except( hash_iter.key(), undef_item, "key 2" );
    test_except( hash_iter.value(), undef_item, "value 2" );
    if( hash_iter.container() != &hash ) cout << "container != &hash\n";

    if( ++hash_iter ) cout << "++iter should return false\n";
    test_except( ++hash_iter, undef_iter, "++2" );
    hash_iter.reset();
    if( hash_iter() ) cout << "iter() should return false\n";
    test_except( hash_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	hash.insert( str_text[ i ], int_array[ i ] );
    };
    hash.forAll( print_dict_val, 0 );
    cout << "\n";

    hash_iter.reset();
    test_except( hash_iter.key(), undef_item, "key 3" );
    test_except( hash_iter.value(), undef_item, "value 3" );
    while( ++hash_iter ) {
	cout << hash_iter.key() << ":";
	cout << hash_iter.value() << " ";
    }
    test_except( hash_iter.key(), undef_item, "key 4" );
    test_except( hash_iter.value(), undef_item, "value 4" );
    cout << "\n";

    hash_iter.reset();
    while( hash_iter() ) {
	cout << hash_iter.key() << ":";
	cout << hash_iter.value() << " ";
    }
}


void test6() {
    WCPtrHashDict<str_data,int> hash( str_data::hash_fn );
    str_data temp;
    int int_array[ 20 ];

    cout << "\ntest6\n";

    WCPtrHashDictIter<str_data,int> hash_iter;

    if( ++hash_iter ) cout << "++iter should have failed";
    str_data temp2;
    hash_iter.key();
    hash_iter.value();
    if( hash_iter.container() != 0 ) cout << "container should return 0\n";

    hash_iter.reset();
    if( hash_iter() ) cout << "iter() should have failed";

    hash_iter.exceptions( WCIterExcept::check_all );

    test_except( ++hash_iter, undef_iter, "++1" );
    test_except( hash_iter.key(), undef_item, "key 1" );
    test_except( hash_iter.value(), undef_item, "value 1" );
    test_except( hash_iter.container(), undef_iter, "container1" );

    hash_iter.reset();
    test_except( hash_iter(), undef_iter, "()1" );

    hash_iter.reset( hash );

    test_except( hash_iter.key(), undef_item, "key 2" );
    test_except( hash_iter.value(), undef_item, "value 2" );
    if( hash_iter.container() != &hash ) cout << "container != &hash\n";

    if( ++hash_iter ) cout << "++iter should return false\n";
    test_except( ++hash_iter, undef_iter, "++2" );
    hash_iter.reset();
    if( hash_iter() ) cout << "iter() should return false\n";
    test_except( hash_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	hash.insert( &str_text[ i ], &int_array[ i ] );
    };
    hash.forAll( print_dict_ptr, 0 );
    cout << "\n";

    hash_iter.reset();
    test_except( hash_iter.key(), undef_item, "key 3" );
    test_except( hash_iter.value(), undef_item, "value 3" );
    while( ++hash_iter ) {
	cout << *hash_iter.key() << ":";
	cout << *hash_iter.value() << " ";
    }
    test_except( hash_iter.key(), undef_item, "key 4" );
    test_except( hash_iter.value(), undef_item, "value 4" );
    cout << "\n";

    hash_iter.reset();
    while( hash_iter() ) {
	cout << *hash_iter.key() << ":";
	cout << *hash_iter.value() << " ";
    }
}
