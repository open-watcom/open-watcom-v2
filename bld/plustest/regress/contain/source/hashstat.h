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

