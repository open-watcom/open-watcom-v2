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
* Description:  Wrappers for miscellaneous C library functions.
*
****************************************************************************/


#include <time.h>

#ifdef _M_I86
#define _FAR __far
#else
#define _FAR
#endif

extern int            _FAR __pascal _clib_intdos( union REGS _FAR *in_regs, union REGS _FAR *out_regs );
extern int            _FAR __pascal _clib_intdosx( union REGS _FAR *in_regs, union REGS _FAR *out_regs, struct SREGS _FAR *seg_regs );
extern int            _FAR __pascal _clib_int86( int inter_no, union REGS _FAR *in_regs, union REGS _FAR *out_regs );
extern int            _FAR __pascal _clib_int86x( int inter_no, union REGS _FAR *in_regs, union REGS _FAR *out_regs, struct SREGS _FAR *seg_regs );
extern void           _FAR __pascal _clib_intr( int inter_no, union REGPACK _FAR *regs );
extern unsigned       _FAR __pascal _clib_dos_findfirst( char _FAR *__path,unsigned __attr, struct find_t _FAR *__buf );
extern unsigned       _FAR __pascal _clib_dos_findnext( struct find_t _FAR *__buf );
extern unsigned       _FAR __pascal _clib_dos_read( int __handle, void _FAR *__buf, unsigned __count, unsigned long _FAR *__bytes );
extern unsigned       _FAR __pascal _clib_dos_write( int __handle, void _FAR *__buf, unsigned __count, unsigned long _FAR *__bytes );
extern clock_t        _FAR __pascal _clib_clock( void );
extern void           _FAR __pascal _clib_delay( unsigned long ms );
extern int            _FAR __pascal _clib_errno( void );
extern unsigned short _FAR __pascal _clib_bios_disk( unsigned __cmd, struct diskinfo_t _FAR *__diskinfo );
extern unsigned short _FAR __pascal _clib_bios_equiplist( void );
extern unsigned short _FAR __pascal _clib_bios_keybrd( unsigned __cmd );
extern unsigned short _FAR __pascal _clib_bios_memsize( void );
extern unsigned short _FAR __pascal _clib_bios_printer( unsigned __cmd, unsigned __port, unsigned __data );
extern unsigned short _FAR __pascal _clib_bios_serialcom( unsigned __cmd, unsigned __port, unsigned __data );
extern unsigned short _FAR __pascal _clib_bios_timeofday( unsigned __cmd, long _FAR *__timeval );
