#include <wclist.h>
#include <iostream.h>
#include "strdata.h"

// test assignment to self, and the index operator

char *text[ 20 ] = {
    "wchash.h",    "Defines", "for", "the", "WATCOM",
    "Container", "Hash", "Table", "Class", "Ian",
    "WCValHashTable", "hash", "table", "for", "values,",
    " values", "to", "not", "need", "do"
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
    WCPtrSList<str_data> list;
    str_data temp;

    cout << "--- test 1\n";
    temp = "Junk";
    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";

    for( int i = 0; i < 20; i++ ) {
        list.append( &str_text[ i ] );
    };

    for( i = 0; i < 20; i++ ) {
        if( i != 13 ) {
            if( list.index( &str_text[ i ] ) != i ) {
                cout << "index failed (i == " << i << "). Returned "
                     << list.index( &str_text[ i ] ) << "\n";
            }
        } else {
            if( list.index( &str_text[ i ] ) != 2 ) {
                cout << "index failed (i == " << i << ")\n";
            }
        }
    }

    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";

    list = list;

    list.forAll( print_ptr, 0 );
    cout << "\n";
}

void test2() {
    WCPtrDList<str_data> list;
    str_data temp;

    cout << "--- test 2\n";
    temp = "Junk";
    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";

    for( int i = 0; i < 20; i++ ) {
        list.append( &str_text[ i ] );
    };

    for( i = 0; i < 20; i++ ) {
        if( i != 13 ) {
            if( list.index( &str_text[ i ] ) != i ) {
                cout << "index failed (i == " << i << "). Returned "
                     << list.index( &str_text[ i ] ) << "\n";
            }
        } else {
            if( list.index( &str_text[ i ] ) != 2 ) {
                cout << "index failed (i == " << i << ")\n";
            }
        }
    }

    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";

    list = list;

    list.forAll( print_ptr, 0 );
    cout << "\n";
}

void test3() {
    WCValSList<str_data> list;
    str_data temp;

    cout << "--- test 3\n";
    temp = "Junk";
    if( list.index( temp ) != -1 ) cout << "index of Junk != -1\n";

    for( int i = 0; i < 20; i++ ) {
        list.append( str_text[ i ] );
    };

    for( i = 0; i < 20; i++ ) {
        if( i != 13 ) {
            if( list.index( str_text[ i ] ) != i ) {
                cout << "index failed (i == " << i << "). Returned "
                     << list.index( str_text[ i ] ) << "\n";
            }
        } else {
            if( list.index( str_text[ i ] ) != 2 ) {
                cout << "index failed (i == " << i << ")\n";
            }
        }
    }

    if( list.index( temp ) != -1 ) cout << "index of Junk != -1\n";

    list = list;

    list.forAll( print_val, 0 );
    cout << "\n";
}

void test4() {
    WCValDList<str_data> list;
    str_data temp = text[ 0 ];

    cout << "--- test 4\n";
    temp = "Junk";
    if( list.index( temp ) != -1 ) cout << "index of Junk != -1\n";

    for( int i = 0; i < 20; i++ ) {
        list.append( str_text[ i ] );
    };

    for( i = 0; i < 20; i++ ) {
        if( i != 13 ) {
            if( list.index( str_text[ i ] ) != i ) {
                cout << "index failed (i == " << i << "). Returned "
                     << list.index( str_text[ i ] ) << "\n";
            }
        } else {
            if( list.index( str_text[ i ] ) != 2 ) {
                cout << "index failed (i == " << i << ")\n";
            }
        }
    }

    if( list.index( temp ) != -1 ) cout << "index of Junk != -1\n";

    list = list;

    list.forAll( print_val, 0 );
    cout << "\n";
}


void print_disv( str_ddata *s, void * ) {
    cout << *s << " ";
}

int d_test_fn( const str_ddata * elem, void * search ) {
    return( *elem == *(str_ddata *)search );
}

void test5() {
    WCIsvSList<str_ddata> list;
    str_ddata temp;
    str_ddata array[ 20 ];

    cout << "--- test 5\n";
    temp = "Junk";
    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";
    if( list.index( &d_test_fn, &temp ) != -1 ) cout << "index of Junk != -1\n";

    for( int i = 0; i < 20; i++ ) {
        array[ i ] = text[ i ];
        list.append( &array[ i ] );
    };

    for( i = 0; i < 20; i++ ) {
            if( list.index( &array[ i ] ) != i ) {
                cout << "index failed (i == " << i << "). Returned "
                     << list.index( &array[ i ] ) << "\n";
            }
    }

    for( i = 0; i < 20; i++ ) {
        if( list.index( &d_test_fn, &array[ i ] ) != i ) cout << "index i==" << i << "failed (OK for i==13)\n";
    }

    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";
    if( list.index( &d_test_fn, &temp ) != -1 ) cout << "index of Junk != -1\n";

    list.forAll( print_disv, 0 );
    cout << "\n";
}

void test6() {
    WCIsvDList<str_ddata> list;
    str_ddata temp;
    str_ddata array[ 20 ];

    cout << "--- test 6\n";
    temp = "Junk";
    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";
    if( list.index( &d_test_fn, &temp ) != -1 ) cout << "index of Junk != -1\n";

    for( int i = 0; i < 20; i++ ) {
        array[ i ] = text[ i ];
        list.append( &array[ i ] );
    };

    for( i = 0; i < 20; i++ ) {
            if( list.index( &array[ i ] ) != i ) {
                cout << "index failed (i == " << i << "). Returned "
                     << list.index( &array[ i ] ) << "\n";
            }
    }

    for( i = 0; i < 20; i++ ) {
        if( list.index( &d_test_fn, &array[ i ] ) != i ) cout << "index i==" << i << "failed (OK for i==13)\n";
    }

    if( list.index( &temp ) != -1 ) cout << "index of Junk != -1\n";
    if( list.index( &d_test_fn, &temp ) != -1 ) cout << "index of Junk != -1\n";

    list.forAll( print_disv, 0 );
    cout << "\n";
}
