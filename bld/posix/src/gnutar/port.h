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
 * Portability declarations for public domain tar.
 *
 * @(#)port.h 1.1       86/03/11        Public Domain by John Gilmore, 1986
 * MS-DOS port 2/87 by Eric Roskos.
 * Minix  port 3/88 by Eric Roskos.
 */

/*
 * Everybody does wait() differently.  There seem to be no definitions
 * for this in V7 (e.g. you are supposed to shift and mask things out
 * using constant shifts and masks.)  So fuck 'em all -- my own non
 * standard but portable macros.  Don't change to a "union wait"
 * based approach -- the ordering of the elements of the struct
 * depends on the byte-sex of the machine.  Foo!
 */
#define TERM_SIGNAL(status)     ((status) & 0x7F)
#define TERM_COREDUMP(status)   (((status) & 0x80) != 0)
#define TERM_VALUE(status)      ((status) >> 8)

extern char *index(char *s, int c);
extern char *rindex(char *s, int c);
extern char *valloc(unsigned size);
extern void uprintf(int, char *, ... );

