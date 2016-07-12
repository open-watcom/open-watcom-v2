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
* Description:  Heap management functions test.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <string.h>
#include <conio.h>
#ifdef _M_I86
    #include <i86.h>
#endif

#ifdef __SW_BW
    #include <wdefwin.h>
#endif

#ifndef TRUE
    #define TRUE        1
    #define FALSE       0
#endif
#define ARGLENGTH       256
#define DOT_INTERVAL    512
#define WSIZE           sizeof(int)
#define SIZE_MARGIN     16

#define TEST_PASS       0
#define TEST_FAIL       1
#define TEST_NOMEM      2
#define TEST_NOSEG      3

#define TYPE_DEFAULT    0
#define TYPE_NEAR       1
#define TYPE_FAR        2
#define TYPE_BASED      3
#define TYPE_HUGE       4

#define FREED_BEFORE    0
#define FREED_AFTER     1
#define INTERNAL_ERR    255

#if defined( _M_I86 )   // Only appropriate to 16-bit versions
    #define     HUGE_NUM_EL     16384 // Must be a power of 2
#endif

#define NUM_EL  8175    // For double, 8175 * 8 = 65400 < 64k
// Note that NUM_EL has to be < 8192. Otherwise, we get overflow when
// calling malloc( NUM_EL * sizeof( double ) ).

#define BASED_HEAP_SIZE 512

// Casting _NULLOFF to long produces 0xFFFFFFFF, while casting the return of
// failing based heap routines to long produces 0xFFFF. At this point it is
// unclear whether that is a compiler bug, whether the _NULLOFF macro is wrong,
// or whether the test makes bad assumptions. For the moment, get around it
// by casting to unsigned in a few specific cases. This hack should be removed
// when it is determined what's actually wrong and the original problem fixed.
#define HACK_CAST   (unsigned)

// This is a macro performing everything needed before returning the call
#if defined( _M_I86 )
    #define _CRET() {                                                   \
        if( doheapwlk ) AskHeapWlk( FREED_BEFORE, type, __LINE__ );     \
        if( type == TYPE_DEFAULT ) {                                    \
            free( (void *) ptr_char );                                  \
            free( (void *) ptr_int );                                   \
            free( (void *) ptr_double );                                \
        } else if( type == TYPE_NEAR ) {                                \
            _nfree( (void __near *) FP_OFF( ptr_char ) );               \
            _nfree( (void __near *) FP_OFF( ptr_int ) );                \
            _nfree( (void __near *) FP_OFF( ptr_double ) );             \
        } else if( type == TYPE_FAR ) {                                 \
            _ffree( (void __far *) ptr_char );                          \
            _ffree( (void __far *) ptr_int );                           \
            _ffree( (void __far *) ptr_double );                        \
        } else if( type == TYPE_BASED ) {                               \
            _DOBFREE();                                                 \
        } else if( type == TYPE_HUGE ) {                                \
            hfree( (void __huge *) ptr_char );                          \
            hfree( (void __huge *) ptr_int );                           \
            hfree( (void __huge *) ptr_double );                        \
        }                                                               \
        if( doheapwlk ) AskHeapWlk( FREED_AFTER, type, __LINE__ );      \
        memavail = _memavl();                                           \
        return;                                                         \
    }

    #define _CRET_HUGE() {                                              \
        if( doheapwlk ) AskHeapWlk( FREED_BEFORE, type, __LINE__ );     \
        if( type == TYPE_HUGE ) {                                       \
            hfree( (void __huge *) ptr_char );                          \
            hfree( (void __huge *) ptr_int );                           \
            hfree( (void __huge *) ptr_double );                        \
        } else {                                                        \
            printf( "INTERNAL ERROR - incorrect heap type\n" );         \
        }                                                               \
        if( doheapwlk ) AskHeapWlk( FREED_AFTER, type, __LINE__ );      \
        memavail = _memavl();                                           \
        return;                                                         \
    }

#else
    #define _CRET() {                                                   \
        if( doheapwlk ) AskHeapWlk( FREED_BEFORE, type, __LINE__ );     \
        if( type == TYPE_DEFAULT ) {                                    \
            free( (char *) ptr_char );                                  \
            free( (int *) ptr_int );                                    \
            free( (double *) ptr_double );                              \
        } else if( type == TYPE_NEAR ) {                                \
            _nfree( (char __near *) ptr_char );                         \
            _nfree( (int __near *) ptr_int );                           \
            _nfree( (double __near *) ptr_double );                     \
        }                                                               \
        if( doheapwlk ) AskHeapWlk( FREED_AFTER, type, __LINE__ );      \
        memavail = _memavl();                                           \
        return;                                                         \
    }
#endif

// _NUL combines NULL and _NULLOFF
#if defined( _M_I86 )
    #define _NUL        (type == TYPE_BASED ? (long)_NULLOFF: (long)NULL)
#else
    #define _NUL        NULL
#endif

