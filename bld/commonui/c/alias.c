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
* Description:  Helper routines for memory aliases.
*
****************************************************************************/


#include "commonui.h"
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include "bool.h"
#include "mem.h"
#include "alias.h"
#ifndef NOUSE3D
    #include "ctl3dcvr.h"
#endif
#include "ldstr.h"
#include "uistr.gh"
#include "wprocmap.h"

#define CONST_LEN       15

static AliasHdl         CurHdl;         /* used for dialog box processing */

/*
 * findAlias - search for an alias with the given handle and identifier
 */
static AnAlias *findAlias( AliasHdl hdl, unsigned long id )
{
    AnAlias     *cur;

    cur = hdl->data;
    while( cur != NULL ) {
        if( cur->id == id ) {
            return( cur );
        }
        if( cur->id > id ) {
            break;
        }
        cur = cur->next;
    }
    return( NULL );

} /* findAlias */

/*
 * InitAliasHdl - initialize an alias handle before any aliases are
 *                assigned to it
 */
void InitAliasHdl( AliasHdl *hdl,
                   void (*updatefn)( unsigned long, char *, char *, void * ),
                   void *userdata )
{
    *hdl = MemAlloc( sizeof( AliasList ) );
    (*hdl)->data = NULL;
    (*hdl)->userdata = userdata;
    (*hdl)->updatefn = updatefn;

} /* InitAliasHdl */

/*
 * insertAlias - insert an alias in the alias list using linear
 *               insertion sort
 */
static void insertAlias( AliasHdl hdl, AnAlias *alias )
{
    AnAlias     **cur;

    cur = &hdl->data;
    for( ;; ) {
        if( *cur == NULL || (*cur)->id > alias->id ) {
            alias->next = *cur;
            *cur = alias;
            break;
        }
        cur = &(*cur)->next;
    }

} /* insertAlias */

/*
 * AddAlias - add an alias to an alias list
 *          - if an alias already exists for this identifier replace it
 */
void AddAlias( AliasHdl hdl, char *text, unsigned long id )
{
    AnAlias     *cur;
    size_t      len;

    cur = findAlias( hdl, id );
    if( cur == NULL ) {
        cur = MemAlloc( sizeof( AnAlias ) );
        cur->id = id;
        insertAlias( hdl, cur );
        if( hdl->updatefn != NULL ) {
            hdl->updatefn( id, text, NULL, hdl->userdata );
        }
    } else {
        if( hdl->updatefn != NULL ) {
            hdl->updatefn( id, text, cur->name, hdl->userdata );
        }
        MemFree( cur->name );
    }
    len = strlen( text ) + 1;
    cur->name = MemAlloc( len );
    strcpy( cur->name, text );

} /* AddAlias */

/*
 * FreeAlias - free all memory associated with an alias list
 *           - InitAliasHdl must be called before this handle can be
 *             used again
 */

void FreeAlias( AliasHdl hdl )
{
    AnAlias     *cur;
    AnAlias     *tmp;

    cur = hdl->data;
    while( cur != NULL ) {
        MemFree( cur->name );
        tmp = cur;
        cur = cur->next;
        MemFree( tmp );
    }
    MemFree( hdl );

} /* FreeAlias */

/*
 * LookupAlias - return the string associated with an identifier or NULL if
 *               no alias exists
 */
char *LookupAlias( AliasHdl hdl, unsigned long id )
{
    AnAlias     *cur;

    cur = findAlias( hdl, id );
    if( cur == NULL ) {
        return( NULL );
    }
    return( cur->name );

} /* LookupAlias */

/*
 * getIthAlias - get the alias in the given alias list with the given index
 */
static AnAlias *getIthAlias( AliasHdl hdl, int i )
{
    AnAlias     *ret;

    ret = hdl->data;
    while( i-- > 0 ) {
        ret = ret->next;
    }
    return( ret );
}

/*
 * findAliasFromText - find an alias in the given alias list with the given text
 */
static AnAlias *findAliasFromText( AliasHdl hdl, char *alias )
{
    AnAlias     *cur;

    cur = hdl->data;
    while( cur != NULL ) {
        if( !strcmp( alias, cur->name ) ) {
            break;
        }
        cur = cur->next;
    }
    return( cur );

} /* findAliasFromText */

/*
 * AliasDlgProc - alias list dialog procedure
 */
