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


typedef enum {
    NEEDS_NOTHING       = 0,
    NEEDS_ST0           = 0x01,
    NEEDS_ST0_ST1       = 0x02,
    PUSHES              = 0x04,
    POPS                = 0x08,
    SETS_ST1            = 0x10,
    EXCHANGES           = 0x20,
    POPS_ALL            = 0x40,
    POPS2               = 0x80
} fp_attr;

enum {
    VIRTUAL_0,
    VIRTUAL_1,
    VIRTUAL_2,
    VIRTUAL_3,
    VIRTUAL_4,
    VIRTUAL_5,
    VIRTUAL_6,
    VIRTUAL_7,
    VIRTUAL_NONE
};

enum {
    ACTUAL_0,
    ACTUAL_1,
    ACTUAL_2,
    ACTUAL_3,
    ACTUAL_4,
    ACTUAL_5,
    ACTUAL_6,
    ACTUAL_7,
    ACTUAL_NONE
};

typedef struct {
    byte        actual_locn[VIRTUAL_NONE];
} st_seq;

typedef struct temp_entry {
    struct temp_entry   *next;
    name                *op;
    name                *actual_op;
    byte                actual_locn;
    instruction         *first;
    instruction         *last;
    instruction         *entry;
    instruction         *exit;
    int                 savings;
    unsigned            cached : 1;
    unsigned            defined : 1;
    unsigned            killed : 1;
    unsigned            global : 1;
    unsigned            whole_block : 1;
} temp_entry;

extern st_seq           *STLocations;
extern int              MaxSeq;
extern byte             *SeqMaxDepth;
extern byte             *SeqCurDepth;
extern temp_entry       *TempList;

#define RegLoc( seq, vir ) ( STLocations + (seq) )->actual_locn[vir]
#define InsLoc( ins, vir ) RegLoc( (ins)->sequence, vir )
