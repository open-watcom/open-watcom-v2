#include <wclist.h>
#include <iostream.h>
#include <string.hpp>


// test allocation and deallocation fn's
static void *alloc_fn( size_t size ){
    char *mem;

    mem = new char[ size + 1 ];
    *mem = '@';
    return( mem + 1 );
}

static void dealloc_fn( void *old, size_t size ){
    size = size;		// we're not refrencing size
    char *mem = (char *)old - 1;
    if( *mem != '@' ){
	cout << "We never allocated the chunk of memory we a freeing!!\n";
    }else{
	cout << "OK, we're freeing a chunk we allocated\n";
    }
    delete [] (char *)mem;
}

class str_ddata : public WCDLink {
public:
    String info;

    inline ~str_ddata() {};
    inline str_ddata() {};
    inline str_ddata( String datum ) : info( datum ) {};
    inline void * operator new( size_t size ){
	return( alloc_fn( size ) );
    }
    inline void operator delete( void * old, size_t size ){
	dealloc_fn( old, size );
    }
};

static void test1( void );

int main() {
    try {
        test1();
    } catch( ... ) {
        cout << "we got caught\n";
    }
    cout.flush();
    return 0;
}


void test1 ( void ) {
    WCIsvDList<str_ddata>  list;
    str_ddata *          d1 = new str_ddata("str#1");
    str_ddata *          d2 = new str_ddata("str#2");
    str_ddata *          d3 = new str_ddata("str#3");

    list.append( d1 );
    list.append( d2 );
    list.append( d3 );
    list.clearAndDestroy();
    cout.flush();
}
