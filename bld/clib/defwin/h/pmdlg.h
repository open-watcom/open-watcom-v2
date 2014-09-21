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


#if defined( __OS2__ )

/*
 * Below are the general memory manager defines, using these
 * you can specify a different memory manager without
 * modifying the source code.
 */
#define PMmalloc        malloc
#define PMrealloc       realloc
#define PMfree          free

/*
 * The conversion of the default windowing system from WINDOWS to PM tried
 * not to make to many modifcations.  The following macros handle some
 * minor differences.
 */
#define _FARmemcpy      memcpy
#define SLEN( a ) (safeStrLen((a))+0)
#define ADJUST_ITEMLEN( a )
#define ADJUST_BLOCKLEN( a )
#define ROUND_CLASSLEN( a ) a
typedef BYTE INFOTYPE;

/*
 * OS/2 PM defaults,
 */
#define CODE_PAGE       850
#define TEMP_TYPE       0
#define RESERVED        0

/*
 * the pm data structures
 */
typedef DLGTITEM                DLGITEMTEMPLATE;

/*
 * these are normally defined in windows.h
 */
typedef void *          GLOBALHANDLE;
#if !defined( WPI_INTERFAC )
typedef void *          LPVOID;
#endif

extern GLOBALHANDLE _DialogTemplate( USHORT temptype, USHORT codepage, USHORT focus );
extern GLOBALHANDLE _DoneAddingControls( GLOBALHANDLE data );
GLOBALHANDLE _AddControl( GLOBALHANDLE data, long style, USHORT dtx,
                        USHORT dty, USHORT dtcx, USHORT dtcy, USHORT id,
                        USHORT children, ULONG nclass, const char *class,
                        const char *text, const char *presparms, const char *ctldata,
                        ULONG *ctldatlen );
int _DynamicDialogBox( PFNWP fn, HWND hwnd, GLOBALHANDLE data );

#endif
