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


#ifndef DWLINE_H_INCLUDED
#define DWLINE_H_INCLUDED

typedef struct dw_include {
    struct dw_include   *next;
    uint                number;
    char                name[1];
} dw_include;


typedef struct dw_include_stack {
    struct dw_include   *next;
    uint                number;
} dw_include_stack;


#define InitDebugLine           DW_InitDebugLine
#define FiniDebugLine           DW_FiniDebugLine
#define GetFileNumber           DW_GetFileNumber

extern  void    InitDebugLine( dw_client cli, const char *, const char *, unsigned );
extern  void    FiniDebugLine( dw_client cli );
extern  uint    GetFileNumber( dw_client cli, const char * );

#endif
