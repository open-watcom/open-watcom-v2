/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  OS/2 dynamic dialog creation functions.
*
****************************************************************************/


#include "commonui.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wpi.h>
#include "os2mem.h"
#include "os2dlg.h"


static TEMPLATE_HANDLE  PMDialogTemplate( USHORT temptype, USHORT codepage, USHORT focus );
static TEMPLATE_HANDLE  PMDoneAddingControls( TEMPLATE_HANDLE dlgtemplate );
static TEMPLATE_HANDLE  PMAddControl( TEMPLATE_HANDLE dlgtemplate, DWORD style, USHORT x, USHORT y, USHORT cx, USHORT cy,
                            USHORT id, USHORT children, const char FAR *classname,
                            const char *captiontext, PVOID presparms, ULONG presparmslen,
                            const void *ctldata, ULONG ctldatalen );
static int              PMDynamicDialogBox( PFNWP fn, HWND hwnd, TEMPLATE_HANDLE dlgtemplate, PVOID dlgdata );

static char             *dataSeg;
static ULONG            dataSegLen;

/*
 * PMDialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE PMDialogTemplate( USHORT temptype, USHORT codepage, USHORT focus )
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
    dt->adlgti[0].cChildren = 0; // set the number of children to zero
    return( dlgtemplate );

} /* PMDialogTemplate */

/*
 * PMAddControl - add a control to a dialog
 */
TEMPLATE_HANDLE PMAddControl( TEMPLATE_HANDLE old_dlgtemplate, DWORD style, USHORT x,
                              USHORT y, USHORT cx, USHORT cy, USHORT id,
                              USHORT children, const char FAR *classname,
                              const char *captiontext, PVOID presparms, ULONG presparmslen,
                              const void *ctldata, ULONG ctldatalen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;
    UINT                blocklen, classlen, textlen, ddatalen;
    WPDLGTEMPLATE       dt;
    WPDLGITEMTEMPLATE   dit;
    char                *dataSegPtr;
    char                *new_text;

    new_text = _wpi_menutext2pm( captiontext );

    /*
     * compute size of block, reallocate block to hold this stuff
     */
    if( SHORT2FROMMP( classname ) == 0xffff ) {
        classlen = 0;
    } else {
        classlen = SLEN( classname );
        if( classlen ) {
            classlen++;
        }
    }
    textlen  = SLEN( new_text ) + 1;

    if( ctldata == NULL ) {
        ctldatalen = 0;
    }

    dt = (WPDLGTEMPLATE)old_dlgtemplate;
    blocklen = sizeof( WDLGITEMTEMPLATE ) + dt->cbTemplate;
    ddatalen =  classlen + textlen + ctldatalen + dataSegLen + presparmslen;

    new_dlgtemplate = PMrealloc( old_dlgtemplate, blocklen );
    dataSeg = PMrealloc( dataSeg, ddatalen );

    if( new_dlgtemplate == NULL || ( dataSeg == NULL && ddatalen ) ) {
        if( dataSeg != NULL )
            PMfree( dataSeg );
        if( new_dlgtemplate != NULL )
            PMfree( new_dlgtemplate );
        if( new_text != NULL )
            _wpi_freemenutext( new_text );
        return( NULL );
    }

    /*
     * point to start of item template, and set up values
     */
    dt = (WPDLGTEMPLATE)new_dlgtemplate;
    dit = (WPDLGITEMTEMPLATE)( (WPCHAR)new_dlgtemplate + dt->cbTemplate );
    dt->cbTemplate += sizeof( WDLGITEMTEMPLATE );

    dit->fsItemStatus = RESERVED;
    if( children ) {
        dit->cChildren  = 0;
    } else {
        dt->adlgti[0].cChildren++;      // increment the number of children
        dit->cChildren  = 0;
    }
    dit->flStyle = style;
    dit->cchText = textlen - 1;
    dit->offText = dataSegLen;
    dit->cchClassName = classlen;
    if( classlen ) {
        dit->offClassName = dataSegLen + textlen;
    } else {
        dit->offClassName = SHORT1FROMMP( classname );
    }

    if( !children ) {
        y = dt->adlgti[0].cy - y - cy;
    }

    dit->x = x;
    dit->y = y;
    dit->cx = cx;
    dit->cy = cy;
    dit->id = id;
    if( presparmslen ) {
        dit->offPresParams = dataSegLen + textlen + classlen;
    } else {
        dit->offPresParams = 0xffff;
    }
    if( ctldatalen ) {
        dit->offCtlData = dataSegLen + textlen + classlen + presparmslen;
    } else {
        dit->offCtlData = 0xffff;
    }

    if( dataSeg != NULL ) {
        dataSegPtr = dataSeg + dataSegLen;

        /*
         * add extra strings to block
         */
        if( textlen == 1 ) {
            *dataSegPtr++ = '\0';
        } else {
            memcpy( dataSegPtr, new_text, textlen );
            dataSegPtr += textlen;
        }
        if( classlen ) {
            memcpy( dataSegPtr, classname, classlen );
            dataSegPtr += classlen;
        }
        if( presparmslen ) {
            memcpy( dataSegPtr, presparms, presparmslen );
            dataSegPtr += presparmslen;
        }
        if( ctldatalen ) {
            if( ctldata != NULL ) {
                memcpy( dataSegPtr, ctldata, ctldatalen );
                dataSegPtr += ctldatalen;
            }
        }
        dataSegLen = ddatalen;
    }

    if( new_text != NULL ) {
        _wpi_freemenutext( new_text );
    }

    return( new_dlgtemplate );

} /* PMAddControl */

