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

typedef GLOBALHANDLE    TEMPLATE_HANDLE;

#if defined(__NT__) && !defined( TWIN )
#define ADJUST_ITEMLEN( a ) a = (((a)+7) & ~7)
#define ADJUST_BLOCKLEN( a ) a = (((a)+3) & ~3)
#define ROUND_CLASSLEN( a ) (((a)+1) & ~1)
#define _ISFAR
#define _FARmemcpy      memcpy
#ifndef MK_FP32
    #define MK_FP32( a )        ( a )
#endif
//#define SLEN( a ) (strlen((a))*2+2)
// fixed to handle DBCS strings properly - rnk 3/1/96
#define SLEN( a )       (2*MultiByteToWideChar( CP_ACP, MB_PRECOMPOSED, (a), -1, NULL, 0 ))
typedef WORD INFOTYPE;
#else
#define SLEN( a ) (strlen((a))+1)
#define ADJUST_ITEMLEN( a )
#define ADJUST_BLOCKLEN( a )
#define ROUND_CLASSLEN( a ) a
#define _ISFAR  __far
#define _FARmemcpy      _fmemcpy
typedef BYTE INFOTYPE;
#endif

#if defined(__NT__)
    #pragma pack(2);
#else
    // Added by Graeme Perrow June 3, 1998
    #pragma pack(push,1)
#endif
typedef struct {
long    dtStyle;
#if defined(__NT__) || defined(WILLOWS)
DWORD   dtExtendedStyle;
WORD    dtItemCount;
#else
BYTE    dtItemCount;
#endif
short   dtX;
short   dtY;
short   dtCX;
short   dtCY;
//char  dtMenuName[];
//char  dtClassName[];
//char  dtCaptionText[];
} _DLGTEMPLATE;

#if !defined(__NT__)
    // Added by Graeme Perrow June 3, 1998
    #pragma pack(pop)
#endif

typedef struct {
short   PointSize;
//char  szTypeFace[];
} FONTINFO;

typedef struct {
#if defined(__NT__) || defined(WILLOWS)
long    dtilStyle;
DWORD   dtExtendedStyle;
#endif
short   dtilX;
short   dtilY;
short   dtilCX;
short   dtilCY;
short   dtilID;
#if defined(__NT__)
unsigned short  crap;
#elif !defined(WILLOWS)
long    dtilStyle;
#endif
//char  dtilClass[];
//char  dtilText[];
//BYTE  dtilInfo;
//BYTE  dtilData;
} _DLGITEMTEMPLATE;
#if defined(__NT__)
#pragma pack(1);
#endif

extern TEMPLATE_HANDLE DialogTemplate( LONG dtStyle, int dtx, int dty,
                                       int dtcx, int dtcy, char *menuname,
                                       char *classname, char *captiontext,
                                       int pointsize, char *typeface );
extern TEMPLATE_HANDLE DoneAddingControls ( TEMPLATE_HANDLE data );
extern TEMPLATE_HANDLE AddControl    ( TEMPLATE_HANDLE data, int dtilx,
                                       int dtily, int dtilcx, int dtilcy,
                                       int id, long style, char *class,
                                       char *text, BYTE infolen,
                                       char *infodata );
int DynamicDialogBox                 ( LPVOID fn, HANDLE inst, HWND hwnd,
                                       TEMPLATE_HANDLE data, LONG lparam );

#endif
