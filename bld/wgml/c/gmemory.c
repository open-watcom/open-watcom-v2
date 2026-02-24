/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2004-2026 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  WGML memory management routines
*               uses project bld\trmem memory tracker code  if TRMEM defined
*
****************************************************************************/


#include "wgml.h"
#ifdef TRMEM
    #include "trmem.h"
#endif


#if defined( TRMEM ) && defined( _M_IX86 ) && ( __WATCOMC__ > 1290 )
#define _XSTR(s)    # s
#define TRMEMAPI(x) _Pragma(_XSTR(aux x __frame))
#else
#define TRMEMAPI(x)
#endif

#ifdef TRMEM

static  _trmem_hdl  TrHdl = _TRMEM_HDL_NONE;       // memory tracker anchor block

/***********************************************************************/
/*  Memory tracker output function                                     */
/***********************************************************************/

static void prt( void * file, const char * buf, size_t len )
{
    /* unused parameters */ (void)file; (void)len;

    fprintf( stderr, "***%s\n", buf );
}

#endif  /* TRMEM */

/***************************************************************************/
/*  init memorytracker                                                     */
/***************************************************************************/

void mem_init( void )
{
#ifdef TRMEM
    TrHdl = _trmem_open( malloc, free, realloc, strdup,
                                NULL, &prt, _TRMEM_DEF );
#endif
}

/***************************************************************************/
/*  display current memory usage                                      */
/***************************************************************************/

void mem_prt_curr_usage( void )
{
#ifdef  TRMEM
    _trmem_prt_usage( TrHdl );
#endif
}

/***************************************************************************/
/*  display peak memory usage                                      */
/***************************************************************************/

unsigned long mem_get_peak_usage( void )
{
#ifdef  TRMEM
    return( _trmem_get_peak_usage( TrHdl ) );
#else
    return( 0 );
#endif
}


/***************************************************************************/
/*  get banner text if trmem is compiled in wgml tool                      */
/***************************************************************************/

void mem_banner( void )
{
#ifdef  TRMEM
    out_msg( "Compiled with TRMEM memory tracker (trmem)\n" );
#endif
}


/***************************************************************************/
/*   memorytracker end processing                                          */
/***************************************************************************/

void mem_fini( void )
{
#ifdef TRMEM
    _trmem_prt_usage( TrHdl );
    _trmem_prt_list_ex( TrHdl, 100 );
    _trmem_close( TrHdl );
#endif
}

static void *check_nomem( void *ptr )
{
    if( ptr == NULL ) {
        xx_simple_err_exit( ERR_NOMEM_AVAIL );
        /* never return */
    }
    return( ptr );
}

/***************************************************************************/
/*  Allocate some storage                                                  */
/***************************************************************************/

TRMEMAPI( MemAlloc )
void *MemAlloc( unsigned size )
{
#ifdef TRMEM
    return( check_nomem( _trMemAlloc( size, _TRMEM_WHO( 1 ), TrHdl ) ) );
#else
    return( check_nomem( malloc( size ) ) );
#endif
}

/***************************************************************************/
/*  Re-allocate some storage                                               */
/***************************************************************************/

TRMEMAPI( MemRealloc )
void *MemRealloc( void * oldp, unsigned size )
{
#ifdef TRMEM
    return( check_nomem( _trMemRealloc( oldp, size, _TRMEM_WHO( 2 ), TrHdl ) ) );
#else
    return( check_nomem( realloc( oldp, size ) ) );
#endif
}


/***************************************************************************/
/*  duplicate string                                                  */
/***************************************************************************/

TRMEMAPI( MemStrdup )
char *MemStrdup( const char *str )
{
    if( str == NULL )
        str = "";
#ifdef TRMEM
    return( check_nomem( _trMemStrdup( str, _TRMEM_WHO( 3 ), TrHdl ) ) );
#else
    return( check_nomem( strdup( str ) ) );
#endif
}

/***************************************************************************/
/*  duplicate token                                                        */
/***************************************************************************/

TRMEMAPI( mem_tostring )
char *mem_tostring( const char *str, unsigned size )
{
    char    *p;

#ifdef TRMEM
    p = check_nomem( _trMemAlloc( size + 1, _TRMEM_WHO( 4 ), TrHdl ) );
#else
    p = check_nomem( malloc( size + 1 ) );
#endif
    strncpy( p, str, size );
    p[size] = '\0';
    return( p );
}

/***************************************************************************/
/*  Free storage                                                           */
/***************************************************************************/

TRMEMAPI( MemFree )
void MemFree( void * p )
{
#ifdef TRMEM
    _trMemFree( p, _TRMEM_WHO( 5 ), TrHdl );
#else
    free( p );
#endif
    p = NULL;
}

/* These functions were added for use in debugging */

/***************************************************************************/
/*  memorytracker validate allocated storage                               */
/***************************************************************************/

int mem_validate( void )
{
#ifdef TRMEM
    return(_trmem_validate_all( TrHdl ));
#endif
    return 1;   // always succeed if trmem not in use
}


/***************************************************************************/
/*   memorytracker print list                                              */
/***************************************************************************/

void mem_prt_list( void )
{
#ifdef TRMEM
    _trmem_prt_list( TrHdl );
#endif
}

