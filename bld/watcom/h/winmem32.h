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


/*
 * Function protypes and general defines for WINMEM32 DLL
 *      Version 1.00
 *
 * NOTE that WINDOWS.H must be included before this file.
 *
 */

/*
 *
 * The functions
 *
 */
WORD    FAR PASCAL GetWinMem32Version(void);
WORD    FAR PASCAL Global32Alloc(DWORD, LPWORD, DWORD, WORD);
WORD    FAR PASCAL Global32Realloc(WORD, DWORD, WORD);
WORD    FAR PASCAL Global32Free(WORD, WORD);
WORD    FAR PASCAL Global16PointerAlloc(WORD, DWORD, LPDWORD, DWORD, WORD);
WORD    FAR PASCAL Global16PointerFree(WORD, DWORD, WORD);
WORD    FAR PASCAL Global32CodeAlias(WORD, LPWORD, WORD);
WORD    FAR PASCAL Global32CodeAliasFree(WORD, WORD, WORD);

/*
 *
 * Error Codes
 *
 */
#define WM32_Invalid_Func       0001
#define WM32_Invalid_Flags      0002
#define WM32_Invalid_Arg        0003
#define WM32_Insufficient_Sels  0004
#define WM32_Insufficient_Mem   0005
