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


//
// FDEFWIN       : FORTRAN API to default windowing system
//

#include "ftnstd.h"
#include "pgmacc.h"
#include "ftnapi.h"

#include <malloc.h>
#include <stddef.h>
#include <string.h>
#include <process.h>
#ifdef __SW_BW
  #include <wdefwin.h>
#else
  #define __SW_BW
  #include <wdefwin.h>
  #undef __SW_BW
#endif


intstar4        __fortran DWFSETAPPTITLE( string PGM *title ) {
//===========================================================

    char        *ttl;

    ttl = alloca( title->len + 1 );
    if( !ttl ) {
        return( 0 );
    }
    pgm_memget( ttl, title->strptr, title->len );
    ttl[ title->len ] = NULLCHAR;
    return( _dwSetAppTitle( ttl ) );
}


intstar4        __fortran DWFSETABOUTDLG( string PGM *title, string PGM *text) {
//============================================================================

    char        *ttl;
    char        *txt;

    ttl = alloca( title->len + 1 );
    if( !ttl ) {
        return( 0 );
    }
    pgm_memget( ttl, title->strptr, title->len );
    ttl[ title->len ] = NULLCHAR;

    txt = alloca( text->len + 1 );
    if( !txt ) {
        return( 0 );
    }
    pgm_memget( txt, text->strptr, text->len );
    txt[ text->len ] = NULLCHAR;
    return( _dwSetAboutDlg( ttl, txt ) );
}


intstar4        __fortran DWFSETCONTITLE( intstar4 *unit, string PGM *title ) {
//===========================================================================

    char        *ttl;
    int         fh;

    fh = SYSHANDLE( unit );
    if( fh == -1 ) return( 0 );
    ttl = alloca( title->len + 1 );
    if( !ttl ) {
        return( 0 );
    }
    pgm_memget( ttl, title->strptr, title->len );
    ttl[ title->len ] = NULLCHAR;
    return( _dwSetConTitle( SYSHANDLE( unit ), ttl ) );
}


intstar4        __fortran DWFDELETEONCLOSE( intstar4 *unit ) {
//==========================================================

    int         fh;

    fh = SYSHANDLE( unit );
    if( fh == -1 ) return( 0 );
    return( _dwDeleteOnClose( fh ) );
}


intstar4        __fortran DWFYIELD( void ) {
//========================================

    return( _dwYield() );
}


intstar4        __fortran DWFSHUTDOWN( void ) {
//===========================================

    return( _dwShutDown() );
}
