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
* Description:  Linker utility routine prototypes.
*
****************************************************************************/


extern void             CheckErr( void );
extern void             CheckStop( void );
extern void             LnkFatal( char * );
extern void             ClearBit( byte *, unsigned );
extern bool             TestBit( byte *, unsigned );
extern char             *ChkStrDup( char * );
extern char             *ChkToString( void *, unsigned );
extern seg_leader       *FindSegment( section *, char * );
extern group_entry      *FindGroup( segment );
extern offset           FindLinearAddr( targ_addr * );
extern offset           FindLinearAddr2( targ_addr * );
extern void             WalkLeaders( void (*rtn)( seg_leader * ) );
extern void             SectWalkClass( section *sect, void * );
extern void             WalkMods( void (*rtn)( mod_entry * ) );
extern void             LinkList( void *, void * );
extern void             FreeList( void * );
extern int              Spawn( void (*)( void ) );
extern void             Suicide( void );
extern f_handle         SearchPath( char * );
extern name_list        *AddNameTable( char *, unsigned, bool, name_list ** );
extern unsigned_16      binary_log( unsigned_16 );
extern unsigned_16      blog_32( unsigned_32 );
extern char             *RemovePath( char *, unsigned * );
extern void             WriteNulls( f_handle, unsigned_32, char * );
extern void             VMemQSort( virt_mem, unsigned, unsigned,
                                    void (*swapfn)(virt_mem, virt_mem),
                                    int (*cmpfn)(virt_mem, virt_mem) );
