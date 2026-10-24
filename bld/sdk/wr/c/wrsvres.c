/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2026 The Open Watcom Contributors. All Rights Reserved.
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


#include "wrglbl.h"
#include <io.h>
#include "wrsvres.h"
#include "wrmsg.h"
#include "wrdatai.h"
#include "wrinfoi.h"
#include "wresall.h"
#include "rcrtns.h"


/****************************************************************************/
/* macro definitions                                                        */
/****************************************************************************/

/****************************************************************************/
/* Here comes the code                                                      */
/****************************************************************************/

static void displayDupMsg( WResID *typeName, WResID *resName )
{
    char        *type;
    char        *name;

    type = WResIDToStr( typeName );
    name = WResIDToStr( resName );

    if( type != NULL && name != NULL ) {
        WRPrintErrorMsg( WR_DUPRESFOUND, type, name );
    }

    if( type != NULL ) {
        MemFree( type );
    }
    if( name != NULL ) {
        MemFree( name );
    }
}

static bool WRWriteResourceToWRES( WResTypeNode *typenode, WResResNode *resnode,
                                  WResDir new_dir, FILE *src_fp,
                                  FILE *dst_fp, bool is32bit )
{
    WResLangType    lang;
    WResLangNode    *langnode;
    uint_32         offset;
    bool            dup;

    if( is32bit ) {
        if( ResWritePadDWord( dst_fp ) ) {
            return( false );
        }
    }

    offset = RESTELL( dst_fp );
    for( langnode = resnode->Head; langnode != NULL; langnode = langnode->next ) {
        lang = langnode->Info.lang;
        if( WResAddResource( &typenode->Info.TypeName, &resnode->Info.ResName,
                             langnode->Info.res_flags, offset,
                             langnode->Info.Length, new_dir,
                             &lang, &dup ) || dup ) {
            if( dup ) {
                displayDupMsg( &typenode->Info.TypeName, &resnode->Info.ResName );
            }
            return( false );
        }

        if( langnode->data != NULL ) {
            if( !WRCopyResFromDataToFile( langnode->data, langnode->Info.Length, dst_fp ) ) {
                return( false );
            }
        } else {
            if( !WRCopyResFromFileToFile( src_fp, langnode->Info.Offset, langnode->Info.Length, dst_fp ) ) {
                return( false );
            }
        }
        if( langnode == resnode->Tail ) {
            break;
        }
        offset += langnode->Info.Length;
    }

    return( true );
}

static ResNameOrOrdinal *WRCreateMRESResName( WResResNode *resnode, WResLangNode *langnode )
{
    ResNameOrOrdinal    *name;
    char                *str;
    int                 len;

    if( resnode == NULL || langnode == NULL ) {
        return( NULL );
    }

    if( !langnode->Info.lang.lang && !langnode->Info.lang.sublang ) {
        name = WResIDToNameOrOrdinal( &resnode->Info.ResName );
    } else {
        if( resnode->Info.ResName.IsName ) {
            len = resnode->Info.ResName.ID.Name.NumChars;
            str = MemAlloc( len + ( 1 + UINT16STRLEN ) * 2 + 1 );
            if( str == NULL ) {
                return( NULL );
            }
            memcpy( str, resnode->Info.ResName.ID.Name.Name, len );
            sprintf( str + len, "_%u_%u",
                        langnode->Info.lang.lang, langnode->Info.lang.sublang );
        } else {
            str = MemAlloc( UINT16STRLEN + ( 1 + UINT16STRLEN ) * 2 + 1 );
            if( str == NULL ) {
                return( NULL );
            }
            sprintf( str, "%u_%u_%u", resnode->Info.ResName.ID.Num,
                        langnode->Info.lang.lang, langnode->Info.lang.sublang );
        }
        name = ResStrToNameOrOrdinal( str );
        MemFree( str );
    }

    return( name );
}

