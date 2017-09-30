#include <wchash.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

// just want a class with sizeof( int ) * 2 to test ItemSize macros
struct two_ints {
    int i;
    int j;
};

static int fail_alloc = 0;
// test allocation and deallocation fn's
static void *alloc_fn( size_t size ){
    char *mem;

    if( fail_alloc ) {
	return( 0 );
    } else {
	mem = new char[ size + 1 ];
	*mem = '@';
	return( mem + 1 );
    }
}

static int num_deallocs = 0;
static void dealloc_fn( void *old, size_t size ){
    size = size;		// we're not referencing size
    char *mem = (char *)old - 1;
    if( *mem != '@' ){
	cout << "We never allocated the chunk of memory we are freeing!!\n";
    }else{
	num_deallocs++;
    }
    delete [] (char *)mem;
}


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

void test1();
void test2();
void test3();
void test4();
void test5();
void test6();

int main() {
    fill_str_text();
    test1();
    cout << "\nnumber of deallocs: " << num_deallocs << "\n";
    num_deallocs = 0;
    test2();
    cout << "\nnumber of deallocs: " << num_deallocs << "\n";
    num_deallocs = 0;
    test3();
    cout << "\nnumber of deallocs: " << num_deallocs << "\n";
    num_deallocs = 0;
    test4();
    cout << "\nnumber of deallocs: " << num_deallocs << "\n";
    num_deallocs = 0;
    test5();
    cout << "\nnumber of deallocs: " << num_deallocs << "\n";
    num_deallocs = 0;
    test6();
    cout << "\nnumber of deallocs: " << num_deallocs << "\n";
    num_deallocs = 0;
    cout.flush();
    return 0;
};


void test1() {
    WCValHashTable<str_data> hash( str_data::hash_fn, 4, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( hash.remove( temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( hash.insert( str_text[ 0 ] ) ) cout << "Alloc should have failed";
    hash.exceptions( WCExcept::check_all );
    test_except( hash.insert( str_text[ 0 ] ), out_of_memory, "test1 insert" );
    hash.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };

    cout << "Num Entries: " << hash.entries() << "\n";
    temp = "for";
    if( hash.removeAll( temp ) != 2 ) cout << "did not remove 2 for's\n";
    temp = "not";
    if( !hash.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( temp ) ) cout << "should not be able to remove not\n";

    WCValHashTable<str_data> hash2( hash );
    cout << "Num Entries: " << hash2.entries() << "\n";
    cout << "ItemSize: " << (WCValHashTableItemSize( two_ints )!=(sizeof(two_ints)+sizeof(void*))) << "\n";
}


void test2() {
    WCPtrHashTable<str_data> hash( str_data::hash_fn, 4, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( hash.remove( &temp ) != 0 ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( hash.insert( &str_text[ 0 ] ) ) cout << "Alloc should have failed";
    hash.exceptions( WCExcept::check_all );
    test_except( hash.insert( &str_text[ 0 ] ), out_of_memory, "test2 insert" );
    hash.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };

    cout << "Num Entries: " << hash.entries() << "\n";
    temp = "for";
    if( hash.removeAll( &temp ) != 2 ) cout << "did not remove 2 for's\n";
    temp = "not";
    if( hash.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( &temp ) ) cout << "should not be able to remove not\n";

    WCPtrHashTable<str_data> hash2( hash );
    cout << "Num Entries: " << hash2.entries() << "\n";
    cout << "ItemSize: " <<(WCPtrHashTableItemSize( two_ints )!=(2*sizeof(void*))) << "\n";
}


void test3() {
    WCValHashSet<str_data> hash( str_data::hash_fn, 4, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( hash.remove( temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( hash.insert( str_text[ 0 ] ) ) cout << "Alloc should have failed";
    hash.exceptions( WCExcept::check_all );
    test_except( hash.insert( str_text[ 0 ] ), out_of_memory, "test3 insert" );
    hash.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash.insert( temp );
    };

    cout << "Num Entries: " << hash.entries() << "\n";
    temp = "not";
    if( !hash.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( temp ) ) cout << "should not be able to remove not\n";

    WCValHashSet<str_data> hash2( hash );
    cout << "Num Entries: " << hash2.entries() << "\n";
    cout << "ItemSize: " <<(WCValHashSetItemSize( two_ints )!=(sizeof(two_ints)+sizeof(void*))) << "\n";
}


void test4() {
    WCPtrHashSet<str_data> hash( str_data::hash_fn, 4, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( hash.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( hash.insert( &str_text[ 0 ] ) ) cout << "Alloc should have failed";
    hash.exceptions( WCExcept::check_all );
    test_except( hash.insert( &str_text[ 0 ] ), out_of_memory, "test4 insert" );
    hash.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	hash.insert( &str_text[ i ] );
    };

    cout << "Num Entries: " << hash.entries() << "\n";
    temp = "not";
    if( hash.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( &temp ) ) cout << "should not be able to remove not\n";

    WCPtrHashSet<str_data> hash2( hash );
    cout << "Num Entries: " << hash2.entries() << "\n";
    cout << "ItemSize: " << (WCPtrHashSetItemSize( two_ints )!=(2*sizeof(void*))) << "\n";
}


void test5() {
    WCValHashDict<str_data,int> hash( str_data::hash_fn, 4, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];
    int tempint = 9;

    if( hash.remove( temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( hash.insert( str_text[ 0 ], tempint ) ) cout << "Alloc should have failed";
    hash.exceptions( WCExcept::check_all );
    test_except( hash.insert( str_text[ 0 ], tempint ), out_of_memory, "test5 insert" );
    hash.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	hash[ temp ] = i;
    };

    cout << "Num Entries: " << hash.entries() << "\n";
    temp = "not";
    if( !hash.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( temp ) ) cout << "should not be able to remove not\n";

    WCValHashDict<str_data, int> hash2( hash );
    cout << "Num Entries: " << hash2.entries() << "\n";
    cout << "ItemSize: " <<(WCValHashDictItemSize( two_ints, int )!=(sizeof(two_ints)+sizeof(void*)+sizeof(int))) << "\n";
}


void test6() {
    WCPtrHashDict<str_data,int> hash( str_data::hash_fn, 4, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];
    int tempint = 9;
    int int_array[ 20 ];

    if( hash.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( hash.insert( &str_text[ 0 ], &tempint ) ) cout << "Alloc should have failed";
    hash.exceptions( WCExcept::check_all );
    test_except( hash.insert( &str_text[ 0 ], &tempint ), out_of_memory, "test6 insert" );
    hash.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	hash[ &str_text[ i ] ] = &int_array[ i ];
    };

    cout << "Num Entries: " << hash.entries() << "\n";
    temp = "not";
    if( *hash.remove( &temp ) != 17 ) cout << "did not remove not\n";
    temp = "not";
    if( hash.remove( &temp ) ) cout << "should not be able to remove not\n";

    WCPtrHashDict<str_data, int> hash2( hash );
    cout << "Num Entries: " << hash2.entries() << "\n";
    cout << "ItemSize: " <<(WCPtrHashDictItemSize( two_ints,int )!=(3*sizeof(void*))) << "\n";
}
