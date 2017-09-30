// date     who             what
// -------- --------------- --------------------------------------------
// 96/02/20 J.W.Welch       Changed some rvalue casts to be lvalue casts

#include <wcskip.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

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

void print_val( str_data s, void * ) {
    cout << s << " ";
}

void print_dict_val( str_data s, int i, void * ) {
    cout << s << ":" << i << " ";
}

void print_ptr( str_data *s, void * ) {
    cout << *s << " ";
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
    int i;
    WCValSkipList<str_data> skip_list;
    str_data temp;

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        skip_list.insert( temp );
    }

    skip_list.forAll( print_val, 0 );

    skip_list.exceptions( WCExcept::check_all );

    WCValSkipList<str_data> skip_list2( skip_list );

    if( skip_list == skip_list2 ) cout << "should NOT be equivalent\n";
    if( !(skip_list == skip_list) ) cout << "SHOULD be equivalent\n";

    if( skip_list2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( skip_list2.entries() != 20 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        skip_list2.insert( temp );
    }

    cout << "\n 2 X skip_list:\n";
    skip_list2.forAll( print_val, 0 );

    skip_list2 = skip_list;
    skip_list2 = skip_list2;

    cout << "\n skip_list2 = skip_list:\n";
    skip_list2.forAll( print_val, 0 );
    skip_list.clear();
    skip_list2.clear();
}


