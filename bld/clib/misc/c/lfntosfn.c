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
* Description:  A function to convert long filenames to short filenames (DOS)
*
****************************************************************************/


#include "variety.h"
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include <dos.h>

_WCRTLINK int _islfn( const char *path )
{
    const char *buff;
    
    buff = strrchr( path, '\\' );
    if( buff == NULL ) {
        buff = path;
        if( buff[0] != '\0' && buff[1] == ':' ) {
            buff += 2;
        }
    } else {
        ++buff;
    }
    if( strlen( buff ) > 12 || strchr( buff, ' ' ) != NULL )
        return( 1 );
    return( 0 );
}

_WCRTLINK char *_lfntosfn( char *orgname, char *shortname )
{
#ifdef __WATCOM_LFN__
    union  REGS     r;
    struct SREGS    s;

    r.w.ax = 0x7160;              /* LFN Truename, CL = 1 */
    r.h.cl = 1;
    r.h.ch = 0;
    s.ds   = FP_SEG( orgname   ); /* LFN path goes in DS:SI */
    r.w.si = FP_OFF( orgname   );
    s.es   = FP_SEG( shortname ); /* Buffer for short name goes in ES:DI */
    r.w.di = FP_OFF( shortname );

    intdosx( &r, &r, &s );        /* Let's call the interrupt */

    /*
     * If ax = 7100, there is probably an LFN TSR but no LFN support for
     * whatever drive or directory is being searched. In that case, return the
     * original path/name.  Also if the function fails, it could be because of
     * no LFN TSR so fall back to the original name.
     */
    if( r.w.cflag || r.w.ax == 0x7100 ) {
        strcpy( shortname, orgname );
        return( NULL );
    }

    /* If there was no failure, return the new short filename */
    return( shortname );
#else
    return( strcpy( shortname, orgname ) );
#endif
}