// Macro to free based pointers and the corresponding segment
#define _DOBFREE() {                                            \
  if( (char __based(seg)*)ptr_char != _NULLOFF ) {              \
      _bfree( seg, (char __based(seg)*)ptr_char );              \
  }                                                             \
  if( (int __based(seg)*)ptr_int != _NULLOFF ) {                \
      _bfree( seg,(int __based(seg)*)ptr_int );                 \
  }                                                             \
  if( (double __based(seg)*)ptr_double != _NULLOFF ) {          \
      _bfree( seg,(double __based(seg)*)ptr_double);            \
  }                                                             \
}

typedef struct _test_result {
    char    funcname[80];
    int     status;
    char    msg[256];
} test_result;

#if defined( _M_I86 ) && !defined(__WINDOWS__)
    size_t      memrecord;
#endif
size_t memavail;
int more_debug = FALSE;
int nomem_lineno = 0;
int noseg_lineno = 0;
int dotrace = FALSE;
int dopause = FALSE;
int tracethisloop;
int doheapwlk = FALSE;
#if defined( _M_I86 )
    __segment   seg = _NULLSEG;
#endif

static const char *errmsg[] = {
    "ABLE TO ALLOCATE MORE THAN ALL OF THE AVAILABLE STACK",            // 0
    "CANNOT ALLOCATE HALF OF THE AVAILABLE STACK",                      // 1
    "RETURNED NULL FOR char BUT SUCCEEDED FOR int OR double",           // 2
    "RETURNED NULL FOR int BUT SUCCEEDED FOR double",                   // 3
    "NOT ABLE TO CLEAR THE ALLOCATED MEMORY (SET TO ZERO)",             // 4
    "NOT ABLE TO REALLOCATE HALF THE ORIGINAL MEMORY BLOCK",            // 5
    "CONTENT OF THE REALLOCATED MEMORY IS ALTERED",                     // 6
    "_memavl() RETURNED A VALUE WHICH IS LESS THAN _memmax()",          // 7
    "NEAR AVAILABLE HEAP SHRINKED AFTER THIS FUNCTION CALL",            // 8
    "RETURNED VALUE IS INCORRECT",                                      // 9
    "REPORTED THAT LESS THAN 1 DOUBLE CAN BE ALLOCATED, BUG?",          // 10
    "CANNOT ALLOCATE THE RETURNED NUMBER OF DOUBLES",                   // 11
    "UNABLE TO ALLOCATE MEMORY OF SIZE SPECIFIED BY _memmax()",         // 12
    "VALUES RETURNED BY _memmax() are inconsistant",                    // 13
    "SUCCEEDED IN ALLOCATING MORE THAN AVAILABLE HEAP",                 // 14
    "NOT ABLE TO EXPAND THE BLOCK BACK TO THE SIZE IT WAS BEFORE",      // 15
    "FAILED IN ALLOCATING MEMORY FROM A FAR HEAP",                      // 16
    "FAILED IN ALLOCATING MEMORY FROM A 32-BIT NEAR HEAP",              // 17
    "FREE NEAR HEAP HAS SHRUNK AFTER ALLOCATING MEMORY FROM A FAR HEAP" // 18
};

void ShowDot( int ctr )
{
    if( ( ctr % DOT_INTERVAL ) == 0 ) putch( '.' );
}

void AskHeapWlk( int timing, int type, int lineno )
{
    char ans;

    if( type == TYPE_HUGE ) return;     // No huge _heapwalk()
    cprintf( "[-h] Dump the heap %s freeing? ",
             (timing == FREED_BEFORE ) ? "before" : "after" );
    ans = getche();
    cprintf( "\n\r" );
    if( ans == 'y' || ans == 'Y' ) {
        struct _heapinfo h_info;
        int heap_status;

        h_info._pentry = NULL;
        printf( "-----------------------------------------\n" );
        printf( "Heap walk result (%s freeing memory):\n",
                (timing == FREED_BEFORE ) ? "before" : "after" );
        for( ;; ) {
            switch( type ) {
                case TYPE_DEFAULT:
                    heap_status = _heapwalk( &h_info );
                    break;
                case TYPE_NEAR:
                    heap_status = _nheapwalk( &h_info );
                    break;
#if defined( _M_I86 )
                case TYPE_FAR:
                    heap_status = _fheapwalk( &h_info );
                    break;
                case TYPE_BASED:
                    heap_status = _bheapwalk( seg, &h_info );
                    break;
#endif
                default:
                    heap_status = INTERNAL_ERR;
                    break;
            }
            if( heap_status != _HEAPOK ) break;
            printf( "  %s block at %Fp of size %4.4X\n",
                    ( h_info._useflag == _USEDENTRY ? "USED" : "FREE" ),
                    h_info._pentry, h_info._size );
        }
        switch( heap_status ) {
            case _HEAPEND:
                printf( "OK - end of heap\n" );
                break;
            case _HEAPEMPTY:
                printf( "OK - heap is empty\n" );
                break;
            case _HEAPBADBEGIN:
                printf( "ERROR - heap is damaged\n" );
                printf( "Reference: line #%d in source.\n", lineno );
                break;
            case _HEAPBADPTR:
                printf( "ERROR - bad pointer to heap\n" );
                printf( "Reference: line #%d in source.\n", lineno );
                break;
            case _HEAPBADNODE:
                printf( "ERROR - bad node in heap\n" );
                printf( "Reference: line #%d in source.\n", lineno );
                break;
            case INTERNAL_ERR:
                printf( "INTERNAL ERROR - unrecognized heap type\n" );
                break;
            default:
                break;
        }
        printf( "-----------------------------------------\n" );
    }
}

