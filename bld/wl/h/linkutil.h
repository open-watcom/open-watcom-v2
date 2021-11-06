/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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


typedef void    class_walk_fn( seg_leader * );
typedef void    mods_walk_fn( mod_entry * );
typedef void    walksecs_fn( section * );
typedef void    parmwalksecs_fn( section *, void * );

extern void             WalkAllSects( walksecs_fn * );
extern void             ParmWalkAllSects( parmwalksecs_fn *, void * );
extern void             CheckErr( void );
extern void             CheckStop( void );
extern void             LnkFatal( const char * );
extern void             ClearBit( byte *, unsigned );
extern bool             TestBit( byte *, unsigned );
extern char             *ChkStrDup( const char * );
extern char             *ChkToString( const void *, size_t );
extern seg_leader       *FindSegment( section *, const char * );
extern group_entry      *FindGroup( segment );
extern offset           FindLinearAddr( targ_addr * );
extern offset           FindLinearAddr2( targ_addr * );
extern void             WalkLeaders( class_walk_fn *rtn );
extern void             SectWalkClass( section *sect, class_walk_fn * );
extern void             WalkMods( mods_walk_fn *rtn );
extern void             LinkList( void *, void * );
extern void             FreeList( void * );
extern int              Spawn( void (*)( void ) );
extern void             Suicide( void );
extern f_handle         FindPath( const char *, char *fullname );
extern obj_name_list    *AddNameTable( const char *, size_t, bool, obj_name_list ** );
extern unsigned_16      log2_16( unsigned_16 );
extern unsigned_16      log2_32( unsigned_32 );
extern const char       *GetBaseName( const char *, size_t, size_t * );
extern void             WriteNulls( f_handle, size_t, const char * );
extern void             VMemQSort( virt_mem, size_t, size_t,
                                    void (*swapfn)(virt_mem, virt_mem),
                                    int (*cmpfn)(virt_mem, virt_mem) );
extern void             InitEnvVars( void );
extern void             FiniEnvVars( void );
extern file_list        *AllocNewFile( member_list *member );
extern char             *FileName( const char *, size_t, file_defext, bool );
extern section          *NewSection( void );
