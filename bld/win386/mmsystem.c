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


#include <windows.h>
#include <mmsystem.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <dos.h>
#include "winext.h"
#include "stubs.h"

#pragma aux BackPatch_mmsystem parm [ax];
extern LPVOID FAR BackPatch_mmsystem( char *str );

static UINT (FAR PASCAL *mmsystemmidiInAddBuffer)(HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemmidiInPrepareHeader)(HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemmidiInUnprepareHeader)(HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemmidiOutLongMsg)(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemmidiOutPrepareHeader)(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemmidiOutUnprepareHeader)(HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemmmioAdvance)(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags);
static UINT (FAR PASCAL *mmsystemmmioGetInfo)(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags);
static HMMIO (FAR PASCAL *mmsystemmmioOpen)(LPSTR szFileName, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags);
static UINT (FAR PASCAL *mmsystemmmioClose)(HMMIO hmmio, UINT flags);
static UINT (FAR PASCAL *mmsystemmmioRename)(LPCSTR szFileName, LPCSTR szNewFileName, LPMMIOINFO lpmmioinfo, DWORD dwRenameFlags);
static UINT (FAR PASCAL *mmsystemmmioSetInfo)(HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags);
static LONG (FAR PASCAL *mmsystemmmioRead)(HMMIO hmmio, HPSTR pch, LONG cch);
static LONG (FAR PASCAL *mmsystemmmioWrite)(HMMIO hmmio, HPSTR pch, LONG cch);
static UINT (FAR PASCAL *mmsystemwaveInAddBuffer)(HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemwaveInPrepareHeader)(HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemwaveInUnprepareHeader)(HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemwaveOutPrepareHeader)(HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemwaveOutUnprepareHeader)(HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr, UINT uSize);
static UINT (FAR PASCAL *mmsystemwaveOutWrite)(HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr, UINT uSize);


typedef struct mminfo_list {
    struct mminfo_list          far *next;
    HMMIO                       handle;
    DWORD                       size;
    DWORD                       alias;
} mminfo_list;
typedef mminfo_list far *LPMMINFOLIST;

static LPMMINFOLIST     mminfoListHead;
static LPMMINFOLIST     mminfoListTail;

/*
 * GetMIDIHDRAlias - alias members of MIDIHDR struct
 */
static void GetMIDIHDRAlias( LPSTR _FAR *orig, LPMIDIHDR ptr )
{
    LPSTR       new;

    *orig = ptr->lpData;
    new = ptr->lpData;
    GetAlias( &new );
    ptr->lpData = new;

} /* GetMIDIHDRAlias */


/*
 * ReleaseMIDIHDRAlias - free aliases of members of MIDIHDR struct
 */
void ReleaseMIDIHDRAlias( LPSTR orig, LPMIDIHDR ptr )
{
    ReleaseAlias( orig, ptr->lpData );
    ptr->lpData = orig;

} /* ReleaseMIDIHDRAlias */

/*
 * GetWAVEHDRAlias - alias members of WAVEHDR struct
 */
static void GetWAVEHDRAlias( LPSTR _FAR *orig, LPWAVEHDR ptr )
{
    LPSTR       new;

    *orig = ptr->lpData;
    new = ptr->lpData;
    GetAlias( &new );
    ptr->lpData = new;

} /* GetWAVEHDRAlias */


/*
 * ReleaseWAVEHDRAlias - free aliases of members of WAVEHDR struct
 */
void ReleaseWAVEHDRAlias( LPSTR orig, LPWAVEHDR ptr )
{
    ReleaseAlias( orig, ptr->lpData );
    ptr->lpData = orig;

} /* ReleaseWAVEHDRAlias */


/*
 * __midiInAddBuffer - cover for mmsystem function midiInAddBuffer
 */
