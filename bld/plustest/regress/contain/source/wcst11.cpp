#include <iostream.h>
#include <wcskipit.h>


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
	    cout << string << "wrong except\n";			\
	}							\
    }

char *text[ 20 ] = {
    "wcskip_list.h",    "Defines", "for", "the", "WATCOM",
    "Container", "skip_list", "Table", "Class", "Ian",
    "WCValSkipList", "Skip_list", "table", "for", "values,",
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
    cout << "\n";
    cout.flush();
    return 0;
};

    
void test1() {
    WCValSkipList<str_data> skip_list( WCSKIPLIST_PROB_HALF, 4 );
    str_data temp;

    cout << "test1\n";

    WCValSkipListIter<str_data> skip_list_iter;

    if( ++skip_list_iter ) cout << "++iter should have failed";
    str_data temp2;
    if( skip_list_iter.current() != temp2 ) cout << "current should be default value\n";
    if( skip_list_iter.container() != 0 ) cout << "container should return 0\n";

    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should have failed";

    skip_list_iter.exceptions( WCIterExcept::check_all );

    test_except( ++skip_list_iter, undef_iter, "++1" );
    test_except( skip_list_iter.current(), undef_item, "current1" );
    test_except( skip_list_iter.container(), undef_iter, "container1" );

    skip_list_iter.reset();
    test_except( skip_list_iter(), undef_iter, "()1" );

    skip_list_iter.reset( skip_list );

    test_except( skip_list_iter.current(), undef_item, "current2" );
    if( skip_list_iter.container() != &skip_list ) cout << "container != &skip_list\n";

    if( ++skip_list_iter ) cout << "++iter should return false\n";
    test_except( ++skip_list_iter, undef_iter, "++2" );
    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should return false\n";
    test_except( skip_list_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };
    skip_list.forAll( print_val, 0 );
    cout << "\n";

    skip_list_iter.reset();
    test_except( skip_list_iter.current(), undef_item, "current3" );
    while( ++skip_list_iter ) {
	cout << skip_list_iter.current() << " ";
    }
    cout << "\n";

    skip_list_iter.reset();
    while( skip_list_iter() ) {
	cout << skip_list_iter.current() << " ";
    }
}


void test2() {
    WCPtrSkipList<str_data> skip_list;
    str_data temp;

    cout << "\ntest2\n";

    WCPtrSkipListIter<str_data> skip_list_iter;

    if( ++skip_list_iter ) cout << "++iter should have failed";
    skip_list_iter.current();
    if( skip_list_iter.container() != 0 ) cout << "container should return 0\n";

    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should have failed";

    skip_list_iter.exceptions( WCIterExcept::check_all );

    test_except( ++skip_list_iter, undef_iter, "++1" );
    test_except( skip_list_iter.current(), undef_item, "current1" );
    test_except( skip_list_iter.container(), undef_iter, "container1" );

    skip_list_iter.reset();
    test_except( skip_list_iter(), undef_iter, "()1" );

    skip_list_iter.reset( skip_list );

    test_except( skip_list_iter.current(), undef_item, "current2" );
    if( skip_list_iter.container() != &skip_list ) cout << "container != &skip_list\n";

    if( ++skip_list_iter ) cout << "++iter should return false\n";
    test_except( ++skip_list_iter, undef_iter, "++2" );
    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should return false\n";
    test_except( skip_list_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };
    skip_list.forAll( print_ptr, 0 );
    cout << "\n";

    skip_list_iter.reset();
    test_except( skip_list_iter.current(), undef_item, "current3" );
    while( ++skip_list_iter ) {
	cout << *skip_list_iter.current() << " ";
    }
    cout << "\n";

    skip_list_iter.reset();
    while( skip_list_iter() ) {
	cout << *skip_list_iter.current() << " ";
    }
}

