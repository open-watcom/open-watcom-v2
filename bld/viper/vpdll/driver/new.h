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


#define ID_ADD          0x0001
#define ID_REMOVE       0x0003
#define ID_RETURN       0x0004
#define ID_DONE         0x0005
#define ID_EDIT         0x0006
#define MENU_GO         101

#define DLLEXPORT       _export _pascal

typedef struct {
    DWORD DLLEXPORT (*VPDLL_GetVersion)( void );
    BOOL DLLEXPORT (*VPDLL_SetCallBack)( FARPROC fn );
    BOOL DLLEXPORT (*VPDLL_ReturnToIDE)( void );
    BOOL DLLEXPORT (*VPDLL_AddFile)( char *fname );
    BOOL DLLEXPORT (*VPDLL_RemoveFile)( char *fname );
    void DLLEXPORT (*VPDLL_VPDone)( void );
    BOOL DLLEXPORT (*VPDLL_BeginFileList)( void );
    BOOL DLLEXPORT (*VPDLL_EndFileList)( void );
}FunctionPtrs;
#define ID_BEGIN_LIST               102
#define ID_END_LIST                 103
