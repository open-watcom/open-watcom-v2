/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


#include "global.h"
#include "rcerrors.h"
#include "autodep.h"
#include "reserr.h"
#include "wresdefn.h"
#include "rcrtns.h"
#include "rccore.h"

#include "clibext.h"


#if !defined( INSIDE_WLINK ) || defined( _OS2 )
#define MAX_OPEN_RESFILES       6

typedef struct {
    bool        used;
    WResStatus  status;
    int         errnum;
} ErrFrame;

static ErrFrame         errFromWres;

static void closeAResFile( ResFileInfo *res )
/*******************************************/
{
    if( res->IsOpen ) {
        ResCloseFile( res->fp );
        res->IsOpen = false;
    }
    if( res->Dir != NULL ) {
        WResFreeDir( res->Dir );
        res->Dir = NULL;
    }
    RESFREE( res );
}

bool OpenResFiles( ExtraRes *resnames, ResFileInfo **resinfo, bool *allopen,
                  ExeType type, const char *exename )
/**************************************************************************/
{
    unsigned        rescnt;
    ResFileInfo     *resfile;
    WResID          *res_name;
    WResID          *res_type;
    bool            error;
    bool            dup_discarded;
    WResTargetOS    res_os;

    *allopen = true;
    *resinfo = NULL;
    rescnt = 0;
    for( ; resnames != NULL; resnames = resnames->next ) {
        resfile = RESALLOC( sizeof( ResFileInfo ) );
        resfile->next = *resinfo;
        *resinfo = resfile;
        resfile->Dir = WResInitDir();
        resfile->name = resnames->name;
        resfile->fp = ResOpenFileRO( resfile->name );
        if( resfile->fp == NULL ) {
            RcError( ERR_CANT_OPEN_FILE, resfile->name, LastWresErrStr() );
            resfile->IsOpen = false;
            goto HANDLE_ERROR;
        } else {
            resfile->IsOpen = true;
        }
        error = WResReadDir2( resfile->fp, resfile->Dir, &dup_discarded, resfile );
        if( error ) {
            switch( LastWresStatus() ) {
            case WRS_BAD_SIG:
                RcError( ERR_INVALID_RES, resfile->name );
                break;
            case WRS_BAD_VERSION:
                RcError( ERR_BAD_RES_VER, resfile->name );
                break;
            default:
                RcError( ERR_READING_RES, resfile->name, LastWresErrStr() );
                break;
            }
            goto HANDLE_ERROR;
        }
        if( rescnt >= MAX_OPEN_RESFILES ) {
            resfile->IsOpen = false;
            ResCloseFile( resfile->fp );
            resfile->fp = NULL;
            *allopen = false;
        }

        // remove the autodepend resource
        res_name = WResIDFromStr( DEP_LIST_NAME );
        res_type = WResIDFromNum( DEP_LIST_TYPE );
        WResDelResource( resfile->Dir, res_type, res_name );
        WResIDFree( res_name );
        WResIDFree( res_type );

        res_os = WResGetTargetOS( resfile->Dir );
        switch( type ) {
        case EXE_TYPE_NE_WIN:
            if( res_os != WRES_OS_WIN16 ) {
                RcError( ERR_NONWIN_RES_TO_WIN_EXE, resfile->name, exename );
                goto HANDLE_ERROR;
            }
            break;
        case EXE_TYPE_NE_OS2:
            // No way to tell MS and IBM resource files apart, and I can't find
            // a good way to figure out if this is a Watcom .res file
            if( res_os != WRES_OS_WIN16 && res_os != WRES_OS_OS2 ) {
                RcError( ERR_NONWIN_RES_TO_WIN_EXE, resfile->name, exename );
                goto HANDLE_ERROR;
            }
            break;
        case EXE_TYPE_PE:
            if( res_os != WRES_OS_WIN32 ) {
                RcError( ERR_NONNT_RES_TO_NT_EXE, resfile->name, exename );
                goto HANDLE_ERROR;
            }
            break;
        case EXE_TYPE_LX:
            // Same problem as with EXE_TYPE_NE_OS2
            if( res_os != WRES_OS_OS2 && res_os != WRES_OS_WIN16 ) {
                RcError( ERR_NONOS2_RES_TO_OS2_EXE, resfile->name, exename );
                goto HANDLE_ERROR;
            }
            break;
        default: // EXE_TYPE_UNKNOWN
            break;
        }
        rescnt++;
    }
    return( true );

HANDLE_ERROR:
    CloseResFiles( *resinfo );
    *resinfo = NULL;
    return( false );
}