UINT FAR PASCAL __midiInAddBuffer( HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemmidiInAddBuffer == NULL ) {
        mmsystemmidiInAddBuffer = BackPatch_mmsystem( "midiInAddBuffer" );
        if( mmsystemmidiInAddBuffer == NULL ) return( 0 );
    }
    GetMIDIHDRAlias( &odata, lpMidiInHdr );
    rc = mmsystemmidiInAddBuffer( hMidiIn, lpMidiInHdr, uSize );
    ReleaseMIDIHDRAlias( odata, lpMidiInHdr );
    return( rc );

} /* __midiInAddBuffer */

/*
 * __midiInPrepareHeader - cover for mmsystem function midiInPrepareHeader
 */
UINT FAR PASCAL __midiInPrepareHeader( HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemmidiInPrepareHeader == NULL ) {
        mmsystemmidiInPrepareHeader = BackPatch_mmsystem( "midiInPrepareHeader" );
        if( mmsystemmidiInPrepareHeader == NULL ) return( 0 );
    }
    GetMIDIHDRAlias( &odata, lpMidiInHdr );
    rc = mmsystemmidiInPrepareHeader( hMidiIn, lpMidiInHdr, uSize );
    ReleaseMIDIHDRAlias( odata, lpMidiInHdr );
    return( rc );

} /* __midiInPrepareHeader */

/*
 * __midiInUnprepareHeader - cover for mmsystem function midiInUnprepareHeader
 */
UINT FAR PASCAL __midiInUnprepareHeader( HMIDIIN hMidiIn,
                        LPMIDIHDR lpMidiInHdr, UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemmidiInUnprepareHeader == NULL ) {
        mmsystemmidiInUnprepareHeader = BackPatch_mmsystem( "midiInUnprepareHeader" );
        if( mmsystemmidiInUnprepareHeader == NULL ) return( 0 );
    }
    GetMIDIHDRAlias( &odata, lpMidiInHdr );
    rc = mmsystemmidiInUnprepareHeader( hMidiIn, lpMidiInHdr, uSize );
    ReleaseMIDIHDRAlias( odata, lpMidiInHdr );
    return( rc );

} /* __midiInUnprepareHeader */

/*
 * __midiOutLongMsg - cover for mmsystem function midiOutLongMsg
 */
UINT FAR PASCAL __midiOutLongMsg( HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemmidiOutLongMsg == NULL ) {
        mmsystemmidiOutLongMsg = BackPatch_mmsystem( "midiOutLongMsg" );
        if( mmsystemmidiOutLongMsg == NULL ) return( 0 );
    }
    GetMIDIHDRAlias( &odata, lpMidiOutHdr );
    rc = mmsystemmidiOutLongMsg( hMidiOut, lpMidiOutHdr, uSize );
    ReleaseMIDIHDRAlias( odata, lpMidiOutHdr );
    return( rc );

} /* __midiOutLongMsg */

/*
 * __midiOutPrepareHeader - cover for mmsystem function midiOutPrepareHeader
 */
UINT FAR PASCAL __midiOutPrepareHeader( HMIDIOUT hMidiOut,
                        LPMIDIHDR lpMidiOutHdr, UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemmidiOutPrepareHeader == NULL ) {
        mmsystemmidiOutPrepareHeader = BackPatch_mmsystem( "midiOutPrepareHeader" );
        if( mmsystemmidiOutPrepareHeader == NULL ) return( 0 );
    }
    GetMIDIHDRAlias( &odata, lpMidiOutHdr );
    rc = mmsystemmidiOutPrepareHeader( hMidiOut, lpMidiOutHdr, uSize );
    ReleaseMIDIHDRAlias( odata, lpMidiOutHdr );
    return( rc );

} /* __midiOutPrepareHeader */

/*
 * __midiOutUnprepareHeader - cover for mmsystem function midiOutUnprepareHeader
 */
