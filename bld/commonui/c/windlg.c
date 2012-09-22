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


#include "precomp.h"
#include "wpi.h"
#include <string.h>
#include <stdlib.h>
#include "windlg.h"

/*
 * copyString - copy from string to memory
 */
static char _ISFAR *copyString( char _ISFAR *mem, char _ISFAR *str, int len )
{
#if defined(__NT__) && !defined( __UNIX__ )
    // convert ANSI or DBCS string to Unicode properly - rnk 3/1/96
    MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, str, -1, (LPWSTR)mem, len );
    return( mem + len );
#if 0 // old non-Kanji safe version - rnk
    int i;

    for(i=0;i<len/2;i++ ) {
        *(short *)mem = *str;
        mem += 2;
        str++;
    }
    return( mem );
#endif
#else
    _FARmemcpy( mem, str, len );
    return( mem+len );
#endif

} /* copyString */


/*
 * DialogTemplate - build a dialog template
 */
TEMPLATE_HANDLE DialogTemplate( LONG dtStyle, int dtx, int dty, int dtcx,
                                int dtcy, char *menuname, char *classname,
                                char *captiontext, int pointsize,
                                char *typeface )
{
    TEMPLATE_HANDLE     data;
    UINT                blocklen,menulen, classlen, captionlen, typefacelen;
    UINT                _ISFAR *numbytes;
    char                _ISFAR *dlgtemp;
    char                _ISFAR *dlgtypeface;
    _DLGTEMPLATE        _ISFAR *dt;
    FONTINFO            _ISFAR *fi;


    /*
     * get size of block and allocate memory
     */
    menulen = SLEN( menuname );
    classlen = SLEN( classname );
    captionlen = SLEN( captiontext );

    blocklen = sizeof( UINT ) + sizeof( _DLGTEMPLATE ) + menulen + classlen +
              captionlen;

    if( dtStyle & DS_SETFONT ) {
      typefacelen = SLEN( typeface );
      blocklen += sizeof(short) + typefacelen;
    } else {
      typefacelen = 0;
    }

    ADJUST_BLOCKLEN( blocklen );
    data = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( data == NULL ) return( NULL );

    numbytes = (UINT _ISFAR *) MK_FP32( GlobalLock( data ) );
    *numbytes = (UINT) blocklen;

    /*
     * set up template
     */

    dt = (_DLGTEMPLATE _ISFAR *) (numbytes + 1);

    dt->dtStyle = dtStyle;
    dt->dtItemCount = 0;
    dt->dtX = dtx;
    dt->dtY = dty;
    dt->dtCX = dtcx;
    dt->dtCY = dtcy;

    dlgtemp = (char _ISFAR *) (dt + 1);

    /*
     * add extra strings to block
     */
    dlgtemp = copyString( dlgtemp, menuname, menulen );
    dlgtemp = copyString( dlgtemp, classname, classlen );
    dlgtemp = copyString( dlgtemp, captiontext, captionlen );


    /*
     * add font data (if needed)
     */
    if (dtStyle & DS_SETFONT) {
      #ifdef ALIGN_WORDS
          fi = (FONTINFO _ISFAR *) ((u_int)dlgtemp + 4 - (u_int)dlgtemp % 4);
      #else
          fi = (FONTINFO _ISFAR *) dlgtemp;
      #endif
      fi->PointSize = pointsize;
      dlgtypeface = (char _ISFAR *) (fi + 1);
      copyString( dlgtypeface, typeface, typefacelen );
    }

    GlobalUnlock( data );
    return( data );

} /* DialogTemplate */

/*
 * AddControl - add a control to a dialog
 */