void test2() {
    int i;
    WCPtrSkipList<str_data> skip_list;

    for( i = 0; i < 20; i++ ) {
        skip_list.insert( &str_text[ i ] );
    }

    skip_list.forAll( print_ptr, 0 );

    skip_list.exceptions( WCExcept::check_all );

    WCPtrSkipList<str_data> skip_list2( skip_list );

    if( skip_list == skip_list2 ) cout << "should NOT be equivalent\n";
    if( !(skip_list == skip_list) ) cout << "SHOULD be equivalent\n";

    if( skip_list2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( skip_list2.entries() != 20 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        skip_list2.insert( &str_text[ i ] );
    }

    cout << "\n 2 X skip_list:\n";
    skip_list2.forAll( print_ptr, 0 );

    skip_list2 = skip_list;
    skip_list2 = skip_list2;

    cout << "\n skip_list2 = skip_list:\n";
    skip_list2.forAll( print_ptr, 0 );
    skip_list.clear();
    skip_list2.clear();
}

    
void test3() {
    int i;
    WCValSkipListSet<str_data> skip_list;
    str_data temp;

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        skip_list.insert( temp );
    }

    skip_list.forAll( print_val, 0 );

    skip_list.exceptions( WCExcept::check_all );

    WCValSkipListSet<str_data> skip_list2( skip_list );

    if( skip_list == skip_list2 ) cout << "should NOT be equivalent\n";
    if( !(skip_list == skip_list) ) cout << "SHOULD be equivalent\n";

    if( skip_list2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( skip_list2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            skip_list2.insert( temp );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        skip_list2.remove( temp );
    }

    cout << "\n skip_list removed:\n";
    skip_list2.forAll( print_val, 0 );

    skip_list2 = skip_list;
    skip_list2 = skip_list2;

    cout << "\n skip_list2 = skip_list:\n";
    skip_list2.forAll( print_val, 0 );
    skip_list.clear();
    skip_list2.clear();
}


void test4() {
    int i;
    WCPtrSkipListSet<str_data> skip_list;
    str_data temp;

    for( i = 0; i < 20; i++ ) {
        skip_list.insert( &str_text[ i ] );
    }

    skip_list.forAll( print_ptr, 0 );

    skip_list.exceptions( WCExcept::check_all );

    WCPtrSkipListSet<str_data> skip_list2( skip_list );

    if( skip_list == skip_list2 ) cout << "should NOT be equivalent\n";
    if( !(skip_list == skip_list) ) cout << "SHOULD be equivalent\n";

    if( skip_list2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( skip_list2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            skip_list2.insert( &str_text[ i ] );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        if( skip_list2.remove( &temp ) != &str_text[ i ] ) cout << "remove returned the wrong value";
    }

    cout << "\n skip_list removed:\n";
    skip_list2.forAll( print_ptr, 0 );

    skip_list2 = skip_list;
    skip_list2 = skip_list2;

    cout << "\n skip_list2 = skip_list:\n";
    skip_list2.forAll( print_ptr, 0 );
    skip_list.clear();
    skip_list2.clear();
}


void test5() {
    int i;
    WCValSkipListDict<str_data,int> skip_list;
    str_data temp;
    int tempint;
    int tempint2;

    cout << "\nVal skip_list\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        skip_list[ temp ] = i;
    }

    skip_list.forAll( print_dict_val, 0 );

    skip_list.exceptions( WCExcept::check_all );

    WCValSkipListDict<str_data,int> skip_list2( skip_list );

    if( skip_list == skip_list2 ) cout << "should NOT be equivalent\n";
    if( !(skip_list == skip_list) ) cout << "SHOULD be equivalent\n";

    if( skip_list2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( skip_list2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            skip_list2.insert( temp, i );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        skip_list2.remove( temp );
    }

    for( i = 0; i < 20; i++ ) {
        if( skip_list[ str_text[ i ] ] != i ) {
            cout << str_text[ i ] << " not at index " << i << "\n";
        }
    }

    cout << "\n skip_list removed:\n";
    skip_list2.forAll( print_dict_val, 0 );

    skip_list2 = skip_list;
    skip_list2 = skip_list2;

    const WCValSkipListDict<str_data, int> skip_list3( skip_list );
    for( i = 0; i < 20; i++ ) {
        if( !skip_list3.findKeyAndValue( str_text[ i ], temp, tempint ) ) cout << i << " find failed\n";
        if( !skip_list3.find( str_text[ i ], tempint2 ) ) cout << i << "find 2 failed\n";
        if( temp != str_text[ i ] ) cout << "Find key return bad!!\n";
        if( i != 2 ) {
            if( skip_list3[ str_text[ i ] ] != i ) cout << str_text[ i ] << " not at index " << i << "\n";
            if( tempint != i ) cout << "Find key return bad!!\n";
            if( tempint2 != i ) cout << "Find value return bad!!\n";
        }
    }
    test_except( skip_list3[ "ahhh!" ], index_range, "test5 bad index" );

    cout << "\n skip_list2 = skip_list:\n";
    skip_list2.forAll( print_dict_val, 0 );
    skip_list.clear();
    skip_list2.clear();
    ((WCValSkipListDict<str_data, int>&)skip_list3).clear();
}


void test6() {
    int i;
    WCPtrSkipListDict<str_data,int> skip_list;
    str_data temp;
    str_data *str_ptr;
    int *tempint_ptr;
    int int_array[ 20 ];

    cout << "\nPtr skip_list\n";

    for( i = 0; i < 20; i++ ) {
        int_array[ i ] = i;
        skip_list[ &str_text[ i ] ] = &int_array[ i ];
    }

    skip_list.forAll( print_dict_ptr, 0 );

    skip_list.exceptions( WCExcept::check_all );

    WCPtrSkipListDict<str_data,int> skip_list2( skip_list );

    if( skip_list == skip_list2 ) cout << "should NOT be equivalent\n";
    if( !(skip_list == skip_list) ) cout << "SHOULD be equivalent\n";

    if( skip_list2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( skip_list2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            skip_list2.insert( &temp, &i );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        if( i != 2 ) {
            if( *skip_list2.remove( &temp ) != i ) cout << "remove returned the wrong value";
        } else {
            skip_list2.remove( &temp );
        }
    }

    for( i = 0; i < 20; i++ ) {
        if( *skip_list[ &str_text[ i ] ] != i ) {
            cout << str_text[ i ] << " not at index " << i << "\n";
        }
    }

    cout << "\n skip_list removed:\n";
    skip_list2.forAll( print_dict_ptr, 0 );

    skip_list2 = skip_list;
    skip_list2 = skip_list2;

    const WCPtrSkipListDict<str_data, int> skip_list3( skip_list );
    for( i = 0; i < 20; i++ ) {
        if( ( tempint_ptr = skip_list3.findKeyAndValue( &str_text[ i ], str_ptr ) ) == 0 ) cout << i << " find failed\n";
        if( *str_ptr != str_text[ i ] ) cout << "Find key return bad!!\n";
        if( i != 2 ) {
            if( skip_list3.find( &str_text[ i ] ) != &int_array[ i ] ) cout << "find value failed!!\n";
            if( *skip_list3[ &str_text[ i ] ] != i ) cout << str_text[ i ] << " not at index " << i << "\n";
            if( *tempint_ptr != i ) cout << "Find key return bad!!\n";
        }
    }
    temp = "ahhh!";
    test_except( skip_list3[ &temp ], index_range, "test6 bad index" );

    cout << "\n skip_list2 = skip_list:\n";
    skip_list2.forAll( print_dict_ptr, 0 );
    skip_list.clear();
    skip_list2.clear();
    ((WCPtrSkipListDict<str_data, int>&)skip_list3).clear();
}
