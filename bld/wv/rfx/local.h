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


typedef unsigned local_ret_t;

extern void LocalTime( int *hour, int *min, int *sec, int *hundredths );
extern void LocalDate( int *year, int *month, int *day, int *weekday );
extern int LocalInteractive( sys_handle );
extern void LocalGetBuff( char *, unsigned );
//extern local_ret_t LocalCreate( char *, bool );
//extern local_ret_t LocalOpen( char *, unsigned, bool );
//extern void LocalClose( handle );
//extern local_ret_t LocalRead( handle, void far *, unsigned );
//extern local_ret_t LocalWrite( handle, void *, unsigned );
//extern local_ret_t LocalErase( char * );
//extern local_ret_t LocalRename( char *, char * );
//extern local_ret_t LocalMkDir( char * );
//extern local_ret_t LocalRmDir( char * );
//extern local_ret_t LocalSetDrv( int );
//extern int LocalGetDrv( void );
//extern local_ret_t LocalSetCWD( char * );
//extern local_ret_t LocalGetFileAttr( char * );
//extern local_ret_t LocalGetFreeSpace( int );
//extern local_ret_t LocalDateTime( handle, int *, int *, int );
//extern local_ret_t LocalGetCwd( int, char * );
//extern local_ret_t LocalFindFirst( char *, void *, unsigned, int );
//extern local_ret_t LocalFindNext( void *, unsigned );
//extern local_ret_t LocalSetFileAttr( char *, long );
