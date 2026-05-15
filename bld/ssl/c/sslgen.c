/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2026      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Output generation routines for ssl.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ssl.h"
#include "sslint.h"


#define SKIP_LABELS(x)  while((x)->opcode == INS_LABEL) (x) = (x)->flink

instruction     *FirstIns;
instruction     *LastIns;

static instruction  *FreeInsPool;

static instruction *NewIns( op_code opcode )
{
    instruction *ins;

    ins = MemAllocSafe( sizeof( instruction ) );
    memset( ins, 0, sizeof( instruction ) );
    ins->opcode = opcode;
    ins->location = NO_LOCATION;
    return( ins );
}

instruction *NewLabel( void )
{
    return( NewIns( INS_LABEL ) );
}

static void SetOperand( instruction *ins, int value )
{
    ins->u1.operand = value;
    ins->is_long = ( (unsigned int)value != (unsigned char)value );
}


static void AddStream( instruction *after, instruction *ins )
{
    if( after == LastIns ) {
        if( after != NULL )
            after->flink = ins;
        ins->blink = after;
        ins->flink = NULL;
        LastIns = ins;
        if( FirstIns == NULL ) {
            FirstIns = ins;
        }
    } else if( after == NULL ) {
        ins->flink = FirstIns;
        ins->blink = NULL;
        FirstIns->blink = ins;
        FirstIns = ins;
        if( LastIns == NULL ) {
            LastIns = ins;
        }
    } else {
        ins->flink = after->flink;
        ins->blink = after;
        after->flink = ins;
        ins->flink->blink = ins;
    }
}


static void DelStream( instruction *ins )
{
    choice_entry        *choice;
    choice_entry        *next;

    if( ins == FirstIns ) {
        FirstIns = ins->flink;
        if( FirstIns != NULL ) {
            FirstIns->blink = NULL;
        } else {
            LastIns = NULL;
        }
    } else if( ins == LastIns ) {
        LastIns = ins->blink;
        LastIns->flink = NULL;
    } else {
        ins->blink->flink = ins->flink;
        ins->flink->blink = ins->blink;
    }
    switch( ins->opcode ) {
    case INS_IN_CHOICE:
    case INS_CHOICE:
        for( choice = ins->u.choice; choice != NULL; choice = next ) {
            next = choice->link;
            /*
             * remove reference to label
             */
            choice->lbl->u1.reference--;
        }
        break;
    case INS_JUMP:
    case INS_CALL:
        /*
         * remove reference to label
         */
        ins->u.lbl->u1.reference--;
        break;
    }
    /*
     * move instruction to free memory pool for later deallocation
     */
    ins->flink = FreeInsPool;
    ins->blink = NULL;
    FreeInsPool = ins;
}

void GenLabel( instruction *lbl )
{
    lbl->location = 0;
    AddStream( LastIns, lbl );
}

void GenExportLabel( instruction *lbl )
{
    lbl->location = 0;
    /*
     * set reference to never gets deleted
     */
    lbl->u1.reference = 1;
    AddStream( LastIns, lbl );
}

void GenInput( int value )
{
    instruction *ins;

    ins = NewIns( INS_INPUT );
    SetOperand( ins, value );
    AddStream( LastIns, ins );
}

void GenInputAny( void )
{
    AddStream( LastIns, NewIns( INS_IN_ANY ) );
}

void GenOutput( int value )
{
    instruction *ins;

    ins = NewIns( INS_OUTPUT );
    SetOperand( ins, value );
    AddStream( LastIns, ins );
}

void GenError( int value )
{
    instruction *ins;

    ins = NewIns( INS_ERROR );
    SetOperand( ins, value );
    AddStream( LastIns, ins );
}

void GenJump( instruction *lbl )
{
    instruction *ins;

    ins = NewIns( INS_JUMP );
    ins->u.lbl = lbl;
    lbl->u1.reference++;
    AddStream( LastIns, ins );
}

void GenReturn( void )
{
    AddStream( LastIns, NewIns( INS_RETURN ) );
}

void GenSetParm( int value )
{
    instruction *ins;

    ins = NewIns( INS_SET_PARM );
    SetOperand( ins, value );
    AddStream( LastIns, ins );
}

