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


#include "cgstd.h"
#include "coderep.h"
#include "data.h"

#define MAX_SHORT_NEG   127
#define MAX_SHORT_POS   127
#define SHORT_RANGE     MAX_SHORT_NEG+MAX_SHORT_POS+1;
#define SHORT_COST      3

#if _TARGET & _TARG_80386
    #define     SHORT_SAVINGS   3
    #define     LONG_COST       12
#else
    #define     SHORT_SAVINGS   1
    #define     LONG_COST       6
#endif

static  int             CalcBaseSave( type_length bp, name *temp );
static  void            SetTempLocations(void);
static  void            SortTemps(void);

extern  name            *DeAlias(name*);
extern  void            *SortList(void *,unsigned,bool (*)(name*,name*) );
extern  void            CountTempRefs(void);

extern  type_length     AdjustBase( void ) {
/*************************************
    Decide how far we should adjust BP away from SP, based on the number
    of short v.s.  long x[bp] references we would have.  It works like
    this.  Set the id field of each temporary to its location past the
    normal BP.  Sort the temporaries by this field.  Count the number of
    references to each temporary (temp->t.u.ref_count), and then figure
    out how may short BP references we would have if BP were to be (1)
    at it normal location and (2) pointing 127 byte in front of each
    temporary, so that we can just reach each that temporary with the
    maximum positive offset.  If we will save enough by turning long
    references into short ones to justify the cost of adjusting BP,
    return the adjustment factor to be used.

*/

    type_length         best_bp;
    type_length         bp;
    int                 best_savings;
    int                 savings;
    type_length         last_location;
    name                *temp;

    if( BlockByBlock ) {
        return( -( MAX_SHORT_POS & ~1 ) );
    }
    SetTempLocations();
    CountTempRefs();
    SortTemps();
    best_bp = 0;
    best_savings = CalcBaseSave( best_bp, Names[ N_TEMP ] );
    temp = Names[ N_TEMP ];
    last_location = NO_LOCATION;
    while( temp != NULL ) {
        if( temp->t.v.alt_location != last_location &&
            temp->t.v.alt_location != NO_LOCATION ) {
            bp = ( temp->t.v.alt_location-(MAX_SHORT_POS-1) ) & ~1;
            savings = CalcBaseSave( bp, temp );
            if( savings > best_savings ) {
                best_bp = bp;
                best_savings = savings;
            }
            last_location = temp->t.v.alt_location;
        }
        temp = temp->n.next_name;
    }
    if( best_bp >= -MAX_SHORT_NEG && best_bp <= MAX_SHORT_POS ) {
        if( best_savings*SHORT_SAVINGS <= SHORT_COST ) return( 0 );
    } else {
        if( best_savings*SHORT_SAVINGS <= LONG_COST  ) return( 0 );
    }
    return( best_bp );
}


static  int             CalcBaseSave( type_length bp, name *temp ) {
/*******************************************************************
    How many short references would be get if BP were adjusted by "temp".

*/

    int         savings;

    savings = 0;
    while( temp != NULL ) {
        if( temp->t.v.alt_location != NO_LOCATION ) {
            if( temp->t.v.alt_location < bp-MAX_SHORT_NEG ) break;
            savings += temp->t.u.ref_count;
        }
        temp = temp->n.next_name;
    }
    return( savings );
}


static  void            SetTempLocations(void) {
/**********************************************/

    name                *base;
    name                *temp;

    for( temp = Names[ N_TEMP ]; temp != NULL; temp = temp->n.next_name ) {
        base = DeAlias( temp );
        if( base->t.location == NO_LOCATION ) {
            temp->t.v.alt_location = NO_LOCATION;
        } else {
            temp->t.v.alt_location = base->t.location
                                   + temp->v.offset - base->v.offset;
        }
    }
}


static  bool    TempBefore( name *t1, name *t2 ) {
/************************************************/

    return( t1->t.v.alt_location > t2->t.v.alt_location );
}


static  void    SortTemps(void) {
/*******************************/

    Names[ N_TEMP ] = SortList( Names[ N_TEMP ], offsetof( name, n.next_name ),
                                TempBefore );
}
