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

void test1();
void test2();
void test3();

int main() {
    fill_str_text();
    test1();
    test2();
    test3();
    cout << "\n";
    cout.flush();
    return 0;
};

    
void test1() {
    WCValHashTable<str_data> hash( str_data::hash_fn );
    str_data temp;

    cout << "test1\n";

    WCValHashTableIter<str_data> hash_iter;

    if( ++hash_iter ) cout << "++iter should have failed";
    str_data temp2;
    if( hash_iter.current() != temp2 ) cout << "current should be default value\n";
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
	temp = text[ i ];
	hash.insert( temp );
    };
    hash.forAll( print_val, 0 );
    cout << "\n";

    hash_iter.reset();
    test_except( hash_iter.current(), undef_item, "current3" );
    while( ++hash_iter ) {
	cout << hash_iter.current() << " ";
    }
    cout << "\n";

    hash_iter.reset();
    while( hash_iter() ) {
	cout << hash_iter.current() << " ";
    }
}


void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn );
    str_data temp;

    cout << "\ntest2\n";

    WCPtrHashTableIter<str_data> hash_iter;

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


void test3() {
    WCValHashSet<str_data> hash( str_data::hash_fn );
    str_data temp;

    cout << "\ntest3\n";

    WCValHashSetIter<str_data> hash_iter;

    if( ++hash_iter ) cout << "++iter should have failed";
    str_data temp2;
    if( hash_iter.current() != temp2 ) cout << "current should be default value\n";
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
	temp = text[ i ];
	hash.insert( temp );
    };
    hash.forAll( print_val, 0 );
    cout << "\n";

    hash_iter.reset();
    test_except( hash_iter.current(), undef_item, "current3" );
    while( ++hash_iter ) {
	cout << hash_iter.current() << " ";
    }
    cout << "\n";

    hash_iter.reset();
    while( hash_iter() ) {
	cout << hash_iter.current() << " ";
    }

}
