#include <iostream.h>
#include <wclist.h>
#include <wclistit.h>
#include <wcskip.h>
#include <wcskipit.h>
#include <stdlib.h>

#pragma warning 549 9

const int ElemsPerBlock = 50;

struct BD {
    char *data;
    BD( size_t amt ) {
	data = new char[amt*ElemsPerBlock];
    }
    ~BD() {
	delete [] data;
    }
private:
    BD( BD const &s );
    void operator =( BD const & );
};
int operator ==( BD const &x, BD const &y )
{
    return &x == &y;
}

//
// Simple block allocation class.  Allocate blocks for ElemsPerBlock
// elements, and use part of the block for each of the next ElemsPerBlock
// allocations, incrementing the number allocated elements.  Repeat getting
// more blocks as needed.
//
// Store the blocks in an intrusive single linked list.
//
// On a element deallocation, assume we allocated the memory and just
// decrement the count of allocated elements.  When the count gets to zero,
// free all allocated blocks
//
// This implementation assumes sizeof( char ) == 1
// 

class BlockAlloc {
private:
    // the size of elements (in bytes)
    unsigned elem_size;			

    // number of elements allocated
    unsigned num_allocated;		

    // free space of this number of elements available in first block
    unsigned num_free_in_block;		

    // list of blocks used to store elements (block are chunks of memory,
    // pointed by (char *) pointers.
    WCPtrSList<BD> block_list;
   
    // pointer to the first block in the list 
    BD *curr_block;

public:
    inline BlockAlloc( unsigned size ) 
    		: elem_size( size ), num_allocated( 0 )
		, num_free_in_block( 0 ) {};
		

    inline ~BlockAlloc() {
	block_list.clearAndDestroy();
    };
    
    // get memory for an element using block allocation 
    void *allocator( size_t elem_size );

    // free memory for an element using block allocation and deallocation
    void deallocator( void *old_ptr, size_t elem_size );
};
		

void *BlockAlloc::allocator( size_t size ) {
    // need a new block to perform allocation
    if( num_free_in_block == 0 ) {
	// allocate memory for ElemsPerBlock elements
	curr_block = new BD( size );
	if( curr_block == 0 ) {
	    // allocation failed
	    return( 0 );
	}
	// add new block to beginning of list
	if( !block_list.insert( curr_block ) ) {
	    // allocation of list element failed
	    delete curr_block;
	    return( 0 );
	}
	num_free_in_block = ElemsPerBlock;
    }
    
    // curr block points to a block of memory with some free memory
    num_allocated++;
    num_free_in_block--;
    // return pointer to a free part of the block, starting at the end
    // of the block
    return( curr_block->data + num_free_in_block * size );
}
	

void BlockAlloc::deallocator( void *, size_t ) {
    // just decrement the count
    // don't free anything until all elements are deallocated
    num_allocated--;
    if( num_allocated == 0 ) {
	// all the elements allocated BlockAlloc object have now been
	// deallocated, free all the blocks
	block_list.clearAndDestroy();
	num_free_in_block = 0;
    }
}
	



const unsigned NumTestElems = 200;

// array with random elements
static long test_elems[ NumTestElems ];

unsigned myrand() {
    static unsigned short s;

    s = (unsigned short)(1|((s|1) * 3 + 49));
    return s;
}

static void fill_test_elems() {
    for( int i = 0; i < NumTestElems; i++ ) {
	test_elems[ i ] = myrand();
    }
} 



void test_isv_list();
void test_val_list();
void test_val_skip_list();


int main() {
    fill_test_elems();

    test_isv_list();
    test_val_list();
    test_val_skip_list();
    return 0;
}


// An intrusive list class

class isvInt : public WCSLink {
public:
    static BlockAlloc memory_manage;
    long data;

    isvInt( long datum ) : data( datum ) {};

    void *operator new( size_t size ) {
	return( memory_manage.allocator( size ) );
    };
    void *operator new []( size_t size ) {
	return( memory_manage.allocator( size ) );
    };

    void operator delete( void *old, size_t size ) {
	memory_manage.deallocator( old, size );
    };
    void operator delete []( void *old, size_t size ) {
	memory_manage.deallocator( old, size );
    };
};

// define static member data
BlockAlloc isvInt::memory_manage( sizeof( isvInt ) );


void test_isv_list() {
    WCIsvSList<isvInt> list;
    
    for( int i = 0; i < NumTestElems; i++ ) {
	list.insert( new isvInt( test_elems[ i ] ) );
    }
    int j = 0;

    WCIsvSListIter<isvInt> iter( list );
    while( ++iter ) {
	cout << long(iter.current()->data) << " ";
	if(( j++ % 8 ) == 1 ) cout << endl;
    }
    cout << "\n\n\n";
    list.clearAndDestroy();
}


// WCValSList<int> memory allocator/dealloctor support
static BlockAlloc val_list_manager( WCValSListItemSize( int ) );

static void *val_list_alloc( size_t size ) {
    return( val_list_manager.allocator( size ) );
}

static void val_list_dealloc( void *old, size_t size ) {
    val_list_manager.deallocator( old, size );
}


// test WCValSList<long>
void test_val_list() {
    WCValSList<long> list( &val_list_alloc, &val_list_dealloc );
    
    for( int i = 0; i < NumTestElems; i++ ) {
	list.insert( test_elems[ i ] );
    }
    int j = 0;
    
    WCValSListIter<long> iter( list );
    while( ++iter ) {
	cout << long(iter.current()) << " ";
	if(( j++ % 8 ) == 1 ) cout << endl;
    }
    cout << "\n\n\n";
    list.clear();
}


// skip list allocator dealloctors: just use allocator and dealloctor
// functions on skip list elements with one and two pointers
// (this will handle 94% of the elements)
const int one_ptr_size = WCValSkipListItemSize( int, 1 );
const int two_ptr_size = WCValSkipListItemSize( int, 2 );

static BlockAlloc one_ptr_manager( one_ptr_size );
static BlockAlloc two_ptr_manager( two_ptr_size );

static void *val_skip_list_alloc( size_t size ) {
    switch( size ) {
	case one_ptr_size:
	    return( one_ptr_manager.allocator( size ) );
	case two_ptr_size:
	    return( two_ptr_manager.allocator( size ) );
	default:
	    return( new char[ size ] );
    }
}

static void val_skip_list_dealloc( void *old, size_t size ) {
    switch( size ) {
	case one_ptr_size:
	    one_ptr_manager.deallocator( old, size );
	    break;
	case two_ptr_size:
	    two_ptr_manager.deallocator( old, size );
	    break;
	default:
	    delete [] (char*) old;
	    break;
    }
}


// test WCValSkipList<int>
void test_val_skip_list() {
    WCValSkipList<long> skiplist( WCSKIPLIST_PROB_QUARTER
    			       , WCDEFAULT_SKIPLIST_MAX_PTRS
			       , &val_skip_list_alloc
    			       , &val_skip_list_dealloc );
    
    for( int i = 0; i < NumTestElems; i++ ) {
	skiplist.insert( test_elems[ i ] );
    }
    int j = 0;
    
    WCValSkipListIter<long> iter( skiplist );
    while( ++iter ) {
	cout << long(iter.current()) << " ";
	if(( j++ % 8 ) == 1 ) cout << endl;
    }
    cout << "\n\n\n";
    skiplist.clear();
}
