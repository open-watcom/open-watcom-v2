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


#include "alloc.h"
#include <stddef.h>
#include <malloc.h>


extern HMMIO PASCAL __mmioOpen(LPSTR szFileName, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags);
extern UINT PASCAL __mmioClose(HMMIO hmmio, UINT flags);
extern LONG PASCAL __mmioRead(HMMIO hmmio, const char *pch, LONG cch);
extern LONG PASCAL __mmioWrite(HMMIO hmmio, HPSTR pch, LONG cch);


typedef struct mminfo_list {
    struct mminfo_list          *next;
    HMMIO                       handle;
    unsigned short              filler;
    DWORD                       size;
    DWORD                       alias;
} mminfo_list;
typedef mminfo_list *LPMMINFOLIST;

static LPMMINFOLIST     mminfoListHead;

/*
 * mmioOpen - cover for mmsystem function mmioOpen
 */
HMMIO PASCAL mmioOpen( LPSTR szFileName, LPMMIOINFO lpmmioinfo,
                        DWORD dwOpenFlags)
{
    HMMIO               rc;
    DWORD               alias;
    LPMMINFOLIST        curr;

    alias = NULL;
    if( !(dwOpenFlags & (MMIO_ALLOCBUF | MMIO_DELETE | MMIO_PARSE | MMIO_EXIST
                                | MMIO_GETTEMP) ) ) {
        if( lpmmioinfo->cchBuffer != 0 && lpmmioinfo->pchBuffer != NULL ) {
            DPMIGetHugeAlias( (DWORD) lpmmioinfo->pchBuffer, &alias,
                                lpmmioinfo->cchBuffer );
            lpmmioinfo->pchBuffer = (LPVOID) alias;
        }
    }

    rc = __mmioOpen( szFileName, lpmmioinfo, dwOpenFlags );

    if( rc != NULL && alias != NULL ) {
        curr = malloc( sizeof( mminfo_list ) );
        if( curr != NULL ) {
            curr->next = mminfoListHead;
            curr->handle = rc;
            curr->alias = alias;
            curr->size = lpmmioinfo->cchBuffer;
            mminfoListHead = curr;
        }
    }
    return( rc );

} /* mmioOpen */

/*
 * mmioClose - cover for mmsystem function mmioClose
 */
UINT PASCAL mmioClose( HMMIO hmmio, UINT flags )
{
    UINT                rc;
    LPMMINFOLIST        curr,prev;

    rc = __mmioClose( hmmio, flags );

    curr = mminfoListHead;
    prev = NULL;
    while( curr != NULL ) {
        if( curr->handle == hmmio ) {
            DPMIFreeHugeAlias( curr->alias, curr->size );
            if( prev == NULL ) {
                mminfoListHead = curr->next;
            } else {
                prev->next = curr->next;
            }
            free( curr );
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    return( rc );

} /* mmioOpen */

/*
 * mmioRead - cover for mmsystem function mmioRead
 */
LONG PASCAL mmioRead( HMMIO hmmio, HPSTR pch, LONG cch )
{
    LONG        rc;
    DWORD       alias;

    DPMIGetHugeAlias( (DWORD) pch, &alias, cch );
    rc = __mmioRead( hmmio, (HPSTR) alias, cch );
    DPMIFreeHugeAlias( alias, cch );
    return( rc );

} /* mmioRead */

/*
 * mmioWrite - cover for mmsystem function mmioWrite
 */
LONG PASCAL mmioWrite( HMMIO hmmio, const char *pch, LONG cch )
{
    LONG        rc;
    DWORD       alias;

    DPMIGetHugeAlias( (DWORD) pch, &alias, cch );
    rc = __mmioWrite( hmmio, (HPSTR) alias, cch );
    DPMIFreeHugeAlias( alias, cch );
    return( rc );

} /* mmioWrite */
