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


#include <string.h>
#include "wresall.h"
#include "reserr.h"
#include "wresdefn.h"
#include "wresrtns.h"


static bool readLangInfoList( FILE *fp, WResResNode *res, void *fileinfo )
{
    unsigned            i;
    WResLangNode        *langnode;
    size_t              numread;

    for( i = 0; i < res->Info.NumResources; i++ ) {
        langnode = WRESALLOC( sizeof( WResLangNode ) );
        if( langnode == NULL )
            return( WRES_ERROR( WRS_MALLOC_FAILED ) );
        if( (numread = WRESREAD( fp, &(langnode->Info), sizeof( WResLangInfo ) )) != sizeof( WResLangInfo ) ) {
            WRESFREE( langnode );
            return( WRES_ERROR( WRESIOERR( fp, numread ) ? WRS_READ_FAILED : WRS_READ_INCOMPLETE ) );
        }
        langnode->data = NULL;
        langnode->fileInfo = fileinfo;
        ResAddLLItemAtEnd( (void **)&(res->Head), (void **)&(res->Tail), langnode );
    }
    return( false );
}

static bool readResList( FILE *fp, WResTypeNode *currtype, uint_16 ver, void *fileinfo )
{
    WResResNode     *newnode = NULL;
    WResResInfo     tmpresinfo;
    WResResInfo1    tmpresinfo1;
    WResLangNode    *langnode;
    bool            error;
    int             resnum;
    int             extrabytes;

    extrabytes = 0;
    error = false;
    /* loop through the list of resources of this type */
    for( resnum = 0; resnum < currtype->Info.NumResources && !error; resnum++ ) {

        /* read a resource record from disk */
        if( ver < 2 ) {
            error = WResReadFixedResRecord1( &tmpresinfo1, fp );
            tmpresinfo.NumResources = 1;
            tmpresinfo.ResName.IsName = tmpresinfo1.ResName.IsName;
            if( tmpresinfo.ResName.IsName ) {
                tmpresinfo.ResName.ID.Name.Name[0] = tmpresinfo1.ResName.ID.Name.Name[0];
                tmpresinfo.ResName.ID.Name.NumChars = tmpresinfo1.ResName.ID.Name.NumChars;
            } else {
                tmpresinfo.ResName.ID.Num = tmpresinfo1.ResName.ID.Num;
            }
        } else if( ver == 2 ) {
            error = WResReadFixedResRecord2( &tmpresinfo, fp );
        } else {
            error = WResReadFixedResRecord( &tmpresinfo, fp );
        }

        if( !error ) {
            /* allocate a new node */
            extrabytes = WResIDExtraBytes( &tmpresinfo.ResName );
            newnode = WRESALLOC( sizeof( WResResNode ) + extrabytes );
            if( newnode == NULL ) {
                error = WRES_ERROR( WRS_MALLOC_FAILED );
            }
        }
        if( !error ) {
            newnode->Head = NULL;
            newnode->Tail = NULL;
            /* copy the new resource info into the new node */
            memcpy( &(newnode->Info), &tmpresinfo, sizeof( WResResInfo ) );

            /* read the extra bytes (if any) */
            if( extrabytes > 0 ) {
                error = WResReadExtraWResID( &(newnode->Info.ResName), fp );
            }

            if( ver < 2 ) {
                langnode = WRESALLOC( sizeof( WResLangNode ) );
                if( langnode == NULL )
                    error = WRES_ERROR( WRS_MALLOC_FAILED );
                if( !error ) {
                    langnode->data = NULL;
                    langnode->fileInfo = fileinfo;
                    langnode->Info.MemoryFlags = tmpresinfo1.MemoryFlags;
                    langnode->Info.Offset = tmpresinfo1.Offset;
                    langnode->Info.Length = tmpresinfo1.Length;
                    langnode->Info.lang.lang = DEF_LANG;
                    langnode->Info.lang.sublang = DEF_SUBLANG;
                    ResAddLLItemAtEnd( (void **)&(newnode->Head), (void **)&(newnode->Tail), langnode );
                }
            } else if( ver == 2 ) {
                error = readLangInfoList( fp, newnode, fileinfo );
            } else {
                error = readLangInfoList( fp, newnode, fileinfo );
            }
        }
        if( !error ) {
            /* add the resource node to the linked list */
            ResAddLLItemAtEnd( (void **)&(currtype->Head), (void **)&(currtype->Tail), newnode );
        }
    }

    return( error );

} /* readResList */

