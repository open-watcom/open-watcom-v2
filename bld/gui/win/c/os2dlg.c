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
* Description:  OS/2 dynamic dialog creation functions.
*
****************************************************************************/


#include "guiwind.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <wpi.h>

#include "os2dlg.h"

static TEMPLATE_HANDLE PMDialogTemplate    ( USHORT temptype, USHORT codepage,
                                             USHORT focus );
static TEMPLATE_HANDLE PMDoneAddingControls( TEMPLATE_HANDLE data );
static TEMPLATE_HANDLE PMAddControl        ( TEMPLATE_HANDLE data, long style,
                                             USHORT dtx, USHORT dty,
                                             USHORT dtcx, USHORT dtcy,
                                             USHORT id, USHORT children,
                                             ULONG nclass, char *class,
                                             char *text, PVOID presparms,
                                             ULONG presparmslen,
                                             PVOID ctldata, ULONG ctldatlen );
static int          PMDynamicDialogBox     ( PFNWP fn, HWND hwnd,
                                             TEMPLATE_HANDLE data,
                                             PVOID dlgdata );

static TEMPLATE_HANDLE  dataSeg;
static ULONG            dataSegLen;

/*
 * copyString - copy from string to memory
 */
static char _ISFAR *copyString( char _ISFAR *mem, char _ISFAR *str, int len )
{
    if ( !mem || !str ) return( mem );
    _FARmemcpy( mem, str, len );
    return( mem+len );

} /* copyString */

/*
 * safeStrLen - measure sizeof string (even NULL );
 */
static long safeStrLen( char _ISFAR *str )
{
    if ( !str ) return( 0 );
    return( strlen( str ) );

} /* safeStrLen */

/*
 * PMDialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE PMDialogTemplate( USHORT temptype, USHORT codepage,
                                  USHORT focus )
{
    TEMPLATE_HANDLE     data;
    UINT                blocklen;
    DLGTEMPLATE _ISFAR *dt;


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
    dt->adlgti[0].cChildren = 0; // set the number of children to zero
    return( data );

} /* PMDialogTemplate */

/*
 * PMAddControl - add a control to a dialog
 */
TEMPLATE_HANDLE PMAddControl( TEMPLATE_HANDLE data, long style, USHORT dtx,
                              USHORT dty, USHORT dtcx, USHORT dtcy, USHORT id,
                              USHORT children, ULONG nclass, char *class,
                              char *text, PVOID presparms, ULONG presparmslen,
                              PVOID ctldata, ULONG ctldatalen )
{
    TEMPLATE_HANDLE     new;
    UINT                blocklen, classlen, textlen, ddatalen;
    DLGTEMPLATE         _ISFAR *dt;
    DLGITEMTEMPLATE     _ISFAR *dit;
    char                _ISFAR *dlgtemp;
    char                *new_text;

    new_text = _wpi_menutext2pm( text );

    /*
     * compute size of block, reallocate block to hold this stuff
     */
    classlen = SLEN( class );
    if ( classlen ) {
        classlen++;
    }
    textlen  = SLEN( new_text ) + 1;

    if ( !ctldata ) {
        ctldatalen = 0;
    }

    dt = data;
    blocklen = sizeof( DLGITEMTEMPLATE ) + dt->cbTemplate;
    ddatalen =  classlen + textlen + ctldatalen + dataSegLen + presparmslen;

    new = PMrealloc( data, blocklen );
    dataSeg = PMrealloc( dataSeg, ddatalen );

    if( !new || ( !dataSeg && ddatalen ) ) {
        if ( dataSeg ) PMfree( dataSeg );
        if ( new ) PMfree( new );
        if ( new_text ) _wpi_freemenutext( new_text );
        return( NULL );
    }


    dt = new;

    /*
     * point to start of item template, and set up values
     */
    dit = ( DLGITEMTEMPLATE *)( (char *)dt + dt->cbTemplate );
    dt->cbTemplate += sizeof( DLGITEMTEMPLATE );

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
    if ( classlen ) {
        dit->offClassName = dataSegLen + textlen;
    } else {
        dit->offClassName = nclass & 0xffff;
    }

    if( !children ) {
        dty = dt->adlgti[0].cy - dty - dtcy;
    }

    dit->x = dtx;
    dit->y = dty;
    dit->cx = dtcx;
    dit->cy = dtcy;
    dit->id = id;
    if ( presparmslen ) {
        dit->offPresParams = dataSegLen + textlen + classlen;
    } else {
        dit->offPresParams = 0xffff;
    }
    if ( ctldatalen ) {
        dit->offCtlData = dataSegLen + textlen + classlen + presparmslen;
    } else {
        dit->offCtlData = 0xffff;
    }

    if( dataSeg ) {
        dlgtemp = (char *)dataSeg + dataSegLen;

        /*
         * add extra strings to block
         */
        if ( textlen == 1 ) {
            *dlgtemp = '\0';
            dlgtemp++;
        } else {
            dlgtemp = copyString( dlgtemp, new_text, textlen );
        }
        if ( classlen ) {
            dlgtemp = copyString( dlgtemp, class, classlen );
        }
        if ( presparmslen ) {
            dlgtemp = copyString( dlgtemp, presparms, presparmslen );
        }
        if ( ctldatalen ) {
            if ( ctldata ) {
                 dlgtemp = copyString( dlgtemp, ctldata, ctldatalen );
            }
        }
        dataSegLen = ddatalen;
    }

    if( new_text ) {
        _wpi_freemenutext( new_text );
    }

    return( new );

} /* PMAddControl */

/*
 * PMDoneAddingControls - called when there are no more controls
 */
