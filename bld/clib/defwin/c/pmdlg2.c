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


#include "variety.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "win.h"

static GLOBALHANDLE     dataSeg;
static ULONG            dataSegLen;

#define DISPLAY(x)      WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );

/*
 * copyString - copy from string to memory
 */
static char _WCI86FAR *copyString( char _WCI86FAR *mem, char _WCI86FAR *str, int len )
{
    if ( !mem || !str ) return( mem );
    _FARmemcpy( mem, str, len );
    return( mem+len );

} /* copyString */

/*
 * safeStrLen - measure sizeof string (even NULL );
 */
static long safeStrLen( char _WCI86FAR *str )
{
    if ( !str ) return( 0 );
    return( strlen( str ) );

} /* safeStrLen */

/*
 * _DialogTemplate - build a dialog template
 */
GLOBALHANDLE _DialogTemplate( USHORT temptype, USHORT codepage, USHORT focus )
{
    GLOBALHANDLE        data;
    UINT                blocklen;
    DLGTEMPLATE _WCI86FAR *dt;


    /*
     * get size of block and allocate memory
     */

    blocklen = sizeof( DLGTEMPLATE );
    dataSegLen = 0;
    dataSeg = NULL;

    data = PMmalloc( blocklen );
    if( !data ) return( NULL );

    /*
     * set up template
     */
    dt = data;
    dt->cbTemplate  = blocklen - sizeof( DLGITEMTEMPLATE );
    dt->type = temptype;
    dt->codepage = codepage;
    dt->offadlgti = dt->cbTemplate;
    dt->fsTemplateStatus = 1; //RESERVED;
    dt->iItemFocus = focus;
    dt->coffPresParams = RESERVED;
    return( data );

} /* _DialogTemplate */

/*
 * _AddControl - add a control to a dialog
 */
GLOBALHANDLE _AddControl( GLOBALHANDLE data, long style, USHORT dtx,
                        USHORT dty, USHORT dtcx, USHORT dtcy, USHORT id,
                        USHORT children, ULONG nclass, char *class,
                        char *text, char *presparms, char *ctldata,
                        ULONG *ctldatlen )
{
    GLOBALHANDLE        new;
    UINT                blocklen, classlen, textlen, ctldatalen, ddatalen;
    DLGTEMPLATE         _WCI86FAR *dt;
    DLGITEMTEMPLATE     _WCI86FAR *dit;
    char                _WCI86FAR *dlgtemp;

    presparms = presparms;
    /*
     * compute size of block, reallocate block to hold this stuff
     */
    classlen = SLEN( class );
    if ( classlen ) {
        classlen++;
    }
    textlen  = SLEN( text );
    if ( textlen ) {
        textlen++;
    }

    if ( !ctldata ) {
        if ( ctldatlen ) {
            ctldatalen = sizeof( ULONG );
        } else {
            ctldatalen = 0;
        }
    } else {
        ctldatalen = (UINT)ctldatlen;
    }

    dt = data;
    blocklen = sizeof( DLGITEMTEMPLATE ) + dt->cbTemplate;
    ddatalen =  classlen + textlen + ctldatalen + dataSegLen;

    new = PMrealloc( data, blocklen );
    dataSeg = PMrealloc( dataSeg, ddatalen );
    if( !new || !dataSeg ) {
        if ( dataSeg ) PMfree( dataSeg );
        if ( new ) PMfree( new );
        return( NULL );
    }
    dt = new;

    /*
     * point to start of item template, and set up values
     */
    dit = ( DLGITEMTEMPLATE *)( (char *)dt + dt->cbTemplate );
    dt->cbTemplate += sizeof( DLGITEMTEMPLATE );

    dit->fsItemStatus = RESERVED;
    dit->cChildren  = children;
    dit->flStyle = style | WS_VISIBLE;
    dit->cchText = textlen;
    if ( textlen ) {
        dit->offText = dataSegLen;
    } else {
        dit->offText = RESERVED;
    }
    dit->cchClassName = classlen;
    if ( classlen ) {
        dit->offClassName = dataSegLen + textlen;
    } else {
        dit->offClassName = nclass & 0xffff;
    }
    dit->x = dtx;
    dit->y = dty;
    dit->cx = dtcx;
    dit->cy = dtcy;
    dit->id = id;
    dit->offPresParams = 0xffff; //RESERVED;
    if ( ctldatalen ) {
        dit->offCtlData = dataSegLen + textlen + classlen;
    } else {
        dit->offCtlData = RESERVED;
    }

    dlgtemp = (char *)dataSeg + dataSegLen;

    /*
     * add extra strings to block
     */
    if ( textlen ) {
        dlgtemp = copyString( dlgtemp, text, textlen );
    }
    if ( classlen ) {
        dlgtemp = copyString( dlgtemp, class, classlen );
    }
    if ( ctldatalen ) {
        if ( ctldata ) {
             dlgtemp = copyString( dlgtemp, ctldata, ctldatalen );
        } else {
             dlgtemp = copyString( dlgtemp, (char *)(&ctldatlen), ctldatalen );
        }
    }
    dataSegLen = ddatalen;

    return( new );

} /* _AddControl */

/*
 * _DoneAddingControls - called when there are no more controls
 */
GLOBALHANDLE _DoneAddingControls( GLOBALHANDLE data )
{
    DLGITEMTEMPLATE     *temp;
    DLGTEMPLATE *dt;
    int                 record;
    int                 max;

    if ( !data || !dataSeg ) {
        return( NULL );
    }
    temp = (DLGITEMTEMPLATE *)( (char *)data + ( sizeof( DLGTEMPLATE ) - sizeof( DLGITEMTEMPLATE ) ) );
    dt = data;
    max = ( dt->cbTemplate - sizeof( DLGTEMPLATE ) + sizeof( DLGITEMTEMPLATE ) )
                / sizeof( DLGITEMTEMPLATE );
    for( record = 0; record < max; record++ ) {
        if ( temp[record].cchText ) {
            temp[record].offText += dt->cbTemplate;
        }
        if ( temp[record].offCtlData ) {
            temp[record].offCtlData += dt->cbTemplate;
        }
        if ( temp[record].cchClassName ) {
            temp[record].offClassName += dt->cbTemplate;
        }
    }
    data= PMrealloc( data, dt->cbTemplate + dataSegLen );
    dt = data;
    memcpy( (DLGITEMTEMPLATE *)( (char *)data + dt->cbTemplate ), dataSeg, dataSegLen );
    dt->cbTemplate += dataSegLen;
    PMfree( dataSeg );
    dataSeg = NULL;
    return( data );

} /* _DoneAddingControls */

/*
 * _DynamicDialogBox - create a dynamic dialog box
 */
int _DynamicDialogBox( PFNWP fn, HWND hwnd, GLOBALHANDLE data )
{
    long rc;
    HWND handle;

    handle = WinCreateDlg( HWND_DESKTOP, hwnd, (PFNWP)fn, (PDLGTEMPLATE)data, NULL );
    if ( !handle ) {
        DISPLAY("Window Creation Error Occurred");
        return( 0 );
    }
    rc = WinProcessDlg( handle );
    WinDestroyWindow( handle );
    return( rc );

} /* _DynamicDialogBox */