TEMPLATE_HANDLE AddControl ( TEMPLATE_HANDLE data, int dtilx, int dtily,
                             int dtilcx, int dtilcy, int id, long style,
                             char *class, char *text, BYTE infolen,
                             char *infodata )
{
    TEMPLATE_HANDLE     new;
    UINT                blocklen, classlen, textlen;
    UINT                _ISFAR *numbytes;
    _DLGTEMPLATE        _ISFAR *dt;
    _DLGITEMTEMPLATE    _ISFAR *dit;
    char                _ISFAR * ditstr;
#if defined(__NT__) && !defined(__DEC__)
    unsigned char       newclass[2];
#endif

    style |= WS_CHILD;

    /*
     * compute size of block, reallocate block to hold this stuff
     */
#if defined(__NT__) && (!defined(__DEC__) || !defined( TWIN ))

    classlen = 2;
    if( !stricmp( class, "combobox" ) ) {
        newclass[0] = 0x85;
    } else if( !stricmp( class,"scrollbar" ) ) {
        newclass[0] = 0x84;
    } else if( !stricmp( class,"listbox" ) ) {
        newclass[0] = 0x83;
    } else if( !stricmp( class,"static" ) ) {
        newclass[0] = 0x82;
    } else if( !stricmp( class,"edit" ) ) {
        newclass[0] = 0x81;
    } else if( !stricmp( class,"button" ) ) {
        newclass[0] = 0x80;
    }
    newclass[1] = 0;
    class = (char *)newclass;
#else
    classlen = SLEN( class );
#endif
    textlen  = SLEN( text );

    blocklen = sizeof( _DLGITEMTEMPLATE ) + classlen + textlen +
               sizeof( INFOTYPE ) + infolen;
    ADJUST_ITEMLEN( blocklen );

    blocklen += *(UINT _ISFAR *) MK_FP32( GlobalLock( data ) );
    GlobalUnlock( data );

    new = GlobalReAlloc( data, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new == NULL ) return( NULL );

    numbytes = (UINT _ISFAR *) MK_FP32( GlobalLock( new ) );

    /*
     * one more item...
     */
    dt = (_DLGTEMPLATE _ISFAR *) (numbytes + 1);
    dt->dtItemCount++;


    /*
     * point to start of item template, and set up values
     */
    dit = (_DLGITEMTEMPLATE _ISFAR *) (((char _ISFAR *) numbytes) + *numbytes);
    dit->dtilStyle = style;
    dit->dtilX = dtilx;
    dit->dtilY = dtily;
    dit->dtilCX = dtilcx;
    dit->dtilCY = dtilcy;
    dit->dtilID = id;
#if defined(__NT__) && !defined(__DEC__)
    dit->crap = 0xffff;
#endif

    ditstr = (char _ISFAR *) (dit + 1);

    /*
     * append extra data
     */
    _FARmemcpy( ditstr, class, classlen );
    ditstr += ROUND_CLASSLEN( classlen );
    ditstr = copyString( ditstr, text, textlen );
    *((INFOTYPE *)ditstr) = infolen;
    ditstr += sizeof( INFOTYPE );
    _FARmemcpy( ditstr, infodata, infolen );
    ditstr += infolen;

    *numbytes = (UINT) ( ditstr - (char _ISFAR *) numbytes);
    ADJUST_BLOCKLEN( *numbytes );

    GlobalUnlock( new );
    return( new );

} /* AddControl */

/*
 * DoneAddingControls - called when there are no more controls
 */
TEMPLATE_HANDLE DoneAddingControls( TEMPLATE_HANDLE data )
{
    UINT        FAR *numbytes;

    numbytes = (UINT _ISFAR *) MK_FP32( GlobalLock( data ) );
    // This next line is dangerous, for a couple of reasons.
    // 1. The 2 at the end should be sizeof( UINT ).
    // 2. There should be parentheses around the *numbytes, for code readability.
    // 3. memcpy is not guaranteed to work if the buffers are overlapping. In
    //    this case, we are assuming that the implementation of memcpy is that
    //    it copies byte by byte starting from the beginning of the src and
    //    the beginning of the destination. If the implementation of memcpy
    //    changes to copy words or dwords, or starts at the end of the buffer
    //    and moves backwards, this code will cease to work.
    // I am not going to change this, since it has been like this for several
    // years now, but I'm leaving this comment as a warning.
    //
    // Graeme Perrow
    // June 4, 1998
    _FARmemcpy( numbytes, numbytes + 1, *numbytes - 2 );
    GlobalUnlock( data );
    return( data );
} /* DoneAddingControls */

/*
 * DynamicDialogBox - create a dynamic dialog box
 */
int DynamicDialogBox( LPVOID fn, HANDLE inst, HWND hwnd, TEMPLATE_HANDLE data,
                      LONG lparam )
{
    WPI_PROC    fp;
    int         rc;

#if defined( WINDU )
    return -1;
#endif

    fp = _wpi_makeprocinstance( fn, inst );
#if defined(__NT__) || defined(TWIN32)
    {
        LPCSTR  ptr;
        ptr = GlobalLock( data );
        rc = DialogBoxIndirectParam( (HINSTANCE)inst, (LPCDLGTEMPLATE)ptr, hwnd, (DLGPROC) fp, (LPARAM)lparam );
        GlobalUnlock( data );
    }
#else
    rc = DialogBoxIndirectParam( inst, data, hwnd, (LPVOID) fp, lparam );
#endif
    FreeProcInstance( fp );
    GlobalFree( data );
    return( rc );

} /* DynamicDialogBox */
