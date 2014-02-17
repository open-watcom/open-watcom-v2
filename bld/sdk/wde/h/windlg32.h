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


#if !defined( __OS2__ )

#ifdef __NT__
    #define ADJUST_ITEMLEN_DWORD( a )   a = (((a) + 3) & ~3)    // DWORD align
    #define ADJUST_BLOCKLEN_DWORD( a )  a = (((a) + 3) & ~3)    // DWORD align
#else
    #define ADJUST_ITEMLEN_DWORD( a )
    #define ADJUST_BLOCKLEN_DWORD( a )
#endif

#if defined( __NT__ )
//#if defined( __ALPHA__ )
//    #include "pushpck1.h"
//#else
    #include "pushpck2.h"
//#endif
#endif

typedef struct {
    WORD        dtVer;
    WORD        dtSignature;
    DWORD       dthelpID;
    DWORD       dtExtendedStyle;
    DWORD       dtStyle;
    WORD        dtItemCount;
    short       dtX;
    short       dtY;
    short       dtCX;
    short       dtCY;
} _DLGEXTEMPLATE;

typedef struct {
    short       PointSize;
    short       weight;
    short       bItalic;
//  char        fontName[];
} FONTEXINFO;

typedef struct {
    DWORD       dtilhelpID;
    DWORD       dtilExtendedStyle;
    DWORD       dtilStyle;
    short       dtilX;
    short       dtilY;
    short       dtilCX;
    short       dtilCY;
    DWORD       dtilID;
} _DLGEXITEMTEMPLATE;

#if defined( __NT__ )
    #include "poppck.h"
#endif

extern GLOBALHANDLE DialogEXTemplate( DWORD dtStyle, DWORD dtExStyle, DWORD dthelpID, int dtx, int dty, int dtcx, int dtcy, char *menuname, char *classname, char *captiontext, short pointsize, char *typeface, short FontWeight, short FontItalic );
extern GLOBALHANDLE AddControlEX( GLOBALHANDLE data, int dtilx, int dtily, int dtilcx, int dtilcy, DWORD id, DWORD style, DWORD exstyle, DWORD helpID, char *class, char *text, BYTE infolen, char *infodata );

#endif
