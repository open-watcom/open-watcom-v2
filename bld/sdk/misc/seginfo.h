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


#define SEGINFO_ADDR            700
#define SEGINFO_BLOCKSIZE       701
#define SEGINFO_LOCKCOUNT       702
#define SEGINFO_PAGELOCKCOUNT   703
#define SEGINFO_HANDLE          704
#define SEGINFO_TYPE            705
#define SEGINFO_OK              706
#define SEGINFO_SCROLL          707
#define SEGINFO_LINE1           711
#define SEGINFO_LINE2           712
#define SEGINFO_LINE3           713
#define SEGINFO_LINE4           714
#define SEGINFO_LINE5           715
#define SEGINFO_LINE6           716
#define SEGINFO_LINE7           717

void DoSegInfo( HWND hwnd, WORD seg, BOOL isdpmi, HANDLE inst );