void test3() {
    WCValSkipListSet<str_data> skip_list;
    str_data temp;

    cout << "\ntest3\n";

    WCValSkipListSetIter<str_data> skip_list_iter;

    if( ++skip_list_iter ) cout << "++iter should have failed";
    str_data temp2;
    if( skip_list_iter.current() != temp2 ) cout << "current should be default value\n";
    if( skip_list_iter.container() != 0 ) cout << "container should return 0\n";

    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should have failed";

    skip_list_iter.exceptions( WCIterExcept::check_all );

    test_except( ++skip_list_iter, undef_iter, "++1" );
    test_except( skip_list_iter.current(), undef_item, "current1" );
    test_except( skip_list_iter.container(), undef_iter, "container1" );

    skip_list_iter.reset();
    test_except( skip_list_iter(), undef_iter, "()1" );

    skip_list_iter.reset( skip_list );

    test_except( skip_list_iter.current(), undef_item, "current2" );
    if( skip_list_iter.container() != &skip_list ) cout << "container != &skip_list\n";

    if( ++skip_list_iter ) cout << "++iter should return false\n";
    test_except( ++skip_list_iter, undef_iter, "++2" );
    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should return false\n";
    test_except( skip_list_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };
    skip_list.forAll( print_val, 0 );
    cout << "\n";

    skip_list_iter.reset();
    test_except( skip_list_iter.current(), undef_item, "current3" );
    while( ++skip_list_iter ) {
	cout << skip_list_iter.current() << " ";
    }
    cout << "\n";

    skip_list_iter.reset();
    while( skip_list_iter() ) {
	cout << skip_list_iter.current() << " ";
    }
}


void test4() {
    WCPtrSkipListSet<str_data> skip_list;
    str_data temp;

    cout << "\ntest4\n";

    WCPtrSkipListSetIter<str_data> skip_list_iter;

    if( ++skip_list_iter ) cout << "++iter should have failed";
    skip_list_iter.current();
    if( skip_list_iter.container() != 0 ) cout << "container should return 0\n";

    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should have failed";

    skip_list_iter.exceptions( WCIterExcept::check_all );

    test_except( ++skip_list_iter, undef_iter, "++1" );
    test_except( skip_list_iter.current(), undef_item, "current1" );
    test_except( skip_list_iter.container(), undef_iter, "container1" );

    skip_list_iter.reset();
    test_except( skip_list_iter(), undef_iter, "()1" );

    skip_list_iter.reset( skip_list );

    test_except( skip_list_iter.current(), undef_item, "current2" );
    if( skip_list_iter.container() != &skip_list ) cout << "container != &skip_list\n";

    if( ++skip_list_iter ) cout << "++iter should return false\n";
    test_except( ++skip_list_iter, undef_iter, "++2" );
    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should return false\n";
    test_except( skip_list_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };
    skip_list.forAll( print_ptr, 0 );
    cout << "\n";

    skip_list_iter.reset();
    test_except( skip_list_iter.current(), undef_item, "current3" );
    while( ++skip_list_iter ) {
	cout << *skip_list_iter.current() << " ";
    }
    cout << "\n";

    skip_list_iter.reset();
    while( skip_list_iter() ) {
	cout << *skip_list_iter.current() << " ";
    }
}


