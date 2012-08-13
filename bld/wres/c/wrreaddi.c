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


#include <string.h>
#include "wresrtns.h"
#include "wres.h"
#include "layer1.h"
#include "mem2.h"
#include "util.h"
#include "reserr.h"


static int readLangInfoList( WResFileID handle, WResResNode *res,
                             void *fileinfo ) {

    unsigned            i;
    WResLangNode        *langnode;
    int                 error;
    int                 numread;

    error = FALSE;
    for( i=0; i < res->Info.NumResources; i++ ) {
        langnode = WRESALLOC( sizeof(WResLangNode) );
        if( langnode == NULL ) {
            error = TRUE;
            WRES_ERROR( WRS_MALLOC_FAILED );
        }
        if( error ) break;
        numread = (* WRESREAD) ( handle, &(langnode->Info),
                                 sizeof( WResLangInfo ) );
        if( numread != sizeof( WResLangInfo ) ) {
            error = TRUE;
            WRES_ERROR( numread == -1 ? WRS_READ_FAILED:WRS_READ_INCOMPLETE );
            WRESFREE( langnode );
            break;
        }
        langnode->data = NULL;
        langnode->fileInfo = fileinfo;
        ResAddLLItemAtEnd( (void **)&(res->Head), (void **)&(res->Tail), langnode );
    }
    return( error );
}

static int readResList( WResFileID handle, WResTypeNode * currtype,
                        uint_16 ver, void *fileinfo )
{
    WResResNode    *newnode;
    WResResInfo     newres;
    WResResInfo1    newres1;
    WResLangNode   *langnode;
    WResID         *resid;
    WResID          tmpresid;
    int             error;
    int             resnum;
    int             extrabytes;

    /* loop through the list of resources of this type */
    for (resnum = 0, error = FALSE; resnum < currtype->Info.NumResources &&
            !error; resnum++) {

        /* read a resource record from disk */
        if( ver < 2 ) {
            error = WResReadFixedResRecord1( &newres1, handle );
            resid = &tmpresid;
            tmpresid.IsName = newres1.ResName.IsName;
            if( tmpresid.IsName ) {
                tmpresid.ID.Name.Name[0] = newres1.ResName.ID.Name.Name[0];
                tmpresid.ID.Name.NumChars = newres1.ResName.ID.Name.NumChars;
            } else {
                tmpresid.ID.Num = newres1.ResName.ID.Num;
            }
        } else if( ver == 2 ) {
            error = WResReadFixedResRecord2( &newres, handle );
            resid = &( newres.ResName );
        } else {
            error = WResReadFixedResRecord( &newres, handle );
            resid = &( newres.ResName );
        }

        if( !error ) {
            /* allocate a new node */
            extrabytes = WResIDExtraBytes( resid );
            newnode = WRESALLOC( sizeof(WResResNode) + extrabytes );
            if( newnode == NULL ) {
                error = TRUE;
                WRES_ERROR( WRS_MALLOC_FAILED );
            }
        }
        if( !error ) {
            newnode->Head = NULL;
            newnode->Tail = NULL;
            /* copy the new resource info into the new node */
            if( ver < 2 ) {
                newnode->Info.NumResources = 1;
                memcpy( &(newnode->Info.ResName), &( newres1.ResName ),
                        sizeof( WResID ) );
            } else {
                memcpy( &(newnode->Info), &newres, sizeof(WResResInfo) );
            }

            /* read the extra bytes (if any) */
            if( extrabytes > 0 ) {
                error = WResReadExtraWResID( &(newnode->Info.ResName), handle );
            }

            if( ver < 2 ) {
                langnode = WRESALLOC( sizeof(WResLangNode) );
                if( langnode == NULL ) {
                    error =  TRUE;
                    WRES_ERROR( WRS_MALLOC_FAILED );
                }
                if( !error ) {
                    langnode->data = NULL;
                    langnode->fileInfo = fileinfo;
                    langnode->Info.MemoryFlags = newres1.MemoryFlags;
                    langnode->Info.Offset = newres1.Offset;
                    langnode->Info.Length = newres1.Length;
                    langnode->Info.lang.lang = DEF_LANG;
                    langnode->Info.lang.sublang = DEF_SUBLANG;
                    ResAddLLItemAtEnd( (void **)&(newnode->Head), (void **)&(newnode->Tail),
                                       langnode );
                }
            } else {
                error = readLangInfoList( handle, newnode, fileinfo );
            }
        }
        if( !error ) {
            /* add the resource node to the linked list */
            ResAddLLItemAtEnd( (void **)&(currtype->Head), (void **)&(currtype->Tail), newnode );
        }
    }

    return( error );

} /* readResList */

