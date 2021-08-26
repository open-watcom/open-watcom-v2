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
* Description:  Dynamically build a Windows dialog template.
*
****************************************************************************/


#include "variety.h"
#define INCLUDE_COMMDLG_H
#include <wwindows.h>
#include "win.h"


/*
 * copyString - copy from string to memory
 */
static WPCHAR copyString( WPCHAR mem, const char *str, int len )
{
    if( str == NULL )
        str = "";
#ifdef __WINDOWS__
    _FARmemcpy( mem, str, len );
    return( mem + len );
#else
    int i;

    for( i = 0; i < len / 2; i++ ) {
        *(short *)mem = *(unsigned char *)str;
        mem += 2;
        str++;
    }
    return( mem );
//    // convert ANSI or DBCS string to Unicode properly
//    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, str, -1, (LPWSTR)mem, len );
//    return( mem + len );
#endif

} /* copyString */


/*
 * copyWord - copy from word to memory
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
    } else if( _stricmp( classname, "combobox" ) == 0 ) {
        class_ordinal = 0x85;
    } else if( _stricmp( classname, "scrollbar" ) == 0 ) {
        class_ordinal = 0x84;
    } else if( _stricmp( classname, "listbox" ) == 0 ) {
        class_ordinal = 0x83;
    } else if( _stricmp( classname, "static" ) == 0 ) {
        class_ordinal = 0x82;
    } else if( _stricmp( classname, "edit" ) == 0 ) {
        class_ordinal = 0x81;
    } else if( _stricmp( classname, "button" ) == 0 ) {
        class_ordinal = 0x80;
    } else {
        class_ordinal = 0;
    }
    return( class_ordinal );
}

/*
 * _DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE _DialogTemplate( DWORD style, int x, int y, int cx, int cy,
                              const char *menuname, const char *classname, const char *captiontext,
                              WORD pointsize, const char *facename, size_t *templatelen )
{
    TEMPLATE_HANDLE     dlgtemplate;
    size_t              blocklen;
    UINT                menulen, classlen, textlen, facenamelen;
    WPCHAR              template;
    WPDLGTEMPLATE       dt;
#ifndef __WINDOWS__
    unsigned char       class_ordinal;
#endif

    *templatelen = 0;

    /*
     * get size of block and allocate memory
     */
    menulen = SLEN( menuname );
#ifdef __WINDOWS__
    classlen = SLEN( classname );
#else
    class_ordinal = getClassOrdinal( classname );
    if( class_ordinal > 0 ) {
        classlen = 4;
    } else {
        classlen = SLEN( classname );
    }
#endif
    textlen = SLEN( captiontext );

    blocklen = sizeof( WDLGTEMPLATE ) + menulen + classlen + textlen;

    if( style & DS_SETFONT ) {
        facenamelen = SLEN( facename );
        blocklen += sizeof( WORD ) + facenamelen;
    } else {
        facenamelen = 0;
    }

    dlgtemplate = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( dlgtemplate == NULL )
        return( (TEMPLATE_HANDLE)NULL );

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
    template = copyString( template, menuname, menulen );
#ifdef __WINDOWS__
    template = copyString( template, classname, classlen );
#else
    if( class_ordinal > 0 ) {
        template = copyWord( template, -1 );
        template = copyWord( template, class_ordinal );
    } else {
        template = copyString( template, classname, classlen );
    }
#endif
    template = copyString( template, captiontext, textlen );

    /*
     * add font data (if needed)
     */
    if( style & DS_SETFONT ) {
        template = copyWord( template, pointsize );
        template = copyString( template, facename, facenamelen );
    }

    GlobalUnlock( dlgtemplate );
    return( dlgtemplate );

} /* _DialogTemplate */

/*
 * _AddControl - add a control to a dialog
 */
TEMPLATE_HANDLE _AddControl( TEMPLATE_HANDLE old_dlgtemplate, int x, int y, int cx, int cy, WORD id, DWORD style,
        const char *classname, const char *captiontext, const void *infodata, BYTE infodatalen, size_t *templatelen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;
    size_t              blocklen;
    UINT                classlen;
    UINT                textlen;
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
#ifdef __WINDOWS__
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
#ifdef __WINDOWS__
    blocklen = item_start + sizeof( WDLGITEMTEMPLATE ) + classlen + textlen + sizeof( BYTE ) + infodatalen;
#else
    blocklen = item_start + sizeof( WDLGITEMTEMPLATE ) + classlen + textlen + sizeof( WORD ) + infodatalen;
#endif

    GlobalUnlock( old_dlgtemplate );

    new_dlgtemplate = GlobalReAlloc( old_dlgtemplate, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new_dlgtemplate == NULL )
        return( (TEMPLATE_HANDLE)NULL );

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
     * append string data
     */

    ditstr = (WPCHAR)( dit + 1 );
    if( class_ordinal > 0 ) {
#ifdef __WINDOWS__
        *ditstr++ = class_ordinal;
#else
        ditstr = copyWord( ditstr, -1 );
        ditstr = copyWord( ditstr, class_ordinal );
#endif
    } else {
        ditstr = copyString( ditstr, classname, classlen );
    }
    ditstr = copyString( ditstr, captiontext, textlen );

    /*
     * append extra data
     */

#ifdef __WINDOWS__
    *ditstr++ = infodatalen;
#else
    ditstr = copyWord( ditstr, infodatalen );
#endif
    _FARmemcpy( ditstr, infodata, infodatalen );
    ditstr += infodatalen;

    *templatelen = (size_t)( ditstr - template );

    GlobalUnlock( new_dlgtemplate );
    return( new_dlgtemplate );

} /* _AddControl */

/*
 * _DoneAddingControls - called when there are no more controls
 */
void _DoneAddingControls( TEMPLATE_HANDLE dlgtemplate )
{
    GlobalUnlock( dlgtemplate );

} /* _DoneAddingControls */

/*
 * _DynamicDialogBox - create a dynamic dialog box
 */
INT_PTR _DynamicDialogBox( DLGPROCx dlgfn, HANDLE inst, HWND hwnd, TEMPLATE_HANDLE dlgtemplate )
{
    DLGPROC     dlgproc;
    INT_PTR     rc;

    dlgproc = MakeProcInstance_DLG( dlgfn, inst );
    rc = DialogBoxIndirect( inst, TEMPLATE_LOCK( dlgtemplate ), hwnd, dlgproc );
    TEMPLATE_UNLOCK( dlgtemplate );
    FreeProcInstance_DLG( dlgproc );
    GlobalFree( dlgtemplate );
    return( rc );

} /* _DynamicDialogBox */
