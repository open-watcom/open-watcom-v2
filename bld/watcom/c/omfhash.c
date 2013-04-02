/****************************************************************************
*
*                            Open Watcom Project
*
*    Copyright (c) 2011 Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  OMF library dictionary hash function
*
****************************************************************************/


#include "omfhash.h"


#define _rotl( a, b )   ( ( a << b ) | ( a >> ( 16 - b ) ) )
#define _rotr( a, b )   ( ( a << ( 16 - b ) ) | ( a  >> b ) )

void omflib_hash( char *name, unsigned len, hash_entry *hash, unsigned num_blocks )
{
    unsigned char   *leftptr;
    unsigned char   *rightptr;
    unsigned short  curr;
    unsigned short  block;
    unsigned short  bucket;
    unsigned short  blockd;
    unsigned short  bucketd;
    unsigned        count;

    count = len;
    leftptr = (unsigned char *)name;
    rightptr = leftptr + count;
    block = count | 0x20;
    blockd = 0;
    bucket = 0;
    bucketd = count | 0x20;
    for( ;; ) {
        --rightptr;
        /* zap to lower case (sort of) */
        curr = *rightptr | 0x20;
        blockd = curr ^ _rotl( blockd, 2 );
        bucket = curr ^ _rotr( bucket, 2 );
        if( --count == 0 ) {
            break;
        }
        curr = *leftptr | 0x20;
        ++leftptr;
        block = curr ^ _rotl( block, 2 );
        bucketd = curr ^ _rotr( bucketd, 2 );
    }
    hash->bucket = bucket % NUM_BUCKETS;
    bucketd %= NUM_BUCKETS;
    if( bucketd == 0 ) {
        bucketd = 1;
    }
    hash->bucketd = bucketd;
    hash->block = block % num_blocks;
    blockd %= num_blocks;
    if( blockd == 0 ) {
        blockd = 1;
    }
    hash->blockd = blockd;
}
