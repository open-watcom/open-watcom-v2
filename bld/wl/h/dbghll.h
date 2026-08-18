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
* Description:  routines for producing HLL debug information in the load file
*
****************************************************************************/


extern void     HllInit( void );
extern void     HllInitModule( mod_entry * );
extern void     HllP1ModuleScanned( void );
extern void     HllP1ModuleFinished( mod_entry * );
extern void     HllAddModule( mod_entry *, section * );
extern void     HllGenModule( void );
extern void     HllAddLocal( seg_leader *, offset );
extern void     HllAddGlobal( symbol * );
extern void     HllGenGlobal( symbol *, section * );
extern void     HllGenLines( lineinfo * );
extern void     HllAddAddrInfo( seg_leader * );
extern void     HllGenAddrInfo( seg_leader * );
extern void     HllAddrStart( void );
extern void     HllDefClass( class_entry *, unsigned_32 );
extern void     HllFini( section * );
extern void     HllWrite( void );

//extern unsigned_32      HllSize;- loadpe.c?