void GenSetResult( int value )
{
    instruction *ins;

    ins = NewIns( INS_SET_RESULT );
    SetOperand( ins, value );
    AddStream( LastIns, ins );
}

void GenLblCall( instruction *lbl )
{
    instruction *ins;

    ins = NewIns( INS_CALL );
    ins->u.lbl = lbl;
    lbl->u1.reference++;
    AddStream( LastIns, ins );
}

void GenSemCall( int num )
{
    instruction *ins;

    ins = NewIns( INS_SEMANTIC );
    SetOperand( ins, num );
    AddStream( LastIns, ins );
}

instruction *GenInpChoice( void )
{
    instruction *ins;

    ins = NewIns( INS_IN_CHOICE );
    AddStream( LastIns, ins );
    return( ins );
}

instruction *GenChoice( void )
{
    instruction *ins;

    ins = NewIns( INS_CHOICE );
    AddStream( LastIns, ins );
    return( ins );
}

void GenTblLabel( instruction *ins, instruction *lbl, int value )
{
    choice_entry *choice;
    choice_entry **owner;
    choice_entry *curr;

    lbl->u1.reference++;
    ins->u1.operand++;
    if( (unsigned int)value != (unsigned char)value ) {
        ins->is_long = true;
    }
    for( owner = &ins->u.choice; (curr = *owner) != NULL; owner = &curr->link ) {
        if( curr->value > value ) {
            break;
        }
    }
    choice = MemAllocSafe( sizeof( choice_entry ) );
    choice->value = value;
    choice->lbl = lbl;
    choice->link = curr;
    *owner = choice;
}

void GenKill( void )
{
    instruction *ins;

    ins = NewIns( INS_KILL );
    SetOperand( ins, SrcLine() );
    AddStream( LastIns, ins );
}

static bool remove_lbl_dup( void )
{
    bool            change;
    instruction     *ins;
    choice_entry    *choice;
    instruction     *lbl;
    instruction     *lbl_next;

    lbl_next = NULL;
    change = false;
    do {
        lbl = lbl_next;
        lbl_next = NULL;
        for( ins = FirstIns; ins != NULL; ins = ins->flink ) {
            switch( ins->opcode ) {
            case INS_JUMP:
            case INS_CALL:
                if( lbl == NULL )
                    break;
                if( ins->u.lbl == lbl ) {
                    ins->u.lbl->u1.reference--;
                    lbl->flink->u1.reference++;
                    ins->u.lbl = lbl->flink;
                    change = true;
                }
                break;
            case INS_IN_CHOICE:
            case INS_CHOICE:
                if( lbl == NULL )
                    break;
                for( choice = ins->u.choice; choice != NULL; choice = choice->link ) {
                    if( choice->lbl == lbl ) {
                        choice->lbl->u1.reference--;
                        lbl->flink->u1.reference++;
                        choice->lbl = lbl->flink;
                        change = true;
                    }
                }
                break;
            case INS_LABEL:
                if( ins->u1.reference == 0
                  || lbl_next != NULL
                  || lbl != NULL
                  && ins == lbl ) {
                    break;
                }
                /*
                 * check if next item is label
                 */
                if( ins->flink->opcode == INS_LABEL
                  && ins->flink->u1.reference ) {
                    lbl_next = ins;
                }
                break;
            }
        }
    } while( lbl_next != NULL );
    return( change );
}

