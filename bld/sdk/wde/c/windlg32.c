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
#include "windlg32.h"
#include "wdedlgut.h"


/*
 * DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE DialogEXTemplate( DWORD style, DWORD exstyle, DWORD helpid,
                               int x, int y, int cx, int cy, const char *menuname,
                               const char *classname, const char *captiontext, WORD font_pointsize,
                               const char *font_facename, WORD fontweight, BYTE fontitalic, BYTE fontcharset, size_t *templatelen )
{
    TEMPLATE_HANDLE     dlgtemplate;
    UINT                blocklen, menulen, classlen, textlen, facenamelen;
    WPCHAR              template;
    WPDLGTEMPLATEEX     dt;
    WPFONTINFOEX        fi;

    *templatelen = 0;

    /*
     * get size of block and allocate memory
     */
    menulen = DlgStringLength( menuname );
    classlen = DlgStringLength( classname );
    textlen = DlgStringLength( captiontext );

    blocklen = sizeof( WDLGTEMPLATEEX ) + menulen + classlen + textlen;

    if( style & DS_SETFONT ) {
        facenamelen = DlgStringLength( font_facename );
        blocklen += sizeof( WFONTINFOEX ) + facenamelen;
    } else {
        facenamelen = 0;
    }

    dlgtemplate = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( dlgtemplate == NULL ) {
        return( NULL );
    }

    template = GetPtrGlobalLock( dlgtemplate );
    *templatelen = blocklen;

    /*
     * set up template
     */
    dt = (WPDLGTEMPLATEEX)template;

    dt->dtVer = 0x0001;                 // signature dword is 0xffff0001
    dt->dtSignature = 0xffff;
    dt->dtHelpID = helpid;
    dt->dtExtendedStyle = exstyle;
    dt->dtStyle = style;
    dt->dtItemCount = 0;

    dt->dtX = x;
    dt->dtY = y;
    dt->dtCX = cx;
    dt->dtCY = cy;

    template = (WPCHAR)( dt + 1 );

    /*
     * add extra strings to block
     */
    template = DlgCopyMBString( template, menuname, menulen );
    template = DlgCopyMBString( template, classname, classlen );
    template = DlgCopyMBString( template, captiontext, textlen );

    /*
     * add font data (if needed)
     */
    if( style & DS_SETFONT ) {
        fi = (WPFONTINFOEX)template;
        fi->PointSize = font_pointsize;
        fi->weight = fontweight;
        fi->bItalic = fontitalic;
        fi->bCharset = fontcharset;
        template = (WPCHAR)( fi + 1 );
        template = DlgCopyMBString( template, font_facename, facenamelen );
    }

    GlobalUnlock( dlgtemplate );
    return( dlgtemplate );

} /* DialogEXTemplate */

/*
 * AddControlEX - add a control to a dialog
 */
TEMPLATE_HANDLE AddControlEX( TEMPLATE_HANDLE old_dlgtemplate, int x, int y, int cx, int cy, DWORD id, DWORD style,
                           DWORD exstyle, DWORD helpid, const char *classname, const char *captiontext,
                           const void *infodata, BYTE infodatalen, size_t *templatelen )
{
    TEMPLATE_HANDLE     new_dlgtemplate;
    UINT                blocklen, classlen, textlen;
    WPCHAR              template;
    WPDLGTEMPLATEEX     dt;
    WPDLGITEMTEMPLATEEX dit;
    WPCHAR              ditstr;
    unsigned char       class_ordinal;
    size_t              item_start;

    /*
     * compute size of block, reallocate block to hold this stuff
     */

    class_ordinal = DlgGetClassOrdinal( classname );
    if( class_ordinal > 0 ) {
#if defined( __WINDOWS__ )
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
#if defined( __WINDOWS__ )
    blocklen = item_start + sizeof( WDLGITEMTEMPLATEEX ) + classlen + textlen + sizeof( BYTE ) + infodatalen;
#else
    blocklen = item_start + sizeof( WDLGITEMTEMPLATEEX ) + classlen + textlen + sizeof( WORD ) + infodatalen;
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
    dt = (WPDLGTEMPLATEEX)template;
    dt->dtItemCount++;

    /*
     * point to start of item template, and set up values
     */
    dit = (WPDLGITEMTEMPLATEEX)( template + item_start );
    dit->ditHelpID = helpid;
    dit->ditExtendedStyle = exstyle;
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
#if defined( __WINDOWS__ )
        *ditstr++ = class_ordinal;
#else
        ditstr = DlgCopyWord( ditstr, -1 );
        ditstr = DlgCopyWord( ditstr, class_ordinal );
#endif
    } else {
        ditstr = DlgCopyMBString( ditstr, classname, classlen );
    }
    ditstr = DlgCopyMBString( ditstr, captiontext, textlen );
#if defined( __WINDOWS__ )
    *ditstr++ = infodatalen;
#else
    ditstr = DlgCopyWord( ditstr, infodatalen );
#endif
    _FARmemcpy( ditstr, infodata, infodatalen );
    ditstr += infodatalen;

    *templatelen = (size_t)( ditstr - template );

    GlobalUnlock( new_dlgtemplate );
    return( new_dlgtemplate );

} /* AddControlEX */