void test5() {
    WCValSkipListDict<str_data,int> skip_list;
    str_data temp;
    int int_array[ 20 ];

    cout << "\ntest5\n";

    WCValSkipListDictIter<str_data,int> skip_list_iter;

    if( ++skip_list_iter ) cout << "++iter should have failed";
    str_data temp2;
    if( skip_list_iter.key() != temp2 ) cout << "key should be default value\n";
    skip_list_iter.value();
    if( skip_list_iter.container() != 0 ) cout << "container should return 0\n";

    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should have failed";

    skip_list_iter.exceptions( WCIterExcept::check_all );

    test_except( ++skip_list_iter, undef_iter, "++1" );
    test_except( skip_list_iter.key(), undef_item, "key 1" );
    test_except( skip_list_iter.value(), undef_item, "value 1" );
    test_except( skip_list_iter.container(), undef_iter, "container1" );

    skip_list_iter.reset();
    test_except( skip_list_iter(), undef_iter, "()1" );

    skip_list_iter.reset( skip_list );

    test_except( skip_list_iter.key(), undef_item, "key 2" );
    test_except( skip_list_iter.value(), undef_item, "value 2" );
    if( skip_list_iter.container() != &skip_list ) cout << "container != &skip_list\n";

    if( ++skip_list_iter ) cout << "++iter should return false\n";
    test_except( ++skip_list_iter, undef_iter, "++2" );
    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should return false\n";
    test_except( skip_list_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	skip_list.insert( str_text[ i ], int_array[ i ] );
    };
    skip_list.forAll( print_dict_val, 0 );
    cout << "\n";

    skip_list_iter.reset();
    test_except( skip_list_iter.key(), undef_item, "key 3" );
    test_except( skip_list_iter.value(), undef_item, "value 3" );
    while( ++skip_list_iter ) {
	cout << skip_list_iter.key() << ":";
	cout << skip_list_iter.value() << " ";
    }
    test_except( skip_list_iter.key(), undef_item, "key 4" );
    test_except( skip_list_iter.value(), undef_item, "value 4" );
    cout << "\n";

    skip_list_iter.reset();
    while( skip_list_iter() ) {
	cout << skip_list_iter.key() << ":";
	cout << skip_list_iter.value() << " ";
    }
}


void test6() {
    WCPtrSkipListDict<str_data,int> skip_list;
    str_data temp;
    int int_array[ 20 ];

    cout << "\ntest6\n";

    WCPtrSkipListDictIter<str_data,int> skip_list_iter;

    if( ++skip_list_iter ) cout << "++iter should have failed";
    str_data temp2;
    skip_list_iter.key();
    skip_list_iter.value();
    if( skip_list_iter.container() != 0 ) cout << "container should return 0\n";

    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should have failed";

    skip_list_iter.exceptions( WCIterExcept::check_all );

    test_except( ++skip_list_iter, undef_iter, "++1" );
    test_except( skip_list_iter.key(), undef_item, "key 1" );
    test_except( skip_list_iter.value(), undef_item, "value 1" );
    test_except( skip_list_iter.container(), undef_iter, "container1" );

    skip_list_iter.reset();
    test_except( skip_list_iter(), undef_iter, "()1" );

    skip_list_iter.reset( skip_list );

    test_except( skip_list_iter.key(), undef_item, "key 2" );
    test_except( skip_list_iter.value(), undef_item, "value 2" );
    if( skip_list_iter.container() != &skip_list ) cout << "container != &skip_list\n";

    if( ++skip_list_iter ) cout << "++iter should return false\n";
    test_except( ++skip_list_iter, undef_iter, "++2" );
    skip_list_iter.reset();
    if( skip_list_iter() ) cout << "iter() should return false\n";
    test_except( skip_list_iter(), undef_iter, "()2" );

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	skip_list.insert( &str_text[ i ], &int_array[ i ] );
    };
    skip_list.forAll( print_dict_ptr, 0 );
    cout << "\n";

    skip_list_iter.reset();
    test_except( skip_list_iter.key(), undef_item, "key 3" );
    test_except( skip_list_iter.value(), undef_item, "value 3" );
    while( ++skip_list_iter ) {
	cout << *skip_list_iter.key() << ":";
	cout << *skip_list_iter.value() << " ";
    }
    test_except( skip_list_iter.key(), undef_item, "key 4" );
    test_except( skip_list_iter.value(), undef_item, "value 4" );
    cout << "\n";

    skip_list_iter.reset();
    while( skip_list_iter() ) {
	cout << *skip_list_iter.key() << ":";
	cout << *skip_list_iter.value() << " ";
    }
}
