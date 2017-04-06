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


#include "commonui.h"
#include "wpi.h"
#include <string.h>
#include <stdlib.h>
#include "windlg.h"

/*
 * copyString - copy from string to memory
 */
static WPCHAR copyString( WPCHAR mem, const char *str, int len )
{
#ifdef __WINDOWS__
    _FARmemcpy( mem, str, len );
    return( mem + len );
#else
    // convert ANSI or DBCS string to Unicode properly
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, str, -1, (LPWSTR)mem, len );
    return( mem + len );
#endif

} /* copyString */


/*
 * copyWord - copy from Word to memory
 */
static WPCHAR copyWord( WPCHAR mem, WORD word )
{
    *mem++ = word;
    *mem++ = word >> 8;
    return( mem );

} /* copyWord */

static unsigned char getClassOrdinal( const char *classname )
{
    unsigned char   class_ordinal;

    if( classname == NULL ) {
        class_ordinal = 0;
    } else if( stricmp( classname, "combobox" ) == 0 ) {
        class_ordinal = 0x85;
    } else if( stricmp( classname, "scrollbar" ) == 0 ) {
        class_ordinal = 0x84;
    } else if( stricmp( classname, "listbox" ) == 0 ) {
        class_ordinal = 0x83;
    } else if( stricmp( classname, "static" ) == 0 ) {
        class_ordinal = 0x82;
    } else if( stricmp( classname, "edit" ) == 0 ) {
        class_ordinal = 0x81;
    } else if( stricmp( classname, "button" ) == 0 ) {
        class_ordinal = 0x80;
    } else {
        class_ordinal = 0;
    }
    return( class_ordinal );
}

/*
 * DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE DialogTemplate( DWORD style, int x, int y, int cx, int cy,
                                const char *menuname, const char *classname, const char *captiontext,
                                WORD pointsize, const char *typeface, size_t *templatelen )
{
    TEMPLATE_HANDLE     dlgtemplate;
    size_t              blocklen;
    UINT                menulen, classlen, captionlen, typefacelen;
    WPCHAR              template;
    WPCHAR              dlgtemp;
    WPDLGTEMPLATE       dt;
#if !defined( __WINDOWS__ )
    unsigned char       class_ordinal;
#endif

    *templatelen = 0;
    /*
     * get size of block and allocate memory
     */
    menulen = SLEN( menuname );
#if defined( __WINDOWS__ )
    classlen = SLEN( classname );
#else
    class_ordinal = getClassOrdinal( classname );
    if( class_ordinal > 0 ) {
        classlen = 4;
    } else {
        classlen = SLEN( classname );
    }
#endif
    captionlen = SLEN( captiontext );

    blocklen = sizeof( WDLGTEMPLATE ) + menulen + classlen + captionlen;

    if( style & DS_SETFONT ) {
        typefacelen = SLEN( typeface );
        blocklen += sizeof( WORD ) + typefacelen;
    } else {
        typefacelen = 0;
    }

    dlgtemplate = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( dlgtemplate == NULL )
        return( NULL );

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

    dlgtemp = (WPCHAR)( dt + 1 );
    dlgtemp = copyString( dlgtemp, menuname, menulen );
#if defined( __WINDOWS__ )
    dlgtemp = copyString( dlgtemp, classname, classlen );
#else
    if( class_ordinal > 0 ) {
        dlgtemp = copyWord( dlgtemp, -1 );
        dlgtemp = copyWord( dlgtemp, class_ordinal );
    } else {
        dlgtemp = copyString( dlgtemp, classname, classlen );
    }
#endif
    dlgtemp = copyString( dlgtemp, captiontext, captionlen );

    /*
     * add font data (if needed)
     */
    if( style & DS_SETFONT ) {
        dlgtemp = copyWord( dlgtemp, pointsize );
        dlgtemp = copyString( dlgtemp, typeface, typefacelen );
    }

    GlobalUnlock( dlgtemplate );
    return( dlgtemplate );

} /* DialogTemplate */

/*
 * AddControl - add a control to a dialog
 */
TEMPLATE_HANDLE AddControl( TEMPLATE_HANDLE dlgtemplate, int x, int y, int cx, int cy, WORD id, DWORD style,
                            const char *classname, const char *captiontext,
                            const void *infodata, BYTE infodatalen, size_t *templatelen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;
    size_t              blocklen;
    UINT                classlen, textlen;
    WPCHAR              template;
    WPDLGTEMPLATE       dt;
    WPDLGITEMTEMPLATE   dit;
    WPCHAR              ditstr;
    unsigned char       class_ordinal;
    size_t              item_start;

    style |= WS_CHILD;

    /*
     * compute size of block, reallocate block to hold this stuff
     */

    class_ordinal = getClassOrdinal( classname );
    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
        classlen = 1;
#else
        classlen = 4;
#endif
    } else {
        classlen = SLEN( classname );
    }
    textlen  = SLEN( captiontext );

    item_start = *templatelen;
    ADJUST_DLGLEN( item_start );
#if defined( __WINDOWS__ )
    blocklen = item_start + sizeof( WDLGITEMTEMPLATE ) + classlen + textlen + sizeof( BYTE ) + infodatalen;
#else
    blocklen = item_start + sizeof( WDLGITEMTEMPLATE ) + classlen + textlen + sizeof( WORD ) + infodatalen;
#endif

    GlobalUnlock( dlgtemplate );
    new_dlgtemplate = GlobalReAlloc( dlgtemplate, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new_dlgtemplate == NULL )
        return( NULL );

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
    dit->ditID = (WORD)id;

    /*
     * append extra data
     */

    ditstr = (WPCHAR)( dit + 1 );
    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
        *ditstr++ = class_ordinal;
#else
        ditstr = copyWord( ditstr, -1 );
        ditstr = copyWord( ditstr, class_ordinal );
#endif
    } else {
        ditstr = copyString( ditstr, classname, classlen );
    }
    ditstr = copyString( ditstr, captiontext, textlen );

#if defined( __WINDOWS__ )
    *ditstr++ = infodatalen;
#else
    ditstr = copyWord( ditstr, infodatalen );
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
TEMPLATE_HANDLE DoneAddingControls( TEMPLATE_HANDLE dlgtemplate )
{
    GlobalUnlock( dlgtemplate );
    return( dlgtemplate );
} /* DoneAddingControls */

/*
 * DynamicDialogBox - create a dynamic dialog box
 */
INT_PTR DynamicDialogBox( DLGPROCx dlgfn, HINSTANCE inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate, LPARAM lparam )
{
    DLGPROC     dlgproc;
    INT_PTR     rc;

    dlgproc = MakeProcInstance_DLG( dlgfn, inst );
    rc = DialogBoxIndirectParam( inst, TEMPLATE_LOCK( dlgtemplate ), hwnd, dlgproc, lparam );
    TEMPLATE_UNLOCK( dlgtemplate );
    FreeProcInstance_DLG( dlgproc );
    GlobalFree( dlgtemplate );
    return( rc );

} /* DynamicDialogBox */