static bool readTypeList( FILE *fp, WResDirHead *dir, uint_16 ver, void *fileinfo )
{
    WResTypeNode    *newnode;
    WResTypeInfo    newtype;
    bool            error;
    int             typenum;
    int             extrabytes;

    newnode = NULL;
    extrabytes = 0;
    error = false;
    /* loop through the list of types */
    for( typenum = 0; typenum < dir->NumTypes && !error; typenum++ ) {
        /* read a type record from disk */
        if( ver < 3 ) {
            error = WResReadFixedTypeRecord1or2( &newtype, fp );
        } else {
            error = WResReadFixedTypeRecord( &newtype, fp );
        }
        if( !error ) {
            /* allocate a new node */
            extrabytes = WResIDExtraBytes( &(newtype.TypeName) );
            newnode = WRESALLOC( sizeof( WResTypeNode ) + extrabytes );
            if( newnode == NULL ) {
                error = WRES_ERROR( WRS_MALLOC_FAILED );
            }
        }
        if( !error ) {
            /* initialize the linked list of resources */
            newnode->Head = NULL;
            newnode->Tail = NULL;
            /* copy the new type info into the new node */
            memcpy( &(newnode->Info), &newtype, sizeof( WResTypeInfo ) );

            /* read the extra bytes (if any) */
            if( extrabytes > 0 ) {
                error = WResReadExtraWResID( &(newnode->Info.TypeName), fp );
            }
        }
        if( !error ) {
            /* add the type node to the linked list */
            ResAddLLItemAtEnd( (void **)&(dir->Head), (void **)&(dir->Tail), newnode );
            /* read in the list of resources of this type */
            error = readResList( fp, newnode, ver, fileinfo );
        }
    }

    return( error );

} /* readTypeList */

static bool readWResDir( FILE *fp, WResDir dir, void *fileinfo )
{
    WResHeader      header;
    WResExtHeader   extheader;
    bool            error;

    extheader.TargetOS = WRES_OS_WIN16;
    /* read the header and check that it is valid */
    error = WResReadHeader( &header, fp );
    if( !error ) {
        if( header.Magic[0] != WRESMAGIC0
          || header.Magic[1] != WRESMAGIC1 ) {
            error = WRES_ERROR( WRS_BAD_SIG );
        }
    }
    if( !error ) {
        if( header.WResVer > WRESVERSION ) {
            error = WRES_ERROR( WRS_BAD_VERSION );
        }
    }
    if( !error ) {
        if( header.WResVer >= 1 ) {
            /*
             * seek to the extended header and read it
             */
            if( WRESSEEK( fp, sizeof( header ), SEEK_CUR ) ) {
                error = WRES_ERROR( WRS_SEEK_FAILED );
            } else {
                error = WResReadExtHeader( &extheader, fp );
            }
        }
    }

    /* set up the initial info for the directory and seek to it's start */
    if( !error ) {
        dir->NumResources = header.NumResources;
        dir->NumTypes = header.NumTypes;
        dir->TargetOS = extheader.TargetOS;
        if( WRESSEEK( fp, header.DirOffset, SEEK_SET ) ) {
            error = WRES_ERROR( WRS_SEEK_FAILED );
        }
    }
    /* read in the list of types (and the resources) */
    if( !error ) {
        error = readTypeList( fp, dir, header.WResVer, fileinfo );
    }

    return( error );

} /* readWResDir */

static bool readMResDir( FILE *fp, WResDir dir, bool *dup_discarded,
                        bool iswin32, void *fileinfo )
