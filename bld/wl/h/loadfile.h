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



enum {
    START_UNDEFED,
    START_IS_SYM,
    START_IS_SDATA
};

typedef struct {
    union {
        symbol *        sym;
        segdata *       sdata;
    } targ;
    offset              off;
    mod_entry *         mod;
    targ_addr           addr;
    unsigned            type : 2;
    unsigned            user_specd : 1;
    unsigned            from_inc : 1;
} startinfo;

extern seg_leader *     StackSegPtr;
extern startinfo        StartInfo;

extern void             InitLoadFile( void );
extern void             FiniLoadFile( void );
extern void             CleanLoadFile( void );
extern void             GetStkAddr( void );
extern void             DefBSSSyms( void );
extern void             GetBSSSize( void );
extern void             SetStkSize( void );
extern void             GetStartAddr( void );
extern void             ClearStartAddr( void );
extern bool             CompareDosSegments( targ_addr *, targ_addr * );
extern bool             CompareOffsets( targ_addr *, targ_addr * );
extern bool             CompareProtSegments( targ_addr *, targ_addr * );
extern void             OrderGroups( bool (*)(targ_addr *, targ_addr *) );
extern bool             WriteDOSGroup( group_entry * );
extern unsigned_32      MemorySize( void );
extern unsigned_32      AppendToLoadFile( char * );
extern void             AddImpLibEntry( char *, char *, unsigned );
extern void             BuildImpLib( void );
extern void             SetStartSym( char * );
extern offset           CalcGroupSize( group_entry * );
extern offset           CalcSplitSize( void );

extern void             FreeOutFiles( void );
extern void             InitBuffFile( outfilelist *, char * );
extern void             OpenBuffFile( outfilelist * );
extern void             CloseBuffFile( outfilelist * );
extern void             WriteLoad3( void *, void *, unsigned );
extern void             WriteLoad( void *, unsigned long );
extern void             PadLoad( unsigned long );
extern void             SeekLoad( unsigned long );
extern void             SeekEndLoad( unsigned long );
extern unsigned long    PosLoad( void );
extern void             WriteLeaderLoad( seg_leader * );
extern void             WriteGroupLoad( group_entry * );
extern unsigned_32      CopyToLoad( f_handle, char * );
extern unsigned long    NullAlign( unsigned );
extern unsigned long    OffsetAlign( unsigned long, unsigned long );
