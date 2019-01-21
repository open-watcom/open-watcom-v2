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
* Description:  Emulate _dos_allocmem()/_dos_freemem() on OS/2.
*
****************************************************************************/


#include "variety.h"
#include <stdlib.h>
#define INCL_DOSMEMMGR
#include <wos2.h>
#include <dos.h>
#include "rtdata.h"
#include "seterrno.h"


#if defined( _M_I86 )
  typedef unsigned              mem_id;
  #if defined(__BIG_DATA__)
    #define MODIFIES __ds __es
  #else
    #define MODIFIES __es
  #endif
#elif defined( _M_IX86 ) || defined( __PPC__ )
  typedef void                  *mem_id;
#else
  #error platform not supported
#endif

_WCRTLINK unsigned _dos_allocmem( unsigned num_of_paras, mem_id *p_mem )
// Note: size is in paragraphs of 16 bytes
{
    APIRET      rc;

#if defined( _M_I86 )
    SEL         mem;
    USHORT      number_segments, remaining_bytes;

    number_segments = num_of_paras >> 12;           // Number of 64k segments
    remaining_bytes = (num_of_paras << 4) & 0xFFFF; // remainder, < 64k
    rc = DosAllocHuge( number_segments, remaining_bytes, &mem, 0, 0 );
#elif defined( _M_IX86 ) || defined( __PPC__ )
    /*
        Note:   _dos_allocmem() needs better documentation for 32-bit
                since mem is an offset instead a selector value.
    */
    void        *mem;

    rc = DosAllocMem( &mem, num_of_paras << 4, PAG_COMMIT | PAG_READ | PAG_WRITE );
#else
    #error platform not supported
#endif
    *p_mem = mem;
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    return( 0 );
}

#if defined( _M_I86 )
  extern unsigned __FreeSeg( mem_id );
  #pragma aux _dos_freemem __modify [MODIFIES]
#endif
_WCRTLINK unsigned _dos_freemem( mem_id mem )
{
#if defined( _M_I86 )
    // defined inside heap
    return( __FreeSeg( mem ) );
#elif defined( _M_IX86 ) || defined( __PPC__ )
    APIRET      rc;
    rc = DosFreeMem( mem );
    if( rc ) {
        return( __set_errno_dos_reterr( rc ) );
    }
    return( 0 );
#else
    #error platform not supported
#endif
}
