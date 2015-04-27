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


#ifndef __NTEX_INCLUDED__
#define __NTEX_INCLUDED__

typedef struct _REGISTRATION_RECORD {
    struct _REGISTRATION_RECORD *RegistrationRecordPrev;
    void                        *RegistrationRecordFilter;
} REGISTRATION_RECORD;

#define __EXCEPTION_RECORD struct _REGISTRATION_RECORD

_WCRTLINK extern void __DefaultExceptionHandler( void );
_WCRTLINK extern void __NewExceptionFilter( REGISTRATION_RECORD * );
_WCRTLINK extern void __DoneExceptionFilter( void );

extern void __GetNTCreateAttr( int mode, LPDWORD desired_access, LPDWORD attr );
extern void __GetNTAccessAttr( int rwmode, LPDWORD desired_access, LPDWORD attr );
extern void __GetNTShareAttr( int share, LPDWORD share_mode );
extern void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t );
extern void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp );
extern int __NTRealKey( INPUT_RECORD * );
extern HANDLE __NTConsoleInput( void );
extern HANDLE __NTConsoleOutput( void );

extern void __GetNTDirInfoA( struct dirent *dirp, LPWIN32_FIND_DATAA ffb );
extern void __GetNTDirInfoW( struct _wdirent *dirp, LPWIN32_FIND_DATAW ffb );
extern BOOL __NTFindNextFileWithAttrA( HANDLE h, DWORD attr, LPWIN32_FIND_DATAA ffb );
extern BOOL __NTFindNextFileWithAttrW( HANDLE h, DWORD attr, LPWIN32_FIND_DATAW ffb );

#ifdef __WIDECHAR__
#define __GetNTDirInfo              __GetNTDirInfoW
#define __NTFindNextFileWithAttr    __NTFindNextFileWithAttrW
#else
#define __GetNTDirInfo              __GetNTDirInfoA
#define __NTFindNextFileWithAttr    __NTFindNextFileWithAttrA
#endif

#endif
