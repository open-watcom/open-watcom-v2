/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983_2002 Sybase, Inc. All Rights Reserved.
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
*    NON_INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  Declare prototypes for buffer.c
*
****************************************************************************/

#ifndef _BUFFER_H_E26634FC_AA8D_4160_A1C7_FD5A689FB8F0
#define _BUFFER_H_E26634FC_AA8D_4160_A1C7_FD5A689FB8F0

extern void    open_archive( int read );
extern void    close_archive( void );
extern void    saverec( union record  **pointer );
extern void    anno( FILE *stream, char *prefix, int savedp );
extern void    userec( union record   *rec );

extern union record    *endofrecs( void );
extern union record    *findrec( void );

#endif /* _BUFFER_H_E26634FC_AA8D_4160_A1C7_FD5A689FB8F0 */
