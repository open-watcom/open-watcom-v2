#include <wclist.h>
#include <iostream.h>
#include <string.hpp>
#include "testexpt.h"

static int fail_alloc = 0;
// test allocation and deallocation fn's
static void *alloc_fn( size_t size ){
    unsigned *mem;

    if( fail_alloc ) {
	return( 0 );
    } else {
	mem = (unsigned*) new char[ size + sizeof(unsigned) ];
	*mem = '@';
	return( mem + 1 );
    }
}

static void dealloc_fn( void *old, size_t size ){
    size = size;		// we're not refrencing size
    unsigned *mem = (unsigned *)old - 1;
    if( *mem != '@' ){
	cout << "We never allocated the chunk of memory we are freeing!!\n";
	delete old;
    }else{
	*mem = '#';
	cout << "OK, we're freeing a chunk we allocated\n";
	delete [] (char *)mem;
    }
}

static void test1( void );
static void test2( void );

int main() {
    try {
        test1();
	test2();
    } catch( ... ) {
        cout << "we got caught\n";
    }
    cout.flush();
    return 0;
}

void test1 ( void ) {
    WCPtrDList<String>  list( alloc_fn, dealloc_fn );
    String *            d1 = new String("str#1");
    String *            d2 = new String("str#2");
    String *            d3 = new String("str#3");

    list.append( d1 );
    list.append( d2 );
    if( !list.append( d3 ) ) cout << "append returned false!!\n";

    fail_alloc = 1;
    if( list.append( d3 ) ) cout << "append returned true!!\n";
    list.exceptions( WCExcept::check_all );
    test_except( list.append( d3 ), out_of_memory, "append out of mem" );
    fail_alloc = 0;

    list.clearAndDestroy();
    cout.flush();
}


void test2 ( void ) {
    WCValDList<String>  list( alloc_fn, dealloc_fn );
    String              d1("str#1");
    String              d2("str#2");
    String              d3("str#3");

    list.append( d1 );
    list.append( d2 );
    if( !list.append( d3 ) ) cout << "append returned false!!\n";

    fail_alloc = 1;
    if( list.append( d3 ) ) cout << "2append returned true!!\n";
    list.exceptions( WCExcept::check_all );
    test_except( list.append( d3 ), out_of_memory, "2append out of mem" );
    fail_alloc = 0;

    cout.flush();
}