int AskTrace( char *name, int lineno )
{
    if( dotrace ) {
        char ans;

        cprintf( "[-t] %s: Do you want to trace into the loop? ", name );
        ans = getche();
        cprintf( "\n\r" );
        if( ans == 'y' || ans == 'Y' ) {
            cprintf( "Into the loop (line #%d)...", lineno );
            return( TRUE );
        }
        cprintf( "Continue on...\r\n" );
    }
    return( FALSE );
}

int CheckContent( long ptr, size_t n, int type )
{
    size_t      ctr;
    int         flag;

    for( ctr = 0, flag = TRUE; ( ctr < n ) && flag; ++ctr ) {
        switch( type ) {
            case TYPE_DEFAULT:
                flag = ( ((int *)ptr)[ctr] == 6 );
                break;
            case TYPE_NEAR:
                flag = ( ((int __near *)ptr)[ctr] == 6 );
                break;
#if defined( _M_I86 )
            case TYPE_FAR:
                flag = ( ((int __far *)ptr)[ctr] == 6 );
                break;
            case TYPE_BASED:
                flag = ( ((int __based( seg ) *)ptr)[ctr] == 6 );
                break;
#endif
            default:
                break;
        }
    }
    return( flag );
}

void Test_alloca_stackavail__memavl__memmax( test_result *result )
{
    size_t      ctr, buffsize, memsize;
    char        *buffer;
    char __near *near_buffer;

    if( more_debug ) {
        printf( "Testing: alloca(), stackavail(), " );
        printf( "_memavl(), _memmax()...\n" );
    }
    strcpy( result->funcname,"_memavl() or _memmax()" );
    memsize = _memmax();
    if( _memavl() < memsize ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[7] );
        return;
    }
    near_buffer = (char __near *) _nmalloc( memsize );
    if( near_buffer == NULL ) {
        strcpy( result->funcname, "_memmax() or _nmalloc()" );
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[12] );
        return;
    }
    _nfree( near_buffer );
    if( memsize != _memmax() ) {
        strcpy( result->funcname, "_memmax() or _nfree()" );
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[13] );
        return;
    }
    memsize = _memavl();
    strcpy( result->funcname,"stackavail() and/or alloca()" );

    buffsize = stackavail() + 1;
    buffer = alloca( buffsize ); // Allocate more than available stack size
    if( buffer != NULL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[0] );
        return;
    }

    buffsize = stackavail() >> 1;
    buffer = alloca( buffsize ); // Allocate half of available stack size
    if( buffer == NULL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[1] );
        return;
    }
    if( _memavl() < memsize ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[8] );
    }
    tracethisloop = AskTrace( result->funcname, __LINE__ );
    for( ctr = 0; ctr < buffsize; ++ctr ) {
        *(buffer + ctr) = 6;
        if( tracethisloop ) {
            ShowDot( ctr );
        }
    }   // Make sure that the buffer is accessible before returning
    if( tracethisloop ) cprintf( "\r\nTrace done. No problems detected.\r\n");
    result->status = TEST_PASS;
}