UINT FAR PASCAL __midiOutUnprepareHeader( HMIDIOUT hMidiOut,
                        LPMIDIHDR lpMidiOutHdr, UINT uSize)
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemmidiOutUnprepareHeader == NULL ) {
        mmsystemmidiOutUnprepareHeader = BackPatch_mmsystem( "midiOutUnprepareHeader" );
        if( mmsystemmidiOutUnprepareHeader == NULL ) return( 0 );
    }
    GetMIDIHDRAlias( &odata, lpMidiOutHdr );
    rc = mmsystemmidiOutUnprepareHeader( hMidiOut, lpMidiOutHdr, uSize );
    ReleaseMIDIHDRAlias( odata, lpMidiOutHdr );
    return( rc );

} /* __midiOutUnprepareHeader */

/*
 * __mmioAdvance - cover for mmsystem function mmioAdvance
 */
UINT FAR PASCAL __mmioAdvance( HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags )
{
    UINT        rc;

    if( mmsystemmmioAdvance == NULL ) {
        mmsystemmmioAdvance = BackPatch_mmsystem( "mmioAdvance" );
        if( mmsystemmmioAdvance == NULL ) return( 0 );
    }
    rc = mmsystemmmioAdvance( hmmio, lpmmioinfo, uFlags );
    return( rc );

} /* __mmioAdvance */

/*
 * __mmioGetInfo - cover for mmsystem function mmioGetInfo
 */
UINT FAR PASCAL __mmioGetInfo( HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags )
{
    UINT        rc;

    if( mmsystemmmioGetInfo == NULL ) {
        mmsystemmmioGetInfo = BackPatch_mmsystem( "mmioGetInfo" );
        if( mmsystemmmioGetInfo == NULL ) return( 0 );
    }
    rc = mmsystemmmioGetInfo( hmmio, lpmmioinfo, uFlags );
    return( rc );

} /* __mmioGetInfo */

/*
 * __mmioOpen - cover for mmsystem function mmioOpen
 */
HMMIO FAR PASCAL __mmioOpen( LPSTR szFileName, LPMMIOINFO lpmmioinfo,
                        DWORD dwOpenFlags)
{
    HMMIO               rc;
    DWORD               alias;
    LPMMINFOLIST        curr;

    if( mmsystemmmioOpen == NULL ) {
        mmsystemmmioOpen = BackPatch_mmsystem( "mmioOpen" );
        if( mmsystemmmioOpen == NULL ) return( 0 );
    }

    alias = NULL;
    if( !(dwOpenFlags & (MMIO_ALLOCBUF | MMIO_DELETE | MMIO_PARSE | MMIO_EXIST
                                | MMIO_GETTEMP) ) ) {
        if( lpmmioinfo->cchBuffer != 0 && lpmmioinfo->pchBuffer != NULL ) {
            DPMIGetHugeAlias( (DWORD) lpmmioinfo->pchBuffer, &alias,
                                lpmmioinfo->cchBuffer );
            lpmmioinfo->pchBuffer = (LPVOID) alias;
        }
    }

    rc = mmsystemmmioOpen( szFileName, lpmmioinfo, dwOpenFlags );

    if( rc != NULL && alias != NULL ) {
        curr = _fmalloc( sizeof( mminfo_list ) );
        if( curr != NULL ) {
            if( mminfoListHead == NULL ) {
                mminfoListHead = mminfoListTail = curr;
            } else {
                mminfoListTail->next = curr;
                mminfoListTail = curr;
            }
            curr->next = NULL;
            curr->handle = rc;
            curr->alias = alias;
            curr->size = lpmmioinfo->cchBuffer;
        }
    }
    return( rc );

} /* __mmioOpen */

/*
 * __mmioClose - cover for mmsystem function mmioClose
 */
