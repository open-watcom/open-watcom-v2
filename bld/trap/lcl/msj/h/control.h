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


#ifdef __cplusplus
extern "C" {
#endif

extern bool             InitProc( void );
extern void             FiniProc( void );
extern bool             StartProc( char *, char * );
extern void             EndProc( void );
extern void             ResumeProc( addr48_ptr *pc );
extern void             TraceProc( addr48_ptr *pc );
extern void             SignalDeath( void );
extern void             DoInterrupt( void );
extern void             TraceOn( void );
extern void             TraceOff( void );
extern void             SetBreakpoint( addr48_ptr * );
extern void             ClearBreakpoint( addr48_ptr * );
extern unsigned         ReadFlags( void );
extern void             ProcessQueuedRepaints( void );
extern char *           GetThreadName( unsigned_32 );
extern unsigned_32      GetNextThread( unsigned_32, unsigned_8 * );
extern unsigned_32      SetThread( unsigned_32 );
extern unsigned         GetLibName( unsigned, char * );
extern HANDLE           FakeOpen( char* );
extern bool             FakeRead( HANDLE, void*, unsigned, unsigned* );
extern void             *GetMethodPointer( addr48_off * );
extern addr48_off       MakeMethodOffset( void *, void *, addr48_off );
extern unsigned         DipCue( int i, void *buff );
extern unsigned         DipFileName( int i, void *buff );
extern unsigned         DipModName( int i, void *buff );
extern unsigned         DipModBase( int i, void *buff );
extern unsigned         DipModEnd( int i, void *buff );
extern unsigned         MadUpStack( addr48_ptr *pc, addr48_ptr *newpc );

extern HANDLE           FakeHandle;

#ifdef __cplusplus
}
#endif