void Test_calloc__msize( test_result *result, int type )
{
    long        ptr_char   = (long)NULL;
    long        ptr_int    = (long)NULL;
    long        ptr_double = (long)NULL;
    size_t      ctr, size, retsize, chkmemavl;

    switch( type ) {
        case TYPE_DEFAULT:
            if( more_debug ) {
                printf( "Testing: calloc(), msize()...\n" );
            }
            strcpy( result->funcname,"calloc()" );
            ptr_char   = (long) calloc( NUM_EL, sizeof( char ) );
            ptr_int    = (long) calloc( NUM_EL, sizeof( int ) );
            chkmemavl = _memavl();
            ptr_double = (long) calloc( NUM_EL, sizeof( double ) );
            break;
        case TYPE_NEAR:
            if( more_debug ) {
                printf( "Testing: _ncalloc(), _nmsize()...\n" );
            }
            strcpy( result->funcname,"_ncalloc()" );
            ptr_char   = (long) _ncalloc( NUM_EL, sizeof( char ) );
            ptr_int    = (long) _ncalloc( NUM_EL, sizeof( int ) );
            ptr_double = (long) _ncalloc( NUM_EL, sizeof( double ) );
            break;
#if defined( _M_I86 )
        case TYPE_FAR:
            if( more_debug ) {
                printf( "Testing: _fcalloc(), _fmsize()...\n" );
            }
            strcpy( result->funcname,"_fcalloc()" );
            ptr_char   = (long) _fcalloc( NUM_EL, sizeof( char ) );
            ptr_int    = (long) _fcalloc( NUM_EL, sizeof( int ) );
            chkmemavl = _memavl();
            ptr_double = (long) _fcalloc( NUM_EL, sizeof( double ) );
            break;
        case TYPE_BASED:
            if( more_debug ) {
                printf( "Testing: _bcalloc(), _bmsize()...\n" );
            }
            if( seg == _NULLSEG ) {
                result->status = TEST_NOSEG;
                if( more_debug ) noseg_lineno = __LINE__;
                return;
            }
            strcpy( result->funcname,"_bcalloc()" );
            ptr_char   = (long) _bcalloc( seg, NUM_EL, sizeof( char ) );
            ptr_int    = (long) _bcalloc( seg, NUM_EL, sizeof( int ) );
            chkmemavl = _memavl();
            ptr_double = (long) _bcalloc( seg, NUM_EL, sizeof( double ) );
            break;
#endif
        default:
            break;
    }
    if( ptr_char == _NUL && ( ptr_int != _NUL || ptr_double != _NUL ) ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[2] );
        _CRET();
    }
    if( ptr_int == _NUL && ptr_double != _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[3] );
        _CRET();
    }
    if( ptr_char == _NUL || ptr_int == _NUL ) {
        result->status = TEST_NOMEM;
        if( more_debug ) nomem_lineno = __LINE__;
        _CRET();
    }
#if defined( _M_I86 ) && (defined(__SMALL__) || defined(__MEDIUM__))
    if( type != TYPE_FAR && HACK_CAST ptr_double != HACK_CAST _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[14] );
        _CRET();
    } else if( type == TYPE_FAR && ptr_double == _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[16] );
        _CRET();
    }
#else       // 32-bit or large data model
    if( type == TYPE_BASED || type == TYPE_NEAR ) {
    #if defined( _M_I86 )
        if( HACK_CAST ptr_double != HACK_CAST _NUL ) {
            result->status = TEST_FAIL;
            strcpy( result->msg, errmsg[14] );
            _CRET();
        }
    #endif
    } else if( ptr_double == _NUL ) {
        result->status = TEST_FAIL;
    #if defined( _M_I86 )   // Different error messages
        strcpy( result->msg, errmsg[16] );
    #else
        strcpy( result->msg, errmsg[17] );
    #endif
        _CRET();
    #if defined( _M_I86 )
    } else if( _memavl() < chkmemavl ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[18] );
        _CRET();
    #endif
    }
#endif

    result->status = TEST_PASS;
    tracethisloop = AskTrace( result->funcname, __LINE__ );
    for( ctr = 0; ctr < NUM_EL && result->status == TEST_PASS; ++ctr ) {
        switch( type ) {
            case TYPE_DEFAULT:
                if( ((char *)ptr_char)[ctr] != 0 ||
                    ((int *)ptr_int)[ctr] != 0 ) {
                    result->status = TEST_FAIL;
                }
                break;
            case TYPE_NEAR:
                if( ((char __near *)ptr_char)[ctr] != 0 ||
                    ((int __near *)ptr_int)[ctr] != 0 ) {
                    result->status = TEST_FAIL;
                }
                break;
#if defined( _M_I86 )
            case TYPE_FAR:
                if( ((char __far *)ptr_char)[ctr] != 0 ||
                    ((int __far *)ptr_int)[ctr] != 0 ) {
                    result->status = TEST_FAIL;
                }
                break;
            case TYPE_BASED:
                if( ((char __based( seg ) *)ptr_char)[ctr] != 0 ||
                    ((int __based( seg ) *)ptr_int)[ctr] != 0 ) {
                    result->status = TEST_FAIL;
                }
                break;
#endif
            default:
                break;
        }
        if( tracethisloop ) ShowDot( ctr );
    }
    if( tracethisloop )cprintf( "\r\nTrace done. No accessing problems.\r\n");
    if( result->status != TEST_PASS ) {
        strcpy( result->msg, errmsg[4] ); // didn't clear the memory
        _CRET();
    }
    size = NUM_EL;
    if( size % WSIZE != 0 ) {
        size += ( WSIZE - ( size % WSIZE ) );   // Align it to WSIZE
    }
    switch( type ) {
        case TYPE_DEFAULT:
            strcpy( result->funcname,"_msize()" );
            retsize = _msize( (char *)ptr_char );
            break;
        case TYPE_NEAR:
            strcpy( result->funcname,"_nmsize()" );
            retsize = _nmsize( (char __near *)ptr_char );
            break;
#if defined( _M_I86 )
        case TYPE_FAR:
            strcpy( result->funcname,"_fmsize()" );
            retsize = _fmsize( (char __far *)ptr_char );
            break;
        case TYPE_BASED:
            strcpy( result->funcname,"_bmsize()" );
            retsize = _bmsize( seg, (char __based( seg ) *)ptr_char );
            break;
#endif
        default:
            break;
    }
    if( retsize < size || retsize > size + SIZE_MARGIN ) {
        result->status = TEST_FAIL;
        sprintf( result->msg, "%s. size = %u, returned = %u", errmsg[9],
                 size, retsize );
    }
    _CRET();
} // Test_calloc__msize() //

