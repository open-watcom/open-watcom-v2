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
* Description:  Dynamically build a Windows dialog template.
*
****************************************************************************/


#include "variety.h"
#include <windows.h>
#include <string.h>
#include <stdlib.h>
#include "win.h"

/*
 * copyString - copy from string to memory
 */
static char _ISFAR *copyString( char _ISFAR *mem, const char _ISFAR *str, int len )
{
#if defined(__NT__) && !defined(__DEC__)
    int i;

    for(i=0;i<len/2;i++ ) {
        *(short *)mem = *str;
        mem += 2;
        str++;
    }
    return( mem );
#else
    _FARmemcpy( mem, str, len );
    return( mem+len );
#endif

} /* copyString */


/*
 * _DialogTemplate - build a dialog template
 */
GLOBALHANDLE _DialogTemplate( LONG dtStyle, int dtx, int dty, int dtcx,
                       int dtcy, const char *menuname, const char *classname,
                       const char *captiontext, int pointsize, const char *typeface )
{
    GLOBALHANDLE        data;
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

    blocklen = sizeof( UINT ) + sizeof( _DLGTEMPLATE ) + menulen + classlen + captionlen;
    ADJUST_BLOCKLEN( blocklen );

    if( dtStyle & DS_SETFONT ) {
        typefacelen = SLEN( typeface );
        blocklen += sizeof(short) + typefacelen;
    } else {
        typefacelen = 0;
    }

    data = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( data == NULL ) return( (GLOBALHANDLE)NULL );

    numbytes = GetPtrGlobalLock( data );
    *numbytes = (UINT)blocklen;

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
        fi = (FONTINFO _ISFAR *) dlgtemp;
        fi->PointSize = pointsize;
        dlgtypeface = (char _ISFAR *) (fi + 1);
        copyString( dlgtypeface, typeface, typefacelen );
    }

    GlobalUnlock( data );
    return( data );

} /* _DialogTemplate */

/*
 * _AddControl - add a control to a dialog
 */
GLOBALHANDLE _AddControl( GLOBALHANDLE data, int dtilx, int dtily,
                   int dtilcx, int dtilcy, int id, long style, const char *class,
                   const char *text, BYTE infolen, const char *infodata )
{
    GLOBALHANDLE        new;
    UINT                blocklen, classlen, textlen;
    UINT                _ISFAR *numbytes;
    _DLGTEMPLATE        _ISFAR *dt;
    _DLGITEMTEMPLATE    _ISFAR *dit;
    char                _ISFAR * ditstr;
#if defined(__NT__) && !defined(__DEC__)
    char                newclass[2];
#endif

    style |= WS_CHILD;

    /*
     * compute size of block, reallocate block to hold this stuff
     */
#if defined(__NT__) && !defined(__DEC__)
    if( !stricmp( class,"listbox" ) ) {
        newclass[0] = 0x83;
    } else if( !stricmp( class,"static" ) ) {
        newclass[0] = 0x82;
    } else if( !stricmp( class,"edit" ) ) {
        newclass[0] = 0x81;
    } else if( !stricmp( class,"button" ) ) {
        newclass[0] = 0x80;
    }
    newclass[1] = 0;
    class = (const char *)newclass;
    classlen = 2;
#else
    classlen = SLEN( class );
#endif
    textlen  = SLEN( text );

    blocklen = sizeof( _DLGITEMTEMPLATE ) + classlen + textlen + sizeof( INFOTYPE ) + infolen;
    ADJUST_ITEMLEN( blocklen );

    blocklen += *(UINT _ISFAR *)GetPtrGlobalLock( data );
    GlobalUnlock( data );

    new = GlobalReAlloc( data, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new == NULL ) return( (GLOBALHANDLE)NULL );

    numbytes = GetPtrGlobalLock( new );

    /*
     * one more item...
     */
    dt = (_DLGTEMPLATE _ISFAR *)( numbytes + 1 );
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
    *((INFOTYPE _ISFAR *)ditstr) = infolen;
    ditstr += sizeof( INFOTYPE );
    _FARmemcpy( ditstr, infodata, infolen );
    ditstr += infolen;

    *numbytes = (UINT) ( ditstr - (char _ISFAR *)numbytes);
    ADJUST_BLOCKLEN( *numbytes );

    GlobalUnlock( new );
    return( new );

} /* _AddControl */

/*
 * _DoneAddingControls - called when there are no more controls
 */
void _DoneAddingControls( GLOBALHANDLE data )
{
    UINT        _ISFAR *numbytes;

    numbytes = GetPtrGlobalLock( data );
    _FARmemcpy( numbytes, numbytes + 1, *numbytes - 2 );
    GlobalUnlock( data );

} /* _DoneAddingControls */

/*
 * _DynamicDialogBox - create a dynamic dialog box
 */
INT_PTR _DynamicDialogBox( DLGPROCx fn, HANDLE inst, HWND hwnd, GLOBALHANDLE data )
{
    FARPROC     fp;
    INT_PTR     rc;

    fp = MakeProcInstance( (FARPROCx)fn, inst );
#ifndef __NT__
    rc = DialogBoxIndirect( inst, data, hwnd, (DLGPROC)fp );
#else
    {
        LPVOID  ptr;
        ptr = GlobalLock( data );
        rc = DialogBoxIndirect( inst, ptr, hwnd, (DLGPROC)fp );
        GlobalUnlock( data );
    }
#endif
    FreeProcInstance( fp );
    GlobalFree( data );
    return( rc );

} /* _DynamicDialogBox */
