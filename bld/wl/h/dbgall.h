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


extern char * SymFileName;

extern void     DBIInit( void );
extern void     DBISectInit( section * );
extern void     DBIInitModule( mod_entry * );
extern void     DBIP1Source( byte *, byte * );
extern section *DBIGetSect( char * );
extern void     DBIColClass( class_entry * );
extern unsigned_16      DBIColSeg( class_entry * );
extern void     DBIP1ModuleScanned( void );
extern bool     DBISkip( seg_leader * );
extern bool     DBINoReloc( seg_leader * );
extern void     DBIPreAddrCalc( void );
extern void     DBIComment( void );
extern void     DBIAddModule( mod_entry *, section * );
extern void     DBIGenModule( void );
extern void     DBIDefClass( class_entry *, unsigned_32 );
extern void     DBIAddLocal( seg_leader *, offset );
extern void     DBIAddGlobal( symbol * );
extern void     DBIGenGlobal( symbol *, section * );
extern void     DBIAddLines( segdata *, void *, size_t, bool );
extern void     DBIAddrStart( void );
extern void     DBIAddrSectStart( section * );
extern void     DBIP2Start( section * );
extern void     DBIFini( section * );
extern void     DBISectCleanup( section * );
extern void     DBICleanup( void );
extern void     DBIWrite( void );
extern void     ResetDBI( void );
