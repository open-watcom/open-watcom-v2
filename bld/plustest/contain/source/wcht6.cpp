// date     who             what
// -------- --------------- --------------------------------------------
// 96/02/20 J.W.Welch       Changed some rvalue casts to be lvalue casts

#include <wchash.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

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
    test2();
    test3();
    test4();
    test5();
    test6();
    cout.flush();
    return 0;
};

    
void test1() {
    int i;
    WCValHashTable<str_data> hash( str_data::hash_fn, 5 );
    str_data temp;

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        hash.insert( temp );
    }

    hash.forAll( print_val, 0 );
    cout << "\n";

    hash.exceptions( WCExcept::check_all );

    WCValHashTable<str_data> hash2( hash );

    if( hash == hash2 ) cout << "should NOT be equivalent\n";
    if( !(hash == hash) ) cout << "SHOULD be equivalent\n";

    if( hash2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( hash2.entries() != 20 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        hash2.insert( temp );
    }

    cout << "\n 2 X hash:\n";
    hash2.forAll( print_val, 0 );
    cout << "\n";

    hash2 = hash;

    cout << "\n hash2 = hash:\n";
    hash2.forAll( print_val, 0 );
    cout << "\n";
    hash.clear();
    hash2.clear();
}


void test2() {
    int i;
    WCPtrHashTable<str_data> hash( str_data::hash_fn, 5 );

    for( i = 0; i < 20; i++ ) {
        hash.insert( &str_text[ i ] );
    }

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    hash.exceptions( WCExcept::check_all );

    WCPtrHashTable<str_data> hash2( hash );

    if( hash == hash2 ) cout << "should NOT be equivalent\n";
    if( !(hash == hash) ) cout << "SHOULD be equivalent\n";

    if( hash2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( hash2.entries() != 20 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        hash2.insert( &str_text[ i ] );
    }

    cout << "\n 2 X hash:\n";
    hash2.forAll( print_ptr, 0 );
    cout << "\n";

    hash2 = hash;

    cout << "\n hash2 = hash:\n";
    hash2.forAll( print_ptr, 0 );
    cout << "\n";
    hash.clear();
    hash2.clear();
}

    
void test3() {
    int i;
    WCValHashSet<str_data> hash( str_data::hash_fn, 5 );
    str_data temp;

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        hash.insert( temp );
    }

    hash.forAll( print_val, 0 );
    cout << "\n";

    hash.exceptions( WCExcept::check_all );

    WCValHashSet<str_data> hash2( hash );

    if( hash == hash2 ) cout << "should NOT be equivalent\n";
    if( !(hash == hash) ) cout << "SHOULD be equivalent\n";

    if( hash2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( hash2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            hash2.insert( temp );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        hash2.remove( temp );
    }

    cout << "\n hash removed:\n";
    hash2.forAll( print_val, 0 );
    cout << "\n";

    hash2 = hash;

    cout << "\n hash2 = hash:\n";
    hash2.forAll( print_val, 0 );
    cout << "\n";
    hash.clear();
    hash2.clear();
}


void test4() {
    int i;
    WCPtrHashSet<str_data> hash( str_data::hash_fn, 5 );
    str_data temp;

    for( i = 0; i < 20; i++ ) {
        hash.insert( &str_text[ i ] );
    }

    hash.forAll( print_ptr, 0 );
    cout << "\n";

    hash.exceptions( WCExcept::check_all );

    WCPtrHashSet<str_data> hash2( hash );

    if( hash == hash2 ) cout << "should NOT be equivalent\n";
    if( !(hash == hash) ) cout << "SHOULD be equivalent\n";

    if( hash2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( hash2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            hash2.insert( &str_text[ i ] );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        hash2.remove( &temp );
    }

    cout << "\n hash removed:\n";
    hash2.forAll( print_ptr, 0 );
    cout << "\n";

    hash2 = hash;

    cout << "\n hash2 = hash:\n";
    hash2.forAll( print_ptr, 0 );
    cout << "\n";
    hash.clear();
    hash2.clear();
}


void test5() {
    int i;
    WCValHashDict<str_data,int> hash( str_data::hash_fn, 5 );
    str_data temp;
    int tempint;
    int tempint2;

    cout << "\nVal Hash\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        hash[ temp ] = i;
    }

    hash.forAll( print_dict_val, 0 );

    hash.exceptions( WCExcept::check_all );

    WCValHashDict<str_data,int> hash2( hash );

    if( hash == hash2 ) cout << "should NOT be equivalent\n";
    if( !(hash == hash) ) cout << "SHOULD be equivalent\n";

    if( hash2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( hash2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            hash2.insert( temp, i );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        hash2.remove( temp );
    }

    for( i = 0; i < 20; i++ ) {
        if( hash[ str_text[ i ] ] != i ) {
            cout << str_text[ i ] << " not at index " << i << "\n";
        }
    }

    cout << "\n hash removed:\n";
    hash2.forAll( print_dict_val, 0 );

    hash2 = hash;

    const WCValHashDict<str_data, int> hash3( hash );
    for( i = 0; i < 20; i++ ) {
        if( !hash3.findKeyAndValue( str_text[ i ], temp, tempint ) ) cout << i << " find failed\n";
        if( !hash3.find( str_text[ i ], tempint2 ) ) cout << i << "find 2 failed\n";
        if( temp != str_text[ i ] ) cout << "Find key return bad!!\n";
        if( i != 2 ) {
            if( hash3[ str_text[ i ] ] != i ) cout << str_text[ i ] << " not at index " << i << "\n";
            if( tempint != i ) cout << "Find key return bad!!\n";
            if( tempint2 != i ) cout << "Find value return bad!!\n";
        }
    }
    test_except( hash3[ "ahhh!" ], index_range, "test5 bad index" );

    cout << "\n hash2 = hash:\n";
    hash2.forAll( print_dict_val, 0 );
    hash.clear();
    hash2.clear();
    ((WCValHashDict<str_data, int>&)hash3).clear();
}


void test6() {
    int i;
    WCPtrHashDict<str_data,int> hash( str_data::hash_fn, 5 );
    str_data temp;
    str_data *str_ptr;
    int *tempint_ptr;
    int int_array[ 20 ];

    cout << "\nPtr Hash\n";

    for( i = 0; i < 20; i++ ) {
        int_array[ i ] = i;
        hash[ &str_text[ i ] ] = &int_array[ i ];
    }

    hash.forAll( print_dict_ptr, 0 );

    hash.exceptions( WCExcept::check_all );

    WCPtrHashDict<str_data,int> hash2( hash );

    if( hash == hash2 ) cout << "should NOT be equivalent\n";
    if( !(hash == hash) ) cout << "SHOULD be equivalent\n";

    if( hash2.exceptions() != WCExcept::check_all ) cout << "except not copied\n";
    if( hash2.entries() != 19 ) cout << "num_entries not copied\n";

    for( i = 0; i < 20; i++ ) {
        temp = text[ i ];
        try{
            hash2.insert( &temp, &i );
            cout << "Should not be here!!\n";
        } catch( WCExcept::failure oops ) {
            WCExcept::wc_state cause = oops.cause();
            if( cause & WCExcept::not_unique ) {
                cout << "(supposed to happen)\n";
            } else {
                cout << "wrong except\n";
            }
        }
        hash2.remove( &temp );
    }

    for( i = 0; i < 20; i++ ) {
        if( *hash[ &str_text[ i ] ] != i ) {
            cout << str_text[ i ] << " not at index " << i << "\n";
        }
    }

    cout << "\n hash removed:\n";
    hash2.forAll( print_dict_ptr, 0 );

    hash2 = hash;

    const WCPtrHashDict<str_data, int> hash3( hash );
    for( i = 0; i < 20; i++ ) {
        if( ( tempint_ptr = hash3.findKeyAndValue( &str_text[ i ], str_ptr ) ) == 0 ) cout << i << " find failed\n";
        if( *str_ptr != str_text[ i ] ) cout << "Find key return bad!!\n";
        if( i != 2 ) {
            if( hash3.find( &str_text[ i ] ) != &int_array[ i ] ) cout << "find value failed!!\n";
            if( *hash3[ &str_text[ i ] ] != i ) cout << str_text[ i ] << " not at index " << i << "\n";
            if( *tempint_ptr != i ) cout << "Find key return bad!!\n";
        }
    }
    temp = "ahhh!";
    test_except( hash3[ &temp ], index_range, "test6 bad index" );

    cout << "\n hash2 = hash:\n";
    hash2.forAll( print_dict_ptr, 0 );
    hash.clear();
    hash2.clear();
    ((WCPtrHashDict<str_data, int>&)hash3).clear();
}
