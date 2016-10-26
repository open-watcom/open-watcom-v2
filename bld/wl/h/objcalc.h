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
* Description:  Header file for salloc.c, objcalc.c and autogrp.c
*
****************************************************************************/


extern int          NumGroups;

/* in autogrp.c */

extern void             AutoGroup( void );
extern group_entry      *AllocGroup( char *, group_entry ** );

/* in salloc.c */

extern void     NormalizeAddr( void );
extern void     AddSize( offset );
extern offset   CAlign( offset, unsigned );
extern void     Align( byte );
extern void     MAlign( byte );
extern void     StartMemMap( void );
extern void     ChkLocated( targ_addr *segadr, bool fixed);
extern void     NewSegment( seg_leader * );

/* in objcalc.c */

extern void     CheckClassOrder( void );
extern bool     IsCodeClass( char *, unsigned );
extern bool     IsConstClass( char *, unsigned );
extern bool     IsStackClass( char *, unsigned );
extern void     CalcAddresses( void );
extern void     AllocClasses( section * );
extern void     CombineSeg( seg_leader *, signed_32 *, offset, byte, byte );
extern void     ProcPubs( mod_entry *, section * );
extern void     StartMapSort( void );
extern void     FinishMapSort( void );
extern void     DoPubs( section * );
extern void     SetSegFlags( seg_flags * );
extern void     CalcSegSizes( void );
extern void     ConvertToFrame( targ_addr *, segment, bool );
extern void     FiniMap( void );
