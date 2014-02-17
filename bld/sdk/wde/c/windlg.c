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

#ifndef MB_ERR_INVALID_CHARS
    #define MB_ERR_INVALID_CHARS 0x00000000
#endif

/*
 * stringLength - get length of string
 */
static int stringLength( char _ISFAR *str )
{
#if defined( __NT__ ) && !defined( __DEC__ )
    if( str == NULL ) {
        return( SLEN( str ) );
    } else {
        int len;
        len = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS, str, -1, NULL, 0 );
        if( len == 0 || len == ERROR_NO_UNICODE_TRANSLATION ) {
            return( SLEN( str ) );
        }
        return( len * sizeof( WCHAR ) );
    }
#else
    return( SLEN( str ) );
#endif

} /* stringLength */

/*
 * copyString - copy from string to memory
 */
static char _ISFAR *copyString( char _ISFAR *mem, char _ISFAR *str, int len )
{
#if defined(__NT__) && !defined(__DEC__)
    int i;

    if( mem != NULL && str != NULL ) {
        for( i = 0; i < len / 2; i++ ) {
            *(short *)mem = *str;
            mem += 2;
            str++;
        }
        return( mem );
    } else {
        return( mem + len );
    }
#else
    if( mem != NULL && str != NULL ) {
        _FARmemcpy( mem, str, len );
    }
    return( mem + len );
#endif

} /* copyString */

/*
 * copyMBString - copy from string to memory
 */
static char _ISFAR *copyMBString( char _ISFAR *mem, char _ISFAR *str, int len )
{
#if defined( __NT__ ) && !defined( __DEC__ )
    if( mem != NULL && str != NULL ) {
        int     len2;
        len2 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS,
                                    str, -1, (LPWSTR)mem, len );
        len2 *= sizeof( WCHAR );
        if( len2 != len ) {
            return( copyString( mem, str, len ) );
        }
        return( mem + len );
    } else {
        return( copyString( mem, str, len ) );
    }
#else
    return( copyString( mem, str, len ) );
#endif

} /* copyMBString */


/*
 * DialogTemplate - build a dialog template
 */
GLOBALHANDLE DialogTemplate( LONG dtStyle, int dtx, int dty,
                             int dtcx, int dtcy, char *menuname, char *classname,
                             char *captiontext, int pointsize, char *typeface )
{
    GLOBALHANDLE        data;
    UINT                blocklen, menulen, classlen, captionlen, typefacelen;
    UINT                _ISFAR *numbytes;
    char                _ISFAR *dlgtemp;
    char                _ISFAR *dlgtypeface;
    _DLGTEMPLATE        _ISFAR *dt;
    FONTINFO            _ISFAR *fi;


    /*
     * get size of block and allocate memory
     */
    menulen = stringLength( menuname );
    classlen = stringLength( classname );
    captionlen = stringLength( captiontext );

    blocklen = sizeof( UINT ) + sizeof( _DLGTEMPLATE ) + menulen + classlen + captionlen;

    if( dtStyle & DS_SETFONT ) {
      typefacelen = stringLength( typeface );
      blocklen += sizeof( short ) + typefacelen;
    } else {
      typefacelen = 0;
    }

    ADJUST_BLOCKLEN( blocklen );

    data = GlobalAlloc( GMEM_MOVEABLE | GMEM_ZEROINIT, blocklen );
    if( data == NULL ) {
        return( NULL );
    }

    numbytes = GetPtrGlobalLock( data );
    *numbytes = (UINT)blocklen;

    /*
     * set up template
     */
    dt = (_DLGTEMPLATE _ISFAR *)(numbytes + 1);

    dt->dtStyle = dtStyle;
    dt->dtItemCount = 0;
    dt->dtX = dtx;
    dt->dtY = dty;
    dt->dtCX = dtcx;
    dt->dtCY = dtcy;

    dlgtemp = (char _ISFAR *)(dt + 1);

    /*
     * add extra strings to block
     */
    dlgtemp = copyMBString( dlgtemp, menuname, menulen );
    dlgtemp = copyMBString( dlgtemp, classname, classlen );
    dlgtemp = copyMBString( dlgtemp, captiontext, captionlen );


    /*
     * add font data (if needed)
     */
    if( dtStyle & DS_SETFONT ) {
        fi = (FONTINFO _ISFAR *)dlgtemp;
        fi->PointSize = pointsize;
        dlgtypeface = (char _ISFAR *)(fi + 1);
        copyMBString( dlgtypeface, typeface, typefacelen );
    }

    GlobalUnlock( data );
    return( data );

} /* DialogTemplate */

#if defined( __NT__ )
#include "pushpck1.h"
typedef struct MyControlClass {
    unsigned short      crap;
    unsigned char       class[2];
} MyControlClass;
#endif

/*
 * AddControl - add a control to a dialog
 */