static bool Optimize( void )
{
    bool        change;
    instruction *ins;
    instruction *next;
    instruction *dest;
    bool        dead_code;

    if( remove_lbl_dup() )
        return( true );

    next = NULL;
    dead_code = false;  /* want to keep code at location 0 */
    change = false;
    for( ins = FirstIns; ins != NULL; ins = next ) {
        for( ; ins != NULL; ins = next ) {
            next = ins->flink;
            if( !dead_code
              || ins->opcode == INS_LABEL )
                break;
            DelStream( ins );
            change = true;
        }
        if( ins == NULL )
            break;
        dead_code = false;
        switch( ins->opcode ) {
        case INS_JUMP:
            dest = ins->u.lbl;
            SKIP_LABELS( dest );
            switch( dest->opcode ) {
            case INS_KILL:
                /*
                 * jump to kill
                 */
                next = NewIns( INS_KILL );
                next->u1.operand = dest->u1.operand;
                next->is_long = dest->is_long;
                AddStream( ins, next );
                DelStream( ins );
                next = next->flink;
                change = true;
                break;
            case INS_RETURN:
                /*
                 * jump to return
                 */
                next = NewIns( INS_RETURN );
                AddStream( ins, next );
                DelStream( ins );
                next = next->flink;
                change = true;
                break;
            case INS_JUMP:
                /*
                 * jump to jump
                 */
                if( ins == dest ) {
                    next = NewIns( INS_KILL );
                    AddStream( ins, next );
                    DelStream( ins );
                    next = next->flink;
                } else {
                    ins->u.lbl->u1.reference--;
                    ins->u.lbl = dest->u.lbl;
                    ins->u.lbl->u1.reference++;
                }
                change = true;
                break;
            }
            dead_code = true;
            break;
        case INS_CALL:
            dest = ins->u.lbl;
            if( dest->location == NO_LOCATION )
                break;
            SKIP_LABELS( dest );
            switch( dest->opcode ) {
            case INS_KILL:
                /*
                 * call to kill
                 */
                dead_code = true;
                next = NewIns( INS_KILL );
                next->u1.operand = dest->u1.operand;
                next->is_long = dest->is_long;
                AddStream( ins, next );
                DelStream( ins );
                change = true;
                break;
            case INS_RETURN:
                /*
                 * call to return
                 */
                DelStream( ins );
                change = true;
                break;
            case INS_JUMP:
                /*
                 * call to jump
                 */
                ins->u.lbl->u1.reference--;
                ins->u.lbl = dest->u.lbl;
                ins->u.lbl->u1.reference++;
                change = true;
                break;
            default:
                dest = ins->flink;
                SKIP_LABELS( dest );
                if( dest->opcode == INS_RETURN ) {
                    /*
                     * call followed by a return
                     */
                    ins->opcode = INS_JUMP;
                    change = true;
                    dead_code = true;
                }
                break;
            }
            break;
        case INS_RETURN:
            dead_code = true;
            break;
        case INS_KILL:
            dead_code = true;
            break;
        case INS_LABEL:
            if( ins->u1.reference == 0 ) {
                /*
                 * remove dead label
                 */
                DelStream( ins );
                change = true;
            }
            break;
        }
    }
    return( change );
}


static unsigned Locate( void )
{
    instruction *ins;
    unsigned    loc;

    loc = 0;
    for( ins = FirstIns; ins != NULL; ins = ins->flink ) {
        ins->location = loc;
        switch( ins->opcode ) {
        case INS_RETURN:
        case INS_IN_ANY:
            loc += sizeof( char );
            break;
        case INS_INPUT:
        case INS_ERROR:
        case INS_OUTPUT:
            loc += sizeof( char ) + sizeof( char );
            if( ins->is_long )
                loc += sizeof( char );
            break;
        case INS_JUMP:
        case INS_CALL:
            loc += sizeof( char ) + sizeof( char );
            if( ins->is_long )
                loc += sizeof( char );
            break;
        case INS_IN_CHOICE:
        case INS_CHOICE:
            loc += sizeof( char ) + sizeof( char )
                + ins->u1.operand * ( sizeof( char ) + sizeof( short ) );
            if( ins->is_long )
                loc += ins->u1.operand * sizeof( char );
            break;
        case INS_SET_RESULT:
        case INS_SET_PARM:
            loc += sizeof( char ) + sizeof( char );
            if( ins->is_long )
                loc += sizeof( char );
            break;
        case INS_SEMANTIC:
            loc += sizeof( char ) + sizeof( char );
            if( ins->is_long )
                loc += sizeof( char );
            break;
        case INS_KILL:
            loc += sizeof( char ) + sizeof( char );
            if( ins->is_long )
                loc += sizeof( char );
            break;
        case INS_LABEL:
            loc += 0;
            break;
        }
    }
    return( loc );
}