TEMPLATE_HANDLE PMDoneAddingControls( TEMPLATE_HANDLE data )
{
    DLGITEMTEMPLATE     *temp;
    DLGTEMPLATE *dt;
    int                 record;
    int                 max;

    if( !data || ( !dataSeg && dataSegLen ) ) {
        return( NULL );
    }
    temp = (DLGITEMTEMPLATE *)( (char *)data + ( sizeof( DLGTEMPLATE ) - sizeof( DLGITEMTEMPLATE ) ) );
    dt = data;
    max = ( dt->cbTemplate - sizeof( DLGTEMPLATE ) +
            sizeof( DLGITEMTEMPLATE ) ) / sizeof( DLGITEMTEMPLATE );

    for( record = 0; record < max; record++ ) {
        temp[record].offText += dt->cbTemplate;
        if ( temp[record].offCtlData != 0xffff ) {
            temp[record].offCtlData += dt->cbTemplate;
        }
        if ( temp[record].cchClassName ) {
            temp[record].offClassName += dt->cbTemplate;
        }
        if ( temp[record].offPresParams != 0xffff ) {
            temp[record].offPresParams += dt->cbTemplate;
        }
    }

    if( dataSeg ) {
        data= PMrealloc( data, dt->cbTemplate + dataSegLen );
        dt = data;
        memcpy( (DLGITEMTEMPLATE *)( (char *)data + dt->cbTemplate ), dataSeg, dataSegLen );
        dt->cbTemplate += dataSegLen;
        PMfree( dataSeg );
        dataSeg = NULL;
    }

    return( data );

} /* PMDoneAddingControls */

/*
 * PMDynamicDialogBox - create a dynamic dialog box
 */
int PMDynamicDialogBox( PFNWP fn, HWND hwnd, TEMPLATE_HANDLE data,
                        PVOID dlgdata )
{
    long rc;
    HWND handle;

    handle = WinCreateDlg( HWND_DESKTOP, hwnd, fn, (PDLGTEMPLATE)data,
                           dlgdata );
    if ( !handle ) {
        return( 0 );
    }
    rc = WinProcessDlg( handle );
    WinDestroyWindow( handle );
    return( rc );

} /* PMDynamicDialogBox */

TEMPLATE_HANDLE DialogTemplate( LONG dtStyle, int dtx, int dty, int dtcx,
                                int dtcy, char *menuname, char *classname,
                                char *captiontext, int pointsize,
                                char *typeface )
{
    TEMPLATE_HANDLE     data;
    TEMPLATE_HANDLE     new;
    ULONG               frame_flags;
    char                *buf;
    PRESPARAMS          *pdata;
    int                 bufsize;
    ULONG               psize;

    menuname = menuname;
    classname = classname;

    pdata = NULL;
    psize = 0;
    if( typeface ) {
        bufsize = strlen( typeface ) + 10 + 1;
        buf = (char *)PMmalloc( bufsize );
        if( buf ) {
            buf[0] = '\0';
            sprintf( buf, "%d.%s", pointsize, typeface );
            bufsize = strlen(buf);
            /* This convoluted calculation makes sure we get the right size
             * regardless of structure packing.
             */
            psize = sizeof(PRESPARAMS) - sizeof(PARAM)
                    + sizeof(BYTE) + 2 * sizeof(ULONG)
                    + bufsize;
            pdata = (PRESPARAMS *) PMmalloc( psize );
            if( pdata ) {
                pdata->cb = psize - sizeof(ULONG);
                pdata->aparam[0].id = PP_FONTNAMESIZE;
                pdata->aparam[0].cb = bufsize + 1;
                memcpy( pdata->aparam[0].ab, buf, bufsize + 1 );
            } else {
                psize = 0;
            }
            PMfree( buf );
        }
    }

    data = PMDialogTemplate( TEMPLATE_TYPE, CODE_PAGE, 0xffff );
    if( !data ) {
        return( NULL );
    }

    frame_flags = dtStyle & 0x0000ffff;
    dtStyle = ( dtStyle & 0xffff0000 ) | WS_SAVEBITS |
                FS_NOBYTEALIGN | FS_DLGBORDER;

    new = PMAddControl( data, dtStyle, dtx, dty, dtcx, dtcy,
                        0, 1, (ULONG) WC_FRAME, NULL, captiontext, pdata,
                        psize, &frame_flags, sizeof(ULONG) );

    if( pdata != NULL ) {
        PMfree( pdata );
    }

    if( new == NULL ) {
        PMfree( data );
    }

    return( new );
}

TEMPLATE_HANDLE AddControl ( TEMPLATE_HANDLE data, int dtx, int dty,
                             int dtcx, int dtcy, int id, long style,
                             char *class, char *text,
                             BYTE infolen, char *infodata )
{
    TEMPLATE_HANDLE     new;
    ULONG               nclass;

    nclass = 0;
    if( ( (ULONG)class & 0xffff0000 ) == 0xffff0000 ) {
        nclass = (ULONG)class;
        class = NULL;
    }

    new = PMAddControl( data, style, dtx, dty, dtcx, dtcy, id,
                        0, nclass, class, text, NULL, 0, infodata, infolen );

    if( new == NULL ) {
        PMfree( data );
    }

    return( new );
}

TEMPLATE_HANDLE DoneAddingControls( TEMPLATE_HANDLE data )
{
    return( PMDoneAddingControls( data ) );
}

int DynamicDialogBox( PVOID fn, WPI_INST inst, HWND hwnd, TEMPLATE_HANDLE data,
                      LPARAM lparam )
{
    inst = inst;
    return( PMDynamicDialogBox( (PFNWP) fn, hwnd, data, (PVOID)lparam ) );
}

