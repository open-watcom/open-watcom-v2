#include <iostream.h>
#include <fstream.h>
#include <wchash.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "strdata.h"

#define NUM_INTS 10000
const int store_size = WCValHashSetItemSize( int );

static char *int_alloc_mem;
static int int_num_stored = 0;

static void *alloc_fn( size_t size ){
    if( size == store_size ) {
	return( int_alloc_mem + int_num_stored++ * store_size );
    } else {
	return( new char[ size ] );
    }
}

static void dealloc_fn( void *old, size_t size ){
    if( size != store_size ) {
	delete( old );
    }
}

unsigned hash_fn( const int & a ) {
    return( WCValHashSet<int>::bitHash( &a, sizeof( int ) ) );
};

unsigned str_data_hash_fn( const str_data & str ) {
    return( WCValHashSet<str_data>::bitHash( str.info, strlen( str.info ) ) );
};

//unsigned str_hash_fn( const char & str ) {
//    return( WCValHashSet<str_data>::bitHash( &str, strlen( &str ) ) );
//};

template <class Type>
class StatsHashSet : public WCValHashSet< Type > {
public:
    inline StatsHashSet( unsigned (*fn)( const Type & )
		       , unsigned buckets = WC_DEFAULT_HASH_SIZE
		       ) : WCValHashSet( fn, buckets ) {};
    inline StatsHashSet( unsigned (*fn)( const Type & )
		        , unsigned buckets 
		        , void * (*user_alloc)( size_t )
		        , void (*user_dealloc)( void *, size_t )
		        ) : WCValHashSet( fn, buckets
		       			 , user_alloc, user_dealloc ) {};

    inline unsigned num_in_bucket( unsigned bucket ) {
	if( bucket < num_buckets ) {
	    return( hash_array[ bucket ].entries() );
	} else {
	    return( -1 );
	}
    }

    unsigned max_in_bucket() {
	unsigned max = 0;
	unsigned in_bucket;
	
	for( int i = 0; i < num_buckets; i++ ) {
	    in_bucket = hash_array[ i ].entries();
	    if( in_bucket > max ) {
		max = in_bucket;
	    }
	}
	return( max );
    }

    unsigned min_in_bucket() {
	unsigned min = 0xFFFF;
	unsigned in_bucket;
	
	for( int i = 0; i < num_buckets; i++ ) {
	    in_bucket = hash_array[ i ].entries();
	    if( in_bucket < min ) {
		min = in_bucket;
	    }
	}
	return( min );
    }

    double average_in_bucket() {
	return( (double)num_entries / (double)num_buckets );
    }

    double std_dev_in_buckets() {
	double sum_of_sq = 0;
	double diff;
	double mean = average_in_bucket();

	for( int i = 0; i < num_buckets; i++ ) {
	    diff = hash_array[ i ].entries() - mean;
	    sum_of_sq += diff * diff;
	}
	return( sqrt( sum_of_sq / num_buckets ) );
    };
};


template <class Type>
class PtrStatsHashSet : public WCPtrHashSet< Type > {
public:
    inline PtrStatsHashSet( unsigned (*fn)( const Type & )
		       , unsigned buckets = WC_DEFAULT_HASH_SIZE
		       ) : WCPtrHashSet( fn, buckets ) {};
    inline PtrStatsHashSet( unsigned (*fn)( const Type & )
		        , unsigned buckets 
		        , void * (*user_alloc)( size_t )
		        , void (*user_dealloc)( void *, size_t )
		        ) : WCPtrHashSet( fn, buckets
		       			 , user_alloc, user_dealloc ) {};

    inline unsigned num_in_bucket( unsigned bucket ) {
	if( bucket < num_buckets ) {
	    return( hash_array[ bucket ].entries() );
	} else {
	    return( -1 );
	}
    }

    unsigned max_in_bucket() {
	unsigned max = 0;
	unsigned in_bucket;
	
	for( int i = 0; i < num_buckets; i++ ) {
	    in_bucket = hash_array[ i ].entries();
	    if( in_bucket > max ) {
		max = in_bucket;
	    }
	}
	return( max );
    }

    unsigned min_in_bucket() {
	unsigned min = 0xFFFF;
	unsigned in_bucket;
	
	for( int i = 0; i < num_buckets; i++ ) {
	    in_bucket = hash_array[ i ].entries();
	    if( in_bucket < min ) {
		min = in_bucket;
	    }
	}
	return( min );
    }

    double average_in_bucket() {
	return( (double)num_entries / (double)num_buckets );
    }

    double std_dev_in_buckets() {
	double sum_of_sq = 0;
	double diff;
	double mean = average_in_bucket();

	for( int i = 0; i < num_buckets; i++ ) {
	    diff = hash_array[ i ].entries() - mean;
	    sum_of_sq += diff * diff;
	}
	return( sqrt( sum_of_sq / num_buckets ) );
    };
};

void test1();
void test2();
void test3();

int main() {
    int_alloc_mem = new char[ NUM_INTS * store_size ];
    test1();
    int_num_stored = 0;
    test2();
    delete( int_alloc_mem );
    test3();
    cout.flush();
    return 0;
}


void test1() {
    StatsHashSet<int> hash( &hash_fn, 2000, alloc_fn, dealloc_fn );

    for( int i = 0; i < 10000; i++ ) {
	hash.insert( i );
    }
    cout << "ordered integers 0 to 9999:\n";
    cout << "  Load:" << hash.average_in_bucket() << "\n";
    cout << "  Std Dev:" << hash.std_dev_in_buckets() << "\n";
    cout << "  Max:" << hash.max_in_bucket() << "\n";
    cout << "  Min:" << hash.min_in_bucket() << "\n";
}

    
void test2() {
    StatsHashSet<int> hash( &hash_fn, 101, alloc_fn, dealloc_fn );

    for( int i = 0; i < 5000; i++ ) {
	hash.insert( rand() );
    }
    cout << "5000 random integers\n";
    cout << "  Load:" << hash.average_in_bucket() << "\n";
    cout << "  Std Dev:" << hash.std_dev_in_buckets() << "\n";
    cout << "  Max:" << hash.max_in_bucket() << "\n";
    cout << "  Min:" << hash.min_in_bucket() << "\n";
}


void test3() {
    StatsHashSet<str_data> hash( &str_data_hash_fn, 5000 );
    fstream file( "WORD1", ios::text|ios::in );
    char buffer[ 80 ];
    unsigned words = 0;
//    int str_len;
//    char *new_str;
    
    if( file.bad() )return;
    while( !file.eof() ) {
       file.getline( buffer, 80 );
//       cout << buffer << "\n";
//       str_len = strlen( buffer ) + 1;
//       new_str = new char[ str_len ];
//       strncpy( new_str, buffer, str_len );
       hash.insert( buffer );
       words++;
       if( words % 10000 == 0 ) cerr << "inserted " << words << " words\n";
    };
    file.close();
//    cout << words << " words from Dave's dictionary\n";
    cout << hash.entries() << " words from Dave's dictionary\n";
    cout << "  Average:" << hash.average_in_bucket() << "\n";
    cout << "  Std Dev:" << hash.std_dev_in_buckets() << "\n";
    cout << "  Max:" << hash.max_in_bucket() << "\n";
    cout << "  Min:" << hash.min_in_bucket() << "\n";
    cerr << "Done getting stats, only deleting now...\n";
//    hash.clearAndDestroy();
}