void Test_malloc_realloc__expand( test_result *result, int type )
{
    size_t      ctr, size, chkmemavl;
    int         f_pass;
    long        ptr_char    = (long)NULL;
    long        ptr_int     = (long)NULL;
    long        ptr_double  = (long)NULL;
    long        tmp_ptr     = (long)NULL;

    switch( type ) {
        case TYPE_DEFAULT:
            if( more_debug ) {
                printf( "Testing: malloc(), realloc(), expand()...\n" );
            }
            strcpy( result->funcname,"malloc()" );
            ptr_char   = (long) malloc( NUM_EL * sizeof( char ) );
            ptr_int    = (long) malloc( NUM_EL * sizeof( int ) );
            chkmemavl = _memavl();
            ptr_double = (long) malloc( NUM_EL * sizeof( double ) );
            break;
        case TYPE_NEAR:
            if( more_debug ) {
                printf( "Testing: _nmalloc(), _nrealloc(), _nexpand()...\n" );
            }
            strcpy( result->funcname,"_nmalloc()" );
            ptr_char   = (long) _nmalloc( NUM_EL * sizeof( char ) );
            ptr_int    = (long) _nmalloc( NUM_EL * sizeof( int ) );
            ptr_double = (long) _nmalloc( NUM_EL * sizeof( double ) );
            break;
#if defined( _M_I86 )
        case TYPE_FAR:
            if( more_debug ) {
                printf( "Testing: _fmalloc(), _frealloc(), _fexpand()...\n" );
            }
            strcpy( result->funcname,"_fmalloc()" );
            ptr_char   = (long) _fmalloc( NUM_EL * sizeof( char ) );
            ptr_int    = (long) _fmalloc( NUM_EL * sizeof( int ) );
            chkmemavl = _memavl();
            ptr_double = (long) _fmalloc( NUM_EL * sizeof( double ) );
            break;
        case TYPE_BASED:
            if( more_debug ) {
                printf( "Testing: _bmalloc(), _brealloc(), _bexpand()...\n" );
            }
            if( seg == _NULLSEG ) {
                result->status = TEST_NOSEG;
                if( more_debug ) noseg_lineno = __LINE__;
                return;
            }
            strcpy( result->funcname,"_bmalloc()" );
            ptr_char   = (long) _bmalloc( seg, NUM_EL * sizeof( char ) );
            ptr_int    = (long) _bmalloc( seg, NUM_EL * sizeof( int ) );
            chkmemavl = _memavl();
            ptr_double = (long) _bmalloc( seg, NUM_EL * sizeof( double ) );
            break;
#endif
        default:
            break;
    }
    if( ptr_char == _NUL && (ptr_int != _NUL || ptr_double !=_NUL) ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[2] );
        _CRET();
    }
    if( ptr_int == _NUL && ptr_double != _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[3] );
        _CRET();
    }
    if( ptr_char == _NUL || ptr_int == _NUL ) {
        result->status = TEST_NOMEM;
        if( more_debug ) nomem_lineno = __LINE__;
        _CRET();
    }
#if defined( _M_I86 ) && (defined(__SMALL__) || defined(__MEDIUM__))
    if( type != TYPE_FAR && HACK_CAST ptr_double != HACK_CAST _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[14] );
        _CRET();
    } else if( type == TYPE_FAR && ptr_double == _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[16] );
        _CRET();
    }
#else       // 32-bit or large data model
    if( type == TYPE_BASED || type == TYPE_NEAR ) {
    #if defined( _M_I86 )
        if( HACK_CAST ptr_double != HACK_CAST _NUL ) {
            result->status = TEST_FAIL;
            strcpy( result->msg, errmsg[14] );
            _CRET();
        }
    #endif
    } else if( ptr_double == _NUL ) {
        result->status = TEST_FAIL;
    #if defined( _M_I86 )   // Different error messages
        strcpy( result->msg, errmsg[16] );
    #else
        strcpy( result->msg, errmsg[17] );
    #endif
        _CRET();
    #if defined( _M_I86 )
    } else if( _memavl() < chkmemavl ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[18] );
        _CRET();
    #endif
    }
