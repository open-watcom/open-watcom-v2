#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <malloc.h>
#include <new.h>

int memchk_fail;

#include "memchk.h"

#define MEMCHK_STATUS( e, s )	s,

static const char * const memchkErrorMessage[] = {
#include "memchkst.h"
};

#pragma initialize before library;

struct memory_block {
    unsigned		xor_ob;
    memory_block	*next;
    unsigned		xor_so;
    unsigned		owner;
    unsigned		xor_sb;
    unsigned		size;
    unsigned		xor_sbo;
    unsigned		bound;
    char		data[];
};

struct list_descriptor {
    memory_block	*list;
};

static list_descriptor simpleAlloc;
static list_descriptor arrayAlloc;

static unsigned allocCount;
static unsigned freeCount;

#define BOUND_CHECK	(-1)

// definitions for Hamming code verification
#define VP_SB_OK	0x01
#define VP_SO_OK	0x02
#define VP_OB_OK	0x04
#define VP_SBO_OK	0x08
#define VP_ALL_OK	0x0f
#define VP_NULL		0x00

#define VP_SIZE_ZAPPED	VP_OB_OK
#define VP_BOUND_ZAPPED	VP_SO_OK
#define VP_OWNER_ZAPPED	VP_SB_OK
#define VP_SBO_ZAPPED	(VP_OB_OK|VP_SO_OK|VP_SB_OK)
#define VP_SB_ZAPPED	(VP_OB_OK|VP_SO_OK|VP_SBO_OK)
#define VP_SO_ZAPPED	(VP_OB_OK|VP_SBO_OK|VP_SB_OK)
#define VP_OB_ZAPPED	(VP_SBO_OK|VP_SO_OK|VP_SB_OK)

#if defined(__I86__) || defined(__386__)
void break_point( void );
#pragma aux break_point = "int 3h";
#else
void break_point( void ) {
    static char *p;
    *p = '0';
}
#endif

static void dumpMessage( const char *msg, ... )
{
    FILE *fp;
    va_list args_file;
    va_list args_stderr;
    static int already_through;

    va_start( args_stderr, msg );
    va_start( args_file, msg );
    vfprintf( stderr, msg, args_stderr );
    if( ! already_through ) {
	fp = fopen( "memchk.log", "w" );
    } else {
	fp = fopen( "memchk.log", "a" );
    }
    already_through = 1;
    if( fp != NULL ) {
	vfprintf( fp, msg, args_file );
	fclose( fp );
    }
    va_end( args_stderr );
    va_end( args_file );
}

extern "C" void MemCheckError( memcheck_error rc, unsigned aindex, int delta )
{
    dumpMessage( "MEMCHK error: < %s > alloc #: %u delta: %d free #: %u\n",
	         memchkErrorMessage[ rc ], aindex, delta, freeCount );
    break_point();
}

static void *memchkAlloc( list_descriptor *kind, size_t amt )
{
    size_t check_amt;

    if( memchk_fail ) {
	return( NULL );
    }
    if( amt == 0 ) {
	amt = sizeof( int );
    }
    check_amt = amt;
    amt += sizeof( int ) - 1;
    amt &= ~( sizeof( int ) - 1 );
    if( amt < check_amt ) {
	return( NULL );
    }
    size_t size = amt;
    amt += sizeof( unsigned );
    amt += sizeof( memory_block );
    if( amt < check_amt ) {
	return( NULL );
    }
    memory_block *e = (memory_block *) malloc( amt );
    if( e == NULL ) {
	return( e );
    }
    memset( e, 0xaa, _msize( e ) );
    unsigned *end_check = (unsigned *) ( e->data + size );
    *end_check = BOUND_CHECK ^ size;
    e->next = kind->list;
    e->bound = BOUND_CHECK ^ -size;
    e->owner = ++allocCount;
    e->size = size;
    e->xor_sbo = e->size ^ e->bound ^ e->owner;
    e->xor_sb = e->size ^ e->bound;
    e->xor_so = e->size ^ e->owner;
    e->xor_ob = e->bound ^ e->owner;
    kind->list = e;
    return( e->data );
}

