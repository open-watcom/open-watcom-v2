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


#define ALIAS_TEXT                  102
#define ALIAS_DO_MORE               108
#define ALIAS_CUR_ID                103
#define ALIAS_ID_LIST               105

typedef struct analias {
    unsigned long       id;
    char                *name;
    struct analias      *next;
} AnAlias;

typedef struct {
    AnAlias             *data;
    void                (*updatefn)();
    void                *userdata;
} AliasList;

typedef AliasList       *AliasHdl;

void InitAliasHdl( AliasHdl *hdl, void (*updatefn)(), void *userdata );
void AddAlias( AliasHdl hdl, char *text, unsigned long id );
void FreeAlias( AliasHdl hdl );
char *LookupAlias( AliasHdl hdl, unsigned long id );
void Query4Aliases( AliasHdl hdl, HANDLE instance, HWND hwnd, char *title );
void EnumAliases( AliasHdl hdl, void (*enumfn)(), void *userdata );
