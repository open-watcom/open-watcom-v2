#include <windows.h>
#include <malloc.h>
#include <string.h>
#include <stdlib.h>
#include "edit.h"

#ifdef __WINDOWS_386__
#define amalloc malloc
#define arealloc realloc
#define afree free
#define amemset memset
#else
#define amalloc _fmalloc
#define arealloc _frealloc
#define afree _ffree
#define amemset _fmemset
#endif


/*
 * MemAlloc - get some memory
 */
ALLOCPTR MemAlloc( unsigned size )
{
ALLOCPTR        ptr;

        if( size == 0 ) return( NULL );
        ptr = amalloc( size );
        if( ptr != NULL ) {
            amemset( ptr, 0, size );
        } else {
            MessageBox( NULL, "Out Of Memory!", EditTitle,
                        MB_SYSTEMMODAL | MB_OK );
            exit( -1 );         /* panic situation */
        }
        return( ptr );

} /* MemAlloc */

/*
 * MemRealloc - reallocate a block
 */
ALLOCPTR MemRealloc( ALLOCPTR ptr, unsigned newsize )
{
        return( arealloc( ptr, newsize ) );

} /* MemRealloc */

/*
 * MemFree - free some memory
 */
void MemFree( ALLOCPTR ptr )
{
        afree( ptr );

} /* MemFree */
