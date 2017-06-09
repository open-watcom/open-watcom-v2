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


#include "wdeglbl.h"
#include "windlg.h"
#include "wdedlgut.h"


/*
 * DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE DialogTemplate( DWORD style, int x, int y, int cx, int cy,
                             const char *menuname, const char *classname, const char *captiontext,
                             WORD font_pointsize, const char *font_facename, size_t *templatelen )
{
    TEMPLATE_HANDLE     dlgtemplate;
    UINT                blocklen, menulen, classlen, textlen, facenamelen;
    WPCHAR              template;
    WPDLGTEMPLATE       dt;
#ifndef __WINDOWS__
    unsigned char       class_ordinal;
#endif

    /*
     * get size of block and allocate memory
     */
    menulen = DlgStringLength( menuname );
#ifdef __WINDOWS__
    classlen = DlgStringLength( classname );
#else
    class_ordinal = DlgGetClassOrdinal( classname );
    if( class_ordinal > 0 ) {
        classlen = 4;
    } else {
        classlen = DlgStringLength( classname );
    }
#endif
    textlen = DlgStringLength( captiontext );

    blocklen = sizeof( WDLGTEMPLATE ) + menulen + classlen + textlen;

    if( style & DS_SETFONT ) {
        facenamelen = DlgStringLength( font_facename );
        blocklen += sizeof( WORD ) + facenamelen;
    } else {
        facenamelen = 0;
    }

    dlgtemplate = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( dlgtemplate == NULL ) {
        return( NULL );
    }

    *templatelen = blocklen;
    template = GetPtrGlobalLock( dlgtemplate );

    /*
     * set up template
     */
    dt = (WPDLGTEMPLATE)template;
    dt->dtStyle = style;
    dt->dtItemCount = 0;
    dt->dtX = (short)x;
    dt->dtY = (short)y;
    dt->dtCX = (short)cx;
    dt->dtCY = (short)cy;

    /*
     * add extra strings to block
     */
    template = (WPCHAR)( dt + 1 );
    template = DlgCopyMBString( template, menuname, menulen );
#ifdef __WINDOWS__
    template = DlgCopyMBString( template, classname, classlen );
#else
    class_ordinal = DlgGetClassOrdinal( classname );
    if( class_ordinal > 0 ) {
        template = DlgCopyWord( template, (WORD)-1 );
        template = DlgCopyWord( template, class_ordinal );
    } else {
        template = DlgCopyMBString( template, classname, classlen );
    }
#endif
    template = DlgCopyMBString( template, captiontext, textlen );

    /*
     * add font data (if needed)
     */
    if( style & DS_SETFONT ) {
        template = DlgCopyWord( template, font_pointsize );
        template = DlgCopyMBString( template, font_facename, facenamelen );
    }

    GlobalUnlock( dlgtemplate );
    return( dlgtemplate );

} /* DialogTemplate */


/*
 * AddControl - add a control to a dialog
 */
TEMPLATE_HANDLE AddControl( TEMPLATE_HANDLE old_dlgtemplate, int x, int y, int cx, int cy, WORD id, DWORD style,
                         const char *classname, const char *captiontext,
                         const void *infodata, BYTE infodatalen, size_t *templatelen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;
    UINT                blocklen, classlen, textlen;
    WPCHAR              template;
    WPDLGTEMPLATE       dt;
    WPDLGITEMTEMPLATE   dit;
    WPCHAR              ditstr;
    unsigned char       class_ordinal;
    size_t              item_start;

    /*
     * compute size of block, reallocate block to hold this stuff
     */

    class_ordinal = DlgGetClassOrdinal( classname );
    if( class_ordinal > 0 ) {
#ifdef __WINDOWS__
        classlen = 1;
#else
        classlen = 4;
#endif
    } else {
        classlen = DlgStringLength( classname );
    }

    textlen = DlgStringLength( captiontext );

    item_start = *templatelen;
    ADJUST_DLGLEN( item_start );
#ifdef __WINDOWS__
    blocklen = item_start + sizeof( WDLGITEMTEMPLATE ) + classlen + textlen + sizeof( BYTE ) + infodatalen;
#else
    blocklen = item_start + sizeof( WDLGITEMTEMPLATE ) + classlen + textlen + sizeof( WORD ) + infodatalen;
#endif

    GlobalUnlock( old_dlgtemplate );
    new_dlgtemplate = GlobalReAlloc( old_dlgtemplate, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new_dlgtemplate == NULL ) {
        return( NULL );
    }

    template = GetPtrGlobalLock( new_dlgtemplate );

    /*
     * one more item...
     */
    dt = (WPDLGTEMPLATE)template;
    dt->dtItemCount++;

    /*
     * point to start of item template, and set up values
     */
    dit = (WPDLGITEMTEMPLATE)( template + item_start );
    dit->ditStyle = style;
    dit->ditX = (short)x;
    dit->ditY = (short)y;
    dit->ditCX = (short)cx;
    dit->ditCY = (short)cy;
    dit->ditID = id;

    /*
     * append extra data
     */

    ditstr = (WPCHAR)( dit + 1 );
    if( class_ordinal > 0 ) {
#ifdef __WINDOWS__
        *ditstr++ = class_ordinal;
#else
        ditstr = DlgCopyWord( ditstr, -1 );
        ditstr = DlgCopyWord( ditstr, class_ordinal );
#endif
    } else {
        ditstr = DlgCopyMBString( ditstr, classname, classlen );
    }

    ditstr = DlgCopyMBString( ditstr, captiontext, textlen );
#ifdef __WINDOWS__
    *ditstr++ = infodatalen;
#else
    ditstr = DlgCopyWord( ditstr, infodatalen );
#endif
    _FARmemcpy( ditstr, infodata, infodatalen );
    ditstr += infodatalen;

    *templatelen = (size_t)( ditstr - template );

    GlobalUnlock( new_dlgtemplate );
    return( new_dlgtemplate );

} /* AddControl */

/*
 * DoneAddingControls - called when there are no more controls
 */
void DoneAddingControls( TEMPLATE_HANDLE dlgtemplate )
{
    GlobalUnlock( dlgtemplate );

} /* DoneAddingControls */

/*
 * DynamicDialogBox - create a dynamic dialog box
 */
INT_PTR DynamicDialogBox( DLGPROCx dlgfn, HANDLE inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate )
{
    DLGPROC     dlgproc;
    INT_PTR     rc;

    dlgproc = MakeProcInstance_DLG( dlgfn, inst );
    rc = DialogBoxIndirect( inst, TEMPLATE_LOCK( dlgtemplate ), hwnd, dlgproc );
    TEMPLATE_UNLOCK( dlgtemplate );
    FreeProcInstance_DLG( dlgproc );
    GlobalFree( dlgtemplate );
    return( rc );

} /* DynamicDialogBox */