UINT FAR PASCAL __mmioClose( HMMIO hmmio, UINT flags )
{
    UINT                rc;
    LPMMINFOLIST        curr,prev;

    if( mmsystemmmioClose == NULL ) {
        mmsystemmmioClose = BackPatch_mmsystem( "mmioClose" );
        if( mmsystemmmioClose == NULL ) {
            return( 0 );
        }
    }

    rc = mmsystemmmioClose( hmmio, flags );

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
            if( curr == mminfoListTail ) {
                mminfoListTail = prev;
            }
            _ffree( curr );
            break;
        }
        prev = curr;
        curr = curr->next;
    }
    return( rc );

} /* __mmioOpen */

/*
 * __mmioRename - cover for mmsystem function mmioRename
 */
UINT FAR PASCAL __mmioRename( LPCSTR szFileName, LPCSTR szNewFileName,
                        LPMMIOINFO lpmmioinfo, DWORD dwRenameFlags )
{
    UINT        rc;
    if( mmsystemmmioRename == NULL ) {
        mmsystemmmioRename = BackPatch_mmsystem( "mmioRename" );
        if( mmsystemmmioRename == NULL ) return( 0 );
    }
    rc = mmsystemmmioRename( szFileName, szNewFileName, lpmmioinfo, dwRenameFlags );
    return( rc );

} /* __mmioRename */

/*
 * __mmioSetInfo - cover for mmsystem function mmioSetInfo
 */
UINT FAR PASCAL __mmioSetInfo( HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags )
{
    UINT        rc;
    if( mmsystemmmioSetInfo == NULL ) {
        mmsystemmmioSetInfo = BackPatch_mmsystem( "mmioSetInfo" );
        if( mmsystemmmioSetInfo == NULL ) return( 0 );
    }
    rc = mmsystemmmioSetInfo( hmmio, lpmmioinfo, uFlags );
    return( rc );

} /* __mmioSetInfo */

/*
 * __mmioRead - cover for mmsystem function mmioRead
 */
LONG FAR PASCAL __mmioRead( HMMIO hmmio, HPSTR pch, LONG cch )
{
    LONG        rc;
    DWORD       alias;

    if( mmsystemmmioRead == NULL ) {
        mmsystemmmioRead = BackPatch_mmsystem( "mmioRead" );
        if( mmsystemmmioRead == NULL ) return( 0 );
    }
    DPMIGetHugeAlias( (DWORD) pch, &alias, cch );
    rc = mmsystemmmioRead( hmmio, (HPSTR) alias, cch );
    DPMIFreeHugeAlias( alias, cch );
    return( rc );

} /* __mmioRead */

/*
 * __mmioWrite - cover for mmsystem function mmioWrite
 */
LONG FAR PASCAL __mmioWrite( HMMIO hmmio, HPSTR pch, LONG cch )
{
    LONG        rc;
    DWORD       alias;

    if( mmsystemmmioWrite == NULL ) {
        mmsystemmmioWrite = BackPatch_mmsystem( "mmioWrite" );
        if( mmsystemmmioWrite == NULL ) return( 0 );
    }
    DPMIGetHugeAlias( (DWORD) pch, &alias, cch );
    rc = mmsystemmmioWrite( hmmio, (HPSTR) alias, cch );
    DPMIFreeHugeAlias( alias, cch );
    return( rc );

} /* __mmioWrite */

/*
 * __waveInAddBuffer - cover for mmsystem function waveInAddBuffer
 */
UINT FAR PASCAL __waveInAddBuffer( HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemwaveInAddBuffer == NULL ) {
        mmsystemwaveInAddBuffer = BackPatch_mmsystem( "waveInAddBuffer" );
        if( mmsystemwaveInAddBuffer == NULL ) return( 0 );
    }
    GetWAVEHDRAlias( &odata, lpWaveInHdr );
    rc = mmsystemwaveInAddBuffer( hWaveIn, lpWaveInHdr, uSize );
    ReleaseWAVEHDRAlias( odata, lpWaveInHdr );
    return( rc );

} /* __waveInAddBuffer */

/*
 * __waveInPrepareHeader - cover for mmsystem function waveInPrepareHeader
 */
