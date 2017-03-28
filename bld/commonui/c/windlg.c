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
static char _ISFAR *copyString( char _ISFAR *mem, const char _ISFAR *str, int len )
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
static char _ISFAR *copyWord( char _ISFAR *mem, WORD word )
{
    *mem++ = word;
    *mem++ = word >> 8;
    return( mem );

} /* copyWord */

static unsigned char getClassOrdinal( const char *class )
{
    unsigned char   class_ordinal;

    if( stricmp( class, "combobox" ) == 0 ) {
        class_ordinal = 0x85;
    } else if( stricmp( class,"scrollbar" ) == 0 ) {
        class_ordinal = 0x84;
    } else if( stricmp( class,"listbox" ) == 0 ) {
        class_ordinal = 0x83;
    } else if( stricmp( class,"static" ) == 0 ) {
        class_ordinal = 0x82;
    } else if( stricmp( class,"edit" ) == 0 ) {
        class_ordinal = 0x81;
    } else if( stricmp( class, "button" ) == 0 ) {
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
                                WORD pointsize, const char *typeface, size_t *datalen )
{
    TEMPLATE_HANDLE     data;
    size_t              blocklen;
    UINT                menulen, classlen, captionlen, typefacelen;
    char                _ISFAR *databytes;
    char                _ISFAR *dlgtemp;
    _DLGTEMPLATE        _ISFAR *dt;

    *datalen = 0;
    /*
     * get size of block and allocate memory
     */
    menulen = SLEN( menuname );
    classlen = SLEN( classname );
    captionlen = SLEN( captiontext );

    blocklen = sizeof( _DLGTEMPLATE ) + menulen + classlen + captionlen;

    if( style & DS_SETFONT ) {
        typefacelen = SLEN( typeface );
        blocklen += sizeof( WORD ) + typefacelen;
    } else {
        typefacelen = 0;
    }

    data = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( data == NULL )
        return( NULL );

    databytes = GetPtrGlobalLock( data );
    *datalen = blocklen;

    /*
     * set up template
     */

    dt = (_DLGTEMPLATE _ISFAR *)databytes;

    dt->dtStyle = style;
    dt->dtItemCount = 0;
    dt->dtX = (short)x;
    dt->dtY = (short)y;
    dt->dtCX = (short)cx;
    dt->dtCY = (short)cy;

    dlgtemp = (char _ISFAR *)( dt + 1 );

    /*
     * add extra strings to block
     */
    dlgtemp = copyString( dlgtemp, menuname, menulen );
    dlgtemp = copyString( dlgtemp, classname, classlen );
    dlgtemp = copyString( dlgtemp, captiontext, captionlen );

    /*
     * add font data (if needed)
     */
    if( style & DS_SETFONT ) {
        dlgtemp = copyWord( dlgtemp, pointsize );
        dlgtemp = copyString( dlgtemp, typeface, typefacelen );
    }

    GlobalUnlock( data );
    return( data );

} /* DialogTemplate */

/*
 * AddControl - add a control to a dialog
 */
TEMPLATE_HANDLE AddControl( TEMPLATE_HANDLE data, int x, int y, int cx, int cy, WORD id, DWORD style,
                            const char *class, const char *text,
                            BYTE infolen, const char *infodata, size_t *datalen )
{
    TEMPLATE_HANDLE     new;
    size_t              blocklen;
    UINT                classlen, textlen;
    char                _ISFAR *databytes;
    _DLGTEMPLATE        _ISFAR *dt;
    _DLGITEMTEMPLATE    _ISFAR *dit;
    char                _ISFAR *ditstr;
    unsigned char       class_ordinal;
    size_t              item_start;

    style |= WS_CHILD;

    /*
     * compute size of block, reallocate block to hold this stuff
     */

    class_ordinal = getClassOrdinal( class );
    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
        classlen = 1;
#else
        classlen = 4;
#endif
    } else {
        classlen = SLEN( class );
    }
    textlen  = SLEN( text );

    item_start = *datalen;
    ADJUST_DLGLEN( item_start );
#if defined( __WINDOWS__ )
    blocklen = item_start + sizeof( _DLGITEMTEMPLATE ) + classlen + textlen + sizeof( BYTE ) + infolen;
#else
    blocklen = item_start + sizeof( _DLGITEMTEMPLATE ) + classlen + textlen + sizeof( WORD ) + infolen;
#endif

    GlobalUnlock( data );

    new = GlobalReAlloc( data, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new == NULL )
        return( NULL );

    databytes = GetPtrGlobalLock( new );

    /*
     * one more item...
     */
    dt = (_DLGTEMPLATE _ISFAR *)databytes;
    dt->dtItemCount++;

    /*
     * point to start of item template, and set up values
     */
    dit = (_DLGITEMTEMPLATE _ISFAR *)( databytes + item_start );
    dit->ditStyle = style;
    dit->ditX = (short)x;
    dit->ditY = (short)y;
    dit->ditCX = (short)cx;
    dit->ditCY = (short)cy;
    dit->ditID = (WORD)id;

    ditstr = (char _ISFAR *)( dit + 1 );

    /*
     * append extra data
     */

    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
        *ditstr++ = class_ordinal;
#else
        ditstr = copyWord( ditstr, -1 );
        ditstr = copyWord( ditstr, class_ordinal );
#endif
    } else {
        ditstr = copyString( ditstr, class, classlen );
    }
    ditstr = copyString( ditstr, text, textlen );
#if defined( __WINDOWS__ )
    *ditstr++ = infolen;
#else
    ditstr = copyWord( ditstr, infolen );
#endif
    _FARmemcpy( ditstr, infodata, infolen );
    ditstr += infolen;

    *datalen = (size_t)( ditstr - databytes );

    GlobalUnlock( new );
    return( new );

} /* AddControl */

/*
 * DoneAddingControls - called when there are no more controls
 */
TEMPLATE_HANDLE DoneAddingControls( TEMPLATE_HANDLE data )
{
    GlobalUnlock( data );
    return( data );
} /* DoneAddingControls */

/*
 * DynamicDialogBox - create a dynamic dialog box
 */
INT_PTR DynamicDialogBox( DLGPROCx dlgfn, HINSTANCE inst, HWND hwnd, TEMPLATE_HANDLE data, LPARAM lparam )
{
    DLGPROC     dlgproc;
    INT_PTR     rc;

    dlgproc = (DLGPROC)MakeDlgProcInstance( (DLGPROCx)dlgfn, inst );
#if defined( __WINDOWS__ )
    rc = DialogBoxIndirectParam( inst, data, hwnd, dlgproc, lparam );
#else
    rc = DialogBoxIndirectParam( inst, GlobalLock( data ), hwnd, dlgproc, lparam );
    GlobalUnlock( data );
#endif
    FreeProcInstance( (FARPROC)dlgproc );
    GlobalFree( data );
    return( rc );

} /* DynamicDialogBox */
