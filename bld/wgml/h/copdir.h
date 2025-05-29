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
* Description:  Declares functions used to manipulate .COP directory files:
*                   get_compact_entry()
*                   get_extended_entry()
*                   get_member_name()
*
* Note:         The field names are intended to correspond to the field names
*               shown in the Wiki. The Wiki structs are named when the structs
*               defined here are defined; they are not identical.
*
****************************************************************************/

#ifndef COPDIR_H_INCLUDED
#define COPDIR_H_INCLUDED

#include <stdint.h>
#include <stdio.h>

/* Macro definition. */

/* Function declarations. */

#ifdef  __cplusplus
extern "C" {    /* Use "C" linkage when in C++ mode. */
#endif

extern char         *search_member_name( const char *dir, const char *file_name );

#ifdef  __cplusplus
}   /* End of "C" linkage for C++. */
#endif

#endif  /* COPDIR_H_INCLUDED */