/*
 * PMDoneAddingControls - called when there are no more controls
 */
TEMPLATE_HANDLE PMDoneAddingControls( TEMPLATE_HANDLE old_dlgtemplate )
{
    WPDLGITEMTEMPLATE   dit;
    WPDLGTEMPLATE       dt;
    int                 record;
    int                 max;
    TEMPLATE_HANDLE     new_dlgtemplate;

    if( old_dlgtemplate == NULL || ( dataSeg == NULL && dataSegLen ) ) {
        return( NULL );
    }
    dt = (WPDLGTEMPLATE)old_dlgtemplate;
    dit = (WPDLGITEMTEMPLATE)( (WPCHAR)old_dlgtemplate + ( sizeof( WDLGTEMPLATE ) - sizeof( WDLGITEMTEMPLATE ) ) );
    max = ( dt->cbTemplate - sizeof( WDLGTEMPLATE ) + sizeof( WDLGITEMTEMPLATE ) ) / sizeof( WDLGITEMTEMPLATE );

    for( record = 0; record < max; record++ ) {
        dit[record].offText += dt->cbTemplate;
        if( dit[record].offCtlData != 0xffff ) {
            dit[record].offCtlData += dt->cbTemplate;
        }
        if( dit[record].cchClassName ) {
            dit[record].offClassName += dt->cbTemplate;
        }
        if( dit[record].offPresParams != 0xffff ) {
            dit[record].offPresParams += dt->cbTemplate;
        }
    }

    if( dataSeg != NULL ) {
        new_dlgtemplate = PMrealloc( old_dlgtemplate, dt->cbTemplate + dataSegLen );
        dt = (WPDLGTEMPLATE)new_dlgtemplate;
        dit = (WPDLGITEMTEMPLATE)( (WPCHAR)new_dlgtemplate + dt->cbTemplate );
        _FARmemcpy( dit, dataSeg, dataSegLen );
        dt->cbTemplate += dataSegLen;
        PMfree( dataSeg );
        dataSeg = NULL;
    } else {
        new_dlgtemplate = old_dlgtemplate;
    }

    return( new_dlgtemplate );

} /* PMDoneAddingControls */

/*
 * PMDynamicDialogBox - create a dynamic dialog box
 */