UINT FAR PASCAL __waveInPrepareHeader( HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemwaveInPrepareHeader == NULL ) {
        mmsystemwaveInPrepareHeader = BackPatch_mmsystem( "waveInPrepareHeader" );
        if( mmsystemwaveInPrepareHeader == NULL ) return( 0 );
    }
    GetWAVEHDRAlias( &odata, lpWaveInHdr );
    rc = mmsystemwaveInPrepareHeader( hWaveIn, lpWaveInHdr, uSize );
    ReleaseWAVEHDRAlias( odata, lpWaveInHdr );
    return( rc );

} /* __waveInPrepareHeader */

/*
 * __waveInUnprepareHeader - cover for mmsystem function waveInUnprepareHeader
 */
UINT FAR PASCAL __waveInUnprepareHeader( HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemwaveInUnprepareHeader == NULL ) {
        mmsystemwaveInUnprepareHeader = BackPatch_mmsystem( "waveInUnprepareHeader" );
        if( mmsystemwaveInUnprepareHeader == NULL ) return( 0 );
    }
    GetWAVEHDRAlias( &odata, lpWaveInHdr );
    rc = mmsystemwaveInUnprepareHeader( hWaveIn, lpWaveInHdr, uSize );
    ReleaseWAVEHDRAlias( odata, lpWaveInHdr );
    return( rc );

} /* __waveInUnprepareHeader */

/*
 * __waveOutPrepareHeader - cover for mmsystem function waveOutPrepareHeader
 */
UINT FAR PASCAL __waveOutPrepareHeader( HWAVEOUT hWaveOut,
                        LPWAVEHDR lpWaveOutHdr, UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemwaveOutPrepareHeader == NULL ) {
        mmsystemwaveOutPrepareHeader = BackPatch_mmsystem( "waveOutPrepareHeader" );
        if( mmsystemwaveOutPrepareHeader == NULL ) return( 0 );
    }
    GetWAVEHDRAlias( &odata, lpWaveOutHdr );
    rc = mmsystemwaveOutPrepareHeader( hWaveOut, lpWaveOutHdr, uSize );
    ReleaseWAVEHDRAlias( odata, lpWaveOutHdr );
    return( rc );

} /* __waveOutPrepareHeader */

/*
 * __waveOutUnprepareHeader - cover for mmsystem function waveOutUnprepareHeader
 */
UINT FAR PASCAL __waveOutUnprepareHeader( HWAVEOUT hWaveOut,
                        LPWAVEHDR lpWaveOutHdr, UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemwaveOutUnprepareHeader == NULL ) {
        mmsystemwaveOutUnprepareHeader = BackPatch_mmsystem( "waveOutUnprepareHeader" );
        if( mmsystemwaveOutUnprepareHeader == NULL ) return( 0 );
    }
    GetWAVEHDRAlias( &odata, lpWaveOutHdr );
    rc = mmsystemwaveOutUnprepareHeader( hWaveOut, lpWaveOutHdr, uSize );
    ReleaseWAVEHDRAlias( odata, lpWaveOutHdr );
    return( rc );

} /* __waveOutUnprepareHeader */

/*
 * __waveOutWrite - cover for mmsystem function waveOutWrite
 */
UINT FAR PASCAL __waveOutWrite( HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr,
                        UINT uSize )
{
    UINT        rc;
    LPSTR       odata;

    if( mmsystemwaveOutWrite == NULL ) {
        mmsystemwaveOutWrite = BackPatch_mmsystem( "waveOutWrite" );
        if( mmsystemwaveOutWrite == NULL ) return( 0 );
    }
    GetWAVEHDRAlias( &odata, lpWaveOutHdr );
    rc = mmsystemwaveOutWrite( hWaveOut, lpWaveOutHdr, uSize );
    ReleaseWAVEHDRAlias( odata, lpWaveOutHdr );
    return( rc );

} /* __waveOutWrite */
