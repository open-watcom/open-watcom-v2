/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DIG loader file I/O interface functions (used for DIP/MAD/TRAP)
*
****************************************************************************/


#include <stdio.h>
#include "digtypes.h"


#define DIGLoader(n)    DIGLoader ## n

extern size_t   DIGLoader( Find )( dig_filetype ftype, const char *base_name, size_t base_name_len, const char *defext, char *filename, size_t filename_maxlen );
extern FILE     *DIGLoader( Open )( const char *filename );
extern int      DIGLoader( Close )( FILE *fp );
extern int      DIGLoader( Read )( FILE *fp, void *buff, size_t len );
extern int      DIGLoader( Seek )( FILE *fp, unsigned long offs, dig_seek where );
