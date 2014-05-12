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


#ifndef WGETFN_INCLUDED
#define WGETFN_INCLUDED

/****************************************************************************/
/* type definitions                                                         */
/****************************************************************************/
typedef struct {
    char *file_name;
    char *title;
    char *filter;
} WGetFileStruct;

/****************************************************************************/
/* function prototypes                                                      */
/****************************************************************************/
extern int  WGetFileFilter( void );
extern void WSetFileFilter( int );
extern char *WGetInitialDir( void );
extern void WSetInitialDir( char * );
extern char *WGetOpenFileName( HWND parent, WGetFileStruct * );
extern char *WGetSaveFileName( HWND parent, WGetFileStruct * );
extern bool WGetInternalRESName( char *filename, char *newname );
extern void WMassageFilter( char *filter );

#endif