#endif

    result->status = TEST_PASS;
    tracethisloop = AskTrace( result->funcname, __LINE__ );
    for( ctr = 0; ctr < NUM_EL; ++ctr ) {
        switch( type ) {
            case TYPE_DEFAULT:
                ((char *)ptr_char)[ctr] = 6;
                ((int *)ptr_int)[ctr]   = 6;
                break;
            case TYPE_NEAR:
                ((char __near *)ptr_char)[ctr] = 6;
                ((int __near *)ptr_int)[ctr]   = 6;
                break;
#if defined( _M_I86 )
            case TYPE_FAR:
                ((char __far *)ptr_char)[ctr] = 6;
                ((int __far *)ptr_int)[ctr]   = 6;
                break;
            case TYPE_BASED:
                ((char __based( seg ) *)ptr_char)[ctr] = 6;
                ((int __based( seg ) *)ptr_int)[ctr]   = 6;
                break;
#endif
            default:
                break;
        }
        if( tracethisloop ) ShowDot( ctr );
    }   // Make sure that array is accessible.
    if(tracethisloop) cprintf( "\r\nTrace done. No problems detected.\r\n" );

    // The following tests realloc() and _expand():
    size = ( NUM_EL >> 1) * sizeof( int ); // shrink by half
    switch( type ) {
        case TYPE_DEFAULT:
            strcpy( result->funcname, "realloc()" );
            tmp_ptr = (long)realloc( (int *)ptr_int, size );
            break;
        case TYPE_NEAR:
            strcpy( result->funcname, "_nrealloc()" );
            tmp_ptr = (long) _nrealloc((int __near*) ptr_int, size );
            break;
#if defined( _M_I86 )
        case TYPE_FAR:
            strcpy( result->funcname, "_frealloc()" );
            tmp_ptr = (long) _frealloc( (int __far *) ptr_int, size );
            break;
        case TYPE_BASED:
            strcpy( result->funcname, "_brealloc()" );
            tmp_ptr = (long) _brealloc( seg,(int __based(seg)*)ptr_int,size);
            break;
#endif
        default:
            break;
    }
    if( tmp_ptr == _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[5] );
        _CRET();
    }

    ptr_int = tmp_ptr;
    f_pass = CheckContent( ptr_int, ( NUM_EL >> 1 ), type );
    if( !f_pass ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[6] ); // Content changed!
        _CRET();
    }   // realloc() done.

    size = NUM_EL * sizeof( int );      // Back to original size
    switch( type ) {
        case TYPE_DEFAULT:
            strcpy( result->funcname, "_expand()" );
            tmp_ptr = (long)_expand( (int *)ptr_int, size );
            break;
        case TYPE_NEAR:
            strcpy( result->funcname, "_nexpand()" );
            tmp_ptr = (long)_nexpand( (int __near *)ptr_int, size );
            break;
#if defined( _M_I86 )
        case TYPE_FAR:
            strcpy( result->funcname, "_fexpand()" );
            tmp_ptr = (long)_fexpand( (int __far *)ptr_int, size );
            break;
        case TYPE_BASED:
            strcpy( result->funcname, "_bexpand()" );
            tmp_ptr = (long)_bexpand( seg, (int __based(seg)*)ptr_int, size );
            break;
#endif
        default:
            break;
    }
    if( tmp_ptr == _NUL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[15] );
        _CRET();
    }

    f_pass = CheckContent( ptr_int, ( NUM_EL >> 1 ), type );
    if( !f_pass ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[6] ); // Content changed!
        _CRET();
    }   // realloc() done.

    for( ctr = ( NUM_EL >> 1 ); ctr < NUM_EL; ++ctr ) {
        switch( type ) {
            case TYPE_DEFAULT:
                ((char *)ptr_char)[ctr] = 6;
                ((int *)ptr_int)[ctr]   = 6;
                break;
            case TYPE_NEAR:
                ((char __near *)ptr_char)[ctr] = 6;
                ((int __near *)ptr_int)[ctr]   = 6;
                break;
#if defined( _M_I86 )
            case TYPE_FAR:
                ((char __far *)ptr_char)[ctr] = 6;
                ((int __far *)ptr_int)[ctr]   = 6;
                break;
            case TYPE_BASED:
                ((char __based( seg ) *)ptr_char)[ctr] = 6;
                ((int __based( seg ) *)ptr_int)[ctr]   = 6;
                break;
#endif
            default:
                break;
        }
    }   // Make sure that rest of the array is still accessible.
    _CRET();
}

void Test__freect( test_result *result )
{
    size_t      num, ctr;

    if( more_debug ) {
        printf( "Testing: _freect()...\n" );
    }

    strcpy( result->funcname, "_freect()" );
    num = _freect( sizeof( double ) );
    if( num <= 1 ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[10] );
        return;
    }
    for( ctr = 0; ctr < num - 1; ++ctr ) {
        if( _nmalloc( sizeof( double ) ) == NULL ) {
            break;
        }
    }
    if( _freect( sizeof( double ) ) != 1 || ctr != num - 1 ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[11] );
        return;
    }
    result->status = TEST_PASS;
    /*
        Note: this should be the last test that involves the near heap
              since the memory allocated is not freed.
    */
