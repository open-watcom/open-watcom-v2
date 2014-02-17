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


extern void   PMfree( void * ptr );
extern void * PMmalloc( size_t size );
extern void * PMrealloc( void * ptr, size_t size );

#define _ISFAR
#define _FARmemcpy      memcpy
#define SLEN( a ) (safeStrLen((a))+0)
#define ADJUST_ITEMLEN( a )
#define ADJUST_BLOCKLEN( a )
#define ROUND_CLASSLEN( a ) a
typedef BYTE INFOTYPE;

/*
 * OS/2 PM defaults,
 */
#define CODE_PAGE               437
#define TEMPLATE_TYPE           0
#define RESERVED                0

/*
 * the pm data structures
 */
typedef DLGTITEM                DLGITEMTEMPLATE;

/*
 * I am not about to change this in ALL the code, and since I am not including
 * windows.h, I have defined it as such
 */
typedef PVOID           TEMPLATE_HANDLE;

extern TEMPLATE_HANDLE DialogTemplate ( LONG dtStyle, int dtx, int dty,
                                        int dtcx, int dtcy, char *menuname,
                                        char *classname, char *captiontext,
                                        int pointsize, char *typeface );
extern TEMPLATE_HANDLE DoneAddingControls ( TEMPLATE_HANDLE data );
extern TEMPLATE_HANDLE AddControl     ( TEMPLATE_HANDLE data, int dtilx,
                                        int dtily, int dtilcx, int dtilcy,
                                        int id, long style, char *class,
                                        char *text, BYTE infolen,
                                        char *infodata );
extern int DynamicDialogBox           ( PFNWP fn, WPI_INST inst, HWND hwnd,
                                        TEMPLATE_HANDLE data, MPARAM lparam );

