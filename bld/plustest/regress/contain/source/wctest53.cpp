#include <wcstack.h>
#include <wcqueue.h>
#include <iostream.h>
#include "testexpt.h"
#include "strdata.h"


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

static void dealloc_fn( void *old, size_t size ){
    size = size;		// we're not refrencing size
    char *mem = (char *)old - 1;
    if( *mem != '@' ){
	cout << "We never allocated the chunk of memory we are freeing!!\n";
    }else{
	cout << "OK, we're freeing a chunk we allocated\n";
    }
    delete [] (char *)mem;
}


void test1();
void test2();


int main() {
    test_except( test1(), not_empty, "test 1" );
    test_except( test2(), not_empty, "test 2" );
    cout.flush();
    return 0;
}


void test1() {
    WCStack<str_data,WCValSList<str_data> >	stack( alloc_fn, dealloc_fn );

    stack.exceptions( WCExcept::check_all );
    if( stack.exceptions() != WCExcept::check_all ) {
	cout << "exceptions different than those set\n";
    }
    str_data hello = "hello";
    str_data goodbye = "goodbye";
    str_data middle = "middle";
    
    stack.push( hello );
    stack.push( middle );
    if( !stack.push( goodbye ) ) cout << "push failed!!\n";

    fail_alloc = 1;
    test_except( stack.push( goodbye ), out_of_memory, "stack out of mem" );
    stack.exceptions( 0 );
    if( stack.push( goodbye ) ) cout << "push should have failed!!\n";
    stack.exceptions( WCExcept::check_all );
    fail_alloc = 0;

    if( stack.pop() != goodbye ) cout << "pop failed!\n";
    if( stack.pop() != middle ) cout << "middle failed!\n";

    cout.flush();
}


void test2() {
    WCQueue<str_data,WCValSList<str_data> >	queue( alloc_fn, dealloc_fn );

    queue.exceptions( WCExcept::check_all );
    if( queue.exceptions() != WCExcept::check_all ) {
	cout << "exceptions different than those set\n";
    }
    str_data hello = "hello";
    str_data goodbye = "goodbye";
    str_data middle = "middle";
    
    queue.insert( hello );
    queue.insert( middle );
    if( !queue.insert( goodbye ) ) cout << "insert failed!!\n";

    fail_alloc = 1;
    test_except( queue.insert( goodbye ), out_of_memory, "Queue out of mem" );
    queue.exceptions( 0 );
    if( queue.insert( goodbye ) ) cout << "insert should have failed!!\n";
    queue.exceptions( WCExcept::check_all );
    fail_alloc = 0;

    if( queue.get() != hello ) cout << "pop failed!\n";
    if( queue.get() != middle ) cout << "middle failed!\n";

    cout.flush();
}
