/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


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

