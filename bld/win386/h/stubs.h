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


#ifdef DLL32
typedef LPVOID far *LPLPVOID;
#else
typedef LPVOID *LPLPVOID;
#endif

void DPMIFreeAlias( WORD seg );
#pragma aux DPMIGetAliases parm[dx ax] [es si] [cx] value[ax];
WORD DPMIGetAliases( DWORD offset, DWORD far *res, WORD cnt );
extern WORD DPMIGetHugeAlias( DWORD, DWORD far *, DWORD );
extern void DPMIFreeHugeAlias( DWORD , DWORD );
extern WORD DataSelector;
extern WORD MyDataSelector;
extern DWORD DataSelectorSize;
extern WORD Has87;
extern void ReleaseAlias( LPVOID orig, LPVOID ptr );
extern void GetAlias( LPLPVOID name );
