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


extern void     ODBIInit( section * );
extern void     ODBIInitModule( mod_entry * );
extern void     ODBIP1Source( byte, byte, char *, int );
extern void     ODBIP1ModuleScanned( void );
extern void     ODBIP1ModuleFinished( mod_entry * );
extern void     ODBIAddModule( mod_entry *, section * );
extern void     ODBIGenModule( void );
extern void     ODBIAddFakeModule( unsigned );
extern void     ODBIGenFakeModule( char * );
extern void     ODBIDefClass( class_entry *, unsigned_32 );
extern void     ODBIAddLocal( unsigned_16, offset );
extern void     ODBIGenLocal( segdata * );
extern void     ODBIAddGlobal( symbol * );
extern void     ODBIGenGlobal( symbol *, section * );
extern void     ODBIGenLines( segdata *, void *, unsigned, bool );
extern void     ODBIGenLines( segdata *, void *, unsigned, bool );
extern void     ODBIAddAddrInfo( seg_leader * );
extern void     ODBIAddrSectStart( section * );
extern void     ODBIP2Start( section * );
extern void     ODBISectCleanup( section * );
extern void     ODBIFini( section * );
extern void     OWriteDBI( void );
extern void     WriteDBISecs( section * );
