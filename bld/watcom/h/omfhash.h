/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2011-2013 The Open Watcom Contributors. All Rights Reserved.
*
* =========================================================================
*
* Description:  OMF library dictionary hash function related declarations
*
****************************************************************************/


#define NUM_BUCKETS     37

typedef struct hash_entry {
    unsigned     block;
    unsigned     blockd;
    unsigned     bucket;
    unsigned     bucketd;
} hash_entry;

extern void omflib_hash( const char *name, size_t len, hash_entry *hash, unsigned num_blocks );