static int readTypeList( WResFileID handle, WResDirHead * currdir,
                         uint_16 ver, void *fileinfo )
{
    WResTypeNode *  newnode;
    WResTypeInfo    newtype;
    int             error;
    int             typenum;
    int             extrabytes;

    /* loop through the list of types */
    for (error = FALSE, typenum = 0; typenum < currdir->NumTypes && !error;
                    typenum++) {
        /* read a type record from disk */
        if( ver < 3 ) {
            error = WResReadFixedTypeRecord2( &newtype, handle );
        } else {
            error = WResReadFixedTypeRecord( &newtype, handle );
        }
        if( !error ) {
            /* allocate a new node */
            extrabytes = WResIDExtraBytes( &(newtype.TypeName) );
            newnode = WRESALLOC( sizeof(WResTypeNode) + extrabytes );
            if( newnode == NULL ) {
                error = TRUE;
                WRES_ERROR( WRS_MALLOC_FAILED );
            }
        }
        if( !error ) {
            /* initialize the linked list of resources */
            newnode->Head = NULL;
            newnode->Tail = NULL;
            /* copy the new type info into the new node */
            memcpy( &(newnode->Info), &newtype, sizeof(WResTypeInfo) );

            /* read the extra bytes (if any) */
            if( extrabytes > 0 ) {
                error = WResReadExtraWResID( &(newnode->Info.TypeName),
                        handle );
            }
        }
        if( !error ) {
            /* add the type node to the linked list */
            ResAddLLItemAtEnd( (void **)&(currdir->Head), (void **)&(currdir->Tail), newnode );
            /* read in the list of resources of this type */
            error = readResList( handle, newnode, ver, fileinfo );
        }
    }

    return( error );

} /* readTypeList */

static int readWResDir( WResFileID handle, WResDir currdir, void *fileinfo )
{
    WResHeader      head;
    WResExtHeader   ext_head;
    int             error;
    off_t           seekpos;

    /* read the header and check that it is valid */
    error = WResReadHeaderRecord( &head, handle );
    if( !error ) {
        if( head.Magic[0] != WRESMAGIC0 || head.Magic[1] != WRESMAGIC1 ) {
            error = TRUE;
            WRES_ERROR( WRS_BAD_SIG );
        }
    }
    if( !error ) {
        if( head.WResVer > WRESVERSION ) {
            error = TRUE;
            WRES_ERROR( WRS_BAD_VERSION );
        }
    }
    if( !error ) {
        if( head.WResVer >= 1 ) {
            /*
             * seek to the extended header and read it
             */
            seekpos = WRESSEEK( handle, sizeof( head ), SEEK_CUR );
            error = (seekpos == -1L);
            if( error ) {
                WRES_ERROR( WRS_SEEK_FAILED );
            } else {
                error = WResReadExtHeader( &ext_head, handle );
            }
        } else {
            ext_head.TargetOS = WRES_OS_WIN16;
        }
    }

    /* set up the initial info for the directory and seek to it's start */
    if( !error ) {
        currdir->NumResources = head.NumResources;
        currdir->NumTypes = head.NumTypes;
        currdir->TargetOS = ext_head.TargetOS;
        seekpos = (* WRESSEEK) ( handle, head.DirOffset, SEEK_SET );
        if( seekpos == -1L ) {
            error = TRUE;
            WRES_ERROR( WRS_SEEK_FAILED );
        }
    }
    /* read in the list of types (and the resources) */
    if( !error ) {
        error = readTypeList( handle, currdir, head.WResVer, fileinfo );
    }

    return( error );

} /* readWResDir */

static int readMResDir( WResFileID handle, WResDir currdir, int *dup_discarded,
                        char iswin32, void *fileinfo )