static bool Expand( void )
{
    instruction *ins;
    instruction *dest;
    int         diff;
    bool        change;
    bool        is_long;

    change = false;
    for( ins = FirstIns; ins != NULL; ins = ins->flink ) {
        switch( ins->opcode ) {
        case INS_JUMP:
        case INS_CALL:
            dest = ins->u.lbl;
            diff = dest->location - ins->location;
            is_long = ( diff != (signed char)diff );
            if( ins->is_long != is_long ) {
                ins->is_long = is_long;
                change = true;
            }
            break;
        }
    }
    return( change );
}

void GenCode( void )
{
    while( Optimize() )
        ;
    do {
        Locate();
    } while( Expand() );
}

static void OutOper( instruction *ins )
{
    if( ins->is_long ) {
        OutWord( ins->u1.operand );
    } else {
        OutByte( (unsigned char)ins->u1.operand );
    }
}

static void OutDisp( instruction *ins )
{
    int disp;

    disp = ins->u.lbl->location - ins->location;
    if( ins->is_long ) {
        OutWord( disp );
    } else {
        OutByte( (unsigned char)disp );
    }
}

void DumpGenCode( void )
{
    instruction *ins;
    instruction *next;
    choice_entry *choice;

    OutStartSect( "Code", Locate() );
    for( ins = FirstIns; ins != NULL; ins = next ) {
        next = ins->flink;
        if( ins->opcode == INS_LABEL ) {
            /*
             * don't generate any code for labels
             */
            Dump( "[%.4x] - ", ins->location );
        } else if( ins->is_long ) {
            OutByte( (unsigned char)( ins->opcode | INS_LONG ) );
            Dump( "[%.4x] L ", ins->location );
        } else {
            OutByte( (unsigned char)ins->opcode );
            Dump( "[%.4x] S ", ins->location );
        }
        switch( ins->opcode ) {
        case INS_INPUT:
            Dump( "INPUT: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_IN_ANY:
            Dump( "IN_ANY\n" );
            break;
        case INS_OUTPUT:
            Dump( "OUTPUT: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_ERROR:
            Dump( "ERROR: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_JUMP:
            Dump( "JUMP L%.4x\n", ins->u.lbl->location );
            OutDisp( ins );
            break;
        case INS_CALL:
            Dump( "CALL L%.4x\n", ins->u.lbl->location );
            OutDisp( ins );
            break;
        case INS_SET_RESULT:
            Dump( "SET RES: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_RETURN:
            Dump( "RETURN\n" );
            break;
        case INS_IN_CHOICE:
        case INS_CHOICE:
            Dump( "%sCHOICE: %d\n", ( ins->opcode == INS_IN_CHOICE ) ? "IN_" : "", ins->u1.operand );
            OutByte( (unsigned char)ins->u1.operand );
            for( choice = ins->u.choice; choice != NULL; choice = choice->link ) {
                Dump( "        %4d L%.4x\n", choice->value, choice->lbl->location );
                if( ins->is_long ) {
                    OutWord( choice->value );
                } else {
                    OutByte( (unsigned char)choice->value );
                }
                OutWord( choice->lbl->location );
            }
            break;
        case INS_SET_PARM:
            Dump( "SET PARM: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_SEMANTIC:
            Dump( "SEMANTIC: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_KILL:
            Dump( "KILL: %d\n", ins->u1.operand );
            OutOper( ins );
            break;
        case INS_LABEL:
            /*
             * don't generate any code for labels
             */
            Dump( "LABEL: %d\n", ins->u1.reference );
            break;
        default:
            Dump( "UNKNOWN: %d\n", ins->opcode );
        }
    }
    OutEndSect();
}

void InsMemFree( instruction *ins )
{
    choice_entry        *choice;

    switch( ins->opcode ) {
    case INS_IN_CHOICE:
    case INS_CHOICE:
        while( (choice = ins->u.choice) != NULL ) {
            ins->u.choice = choice->link;
            MemFree( choice );
        }
        break;
    }
    MemFree( ins );
}

void FreeGenCode( void )
{
    instruction *next;

    /*
     * deallocate instructions in generated code
     */
    while( FirstIns != NULL ) {
        next = FirstIns->flink;
        InsMemFree( FirstIns );
        FirstIns = next;
    }
    /*
     * deallocate pool of free instructions
     */
    while( FreeInsPool != NULL ) {
        next = FreeInsPool->flink;
        InsMemFree( FreeInsPool );
        FreeInsPool = next;
    }
}
