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

static void             *dataSeg;
static ULONG            dataSegLen;

#define DISPLAY(x)      WinMessageBox( HWND_DESKTOP, NULL, x, "Error", 0, MB_APPLMODAL | MB_NOICON | MB_OK | MB_MOVEABLE );

/*
 * copyString - copy from string to memory
 */
static WPCHAR copyString( WPCHAR mem, const char *str, int len )
{
    if( !mem || !str )
        return( mem );
    _FARmemcpy( mem, str, len );
    return( mem + len );

} /* copyString */

/*
 * safeStrLen - measure sizeof string (even NULL );
 */
static long safeStrLen( const char *str )
{
    if ( !str )
        return( 0 );
    return( strlen( str ) );

} /* safeStrLen */

/*
 * _DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE _DialogTemplate( USHORT temptype, USHORT codepage, USHORT focus )
{
    TEMPLATE_HANDLE     dlgtemplate;
    UINT                blocklen;
    WPDLGTEMPLATE       dt;


    /*
     * get size of block and allocate memory
     */

    blocklen = sizeof( WDLGTEMPLATE );
    dataSegLen = 0;
    dataSeg = NULL;

    dlgtemplate = PMmalloc( blocklen );
    if( dlgtemplate == NULL )
        return( NULL );

    /*
     * set up template
     */
    dt = (WPDLGTEMPLATE)dlgtemplate;
    dt->cbTemplate = blocklen - sizeof( WDLGITEMTEMPLATE );
    dt->type = temptype;
    dt->codepage = codepage;
    dt->offadlgti = dt->cbTemplate;
    dt->fsTemplateStatus = 1; //RESERVED;
    dt->iItemFocus = focus;
    dt->coffPresParams = RESERVED;
    return( dlgtemplate );

} /* _DialogTemplate */

/*
 * _AddControl - add a control to a dialog
 */
TEMPLATE_HANDLE _AddControl( TEMPLATE_HANDLE old_dlgtemplate, long style, USHORT x,
                        USHORT y, USHORT cx, USHORT cy, USHORT id,
                        USHORT children, ULONG nclass,
                        const char *class, const char *text, const char *presparms,
                        const void *ctldata, ULONG ctldatalen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;
    UINT                blocklen, classlen, textlen, ddatalen;
    WPDLGTEMPLATE       dt;
    WPDLGITEMTEMPLATE   dit;
    WPCHAR              dlgtemp;
    ULONG               _ctldata;

    presparms = presparms;
    /*
     * compute size of block, reallocate block to hold this stuff
     */
    classlen = SLEN( class );
    if( classlen ) {
        classlen++;
    }
    textlen  = SLEN( text );
    if( textlen ) {
        textlen++;
    }

    if( ctldata == NULL ) {
        _ctldata = ctldatalen;
        ctldatalen = sizeof( ULONG );
    }

    dt = (WPDLGTEMPLATE)old_dlgtemplate;
    blocklen = sizeof( WDLGITEMTEMPLATE ) + dt->cbTemplate;
    ddatalen =  classlen + textlen + ctldatalen + dataSegLen;

    new_dlgtemplate = PMrealloc( old_dlgtemplate, blocklen );
    dataSeg = PMrealloc( dataSeg, ddatalen );
    if( new_dlgtemplate == NULL || !dataSeg ) {
        if( dataSeg )
            PMfree( dataSeg );
        if( new_dlgtemplate != NULL )
            PMfree( new_dlgtemplate );
        return( NULL );
    }

    /*
     * point to start of item template, and set up values
     */
    dt = (WPDLGTEMPLATE)new_dlgtemplate;
    dit = (WPDLGITEMTEMPLATE)( (WPCHAR)new_dlgtemplate + dt->cbTemplate );
    dt->cbTemplate += sizeof( WDLGITEMTEMPLATE );

    dit->fsItemStatus = RESERVED;
    dit->cChildren  = children;
    dit->flStyle = style | WS_VISIBLE;
    dit->cchText = textlen;
    if( textlen ) {
        dit->offText = dataSegLen;
    } else {
        dit->offText = RESERVED;
    }
    dit->cchClassName = classlen;
    if( classlen ) {
        dit->offClassName = dataSegLen + textlen;
    } else {
        dit->offClassName = nclass & 0xffff;
    }
    dit->x = x;
    dit->y = y;
    dit->cx = cx;
    dit->cy = cy;
    dit->id = id;
    dit->offPresParams = 0xffff; //RESERVED;
    if( ctldatalen ) {
        dit->offCtlData = dataSegLen + textlen + classlen;
    } else {
        dit->offCtlData = RESERVED;
    }

    dlgtemp = (WPCHAR)dataSeg + dataSegLen;

    /*
     * add extra strings to block
     */
    if( textlen ) {
        dlgtemp = copyString( dlgtemp, text, textlen );
    }
    if( classlen ) {
        dlgtemp = copyString( dlgtemp, class, classlen );
    }
    if( ctldatalen ) {
        if( ctldata != NULL ) {
             dlgtemp = (WPCHAR)_FARmemcpy( dlgtemp, ctldata, ctldatalen );
        } else {
             dlgtemp = (WPCHAR)_FARmemcpy( dlgtemp, &_ctldata, ctldatalen );
        }
        dlgtemp += ctldatalen;
    }
    dataSegLen = ddatalen;

    return( new_dlgtemplate );

} /* _AddControl */

