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
* Description:  Instruction scoring related structs, enums and defines.
*
****************************************************************************/


/* aligned */
#include "hostsys.h"

#define    NO_INDEX     (-1)

typedef enum {
        CHANGE_NORMAL   = 0x00,         /* default behaviour of ChangeIns */
        CHANGE_ALL      = 0x01,         /* replace all occurrences of the var */
        CHANGE_GEN      = 0x02,         /* the gen tables have changed */
        CHANGE_CHECK    = 0x04          /* just check - don't actually change */
} change_type;

typedef enum {
        UNCHANGED,
        MODIFIED,
        DELETED
} ins_mod;

typedef struct score_reg {
        union name              *reg_name;
        hw_reg_set              reg;
        int                     low;    /*  index of low part of reg, if any */
        int                     high;   /*  index of high part of reg, if any */
        int                     high_of;
        int                     low_of;
        type_length             size;
        type_class_def          class;
} score_reg;


typedef struct score_info {
        signed_32               offset;
        union  name             *base;  /*  indexed names only */
        union {
          struct temp_name      *t;
          struct var_name       *v;
          void                  *p;
        } symbol;
        int                     index_reg;      /*  indexed names only */
        int                     scale;          /*  indexed names only */
        name_class_def          class;
} score_info;


typedef struct score_list {
        struct score_list       *next;
        struct score_info       info;
} score_list;

typedef score_list              *list_head;

typedef struct score {
        list_head               *list;
        struct score            *next_reg;
        struct score            *prev_reg;
        byte                    index;
        byte                    generation;
        unsigned_16             __pad_to_16;
} score;


// scdata.c
extern  score_reg       **ScoreList;
extern  int             ScoreCount;
extern  score_info      *ScZero;
extern  pointer         *ScListFrl;

// scblock.c
bool    DoScore( block *blk );
byte    HasZero( score *sc, name *n );

// scinfo.c
bool    ScoreLookup( score *p, score_info *info );
bool    ScoreEqual( score *p, int index, score_info *info );
void    ScoreAssign( score *p, int index, score_info *info );
void    ScoreInfo( score_info *info, name *op );
bool    ScoreLAInfo( score_info *info, name *op );
void    ScoreKillInfo( score *sc, name *op, score_info *info, hw_reg_set except );

// scins.c
bool    ChangeIns( instruction *ins, name *to, name **op, change_type flags );
bool    FindRegOpnd( score *sc, instruction *ins );
void    ScoreMakeEqual( score *sc, name *op1, name *op2 );
bool    ScoreMove( score *sc, instruction *ins );
bool    ScoreLA( score *sc, instruction *ins );
void    ScZeroCheck( score *sc, instruction *ins );

// scregs.c
void    RegInsert( score *sc, int dst_idx, int src_idx );
bool    RegsEqual( score *sc, int i1, int i2 );
void    RegKill( score *sc, hw_reg_set regs );
void    RegAdd( score *sc, int dst_idx, int src_idx );

// scthrash.c
bool    RegThrash( block *blk );

// scutil.c
pointer ScAlloc( int size );
void    ScFree( pointer chunk, int size );
void    ScoreCalcList( void );
void    ScoreClear( score *p );
void    FreeScListEntry( score_list *list );
void    ScoreFreeList( score *p );
void    FreeScoreBoard( score *p );
void    MemChanged( score *p, bool statics_too );
bool    ScoreFrlFree( void );
score_list      *NewScListEntry( void );

// sczero.c
bool    ScoreZero( score *sc, instruction **pins );

// cpu-specific
void    ScInitRegs( score *sc );
void    ScoreSegments( score *sc );
bool    ScConvert( instruction *ins );
bool    ScAddOk( hw_reg_set reg1, hw_reg_set reg2 );
void    AddRegs( void );
bool    ScRealRegister( name *reg );