/**********************************************************************/
{
    MResResourceHeader      *header = NULL;
    M32ResResourceHeader    *header32 = NULL;
    WResDirWindow           dup;
    bool                    error;
    WResID                  *name;
    WResID                  *type;

    error = false;
    if( iswin32 ) {
        /* Read NULL header */
        header32 = M32ResReadResourceHeader( fp );
        if( header32 != NULL ) {
            MResFreeResourceHeader( header32->head16 );
            WRESFREE( header32 );
        } else {
            error = true;
        }
        if( !error ) {
            header32 = M32ResReadResourceHeader( fp );
            if( header32 != NULL ) {
                header = header32->head16;
            } else {
                error = true;
            }
        }
    } else {
        header = MResReadResourceHeader( fp );
        if( header == NULL ) {
            error = true;
        }
    }
    if( dup_discarded != NULL  ) {
        *dup_discarded = false;
    }
    if( iswin32 ) {
        dir->TargetOS = WRES_OS_WIN32;
    } else {
        dir->TargetOS = WRES_OS_WIN16;
    }
    /* assume that a NULL header is the EOF which is the only way of detecting */
    /* the end of a MS .RES file */
    while( header != NULL && !( iswin32 && header32 == NULL ) && !error ) {
        name = WResIDFromNameOrOrdinal( header->Name );
        type = WResIDFromNameOrOrdinal( header->Type );
        error = (name == NULL || type == NULL);

        /* MResReadResourceHeader leaves the file at the start of the resource*/
        if( !error ) {
            if( type->IsName == 0
              && type->ID.Num == (uint_16)RESOURCE2INT( RT_NAMETABLE ) ) {
                error = false;
            } else {
                error = WResAddResource2( type, name, header->MemoryFlags,
                            WRESTELL( fp ), header->Size, dir, NULL,
                            &dup, fileinfo );
                if( error
                  && !WResIsEmptyWindow( dup ) ) {
                    error = false;
                    if( dup_discarded != NULL  ) {
                        *dup_discarded = true;
                    }
                }
            }
        }

        if( !error ) {
            if( WRESSEEK( fp, header->Size, SEEK_CUR ) ) {
                error = WRES_ERROR( WRS_SEEK_FAILED );
            }
        }

        if( name != NULL ) {
            WRESFREE( name );
            name = NULL;
        }
        if( type != NULL ) {
            WRESFREE( type );
            type = NULL;
        }
        MResFreeResourceHeader( header );
        if( iswin32 ) {
            WRESFREE( header32 );
        }

        if( !error ) {
            if( iswin32 ) {
                header32 = M32ResReadResourceHeader( fp );
                if( header32 != NULL ) {
                    header = header32->head16;
                }
            } else {
                header = MResReadResourceHeader( fp );
            }
        }
    }

    return( error );

} /* readMResDir */

bool WResReadDir( FILE *fp, WResDir *dir, bool *dup_discarded )
{
    return( WResReadDir2( fp, dir, dup_discarded, NULL ) );
}

bool WResReadDir2( FILE *fp, WResDir *pdir, bool *dup_discarded, void *fileinfo )
{
    bool            error;
    WResResType     res_type;
    WResDir         dir;

    /* var representing whether or not a duplicate dir entry was
     * discarded is set to false.
     * NOTE: duplicates are not discarded by calls to readWResDir.
     */
    if( dup_discarded != NULL  ) {
        *dup_discarded = false;
    }

    /* get rid of any directory info that is already in memory */
    dir = *pdir;
    if( dir->Head != NULL ) {
        __FreeTypeList( dir );
    }

    /* seek to the start of the file */
    if( WRESSEEK( fp, 0, SEEK_SET ) ) {
        return( WRES_ERROR( WRS_SEEK_FAILED ) );
    }
    res_type = WResReadResType( fp );
    if( res_type == RT_WATCOM ) {
        error = readWResDir( fp, dir, fileinfo );
    } else if( res_type == RT_WIN16 ) {
        error = readMResDir( fp, dir, dup_discarded, false, fileinfo );
    } else {
        error = readMResDir( fp, dir, dup_discarded, true, fileinfo );
    }
    return( error );

} /* WResReadDir */