static bool WRWriteResourceToMRES( WResTypeNode *typenode, WResResNode *resnode,
                                  FILE *src_fp, FILE *dst_fp )
{
    WResLangNode        *langnode;
    MSResResourceHeader msheader;
    bool                ok;
    bool                iswin32;

    iswin32 = false;
    ok = true;
    for( langnode = resnode->Head; langnode != NULL && ok; langnode = langnode->next ) {
        msheader.Size = langnode->Info.Length;
        msheader.res_flags = langnode->Info.res_flags;
        msheader.Type = WResIDToNameOrOrdinal( &typenode->Info.TypeName );
        msheader.Name = WRCreateMRESResName( resnode, langnode );
        ok = (msheader.Type != NULL && msheader.Name != NULL);
        if( ok ) {
            if( iswin32 ) {
                ok = !MSResWriteResourceHeader32( &msheader, dst_fp );
            } else {
                ok = !MSResWriteResourceHeader( &msheader, dst_fp );
            }
        }
        if( ok ) {
            if( langnode->data != NULL ) {
                ok = WRCopyResFromDataToFile( langnode->data, langnode->Info.Length, dst_fp );
            } else {
                ok = WRCopyResFromFileToFile( src_fp, langnode->Info.Offset, langnode->Info.Length, dst_fp );
            }
        }
        if( msheader.Type != NULL ) {
            MemFree( msheader.Type );
        }
        if( msheader.Name != NULL ) {
            MemFree( msheader.Name );
        }
        if( langnode == resnode->Tail ) {
            break;
        }
    }

    return( ok );
}

static bool WRWriteResourcesToMRES( WRInfo *info, FILE *src_fp, FILE *dst_fp )
{
    WResDir             old_dir;
    WResTypeNode        *typenode;
    WResResNode         *resnode;

    old_dir = info->dir;

    if( old_dir == NULL ) {
        typenode = NULL;
    } else {
        typenode = old_dir->Head;
    }
    for( ; typenode != NULL; typenode = typenode->next ) {
        for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
            if( !WRWriteResourceToMRES( typenode, resnode, src_fp, dst_fp ) ) {
                return( false );
            }
            if( resnode == typenode->Tail ) {
                break;
            }
        }
        if( typenode == old_dir->Tail ) {
            break;
        }
    }

    return( true );
}

static bool WRWriteResourcesToWRES( WRInfo *info, WResDir new_dir,
                                   FILE *src_fp, FILE *dst_fp, bool is32bit )
{
    WResDir         old_dir;
    WResTypeNode    *typenode;
    WResResNode     *resnode;

    old_dir = info->dir;

    if( old_dir == NULL ) {
        typenode = NULL;
    } else {
        typenode = old_dir->Head;
    }
    for( ; typenode != NULL; typenode = typenode->next ) {
        for( resnode = typenode->Head; resnode != NULL; resnode = resnode->next ) {
            if( !WRWriteResourceToWRES( typenode, resnode, new_dir, src_fp, dst_fp, is32bit ) ) {
                return( false );
            }
            if( resnode == typenode->Tail ) {
                break;
            }
        }
        if( typenode == old_dir->Tail ) {
            break;
        }
    }

    return( true );
}

static bool WRSaveResourceToWRES( WRInfo *info, FILE *src_fp, FILE *dst_fp )
{
    WResDir     new_dir;
    WRFileType  save_type;
    bool        is32bit;
    bool        ok;

    ok = ((new_dir = WResInitDir()) != NULL);

    if( ok ) {
        save_type = info->save_type;
        if( save_type != WR_WIN16M_RES && save_type != WR_WIN16W_RES &&
            save_type != WR_WINNTM_RES && save_type != WR_WINNTW_RES ) {
            if( info->internal_type != WR_DONT_KNOW ) {
                save_type = info->internal_type;
            }
        }
        is32bit = WRIs32Bit( save_type );
        if( is32bit ) {
            new_dir->TargetOS = WRES_OS_WIN32;
        }
    }

    if( ok ) {
        ok = WRWriteResourcesToWRES( info, new_dir, src_fp, dst_fp, is32bit );
    }

    if( ok ) {
        ok = !WResWriteDir( dst_fp, new_dir );
    }

    if( new_dir != NULL ) {
        WResFreeDir( new_dir );
    }

    return( ok );
}

static bool WRSaveResourceToMRES( WRInfo *info, FILE *src_fp, FILE *dst_fp )
{
    return( WRWriteResourcesToMRES( info, src_fp, dst_fp ) );
}

