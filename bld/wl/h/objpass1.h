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


typedef struct {
        void *  (*getstart)( void );
        void    (*setstart)( void * );
        bool    (*isend)( void );
        char *  (*getname)( void );
} vflistrtns;

typedef struct cdat_piece {
    struct cdat_piece   *next;
    unsigned_8          *data;
    offset              length;
    unsigned            free_data : 1;
} comdat_piece;

typedef struct cdat_info {
    struct cdat_info *  next;
    segdata *           sdata;
    symbol *            sym;
    comdat_piece *      pieces;
    sym_info            flags;
} comdat_info;

extern void             P1Start( void );
extern unsigned long    IncPass1( void );
extern void             DoIncGroupDefs( void );
extern void             Set64BitMode( void );
extern void             Set32BitMode( void );
extern void             Set16BitMode( void );
extern void             AllocateSegment( segnode *, char *clname );
extern void             AddSegment( segdata *, class_entry * );
extern class_entry      *DuplicateClass( class_entry *old );
extern class_entry *    FindClass( section *, const char *, bool, bool );
extern seg_leader *     InitLeader( const char * );
extern void             FreeLeader( void * );
extern void             AddToGroup( group_entry *, seg_leader * );
extern void             SetAddPubSym( symbol *, sym_info, mod_entry *, offset, unsigned_16 );
extern void             DefineSymbol( symbol *, segnode *, offset, unsigned_16);
extern void             AllocCommunal( symbol *, offset );
extern symbol *         MakeCommunalSym( symbol *, offset, bool, bool );
extern void             SetComdatSym( symbol *, segdata * );
extern void             StoreInfoData( comdat_info *info );
extern void             InfoCDatAltDef( comdat_info * );
extern void             DefineLazyExtdef( symbol *, symbol *, bool );
extern void             DefineVFTableRecord( symbol *, symbol *, bool, vflistrtns * );
extern void             DefineVFReference( void *, symbol *, bool );
extern void             DefineReference( symbol * );
extern void             CheckComdatSym( symbol *, sym_info );
extern void             DefineComdat( segdata *, symbol *, offset, sym_info,
                                      unsigned_8 * );
extern group_entry *    GetGroup( const char * );
extern group_entry *    SearchGroups( const char * );
extern bool             SeenDLLRecord( void );
extern void             HandleImport( length_name *, length_name *, length_name *, ordinal_t );
extern void             HandleExport( length_name *, length_name *, unsigned, ordinal_t );
extern bool             CheckVFList( symbol * );
extern void             SetCurrSeg( segdata *, offset, unsigned_8 * );
extern void             ResetObjPass1( void );

extern obj_format       ObjFormat;