void CloseResFiles( ResFileInfo *resfiles )
/*****************************************/
{
    ResFileInfo         *res;

    while( (res = resfiles) != NULL ) {
        resfiles = res->next;
        closeAResFile( res );
    }
}

bool WresRecordError( WResStatus status )
/***************************************/
{
    errFromWres.used = true;
    errFromWres.status = status;
    errFromWres.errnum = errno;
    return( true );
}

char *LastWresErrStr( void )
/**************************/
{
    if( errFromWres.used ) {
        switch( errFromWres.status ) {
        case WRS_READ_INCOMPLETE:
            return( "Unexpected end of file" );
        default:
            return( strerror( errFromWres.errnum ) );
        }
    } else {
        return( "" );
    }
}

int LastWresErr( void )
/*********************/
{
    if( errFromWres.used ) {
        return( errFromWres.errnum );
    }
    return( 0 );
}

int LastWresStatus( void )
/************************/
{
    if( errFromWres.used ) {
        return( errFromWres.status );
    }
    return( 0 );
}

void SharedIOInitStatics( void )
/******************************/
{
    memset( &errFromWres, 0, sizeof( ErrFrame ) );
}

void ReportDupResource( WResID *nameid, WResID *typeid, const char *file1,
                           const char *file2, bool warn )
/*******************************************************************/
{
    char        *type;
    char        *name;
    char        namebuf[20];
    char        typebuf[20];
    unsigned    strbase;

    if( typeid->IsName ) {
        type = WResIDToStr( typeid );
    } else {
        switch( typeid->ID.Num ) {
        case RESOURCE2INT( RT_CURSOR ):
        case RESOURCE2INT( RT_GROUP_CURSOR ):
            type = "cursor";
            break;
        case RESOURCE2INT( RT_BITMAP ):
            type = "bitmap";
            break;
        case RESOURCE2INT( RT_ICON ):
        case RESOURCE2INT( RT_GROUP_ICON ):
            type = "icon";
            break;
        case RESOURCE2INT( RT_MENU ):
            type = "menu";
            break;
        case RESOURCE2INT( RT_DIALOG ):
            type = "dialog";
            break;
        case RESOURCE2INT( RT_FONTDIR ):
            type = "font directory";
            break;
        case RESOURCE2INT( RT_FONT ):
            type = "font";
            break;
        case RESOURCE2INT( RT_ACCELERATOR ):
            type = "accelerator";
            break;
        case RESOURCE2INT( RT_RCDATA ):
            type = "rcdata";
            break;
        case RESOURCE2INT( RT_VERSIONINFO ):
            type = "version info";
            break;
        case RESOURCE2INT( RT_MESSAGETABLE ):
            type = "message table";
            break;
        case RESOURCE2INT( RT_TOOLBAR ):
            type = "toolbar";
            break;
        default:
            type = typebuf;
            utoa( typeid->ID.Num, type, 10 );
            break;
        }
    }

    if( nameid->IsName ) {
        name = WResIDToStr( nameid );
    } else {
        name = namebuf;
        utoa( nameid->ID.Num, name, 10 );
    }
    if( !typeid->IsName && typeid->ID.Num == RESOURCE2INT( RT_STRING ) ) {
        strbase = ( nameid->ID.Num - 1 ) * 16;
        if( file1 != NULL && file2 != NULL ) {
            if( warn ) {
                RcWarning( ERR_DUPLICATE_STR_TABLE_FILE,
                                strbase, strbase + 15, file1, file2 );
            } else {
                RcError( ERR_DUPLICATE_STR_TABLE_FILE,
                                strbase, strbase + 15, file1, file2 );
            }
        } else {
            if( warn ) {
                RcWarning( ERR_DUPLICATE_STR_TABLE, strbase, strbase + 15 );
            } else {
                RcError( ERR_DUPLICATE_STR_TABLE, strbase, strbase + 15 );
            }
        }
    } else if( file1 != NULL && file2 != NULL ) {
        if( warn ) {
            RcWarning( ERR_DUPLICATE_RES_FILE, type, name, file1, file2 );
        } else {
            RcError( ERR_DUPLICATE_RES_FILE, type, name, file1, file2 );
        }
    } else {
        if( warn ) {
            RcWarning( ERR_DUPLICATE_RES, type, name );
        } else {
            RcError( ERR_DUPLICATE_RES, type, name );
        }
    }
    if( nameid->IsName ) {
        RESFREE( name );
    }
    if( typeid->IsName ) {
        RESFREE( type );
    }
}
#endif