/******************************************************************************/
{
    MResResourceHeader     *head = NULL;
    M32ResResourceHeader   *head32 = NULL;
    WResDirWindow           dup;
    int                     error;
    off_t                   seek_rc;
    WResID                 *name;
    WResID                 *type;

    error = FALSE;
    if( iswin32 ) {
        /* Read NULL header */
        head32 = M32ResReadResourceHeader( handle );
        if( head32 != NULL ) {
            MResFreeResourceHeader( head32->head16 );
            WRESFREE( head32 );
        } else {
            error = TRUE;
        }
        if( !error ) {
            head32 = M32ResReadResourceHeader( handle );
            if( head32 != NULL ) {
                head = head32->head16;
            } else {
                error = TRUE;
            }
        }
    } else {
        head = MResReadResourceHeader( handle );
        if( head == NULL ) error = TRUE;
    }
    if(  dup_discarded != NULL  ) {
        *dup_discarded = FALSE;
    }
    if( iswin32 ) {
        currdir->TargetOS = WRES_OS_WIN32;
    } else {
        currdir->TargetOS = WRES_OS_WIN16;
    }
    /* assume that a NULL head is the EOF which is the only way of detecting */
    /* the end of a MS .RES file */
    while( head != NULL && !( iswin32 && head32 == NULL ) && !error ) {
        name = WResIDFromNameOrOrd( head->Name );
        type = WResIDFromNameOrOrd( head->Type );
        error = (name == NULL || type == NULL);

        /* MResReadResourceHeader leaves the file at the start of the resource*/
        if( !error ) {
            if( !type->IsName && type->ID.Num == RT_NAMETABLE ) {
                error = FALSE;
            } else {
                error = WResAddResource2( type, name, head->MemoryFlags,
                            WRESTELL( handle ), head->Size, currdir, NULL,
                            &dup, fileinfo );
                if(  error && !WResIsEmptyWindow( dup ) ) {
                    error = FALSE;
                    if(  dup_discarded != NULL  ) {
                        *dup_discarded = TRUE;
                    }
                }
            }
        }

        if( !error ) {
            seek_rc = WRESSEEK( handle, head->Size, SEEK_CUR );
            if( seek_rc == -1L ) {
                error =  TRUE;
                WRES_ERROR( WRS_SEEK_FAILED );
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
        MResFreeResourceHeader( head );
        if( iswin32 ) {
            WRESFREE( head32 );
        }

        if( !error ) {
            if( iswin32 ) {
                head32 = M32ResReadResourceHeader( handle );
                if( head32 != NULL ) {
                    head = head32->head16;
                }
            } else {
                head = MResReadResourceHeader( handle );
            }
        }
    }

    return( error );

} /* readMResDir */

int WResReadDir( WResFileID handle, WResDir currdir,
                 int * dup_discarded )
{
    return( WResReadDir2( handle, currdir, dup_discarded, NULL ) );
}

int WResReadDir2( WResFileID handle, WResDir currdir,
                 int *dup_discarded, void *fileinfo )
{
    int           error;
    off_t         seekpos;
    ResTypeInfo   restype;

    /* var representing whether or not a duplicate dir entry was
     * discarded is set to FALSE.
     * NOTE: duplicates are not discarded by calls to readWResDir.
     */
    error = FALSE;
    if(  dup_discarded != NULL  ) {
        *dup_discarded = FALSE;
    }

    /* get rid of any directory info that is already in memory */
    if( currdir->Head != NULL ) {
        __FreeTypeList( currdir );
    }

    /* seek to the start of the file */
    seekpos = WRESSEEK( handle, 0, SEEK_SET );
    if( seekpos == -1L ) {
        error = TRUE;
        WRES_ERROR( WRS_SEEK_FAILED );
    }

    if( !error ) {
        restype = WResFindResType( handle );
        if( restype == RT_WATCOM ) {
            error = readWResDir( handle, currdir, fileinfo );
        } else if( restype == RT_WIN16 ) {
            error = readMResDir( handle, currdir, dup_discarded, FALSE,
                                 fileinfo );
        } else {
            error = readMResDir( handle, currdir, dup_discarded, TRUE,
                                 fileinfo );
        }
    }

    return( error );

} /* WResReadDir */