WINEXPORT INT_PTR CALLBACK AliasDlgProc( HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam )
{
    AnAlias     *cur;
    size_t      len;
    char        buf[CONST_LEN];
    char        msgbuf[256];
    int         sel;
    char        *endptr;
    char        *realend;
    char        *alias;
    long        id;
    WORD        cmd;

    switch( msg ) {
    case WM_INITDIALOG:
        cur = CurHdl->data;
        if( (char *)lparam != NULL ) {
            SetWindowText( hwnd, (char *)lparam );
        }
        SendDlgItemMessage( hwnd, ALIAS_TEXT, EM_LIMITTEXT, 20, 0 );
        while( cur != NULL ) {
            sprintf( buf, "0x%08X", cur->id );
            SendDlgItemMessage( hwnd, ALIAS_ID_LIST, LB_ADDSTRING, 0, (LPARAM)buf );
            cur = cur->next;
        }
        break;
#ifndef NOUSE3D
    case WM_SYSCOLORCHANGE:
        CvrCtl3dColorChange();
        break;
#endif
    case WM_COMMAND:
        cmd = LOWORD( wparam );
        switch( cmd ) {
            case IDOK:
            case ALIAS_DO_MORE:
                SendDlgItemMessage( hwnd, ALIAS_CUR_ID, WM_GETTEXT, CONST_LEN, (LPARAM)buf );
                realend = buf;
                while( *realend != '\0' ) {
                    realend++;
                }
                realend--;
                while( isspace( *realend ) ) {
                    realend--;
                }
                realend++;
                id = strtol( buf, &endptr, 0 );
                if( endptr != realend || *buf == '\0' ) {
                    RCMessageBox( hwnd, ALIAS_VALUE_MUST_BE_INT, "", MB_OK );
                    break;
                }
                len = SendDlgItemMessage( hwnd, ALIAS_TEXT, WM_GETTEXTLENGTH, 0, 0 );
                alias = MemAlloc( len + 1 );
                len = SendDlgItemMessage( hwnd, ALIAS_TEXT, WM_GETTEXT, len + 1, (LPARAM)alias );
                /* check for spaces */
                endptr = alias;
                while( !isspace( *endptr ) && *endptr != '\0' ) {
                    endptr++;
                }
                realend = endptr;
                while( isspace( *endptr ) ) {
                    endptr++;
                }
                if( *endptr != '\0' ) {
                    RCMessageBox( hwnd, ALIAS_NO_SPACES_ALLOWED, "", MB_OK );
                    MemFree( alias );
                    break;
                }
                realend = '\0'; /* truncate trailing spaces */
                cur = findAliasFromText( CurHdl, alias );
                if( cur == NULL ) {
                    AddAlias( CurHdl, alias, id );
                } else {
                    RCsprintf( msgbuf, ALIAS_NO_DUPLICATES_ALLOWED, alias, (DWORD)cur->id );
                    MessageBox( hwnd, msgbuf, "", MB_OK );
                    MemFree( alias );
                    break;
                }
                MemFree( alias );
                EndDialog( hwnd, cmd );
                break;
            case IDCANCEL:
                EndDialog( hwnd, cmd );
                break;
            case ALIAS_ID_LIST:
                if( GET_WM_COMMAND_CMD( wparam, lparam ) == LBN_SELCHANGE ) {
                    sel = (int)SendDlgItemMessage( hwnd, ALIAS_ID_LIST, LB_GETCURSEL, 0, 0L );
                    SendDlgItemMessage( hwnd, ALIAS_ID_LIST, LB_GETTEXT, sel, (LPARAM)buf );
                    SendDlgItemMessage( hwnd, ALIAS_CUR_ID, WM_SETTEXT, 0, (LPARAM)buf );
                    cur = getIthAlias( CurHdl, sel );
                    SendDlgItemMessage( hwnd, ALIAS_TEXT, WM_SETTEXT, 0, (LPARAM)cur->name );
                }
                break;
            default:
                return( FALSE );
        }
    default:
        return( FALSE );
    }
    return( TRUE );
}

/*
 * Query4Aliases - display the alias list dialog box
 */
void Query4Aliases( AliasHdl hdl, HANDLE instance, HWND hwnd, char *title )
{
    FARPROC     fp;
    INT_PTR     ret;

    CurHdl = hdl;
    fp = MakeDlgProcInstance( AliasDlgProc, instance );
    for( ;; ) {
        ret = DialogBoxParam( instance, "ALIAS_DLG", hwnd, (DLGPROC)fp, (LPARAM)title );
        if( ret != ALIAS_DO_MORE ) {
            break;
        }
    }
    FreeProcInstance( fp );
    CurHdl = NULL;

} /* Query4Aliases */

/*
 * EnumAliases - enumerate all aliases in a given alias list
 */
void EnumAliases( AliasHdl hdl, void (*enumfn)( unsigned long, char *, void * ), void *userdata )
{
    AnAlias     *cur;

    cur = hdl->data;
    while( cur != NULL ) {
        enumfn( cur->id, cur->name, userdata );
        cur = cur->next;
    }
    enumfn( (DWORD)-1, NULL, userdata );

} /* EnumAliases */
