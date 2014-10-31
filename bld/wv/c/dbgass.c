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


#include <string.h>
#include "dbgdefn.h"
#include "dbgreg.h"
#include "dbgio.h"
#include "dbginfo.h"
#include "dbglit.h"
#include "dbgtoken.h"
#include "dbgerr.h"
#include "dbgmem.h"
#include "dbgtoggl.h"
#include "dbgitem.h"
#include "dbgbreak.h"
#include "mad.h"

extern char             *StrAddr(address *,char *,unsigned);

extern unsigned char    CurrRadix;
extern machine_state    *DbgRegs;
extern char             *TxtBuff;

static walk_result MemRefDisp( address a, mad_type_handle th,
                        mad_memref_kind mk, void *d )
{
    char                *p = TxtBuff;
    item_mach           item;
    unsigned            max;

    d = d;
    if( mk & MMK_IMPLICIT ) return( WR_CONTINUE );
    if( p[0] != '\0' ) {
        p = &p[strlen( p )];
        *p++ = ' ';
    }
    p = StrAddr( &a, p, TXT_LEN - ( p - TxtBuff ) );
#ifdef EXPERIMENTAL
    //MAD: can be more explicit about transfer direction
    switch( mk & (MMK_READ|MMK_WRITE) ) {
    case 0:
        *p = '\x1b';
        break;
    case MMK_READ:
        *p = '\x1b';
        break;
    case MMK_WRITE:
        *p = '\x1a';
        break;
    case MMK_READ|MMK_WRITE:
        *p = '\x1d';
        break;
    }
#else
    *p = '=';
#endif
    ++p;
    ItemGetMAD( &a, &item, IT_NIL, th );
    max = TXT_LEN - ( p - TxtBuff );
    MADTypeHandleToString( CurrRadix, th, &item, p, &max );
    return( WR_CONTINUE );
}

bool InsMemRef( mad_disasm_data *dd )
{
    TxtBuff[0] = '\0';
    MADDisasmMemRefWalk( dd, MemRefDisp, &DbgRegs->mr, NULL );
    return( TxtBuff[0] != '\0' );
}

