#include <wcskip.h>
#include <iostream.h>
#include "strdata.h"
#include "testexpt.h"

static int fail_alloc = 0;
// test allocation and deallocation fn's
static void *alloc_fn( size_t size ){
    char *mem;

    if( fail_alloc ) {
	return( 0 );
    } else {
	mem = new char[ size + 1 + sizeof( int ) ];
	*mem = '@';
	*(int *)(mem + 1) = size;
	return( mem + 1 + sizeof( int ) );
    }
}

static int num_deallocs = 0;
static void dealloc_fn( void *old, size_t size ){
    size = size;		// we're not referencing size
    char *mem = (char *)old - 1 - sizeof( int );
    if( *mem != '@' ){
	cout << "We never allocated the chunk of memory we are freeing!!\n";
    }else{
	if( *(int *)(mem + 1) != size )cout << "dealloc size (" << size << ") != alloc size (" << *(int *)(mem + 1) << ")\n";
	num_deallocs++;
    }
    delete [] (char *)mem;
}

class ClassOfSize8 {
    public:
    char a[8];
};

char *text[ 20 ] = {
    "wcskip_list.h",    "Defines", "for", "the", "WATCOM",
    "Container", "skip_list", "Table", "Class", "Ian",
    "WCValskip_listTable", "skip_list", "table", "for", "values,",
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
    cout << "number of deallocs: " << num_deallocs << "\n\n";
    num_deallocs = 0;
    test2();
    num_deallocs = 0;
    test3();
    cout << "number of deallocs: " << num_deallocs << "\n\n";
    num_deallocs = 0;
    test4();
    cout << "number of deallocs: " << num_deallocs << "\n\n";
    num_deallocs = 0;
    test5();
    cout << "number of deallocs: " << num_deallocs << "\n\n";
    num_deallocs = 0;
    test6();
    cout << "number of deallocs: " << num_deallocs << "\n\n";
    num_deallocs = 0;
    cout.flush();
    return 0;
};


