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
* Description:  Output generation routines for ssl.
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>

#include "ssl.h"
#include "sslint.h"


instruction     *FirstIns;
instruction     *LastIns;

static instruction *NewIns( op_code op )
{
    instruction *ins;

    ins = malloc( sizeof( instruction ) );
    if( ins == NULL ) Error( "out of memory for instructions" );
    ins->ins = op;
    ins->flink = NULL;
    ins->blink = NULL;
    ins->ptr = NULL;
    ins->operand = 0;
    ins->location = NO_LOCATION;
    return( ins );
}

static void CheckLong( instruction *ins )
{
    if( (unsigned int)ins->operand != (unsigned char)ins->operand) {
        ins->ins |= INS_LONG; /* change to long form of instruction */
    }
}


static void AddStream( instruction *after, instruction *ins )
{
    if( after == LastIns ) {
        if( after != NULL ) after->flink = ins;
        ins->blink = after;
        ins->flink = NULL;
        LastIns = ins;
        if( FirstIns == NULL ) FirstIns = ins;
    } else if( after == NULL ) {
        ins->flink = FirstIns;
        ins->blink = NULL;
        FirstIns->blink = ins;
        FirstIns = ins;
        if( LastIns == NULL ) LastIns = ins;
    } else {
        ins->flink = after->flink;
        ins->blink = after;
        after->flink = ins;
        ins->flink->blink = ins;
    }
}


static void DelStream( instruction *ins )
{
    choice_entry        *tbl;
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
    switch( ins->ins & INS_MASK ) {
    case INS_IN_CHOICE:
    case INS_CHOICE:
        for( tbl = ins->ptr; tbl != NULL; tbl = next ) {
            next = tbl->link;
            tbl->lbl->operand--; /* decrement label reference count */
            free( tbl );
        }
        break;
    case INS_JUMP:
    case INS_CALL:
        ((instruction *)ins->ptr)->operand--;
        break;
    }
}

instruction *GenNewLbl(void)
{
    return( NewIns( INS_LABEL ) );
}

void GenLabel( instruction *lbl )
{
    lbl->location = 0;
    AddStream( LastIns, lbl );
}

void GenExportLabel( instruction *lbl )
{
    GenLabel( lbl );
    lbl->operand++;     /* so it never gets deleted */
}

void GenInput( unsigned value )
{
    instruction *ins;

    ins = NewIns( INS_INPUT );
    ins->operand = value;
    CheckLong( ins );
    AddStream( LastIns, ins );
}

void GenInputAny( void )
{
    AddStream( LastIns, NewIns( INS_IN_ANY ) );
}

void GenOutput( unsigned value )
{
    instruction *ins;

    ins = NewIns( INS_OUTPUT );
    ins->operand = value;
    CheckLong( ins );
    AddStream( LastIns, ins );
}

void GenError( unsigned value )
{
    instruction *ins;

    ins = NewIns( INS_ERROR );
    ins->operand = value;
    CheckLong( ins );
    AddStream( LastIns, ins );
}

void GenJump( instruction *lbl )
{
    instruction *ins;

    ins = NewIns( INS_JUMP );
    ins->ptr = lbl;
    lbl->operand++;
    AddStream( LastIns, ins );
}

void GenReturn(void)
{
    AddStream( LastIns, NewIns( INS_RETURN ) );
}

void GenSetParm( unsigned value )
{
    instruction *ins;

    ins = NewIns( INS_SET_PARM );
    ins->operand = value;
    CheckLong( ins );
    AddStream( LastIns, ins );
}

void GenSetResult( unsigned value )
{
    instruction *ins;

    ins = NewIns( INS_SET_RESULT );
    ins->operand = value;
    CheckLong( ins );
    AddStream( LastIns, ins );
}

void GenLblCall( instruction *lbl )
{
    instruction *ins;

    ins = NewIns( INS_CALL );
    ins->ptr = lbl;
    lbl->operand++;
    AddStream( LastIns, ins );
}

void GenSemCall( unsigned num )
{
    instruction *ins;

    ins = NewIns( INS_SEMANTIC );
    ins->operand = num;
    CheckLong( ins );
    AddStream( LastIns, ins );
}

instruction *GenInpChoice(void)
{
    instruction *ins;

    ins = NewIns( INS_IN_CHOICE );
    AddStream( LastIns, ins );
    return( ins );
}

instruction *GenChoice(void)
{
    instruction *ins;

    ins = NewIns( INS_CHOICE );
    AddStream( LastIns, ins );
    return( ins );
}

