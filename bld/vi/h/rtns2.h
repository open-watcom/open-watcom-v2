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
void PositionScrollThumb( void );

/* biosos2.c */
void MyVioShowBuf( unsigned short, unsigned short );

/* biosqnx.c */
int KeyboardInit( void );
void KeyboardFini( void );
void KeyboardRestore( void );
void RestoreKeyboard( void );
void StopKeyboard( void );
void WaitForProxy( void );

/* qnxmouse.c */
void StopMouse( void );

/* int.c */
void SetInterrupts( void );
void RestoreInterrupts( void );

/* dosdos.c */
void MyDelay( int );
void ClearScreen( void );
long MemSize( void );
void MyBeep( void );
int SetCWD( char * );
void KillCursor( void );
void TurnOffCursor( void );
void RestoreCursor( void );
void NewCursor( window_id, cursor_type );
void SetCursorBlinkRate( int );
void SetCursorOnScreen( int, int );
int PixelFromColumnOnCurrentLine( int );
int GetNumColors( void );
bool GetColorSetting( int clr, rgb *c  );
void ScreenInit( void );
void ScreenFini( void );
void ChkExtendedKbd( void );
bool KeyboardHit( void );
int GetKeyboard( int * );
int SetAColor( char * );
int SetFont( char * );
void InitColors( void );
void ResetColors( void );
void FiniColors( void );
void SetColorPalette( char * );
void GetColorPalette( char * );
void SetColorRegister( int, rgb * );
void GetBlockOfColorRegisters( int, int, rgb * );
void ScreenPage( int );
void GetClockStart( void );
void GetSpinStart( void );
int SameFile( char *, char * );
int ChangeDrive( int );
void PushDirectory( char * );
void PopDirectory( void );
bool ShiftDown( void );
bool CtrlDown( void );
bool AltDown( void );
void TurnOffCapsLock( void );
drive_type DoGetDriveType( int );
//void FreeReserved( void );
//void GrabMinMem( void );
/* xmem.c */
void ExtendedMemoryInit( void );
void ExtendedMemoryFini( void );
