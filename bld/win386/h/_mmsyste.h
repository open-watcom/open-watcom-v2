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
* Description:  Cover functions for mmsystem.dll.
*
****************************************************************************/


extern void     GetMIDIHDRAlias( LPSTR _FAR *orig, LPMIDIHDR ptr );
extern void     ReleaseMIDIHDRAlias( LPSTR orig, LPMIDIHDR ptr );
extern void     GetWAVEHDRAlias( LPSTR _FAR *orig, LPWAVEHDR ptr );
extern void     ReleaseWAVEHDRAlias( LPSTR orig, LPWAVEHDR ptr );
extern UINT     FAR PASCAL __midiInAddBuffer( HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr, UINT uSize );
extern UINT     FAR PASCAL __midiInPrepareHeader( HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr, UINT uSize );
extern UINT     FAR PASCAL __midiInUnprepareHeader( HMIDIIN hMidiIn, LPMIDIHDR lpMidiInHdr, UINT uSize );
extern UINT     FAR PASCAL __midiOutLongMsg( HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize );
extern UINT     FAR PASCAL __midiOutPrepareHeader( HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize );
extern UINT     FAR PASCAL __midiOutUnprepareHeader( HMIDIOUT hMidiOut, LPMIDIHDR lpMidiOutHdr, UINT uSize);
extern UINT     FAR PASCAL __mmioAdvance( HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags );
extern UINT     FAR PASCAL __mmioGetInfo( HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags );
extern HMMIO    FAR PASCAL __mmioOpen( LPSTR szFileName, LPMMIOINFO lpmmioinfo, DWORD dwOpenFlags);
extern UINT     FAR PASCAL __mmioClose( HMMIO hmmio, UINT flags );
extern UINT     FAR PASCAL __mmioRename( LPCSTR szFileName, LPCSTR szNewFileName, LPMMIOINFO lpmmioinfo, DWORD dwRenameFlags );
extern UINT     FAR PASCAL __mmioSetInfo( HMMIO hmmio, LPMMIOINFO lpmmioinfo, UINT uFlags );
extern LONG     FAR PASCAL __mmioRead( HMMIO hmmio, HPSTR pch, LONG cch );
extern LONG     FAR PASCAL __mmioWrite( HMMIO hmmio, HPSTR pch, LONG cch );
extern UINT     FAR PASCAL __waveInAddBuffer( HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr, UINT uSize );
extern UINT     FAR PASCAL __waveInPrepareHeader( HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr, UINT uSize );
extern UINT     FAR PASCAL __waveInUnprepareHeader( HWAVEIN hWaveIn, LPWAVEHDR lpWaveInHdr, UINT uSize );
extern UINT     FAR PASCAL __waveOutPrepareHeader( HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr, UINT uSize );
extern UINT     FAR PASCAL __waveOutUnprepareHeader( HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr, UINT uSize );
extern UINT     FAR PASCAL __waveOutWrite( HWAVEOUT hWaveOut, LPWAVEHDR lpWaveOutHdr, UINT uSize );