#if defined( _M_I86 ) && !defined(__WINDOWS__)
    // To avoid the warning in TranslateResult()
    //      - near heap should almost be used up.
    memrecord = memavail = _memavl();
#endif
}

#if defined( _M_I86 )
void Test_halloc( test_result *result )
{
    int           ctr;
    char __huge   *ptr_char   = NULL;
    int __huge    *ptr_int    = NULL;
    double __huge *ptr_double = NULL;
    int           type = TYPE_HUGE;

    if( more_debug ) {
        printf( "Testing: halloc()...\n" );
    }

    strcpy( result->funcname,"halloc()" );
    ptr_char   = (char __huge *)halloc( HUGE_NUM_EL, sizeof( char ) );
    ptr_int    = (int __huge *)halloc( HUGE_NUM_EL, sizeof( int ) );
    if( ptr_char == NULL && ptr_int != NULL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[2] );
        _CRET_HUGE();
    }
    if( ptr_char == NULL ) {
        result->status = TEST_NOMEM ;
        if( more_debug ) nomem_lineno = __LINE__;
        _CRET_HUGE();
    }
    ptr_double = (double __huge *)halloc( HUGE_NUM_EL, sizeof( double ) );
    if( ptr_int == NULL && ptr_double != NULL ) {
        result->status = TEST_FAIL;
        strcpy( result->msg, errmsg[3] );
        _CRET_HUGE();
    }
    if( ptr_int == NULL || ptr_double == NULL) {
        result->status = TEST_NOMEM ;
        if( more_debug ) nomem_lineno = __LINE__;
        _CRET_HUGE();
    }
    result->status = TEST_PASS;
    for( ctr = 0; ctr < HUGE_NUM_EL && result->status == TEST_PASS; ++ctr ) {
        if( ptr_char[ctr] != 0 ||
            ptr_int[ctr] != 0 ||
            ptr_double[ctr] != 0 ) {
            result->status = TEST_FAIL;
        }
    }
    if( result->status != TEST_PASS ) {
        strcpy( result->msg, errmsg[4] ); // didn't clear the memory
    }
    tracethisloop = AskTrace( result->funcname, __LINE__ );
    for( ctr = 0; ctr < HUGE_NUM_EL; ++ctr ) {
        ptr_char[ctr]   = ptr_int[ctr] = ptr_double[ctr] = 6;
        if( tracethisloop ) ShowDot( ctr );
    }   // Make sure that array is accessible.
    if( tracethisloop ) cprintf( "\r\nTrace done. No problems detected.\r\n");
    _CRET_HUGE();
}
#endif

void TranslateResult( test_result *result )
{
    if( result->status == TEST_FAIL ) {
        printf( "FUNCTION(S) FAILED: %s.\n", result->funcname );
        printf( "Message: %s.\n", result->msg );
    } else if( result->status == TEST_NOMEM ) {
        printf( "FUNCTION(S): %s.\n", result->funcname );
        printf( "Encountered insufficient memory. Test section aborted.\n" );
        if( more_debug ) {
            printf( "[-d] Allocation failure was checked in line " );
            printf( "#%d.\n", nomem_lineno );
        }
    } else if( result->status == TEST_NOSEG ) {
        printf( "FUNCTION(S): %s.\n", result->funcname );
        printf( "Encountered insufficient segment. Test section aborted.\n" );
        if( more_debug ) {
            printf( "[-d] Allocation failure was checked in line " );
            printf( "#%d.\n", noseg_lineno );
        }
    } else if( result->status != TEST_PASS ) {
        printf( "INTERNAL: UNEXPECTED TEST RESULT.\n" );
        exit( EXIT_FAILURE );
    }
#if defined( _M_I86 ) && !defined(__WINDOWS__)
    if( memrecord != memavail ) {
        printf( "FUNCTION(S): %s.\n", result->funcname );
        printf( "WARNING: SIZE OF FREE NEAR HEAP HAS BEEN CHANGED.\n" );
        if( more_debug ) {
            printf( "Before allocation of memory:        " );
            printf( "_memavl() = %u\n", memrecord );
            printf( "After freeing the allocated memory: " );
            printf( "_memavl() = %u\n", memavail );
        }
    }
    memrecord = _memavl();
#endif
    if( dopause ) {
        cprintf( "[-p] End of this test. Press any key to continue." );
        getche();
        cprintf( "\r\n" );
    }
}