void GenTblLabel( instruction *choice, instruction *lbl, unsigned value )
{
    choice_entry *new;
    choice_entry **owner;
    choice_entry *curr;

    new = malloc( sizeof( choice_entry ) );
    if( new == NULL ) Error( "no memory for choice entry" );
    lbl->operand++;
    new->value = value;
    if( (unsigned int)value != (unsigned char)value ) {
        choice->ins |= INS_LONG;
    }
    new->lbl = lbl;
    owner = (choice_entry **)&choice->ptr;
    for( ;; ) {
        curr = *owner;
        if( curr == NULL ) break;
        if( curr->value > value ) break;
        owner = &curr->link;
    }
    new->link = curr;
    *owner = new;
    choice->operand++;
}

void GenKill(void)
{
    instruction *ins;

    ins = NewIns( INS_KILL );
    ins->operand = SrcLine();
    CheckLong( ins );
    AddStream( LastIns, ins );
}


static char Optimize(void)
{
    char        change;
    instruction *ins;
    instruction *next;
    instruction *dest;
    char        dead_code;

    dead_code = 0; /* want to keep code at location 0 */
    change = 0;
    for( ins = FirstIns; ; ins = next ) {
        for( ;; ) {
            if( ins == NULL ) return( change );
            next = ins->flink;
            if( !dead_code ) break;
            if( ins->ins == INS_LABEL ) break;
            DelStream( ins );
            ins = next;
            change = 1;
        }
        dead_code = 0;
        switch( ins->ins & INS_MASK ) {
        case INS_JUMP:
            for( dest = ins->ptr; dest->ins == INS_LABEL; dest = dest->flink )
                ;
            switch( dest->ins & INS_MASK ) {
            case INS_KILL:
                /* jump to kill */
                next = NewIns( dest->ins );
                next->operand = dest->operand;
                AddStream( ins, next );
                DelStream( ins );
                next = next->flink;
                change = 1;
                break;
            case INS_RETURN:
                /* jump to return */
                next = NewIns( INS_RETURN );
                AddStream( ins, next );
                DelStream( ins );
                next = next->flink;
                change = 1;
                break;
            case INS_JUMP:
                /* jump to jump */
                if( ins == dest ) {
                    next = NewIns( INS_KILL );
                    next->operand = 0;
                    AddStream( ins, next );
                    DelStream( ins );
                    next = next->flink;
                } else {
                    ((instruction *)ins->ptr)->operand--;
                    ins->ptr = dest->ptr;
                    ((instruction *)ins->ptr)->operand++;
                }
                change = 1;
                break;
            }
            dead_code = 1;
            break;
        case INS_CALL:
            dest = ins->ptr;
            if( dest->location == NO_LOCATION ) break;
            for( ; dest->ins == INS_LABEL; dest = dest->flink ) {
            }
            switch( dest->ins & INS_MASK ) {
            case INS_KILL:
                /* call to kill */
                dead_code = 1;
                next = NewIns( dest->ins );
                next->operand = dest->operand;
                AddStream( ins, next );
                DelStream( ins );
                change = 1;
                break;
            case INS_RETURN:
                /* call to return */
                DelStream( ins );
                change = 1;
                break;
            case INS_JUMP:
                /* call to jump */
                ((instruction *)ins->ptr)->operand--;
                ins->ptr = dest->ptr;
                ((instruction *)ins->ptr)->operand++;
                change = 1;
                break;
            default:
                for( dest = ins->flink;
                     dest->ins == INS_LABEL;
                     dest = dest->flink )
                    ;
                if( dest->ins == INS_RETURN ) {
                    /* call followed by a return */
                    ins->ins = INS_JUMP;
                    change = 1;
                    dead_code = 1;
                }
                break;
            }
            break;
        case INS_RETURN:
            dead_code = 1;
            break;
        case INS_KILL:
            dead_code = 1;
            break;
        case INS_LABEL:
            /* dead label */
            if( ins->operand == 0 ) {
                DelStream( ins );
                change = 1;
            }
            break;
        }
    }
}


