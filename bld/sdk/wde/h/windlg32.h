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

#if defined( __WINDOWS__ )
  #include "pushpck1.h"
#else
  #include "pushpck2.h"
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
    WORD        PointSize;
    WORD        weight;
    BYTE        bItalic;
    BYTE        bCharset;
//  char        fontName[];
} _FONTEXINFO;

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
#include "poppck.h"

extern GLOBALHANDLE DialogEXTemplate( DWORD dtStyle, DWORD dtExStyle, DWORD dthelpID, int dtx, int dty, int dtcx, int dtcy, const char *menuname, const char *classname, const char *captiontext, short pointsize, const char *typeface, short FontWeight, char FontItalic, char FontCharset, size_t *datalen );
extern GLOBALHANDLE AddControlEX( GLOBALHANDLE data, int dtilx, int dtily, int dtilcx, int dtilcy, DWORD id, DWORD style, DWORD exstyle, DWORD helpID, const char *class, const char *text, BYTE infolen, const char *infodata, size_t *datalen );

#endif
