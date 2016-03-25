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


#undef _FAR
#ifdef DLL32
    #define _FAR __far
#else
    #define _FAR
#endif

#define Align4K( x )  (((x) + 0xfffL) & ~0xfffL)
#define Align64K( x ) (((x) + 0xffffL) & ~0xffffL)

typedef struct {
    void (__far *rtn)();
} rtns;

typedef struct {
    DWORD off;
    WORD seg;
} addr_48;

typedef struct {
    char    sig[2];
    WORD    file_size1;
    WORD    file_size2;
    WORD    reloc_cnt;
    WORD    file_header;
    WORD    min_data;
    WORD    max_data;
    DWORD   initial_esp;
    WORD    checksum;
    DWORD   initial_eip;
    WORD    first_reloc;
    WORD    overlay_number;
    WORD    one;
} rex_exe;

typedef struct {
    DWORD datastart;
    DWORD stackstart;
} exe_data;

extern void FAR FiniDLLs( void );
extern int  WINAPI LibMain( HINSTANCE hmod, WORD dataseg, WORD heap, LPSTR cmdline );