static unsigned Locate(void)
{
    instruction *ins;
    unsigned    loc;

    loc = 0;
    for( ins = FirstIns; ins != NULL; ins = ins->flink ) {
        ins->location = loc;
        switch( ins->ins & INS_MASK ) {
        case INS_RETURN:
        case INS_IN_ANY:
            loc += sizeof( char );
            break;
        case INS_INPUT:
        case INS_ERROR:
        case INS_OUTPUT:
            loc += sizeof( char ) + sizeof( char );
            if( ins->ins & INS_LONG ) loc += sizeof( char );
            break;
        case INS_JUMP:
        case INS_CALL:
            loc += sizeof( char ) + sizeof( char );
            if( ins->ins & INS_LONG ) loc += sizeof( char );
            break;
        case INS_IN_CHOICE:
        case INS_CHOICE:
            loc += sizeof( char ) + sizeof( char ) + ins->operand *
                       ( sizeof( char ) + sizeof( unsigned short ) );
            if( ins->ins & INS_LONG ) loc += ins->operand * sizeof( char );
            break;
        case INS_SET_RESULT:
        case INS_SET_PARM:
            loc += sizeof( char ) + sizeof( char );
            if( ins->ins & INS_LONG ) loc += sizeof( char );
            break;
        case INS_SEMANTIC:
            loc += sizeof( char ) + sizeof( char );
            if( ins->ins & INS_LONG ) loc += sizeof( char );
            break;
        case INS_KILL:
            loc += sizeof( char ) + sizeof( char );
            if( ins->ins & INS_LONG ) loc += sizeof( char );
            break;
        case INS_LABEL:
            loc += 0;
            break;
        }
    }
    return( loc );
}


static char Expand(void)
{
    instruction *ins;
    instruction *dest;
    signed int  diff;
    char        change;

    change = 0;
    for( ins = FirstIns; ins != NULL; ins = ins->flink ) {
        switch( ins->ins ) {
        case INS_JUMP: /* short form */
        case INS_CALL: /* short form */
            dest = ins->ptr;
            diff = dest->location - ins->location;
            if( diff != (signed char) diff ) {
                ins->ins |= INS_LONG; /* change to long form */
                change = 1;
            }
            break;
        }
    }
    return( change );
}

extern void GenCode(void)
{
    while( Optimize() )
        ;
    do {
        Locate();
    } while( Expand() );
}

static void OutOper( instruction *ins )
{
    if( ins->ins & INS_LONG ) {
        OutWord( ins->operand );
    } else {
        OutByte( ins->operand );
    }
}

static void OutDisp( instruction *ins )
{
    int disp;

    disp = ((instruction *)ins->ptr)->location - ins->location;
    if( ins->ins & INS_LONG ) {
        OutWord( disp );
    } else {
        OutByte( disp );
    }
}

void DumpGenCode(void)
{
    instruction *ins;
    instruction *next;
    choice_entry *choice;

    OutStartSect( "Code", Locate() );
    for( ins = FirstIns; ins != NULL; ins = next ) {
        next = ins->flink;
        if( ins->ins != INS_LABEL ) OutByte( ins->ins );
        Dump( "[%.4x] %c ", ins->location, (ins->ins & INS_LONG) ? 'L' : 'S' );
        switch( ins->ins & INS_MASK ) {
        case INS_INPUT:
            Dump( "INPUT: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_IN_ANY:
            Dump( "IN_ANY\n" );
            break;
        case INS_OUTPUT:
            Dump( "OUTPUT: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_ERROR:
            Dump( "ERROR: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_JUMP:
            Dump( "JUMP L%.4x\n", ((instruction *)ins->ptr)->location );
            OutDisp( ins );
            break;
        case INS_CALL:
            Dump( "CALL L%.4x\n", ((instruction *)ins->ptr)->location );
            OutDisp( ins );
            break;
        case INS_SET_RESULT:
            Dump( "SET RES: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_RETURN:
            Dump( "RETURN\n" );
            break;
        case INS_IN_CHOICE:
        case INS_CHOICE:
            Dump( "%sCHOICE: %d\n", (ins->ins == INS_IN_CHOICE) ? "IN_" : "",
                  ins->operand );
            OutByte( ins->operand );
            for( choice = ins->ptr; choice != NULL; choice = choice->link ) {
                Dump( "        %4d L%.4x\n", choice->value,
                      choice->lbl->location );
                if( ins->ins & INS_LONG ) {
                    OutWord( choice->value );
                } else {
                    OutByte( choice->value );
                }
                OutWord( choice->lbl->location );
            }
            break;
        case INS_SET_PARM:
            Dump( "SET PARM: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_SEMANTIC:
            Dump( "SEMANTIC: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_KILL:
            Dump( "KILL: %d\n", ins->operand );
            OutOper( ins );
            break;
        case INS_LABEL:
            Dump( "LABEL: %d\n", ins->operand );
            break;
        }
        DelStream( ins );
    }
    OutEndSect();
}