GLOBALHANDLE AddControl( GLOBALHANDLE data, int dtilx, int dtily,
                         int dtilcx, int dtilcy, int id, long style,
                         char *class, char *text, BYTE infolen, char *infodata )
{
    GLOBALHANDLE        new;
    UINT                blocklen, classlen, textlen;
    UINT                _ISFAR *numbytes;
    _DLGTEMPLATE        _ISFAR *dt;
    _DLGITEMTEMPLATE    _ISFAR *dit;
    char                _ISFAR * ditstr;
#if defined( __NT__ ) && !defined( __DEC__ )
    MyControlClass      cclass;
    BOOL                textClass;
#endif

    /*
     * compute size of block, reallocate block to hold this stuff
     */
#if defined( __NT__ ) && !defined( __DEC__ )

    textClass = FALSE;
    classlen = sizeof( cclass );
    cclass.crap = 0xffff;
    cclass.class[0] = 0;
    cclass.class[1] = 0;

    if( class[0] & 0x80 ) {
        cclass.class[0] = class[0];
    } else {
        if( !stricmp( class, "combobox" ) ) {
            cclass.class[0] = 0x85;
        } else if( !stricmp( class, "scrollbar" ) ) {
            cclass.class[0] = 0x84;
        } else if( !stricmp( class, "listbox" ) ) {
            cclass.class[0] = 0x83;
        } else if( !stricmp( class, "static" ) ) {
            cclass.class[0] = 0x82;
        } else if( !stricmp( class, "edit" ) ) {
            cclass.class[0] = 0x81;
        } else if( !stricmp( class, "button" ) ) {
            cclass.class[0] = 0x80;
        }

        if( cclass.class[0] == 0 ) {
            classlen = stringLength( class );
            textClass = TRUE;
        }
    }

#else
    classlen = stringLength( class );
#endif

    textlen = stringLength( text );

    blocklen = sizeof( _DLGITEMTEMPLATE ) + classlen + textlen +
               sizeof( INFOTYPE ) + infolen;
    ADJUST_ITEMLEN( blocklen );

    blocklen += *(UINT _ISFAR *)GetPtrGlobalLock( data );
    GlobalUnlock( data );

    new = GlobalReAlloc( data, blocklen, GMEM_MOVEABLE | GMEM_ZEROINIT );
    if( new == NULL ) {
        return( NULL );
    }

    numbytes = GetPtrGlobalLock( new );

    /*
     * one more item...
     */
    dt = (_DLGTEMPLATE _ISFAR *)(numbytes + 1);
    dt->dtItemCount++;


    /*
     * point to start of item template, and set up values
     */
    dit = (_DLGITEMTEMPLATE _ISFAR *)((char _ISFAR *)numbytes + *numbytes);
    dit->dtilStyle = style;
    dit->dtilX = dtilx;
    dit->dtilY = dtily;
    dit->dtilCX = dtilcx;
    dit->dtilCY = dtilcy;
    dit->dtilID = id;

    ditstr = (char _ISFAR *)(dit + 1);

    /*
     * append extra data
     */

#if defined( __NT__ ) && !defined( __DEC__ )
    if( textClass != NULL ) {
        ditstr = copyMBString( ditstr, class, classlen );
    } else {
        _FARmemcpy( ditstr, &cclass, classlen );
        ditstr += ROUND_CLASSLEN( classlen );
    }
#else
    if( class[0] & 0x80 ) {
        _FARmemcpy( ditstr, class, classlen );
        ditstr += ROUND_CLASSLEN( classlen );
    } else {
        ditstr = copyMBString( ditstr, class, classlen );
    }
#endif

    ditstr = copyMBString( ditstr, text, textlen );
    *(INFOTYPE *)ditstr = infolen;
    ditstr += sizeof( INFOTYPE );
    _FARmemcpy( ditstr, infodata, infolen );
    ditstr += infolen;

    *numbytes = (UINT)(ditstr - (char _ISFAR *)numbytes);
    ADJUST_BLOCKLEN( *numbytes );

    GlobalUnlock( new );
    return( new );

} /* AddControl */

/*
 * DoneAddingControls - called when there are no more controls
 */
void DoneAddingControls( GLOBALHANDLE data )
{
    UINT        _ISFAR *numbytes;

    numbytes = GetPtrGlobalLock( data );
    _FARmemcpy( numbytes, numbytes + 1, *numbytes - sizeof( UINT ) );
    GlobalUnlock( data );

} /* DoneAddingControls */

/*
 * DynamicDialogBox - create a dynamic dialog box
 */
INT_PTR DynamicDialogBox( DLGPROC fn, HANDLE inst, HWND hwnd, GLOBALHANDLE data )
{
    FARPROC     fp;
    INT_PTR     rc;
#ifdef __NT__
    LPVOID      ptr;
#endif

    fp = MakeProcInstance( (FARPROC)fn, inst );
#ifndef __NT__
    rc = DialogBoxIndirect( inst, data, hwnd, (DLGPROC)fp );
#else
    ptr = GlobalLock( data );
    rc = DialogBoxIndirect( inst, ptr, hwnd, (DLGPROC)fp );
    GlobalUnlock( data );
#endif
    FreeProcInstance( fp );
    GlobalFree( data );
    return( rc );

} /* DynamicDialogBox */