/*
 * _DoneAddingControls - called when there are no more controls
 */
TEMPLATE_HANDLE _DoneAddingControls( TEMPLATE_HANDLE old_dlgtemplate )
{
    WPDLGITEMTEMPLATE   dit;
    WPDLGTEMPLATE       dt;
    int                 record;
    int                 max;
    TEMPLATE_HANDLE     new_dlgtemplate;

    if ( old_dlgtemplate == NULL || !dataSeg ) {
        return( NULL );
    }
    dt = (WPDLGTEMPLATE)old_dlgtemplate;
    dit = (WPDLGITEMTEMPLATE)( (WPCHAR)old_dlgtemplate + ( sizeof( WDLGTEMPLATE ) - sizeof( WDLGITEMTEMPLATE ) ) );
    max = ( dt->cbTemplate - sizeof( WDLGTEMPLATE ) + sizeof( WDLGITEMTEMPLATE ) ) / sizeof( WDLGITEMTEMPLATE );
    for( record = 0; record < max; record++ ) {
        if( dit[record].cchText ) {
            dit[record].offText += dt->cbTemplate;
        }
        if( dit[record].offCtlData ) {
            dit[record].offCtlData += dt->cbTemplate;
        }
        if( dit[record].cchClassName ) {
            dit[record].offClassName += dt->cbTemplate;
        }
    }
    new_dlgtemplate = PMrealloc( old_dlgtemplate, dt->cbTemplate + dataSegLen );
    dt = new_dlgtemplate;
    dit = (WPDLGITEMTEMPLATE)( (WPCHAR)new_dlgtemplate + dt->cbTemplate );
    memcpy( dit, dataSeg, dataSegLen );
    dt->cbTemplate += dataSegLen;
    PMfree( dataSeg );
    dataSeg = NULL;
    return( new_dlgtemplate );

} /* _DoneAddingControls */

/*
 * _DynamicDialogBox - create a dynamic dialog box
 */
int _DynamicDialogBox( PFNWP fn, HWND hwnd, TEMPLATE_HANDLE dlgtemplate )
{
    long rc;
    HWND handle;

    handle = WinCreateDlg( HWND_DESKTOP, hwnd, fn, (WPDLGTEMPLATE)dlgtemplate, NULL );
    if ( !handle ) {
        DISPLAY("Window Creation Error Occurred");
        return( 0 );
    }
    rc = WinProcessDlg( handle );
    WinDestroyWindow( handle );
    return( rc );

} /* _DynamicDialogBox */