static bool saveResourceToRES( WRInfo *info, bool backup, const char *save_name, const char *file_name )
{
    FILE        *src_fp;
    FILE        *dst_fp;
    bool        is_wres;
    bool        ok;
    bool        use_rename;
    WRFileType  save_type;

    src_fp = NULL;
    dst_fp = NULL;

    ok = true;

    if( ok ) {
        save_type = info->save_type;
        if( save_type != WR_WIN16M_RES && save_type != WR_WIN16W_RES &&
            save_type != WR_WINNTM_RES && save_type != WR_WINNTW_RES ) {
            if( info->internal_type != WR_DONT_KNOW ) {
                save_type = info->internal_type;
            }
        }
#ifndef __NT__
        ok = !WRIs32Bit( save_type );
        if( !ok ) {
            WRDisplayErrorMsg( WR_NOSAVE32IN16 );
        }
#endif
    }

    if( ok ) {
        if( backup && WRFileExists( save_name ) ) {
            use_rename = (file_name != NULL && stricmp( file_name, save_name ));
            ok = WRBackupFile( save_name, use_rename );
        }
    }

    if( ok ) {
        if( file_name != NULL ) {
            ok = ((src_fp = ResOpenFileRO( info->tmp_file )) != NULL);
        }
    }

    is_wres = (save_type == WR_WIN16W_RES || save_type == WR_WINNTW_RES);

    if( ok ) {
        ok = ((dst_fp = ResOpenFileNew( save_name )) != NULL);
        if( ok && is_wres ) {
            ok = !WResFileInit( dst_fp );
        }
    }

    if( ok ) {
        if( is_wres ) {
            ok = WRSaveResourceToWRES( info, src_fp, dst_fp );
        } else {
            ok = WRSaveResourceToMRES( info, src_fp, dst_fp );
        }
    }

    if( src_fp != NULL ) {
        ResCloseFile( src_fp );
    }

    if( dst_fp != NULL ) {
        ResCloseFile( dst_fp );
    }

    if( !ok ) {
        if( dst_fp != NULL ) {
            WRDeleteFile( save_name );
        }
    }

    return( ok );
}

bool WRSaveResourceTo_RC( WRInfo *info, bool backup )
{
    char                fn_path[_MAX_PATH];
    bool                ok;

    ok = (info != NULL && info->save_name != NULL);

    if( ok ) {
        WRGetInternalRESName( info->save_name, fn_path );
        // if the internal save type has not been set up yet
        // then we must take a crude guess
        if( info->internal_type == WR_DONT_KNOW ) {
            info->internal_type = WRSelectFileType( (HWND)NULL, fn_path,
#ifdef __NT__
                                                    true,
#else
                                                    false,
#endif
                                                    false, NULL );
        }
        ok = saveResourceToRES( info, backup, fn_path,
            info->internal_filename != NULL ? info->internal_filename : info->file_name );
    }

    return( ok );
}

bool WRSaveResourceToRES( WRInfo *info, bool backup )
{
    return( saveResourceToRES( info, backup, info->save_name, info->file_name ) );
}

bool WRCopyResFromFileToFile( FILE *src_fp, uint_32 offset, uint_32 length, FILE *dst_fp )
{
    char        *buf;
    bool        ok;

    buf = NULL;

    ok = (src_fp != NULL && dst_fp != NULL);

    ok = (ok && (buf = MemAlloc( CHUNK_SIZE )) != NULL);

    ok = ( ok && !RESSEEK( src_fp, offset, SEEK_SET ) );

    while( ok && length > CHUNK_SIZE ) {
        ok = ok && WRReadResData( src_fp, buf, CHUNK_SIZE );
        ok = ok && WRWriteResData( dst_fp, buf, CHUNK_SIZE );
        length -= CHUNK_SIZE;
    }
    ok = ok && WRReadResData( src_fp, buf, length );
    ok = ok && WRWriteResData( dst_fp, buf, length );

    if( buf != NULL ) {
        MemFree( buf );
    }

    return( ok );
}

bool WRCopyResFromDataToFile( void *data, uint_32 len, FILE *fp )
{
    return( WRWriteResData( fp, data, len ) );
}