int PMDynamicDialogBox( PFNWP fn, HWND hwnd, TEMPLATE_HANDLE dlgtemplate, PVOID dlgdata )
{
    long rc;
    HWND handle;

    handle = WinCreateDlg( HWND_DESKTOP, hwnd, fn, (WPDLGTEMPLATE)dlgtemplate, dlgdata );
    if( handle == NULLHANDLE ) {
        return( 0 );
    }
    rc = WinProcessDlg( handle );
    WinDestroyWindow( handle );
    return( rc );

} /* PMDynamicDialogBox */

TEMPLATE_HANDLE DialogTemplate( DWORD style, int x, int y, int cx, int cy,
                                const char *menuname, const char *classname,
                                const char *captiontext, WORD pointsize,
                                const char *facename, size_t *templatelen )
{
    TEMPLATE_HANDLE     old_dlgtemplate;
    TEMPLATE_HANDLE     new_dlgtemplate;
    ULONG               frame_flags;
    char                *buf;
    PRESPARAMS          *pdata;
    int                 bufsize;
    ULONG               psize;

    templatelen = templatelen;
    menuname = menuname;
    classname = classname;

    pdata = NULL;
    psize = 0;
    if( facename != NULL ) {
        bufsize = strlen( facename ) + 10 + 1;
        buf = (char *)PMmalloc( bufsize );
        if( buf != NULL ) {
            buf[0] = '\0';
            sprintf( buf, "%u.%s", pointsize, facename );
            bufsize = strlen(buf);
            /* This convoluted calculation makes sure we get the right size
             * regardless of structure packing.
             */
            psize = sizeof( PRESPARAMS ) - sizeof( PARAM )
                    + sizeof( BYTE ) + 2 * sizeof( ULONG )
                    + bufsize;
            pdata = (PRESPARAMS *)PMmalloc( psize );
            if( pdata != NULL ) {
                pdata->cb = psize - sizeof( ULONG );
                pdata->aparam[0].id = PP_FONTNAMESIZE;
                pdata->aparam[0].cb = bufsize + 1;
                memcpy( pdata->aparam[0].ab, buf, bufsize + 1 );
            } else {
                psize = 0;
            }
            PMfree( buf );
        }
    }

    old_dlgtemplate = PMDialogTemplate( TEMPLATE_TYPE, CODE_PAGE, 0xffff );
    if( old_dlgtemplate == NULL ) {
        return( NULL );
    }

    frame_flags = style & 0x0000ffff;
    style = (style & 0xffff0000) | WS_SAVEBITS | FS_NOBYTEALIGN | FS_DLGBORDER;

    new_dlgtemplate = PMAddControl( old_dlgtemplate, style, x, y, cx, cy, 0, 1,
                        WC_FRAME, captiontext, pdata, psize, NULL, frame_flags );

    if( pdata != NULL ) {
        PMfree( pdata );
    }

    if( new_dlgtemplate == NULL ) {
        PMfree( old_dlgtemplate );
    }

    return( new_dlgtemplate );
}

TEMPLATE_HANDLE AddControl( TEMPLATE_HANDLE old_dlgtemplate, int x, int y,
                             int cx, int cy, WORD id, DWORD style,
                             const char FAR *classname, const char *captiontext,
                             const void *infodata, BYTE infodatalen, size_t *templatelen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;

    templatelen = templatelen;

    new_dlgtemplate = PMAddControl( old_dlgtemplate, style, x, y, cx, cy, id, 0, classname, captiontext, NULL, 0, infodata, infodatalen );

    if( new_dlgtemplate == NULL ) {
        PMfree( old_dlgtemplate );
    }

    return( new_dlgtemplate );
}

TEMPLATE_HANDLE DoneAddingControls( TEMPLATE_HANDLE dlgtemplate )
{
    return( PMDoneAddingControls( dlgtemplate ) );
}

int DynamicDialogBox( PFNWP fn, WPI_INST inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate, LPARAM lparam )
{
    inst = inst;
    return( PMDynamicDialogBox( fn, hwnd, dlgtemplate, (PVOID)lparam ) );
}