void test1() {
    WCValSkipList<str_data> skip_list( WCSKIPLIST_PROB_HALF, 8, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( skip_list.remove( temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( skip_list.insert( str_text[ 0 ] ) ) cout << "Alloc should have failed";
    skip_list.exceptions( WCExcept::check_all );
    test_except( skip_list.insert( str_text[ 0 ] ), out_of_memory, "test1 insert" );
    skip_list.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };

    cout << "Num Entries: " << skip_list.entries() << "\n";
    temp = "for";
    if( skip_list.removeAll( temp ) != 2 ) cout << "did not remove 2 for's\n";
    temp = "not";
    if( !skip_list.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove not\n";

    WCValSkipList<str_data> skip_list2( skip_list );
    cout << "Num Entries: " << skip_list2.entries() << "\n";
    cout << "ItemSize (1 PTR): " << WCValSkipListItemSize( ClassOfSize8, 1 ) << "\n";
    cout << "ItemSize (2 PTR): " << WCValSkipListItemSize( ClassOfSize8, 2 ) << "\n";
}


void test2() {
    WCPtrSkipList<str_data> skip_list( WCSKIPLIST_PROB_HALF, 8, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( skip_list.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( skip_list.insert( &str_text[ 0 ] ) ) cout << "Alloc should have failed";
    skip_list.exceptions( WCExcept::check_all );
    test_except( skip_list.insert( &str_text[ 0 ] ), out_of_memory, "test2 insert" );
    skip_list.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };

    cout << "Num Entries: " << skip_list.entries() << "\n";
    temp = "for";
    if( skip_list.removeAll( &temp ) != 2 ) cout << "did not remove 2 for's\n";
    temp = "not";
    if( skip_list.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove not\n";

    WCPtrSkipList<str_data> skip_list2( skip_list );
    cout << "Num Entries: " << skip_list2.entries() << "\n";
    cout << "ItemSize (1 PTR): " << WCPtrSkipListItemSize( ClassOfSize8, 1 ) << "\n";
    cout << "ItemSize (2 PTR): " << WCPtrSkipListItemSize( ClassOfSize8, 2 ) << "\n";
}


void test3() {
    WCValSkipListSet<str_data> skip_list( WCSKIPLIST_PROB_HALF, 8, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( skip_list.remove( temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( skip_list.insert( str_text[ 0 ] ) ) cout << "Alloc should have failed";
    skip_list.exceptions( WCExcept::check_all );
    test_except( skip_list.insert( str_text[ 0 ] ), out_of_memory, "test3 insert" );
    skip_list.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list.insert( temp );
    };

    cout << "Num Entries: " << skip_list.entries() << "\n";
    temp = "not";
    if( !skip_list.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove not\n";

    WCValSkipListSet<str_data> skip_list2( skip_list );
    cout << "Num Entries: " << skip_list2.entries() << "\n";
    cout << "ItemSize (1 PTR): " << WCValSkipListSetItemSize( ClassOfSize8, 1 ) << "\n";
    cout << "ItemSize (2 PTR): " << WCValSkipListSetItemSize( ClassOfSize8, 2 ) << "\n";
}


void test4() {
    WCPtrSkipListSet<str_data> skip_list( WCSKIPLIST_PROB_HALF, 8, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];

    if( skip_list.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( skip_list.insert( &str_text[ 0 ] ) ) cout << "Alloc should have failed";
    skip_list.exceptions( WCExcept::check_all );
    test_except( skip_list.insert( &str_text[ 0 ] ), out_of_memory, "test4 insert" );
    skip_list.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	skip_list.insert( &str_text[ i ] );
    };

    cout << "Num Entries: " << skip_list.entries() << "\n";
    temp = "not";
    if( skip_list.remove( &temp ) != &str_text[ 17 ] ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove not\n";

    WCPtrSkipListSet<str_data> skip_list2( skip_list );
    cout << "Num Entries: " << skip_list2.entries() << "\n";
    cout << "ItemSize (1 PTR): " << WCPtrSkipListSetItemSize( ClassOfSize8, 1 ) << "\n";
    cout << "ItemSize (2 PTR): " << WCPtrSkipListSetItemSize( ClassOfSize8, 2 ) << "\n";
}


void test5() {
    WCValSkipListDict<str_data,int> skip_list( WCSKIPLIST_PROB_HALF, 8, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];
    int tempint = 9;

    if( skip_list.remove( temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( skip_list.insert( str_text[ 0 ], tempint ) ) cout << "Alloc should have failed";
    skip_list.exceptions( WCExcept::check_all );
    test_except( skip_list.insert( str_text[ 0 ], tempint ), out_of_memory, "test5 insert" );
    skip_list.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	temp = text[ i ];
	skip_list[ temp ] = i;
    };

    cout << "Num Entries: " << skip_list.entries() << "\n";
    temp = "not";
    if( !skip_list.remove( temp ) ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( temp ) ) cout << "should not be able to remove not\n";

    WCValSkipListDict<str_data, int> skip_list2( skip_list );
    cout << "Num Entries: " << skip_list2.entries() << "\n";
    cout << "ItemSize (1Ptr): " << WCValSkipListDictItemSize( ClassOfSize8, int, 1 )-sizeof(int) << "\n";
    cout << "ItemSize (2Ptrs): " << WCValSkipListDictItemSize( ClassOfSize8, int, 2 )-sizeof(int) << "\n";
}


void test6() {
    WCPtrSkipListDict<str_data,int> skip_list( WCSKIPLIST_PROB_HALF, 8, &alloc_fn, &dealloc_fn );
    str_data temp = text[ 0 ];
    int tempint = 9;
    int int_array[ 20 ];

    if( skip_list.remove( &temp ) ) cout << "Remove on empty list returned true\n";

    fail_alloc = 1;
    if( skip_list.insert( &str_text[ 0 ], &tempint ) ) cout << "Alloc should have failed";
    skip_list.exceptions( WCExcept::check_all );
    test_except( skip_list.insert( &str_text[ 0 ], &tempint ), out_of_memory, "test6 insert" );
    skip_list.exceptions( 0 );
    fail_alloc = 0;

    for( int i = 0; i < 20; i++ ) {
	int_array[ i ] = i;
	skip_list[ &str_text[ i ] ] = &int_array[ i ];
    };

    cout << "Num Entries: " << skip_list.entries() << "\n";
    temp = "not";
    if( *skip_list.remove( &temp ) != 17 ) cout << "did not remove not\n";
    temp = "not";
    if( skip_list.remove( &temp ) ) cout << "should not be able to remove not\n";

    WCPtrSkipListDict<str_data, int> skip_list2( skip_list );
    cout << "Num Entries: " << skip_list2.entries() << "\n";
    cout << "ItemSize (1Ptr): " << WCPtrSkipListDictItemSize( ClassOfSize8, int, 1 ) << "\n";
    cout << "ItemSize (2Ptrs): " << WCPtrSkipListDictItemSize( ClassOfSize8, int, 2 ) << "\n";
}
