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


#include "variety.h"
#include "widechar.h"
#include <io.h>
#include <dos.h>
#include "tinyio.h"
#include <errno.h>
#include "seterrno.h"
#ifdef __WIDECHAR__
    #include <mbstring.h>
    #include <stdlib.h>
    #include "mbwcconv.h"
#endif

extern  int           _dosret0(unsigned,unsigned);
extern  unsigned long _chmode(const char *,unsigned);
extern  unsigned long _BDchmode(const char *,unsigned);

#if defined(__386__)
#pragma aux     _chmode   = "   sub eax,eax" \
                            "   mov ah,43h"  \
                                _INT_21      \
                            "   jc  short L3"\
                            "   test bl,80h" \
                            "   je  short L1"\
                            "   and cl,0feh" \
                            "   jmp short L2"\
                            "L1:or  cl,01h"  \
                            "L2:mov ah,43h"  \
                            "   mov al,01h"  \
                                _INT_21      \
                            "L3:rcl eax,1"   \
                            "   ror eax,1"   \
                        parm caller [edx] [ebx] value [eax] modify [ecx];
#elif defined(M_I86)
#pragma aux     _chmode   = "   mov ax,4300h"\
                            "   int 21h"     \
                            "   jc  short L3"\
                            "   test bl,80h" \
                            "   je  short L1"\
                            "   and cl,0feh" \
                            "   jmp short L2"\
                            "L1:or  cl,01h"  \
                            "L2:mov ax,4301h"\
                            "   int 21h"     \
                            "L3:sbb dx,dx"   \
                        parm caller [dx] [bx] value [ax dx] modify [cx];

#pragma aux     _BDchmode = "   mov ax,4300h"\
                            "   push ds"     \
                            "   mov ds,cx"   \
                            "   int 21h"     \
                            "   jc  short L3"\
                            "   test bl,80h" \
                            "   je  short L1"\
                            "   and cl,0feh" \
                            "   jmp short L2"\
                            "L1:or  cl,01h"  \
                            "L2:mov ax,4301h"\
                            "   int 21h"     \
                            "L3:sbb dx,dx"   \
                            "   pop ds"      \
                        parm caller [dx cx] [bx] value [ax dx] modify [cx];
#else
#error platform not supported
#endif


_WCRTLINK int __F_NAME(chmod,_wchmod)( const CHAR_TYPE *pathname, int pmode )
{
#if defined(__PENPOINT__)

    pathname = pathname;
    pmode = pmode;

    __set_errno( EACCES );  /* Go will provide their own version of chmod */
    return( -1 );

#else
    unsigned long rc;
    unsigned ax, dx;

#ifdef __WIDECHAR__
    char                mbPath[MB_CUR_MAX*_MAX_PATH];
    __filename_from_wide( mbPath, pathname );
#endif

#if defined(__BIG_DATA__)
    rc = _BDchmode( __F_NAME(pathname,mbPath), pmode );
#else
    rc = _chmode( __F_NAME(pathname,mbPath), pmode );
#endif
    ax = rc & 0xffff;
    dx = rc >> 16;
    return _dosret0( ax, dx );
#endif
}

