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


/* win\eddrvr.c */
extern void         PositionScrollThumb( void );

/* biosos2.c */
extern void         MyVioShowBuf( unsigned, unsigned );

/* biosqnx.c */
extern vi_rc        KeyboardInit( void );
extern void         KeyboardFini( void );
extern void         KeyboardRestore( void );
extern void         RestoreKeyboard( void );
extern void         StopKeyboard( void );
extern void         WaitForProxy( void );

/* qnxmouse.c */
extern void         StopMouse( void );

/* int.c */
extern void         SetInterrupts( void );
extern void         RestoreInterrupts( void );

/* dosdos.c */
extern void         MyDelay( int );
extern void         ClearScreen( void );
extern long         MemSize( void );
extern void         MyBeep( void );
extern vi_rc        SetCWD( const char * );
extern void         KillCursor( void );
extern void         TurnOffCursor( void );
extern void         RestoreCursor( void );
extern void         NewCursor( window_id, cursor_type );
extern void         SetCursorBlinkRate( int );
extern int          PixelFromColumnOnCurrentLine( int );
extern int          GetNumColors( void );
extern bool         GetColorSetting( vi_color clr, rgb *c  );
extern void         ScreenInit( void );
extern void         ScreenFini( void );
extern void         ChkExtendedKbd( void );
extern bool         KeyboardHit( void );
extern vi_key       GetKeyboard( void );
extern vi_rc        SetAColor( const char * );
extern vi_rc        SetFont( const char * );
extern void         InitColors( void );
extern void         ResetColors( void );
extern void         FiniColors( void );
extern void         ScreenPage( int );
extern void         GetClockStart( void );
extern void         GetSpinStart( void );
extern bool         SameFile( const char *, const char * );
extern vi_rc        ChangeDrive( int );
extern void         PushDirectory( const char * );
extern void         PopDirectory( void );
extern bool         ShiftDown( void );
extern bool         CtrlDown( void );
extern bool         AltDown( void );
extern void         TurnOffCapsLock( void );
extern drive_type   DoGetDriveType( int );
//void FreeReserved( void );
//void GrabMinMem( void );
extern void         SetPosToMessageLine( void );
extern void         HideCursor( void );

/* xmem.c */
extern void         ExtendedMemoryInit( void );
extern void         ExtendedMemoryFini( void );
