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

#include "variety.h"

typedef struct _REGISTRATION_RECORD {
    struct _REGISTRATION_RECORD *RegistrationRecordPrev;
    void                        *RegistrationRecordFilter;
} REGISTRATION_RECORD;

#define __EXCEPTION_RECORD struct _REGISTRATION_RECORD

_WCRTLINK void __NewExceptionHandler( REGISTRATION_RECORD *, int reg_unh );
_WCRTLINK void __DoneExceptionHandler( void );

struct dirent;

void __GetNTCreateAttr( int mode, LPDWORD desired_access, LPDWORD attr );
extern void __GetNTAccessAttr( int rwmode, LPDWORD desired_access, LPDWORD attr );
extern void __GetNTShareAttr( int share, LPDWORD share_mode );
extern void __MakeDOSDT( FILETIME *NT_stamp, unsigned short *d, unsigned short *t );
extern void __FromDOSDT( unsigned short d, unsigned short t, FILETIME *NT_stamp );
void __GetNTDirInfo( struct dirent *dirp, LPWIN32_FIND_DATA ffb );
void _w__GetNTDirInfo( struct _wdirent *dirp, LPWIN32_FIND_DATA ffb );
BOOL __NTFindNextFileWithAttr( HANDLE h, DWORD attr, LPWIN32_FIND_DATA ffb );
BOOL _w__NTFindNextFileWithAttr( HANDLE h, DWORD attr, LPWIN32_FIND_DATA ffb );
int __NTRealKey( INPUT_RECORD * );
HANDLE __NTConsoleInput( void );
HANDLE __NTConsoleOutput( void );

#define HANDLE_OF(dirp) ( *( HANDLE * )( &(((char *)(dirp))[0]) ) )
#define BAD_HANDLE      ((HANDLE)(~0))
#define ATTR_OF(dirp)  ( *( DWORD * )( &(((char *)(dirp))[4]) ) )
#endif
