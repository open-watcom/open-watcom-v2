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
* Description:  Memory aliases interface.
*
****************************************************************************/


#ifndef _ALIAS_H_INCLUDED
#define _ALIAS_H_INCLUDED

#include "alias.rh"

typedef struct analias {
    ULONG_PTR           id;
    char                *name;
    struct analias      *next;
} AnAlias;

typedef struct {
    AnAlias             *data;
    void                (*updatefn)( ULONG_PTR, char *, char *, void * );
    void                *userdata;
} AliasList;

typedef AliasList       *AliasHdl;

void    InitAliasHdl( AliasHdl *hdl, void (*updatefn)( ULONG_PTR, char *, char *, void * ), void *userdata );
void    AddAlias( AliasHdl hdl, char *text, ULONG_PTR id );
void    FreeAlias( AliasHdl hdl );
char    *LookupAlias( AliasHdl hdl, ULONG_PTR id );
void    Query4Aliases( AliasHdl hdl, HANDLE instance, HWND hwnd, char *title );
void    EnumAliases( AliasHdl hdl, void (*enumfn)( ULONG_PTR, char *, void * ), void *userdata );

#endif /* _ALIAS_H_INCLUDED */