void Usage( char *filename )
{
    char *tmp;

    tmp = strrchr( filename, '\\' );
    if( tmp != NULL ) filename = tmp + 1;
    cprintf( "%-16s",filename );
    cprintf( "Test program for the memory allocation routines.\r\n" );
    cprintf( "Usage:  %s [-d][-p][-h][-t][-c]\r\n", filename );
    cprintf( "Where:  -d      activates extra debugging information.\r\n" );
    cprintf( "        -p      does [-d] and pauses between tests.\r\n" );
    cprintf( "        -h      [-d] & uses _heapwalk() to check the heap.\r\n" );
    cprintf( "        -t      enables loop tracing for some tests.\r\n" );
    cprintf( "        -c      does a comprehensive test (-h -t).\r\n" );
    cprintf( "        -?      displays this message." );
}

void ParseArgs( int argc, char *argv[] )
{
    int ctr, charcount;
    char *p;
    char buffer[ARGLENGTH];
    char *delims = { "/-" };

    if( argc == 1 ) return;
    *buffer = '\0';
    if( argv[1][0] == '?' ) {
        Usage( argv[0] );
        exit( EXIT_FAILURE );
    } else if( argv[1][0] != '/' && argv[1][0] != '-' ) {
        cprintf( "Invalid option '%s'.\r\n", argv[1] );
        exit( EXIT_FAILURE );
    }
    for( ctr = 1, charcount = 0; ctr < argc; ++ctr ) {
        charcount += ( strlen( argv[ctr] ) + 1 );
        if( charcount >= ARGLENGTH ) {
            fprintf( stderr, "Argument list too long.\n" );
            exit( EXIT_FAILURE );
        }
        strcat( buffer, argv[ctr] );
    }
    p = strtok( buffer, delims );
    while( p != NULL ) {
        switch( p[0] ) {
            case 'c':
            case 'C':
                more_debug = TRUE;
                doheapwlk = TRUE;
                dotrace = TRUE;
                break;
            case 'p':
            case 'P':
                dopause = TRUE;
            case 'd':
            case 'D':
                more_debug = TRUE;
                break;
            case 'h':
            case 'H':
                doheapwlk = TRUE;
                more_debug = TRUE;
                break;
            case 't':
            case 'T':
                dotrace = TRUE;
                break;
            case '?':
                Usage( argv[0] );
                exit( EXIT_FAILURE );
                break;
            default:
                cprintf( "Invalid option '%s'.\r\n", p );
                exit( EXIT_FAILURE );
                break;
        }
        p = strtok( NULL, delims );
    }
}

void DisplayConstants( void )
{
    printf( "=================================================\n" );
    printf( "     Important constants used in the test        \n" );
    printf( "-------------------------------------------------\n" );
    printf( "  Number of elements (NUM_EL)          = %-7d \n", NUM_EL );
#if defined( _M_I86 )
    printf( "  Number of huge elemnts (HUGE_NUM_EL) = %-7d \n", HUGE_NUM_EL );
    printf( "  BASED_HEAP_SIZE                      = " );
    printf( "%-7d \n", BASED_HEAP_SIZE );
#endif
    printf( "=================================================\n" );
}

int main( int argc, char *argv[] )
{
    test_result result;

#ifdef __SW_BW
    FILE *my_stdout;
    my_stdout = freopen( "tmp.log", "a", stdout );
    if( my_stdout == NULL ) {
        fprintf( stderr, "Unable to redirect stdout\n" );
        return( EXIT_FAILURE );
    }
#endif
    ParseArgs( argc, argv );

    if( more_debug ) DisplayConstants();

#if defined( _M_I86 ) && !defined(__WINDOWS__)
    _nheapgrow();
    memrecord = memavail = _memavl();
#endif

    Test_alloca_stackavail__memavl__memmax( &result );
    TranslateResult( &result );

    Test_calloc__msize( &result, TYPE_DEFAULT );
    TranslateResult( &result );

    Test_malloc_realloc__expand( &result, TYPE_DEFAULT );
    TranslateResult( &result );

    Test_calloc__msize( &result, TYPE_NEAR );
    TranslateResult( &result );

    Test_malloc_realloc__expand( &result, TYPE_NEAR );
    TranslateResult( &result );

    Test__freect( &result );
    TranslateResult( &result );
#if defined( _M_I86 )
    Test_calloc__msize( &result, TYPE_FAR );
    TranslateResult( &result );

    Test_malloc_realloc__expand( &result, TYPE_FAR );
    TranslateResult( &result );

    seg = _bheapseg( BASED_HEAP_SIZE );

    Test_calloc__msize( &result, TYPE_BASED );
    TranslateResult( &result );

    Test_malloc_realloc__expand( &result, TYPE_BASED );
    TranslateResult( &result );

    if( seg != _NULLSEG ) _bfreeseg( seg );

    Test_halloc( &result );
    TranslateResult( &result );
#endif
    printf( "Tests completed (%s).\n", strlwr( argv[0] ) );
#ifdef __SW_BW
    fprintf( stderr, "Tests completed (%s).\n", strlwr( argv[0] ) );
    fclose( my_stdout );
    _dwShutDown();
#endif
    return( EXIT_SUCCESS );
}