static int verifyBlock( memory_block *e )
{
    int delta;
    unsigned valid_parts;
    unsigned owner;
    unsigned *end_check;

    valid_parts = VP_NULL;
    if( e->xor_sb == ( e->size ^ e->bound ) ) {
	valid_parts |= VP_SB_OK;
    }
    if( e->xor_so == ( e->size ^ e->owner ) ) {
	valid_parts |= VP_SO_OK;
    }
    if( e->xor_ob == ( e->owner ^ e->bound ) ) {
	valid_parts |= VP_OB_OK;
    }
    if( e->xor_sbo == ( e->size ^ e->bound ^ e->owner ) ) {
	valid_parts |= VP_SBO_OK;
    }
    delta = -offsetof( memory_block, data );
    switch( valid_parts ) {
    case VP_ALL_OK:
	break;
    case VP_SIZE_ZAPPED:
	delta += offsetof( memory_block, size );
	MemCheckError( MC_CHECKSUM_ERROR, e->owner, delta );
	return( 1 );
    case VP_BOUND_ZAPPED:
	delta += offsetof( memory_block, bound );
	MemCheckError( MC_CHECKSUM_ERROR, e->owner, delta );
	return( 1 );
    case VP_OWNER_ZAPPED:
	delta += offsetof( memory_block, owner );
	// recover owner if possible
	owner = e->xor_sbo ^ e->size ^ e->bound;
	if( owner != ( e->xor_so ^ e->size ) ) {
	    owner = 0;
	}
	if( owner != ( e->xor_ob ^ e->bound ) ) {
	    owner = 0;
	}
	MemCheckError( MC_CHECKSUM_ERROR, owner, delta );
	return( 1 );
    case VP_SBO_ZAPPED:
	delta += offsetof( memory_block, xor_sbo );
	MemCheckError( MC_CHECKSUM_ERROR, e->owner, delta );
	return( 1 );
    case VP_SB_ZAPPED:
	delta += offsetof( memory_block, xor_sb );
	MemCheckError( MC_CHECKSUM_ERROR, e->owner, delta );
	return( 1 );
    case VP_SO_ZAPPED:
	delta += offsetof( memory_block, xor_so );
	MemCheckError( MC_CHECKSUM_ERROR, e->owner, delta );
	return( 1 );
    case VP_OB_ZAPPED:
	delta += offsetof( memory_block, xor_ob );
	MemCheckError( MC_CHECKSUM_ERROR, e->owner, delta );
	return( 1 );
    default:
	MemCheckError( MC_CHECKSUM_ERROR, 0, 0 );
	return( 1 );
    }
    if( e->bound != ( BOUND_CHECK ^ -e->size ) ) {
	delta += offsetof( memory_block, bound );
	MemCheckError( MC_LO_BOUND_ERROR, e->owner, delta );
	return( 1 );
    }
    end_check = (unsigned *) ( e->data + e->size );
    if( *end_check != ( BOUND_CHECK ^ e->size ) ) {
	MemCheckError( MC_HI_BOUND_ERROR, e->owner, e->size );
	return( 1 );
    }
    return( 0 );
}

static memory_block *findAlloc( list_descriptor *kind, memory_block *b )
{
    memory_block *c;

    for( c = kind->list; c != NULL; c = c->next ) {
	if( c == b ) {
	    return( c );
	}
    }
    return( NULL );
}

static void memchkFree( list_descriptor *kind, void *p, list_descriptor *other_kind,
			 memcheck_error other_error )
{
    memory_block *e;
    memory_block *f;
    memory_block *c;
    memory_block **u;

    ++freeCount;
    if( p == NULL ) {
	return;
    }
    u = &(kind->list);
    e = (memory_block *)(((char *)p) - offsetof( memory_block, data ));
    for( c = *u; c != NULL; c = c->next ) {
	if( c == e ) break;
	u = &(c->next);
    }
    if( c == NULL ) {
	if( findAlloc( other_kind, e ) != NULL ) {
	    MemCheckError( other_error, e->owner, 0 );
	} else {
	    MemCheckError( MC_FREE_UNALLOC_ERROR, e->owner, 0 );
	}
	verifyBlock( e );
	return;
    }
    if( verifyBlock( e ) ) {
	return;
    }
    *u = e->next;
    if( u != &(kind->list) ) {
	f = (memory_block *)(((char *)u) - offsetof( memory_block, next ));
	if( verifyBlock( f ) ) {
	    return;
	}
    }
    memset( e, 0xdd, _msize( e ) );
    free( e );
}

void *operator new( size_t amt )
{
    return( memchkAlloc( &simpleAlloc, amt ) );
}

void operator delete( void *p )
{
    memchkFree( &simpleAlloc, p, &arrayAlloc, MC_ARRAY_SIMPLE_ERROR );
}

#if __WATCOMC__ > 950
void *operator new []( size_t amt )
{
    return( memchkAlloc( &arrayAlloc, amt ) );
}

void operator delete []( void *p )
{
    memchkFree( &arrayAlloc, p, &simpleAlloc, MC_SIMPLE_ARRAY_ERROR );
}
#endif
