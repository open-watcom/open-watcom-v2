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
* Description:  Register tree manipulation.
*
****************************************************************************/


#include "cgstd.h"
#include "coderep.h"
#include "conflict.h"
#include "regset.h"
#include "freelist.h"
#include "zoiks.h"

extern  reg_set_index   RegIntersect(reg_set_index,reg_set_index);
extern  hw_reg_set      HighOffsetReg(hw_reg_set);
extern  hw_reg_set      LowOffsetReg(hw_reg_set);
extern  reg_set_index   GetPossibleForTemp(conflict_node *, name *);

extern  hw_reg_set      *RegSets[];

static  pointer         *RegFrl;
static  pointer         *TreeFrl;


#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
static bool HasSegRegs( reg_tree *tree )
{
    hw_reg_set  *regs;

    regs = tree->regs;
    if( regs != NULL ) {
        for( ; !HW_CEqual( *regs, HW_EMPTY ); ++regs ) {
            if( HW_COvlap( *regs, HW_SEGS ) ) return( true );
        }
    }
    return( false );
}
#endif


static  reg_tree        *AllocRegTree( void )
/*******************************************/
{
    reg_tree    *tree;

    tree = AllocFrl( &TreeFrl, sizeof( reg_tree ) );
    return( tree );
}



static  void    FreeRegTree( reg_tree *tree )
/*******************************************/
{
    FrlFreeSize( &TreeFrl, (pointer *)tree, sizeof( reg_tree ) );
}


static  hw_reg_set      *AllocRegSet( void )
/******************************************/
{
    int         i;
    hw_reg_set  *regs;
    hw_reg_set  *curr;

    regs = AllocFrl( &RegFrl, REGSET_SIZE );
    curr = regs;
    for( i = REG_COUNT; i > 0; --i ) {
        HW_CAsgn( *curr++, HW_EMPTY );
    }
    return( regs );
}


static  void    FreeRegSet( hw_reg_set *regs )
/********************************************/
{
    FrlFreeSize( &RegFrl, (pointer *)regs, REGSET_SIZE );
}


extern  bool    RegTreeFrlFree( void )
/************************************/
{
    bool        freed;

    freed = FrlFreeAll( &TreeFrl, sizeof( reg_tree ) );
    freed |= FrlFreeAll( &RegFrl, REGSET_SIZE );
    return( freed );
}


static  void    CheckBigPointer( reg_tree *tree )
/***********************************************/
{
#if _TARGET & ( _TARG_80386 | _TARG_IAPX86 )
/* this routine is a sickening concession to the foolish design of the 8086
   (may all intel designers rot in hell forever, amen)

   This routine says, If the high part only wants word registers, then
   we'd better let the whole part have any pairs with segment
   registers in them. It also says don't let the LOW part of anything
   get a segment register
*/

    if( tree->lo != NULL && HasSegRegs( tree->lo ) ) {
        tree->lo->idx = RL_NUMBER_OF_SETS;
        FreeRegSet( tree->lo->regs );
        tree->lo->regs = NULL;
    }
    if( tree->hi != NULL && tree->hi->idx == RL_WORD && HasSegRegs( tree ) ) {
        tree->hi->idx = RL_NUMBER_OF_SETS;
        FreeRegSet( tree->hi->regs );
        tree->hi->regs = NULL;
    }
#else
    tree = tree;
#endif
}


static  reg_tree        *NewTree( void )
/**************************************/
{
    reg_tree    *tree;

    tree = AllocRegTree();
    tree->alt = NULL;
    tree->lo = NULL;
    tree->hi = NULL;
    tree->regs = NULL;
    tree->has_name = false;
    tree->idx = RL_NUMBER_OF_SETS;
    tree->temp = NULL;
    return( tree );
}


static  void    BuildPossible( reg_tree *tree )
/*********************************************/
{
    hw_reg_set  *src;
    hw_reg_set  *dst;

    if( tree != NULL ) {
        BuildPossible( tree->lo );
        BuildPossible( tree->hi );
        if( tree->idx != RL_NUMBER_OF_SETS ) {
            tree->regs = AllocRegSet();
            dst = tree->regs;
            for( src = RegSets[tree->idx]; !HW_CEqual( *src, HW_EMPTY ); ++src ) {
                *dst++ = *src;
            }
            *dst = *src;
#ifndef NDEBUG
            if ( dst - tree->regs >= REG_COUNT ) { /* '>=' 'coz no increment before 'break' */
                Zoiks( ZOIKS_143 );
            }
#endif
        }
    }
}


static  void    SimpleTree( conflict_node *conf )
/***********************************************/
{
    reg_tree    *tree;
    name        *temp;

    tree = NewTree();
    temp = conf->name;
    tree->temp = temp;
    tree->size = temp->n.size;
    tree->offset = temp->v.offset;
    tree->idx = conf->possible;
    BuildPossible( tree );
    conf->tree = tree;
}


static  reg_tree        *BuildTree( name *alias, name *master,
                                    type_length offset, type_length size,
                                    conflict_node *conf )
/************************************************************************/
{
    reg_tree    *tree;
    name        *temp;
    bool        have_lo;
    bool        have_hi;
    type_length losize;
    type_length hisize;
    type_length midpoint;

    tree = NewTree();
    tree->offset = offset;
    tree->size = size;
    if( alias != NULL ) {
        tree->temp = alias;
        tree->has_name = true;
        alias->t.temp_flags |= VISITED;
        temp = alias->t.alias;
        while( temp != alias ) {
            if( temp->v.offset == offset && temp->n.size == size ) {
                tree->alt = temp; /* signed vs. unsigned*/
                temp->t.temp_flags |= VISITED;
                break;
            }
            temp = temp->t.alias;
        }
    }
    if( tree->alt == NULL ) {
        if( tree->temp != NULL ) {
            tree->idx = GetPossibleForTemp( conf, tree->temp );
        }
    } else {
        tree->idx = RegIntersect( GetPossibleForTemp( conf, tree->temp ),
                                  GetPossibleForTemp( conf, tree->alt  ) );
    }
    if( size == 6 ) { /* this is harmlessly specific to 80386 big pointers */
        losize = 4;
        hisize = 2;
    } else {
        losize = size / 2;
        hisize = size / 2;
    }
    midpoint = offset + losize;
    if( losize != 0 ) {
        have_lo = false;
        have_hi = false;
        temp = master->t.alias;
        while( temp != master ) {
            if( have_lo == false
             && temp->v.offset == offset && temp->n.size == losize ) {
                tree->lo = BuildTree( temp, master, offset, losize, conf );
                have_lo = true;
            } else if( have_hi == false
                 && temp->v.offset == midpoint && temp->n.size == hisize ) {
                tree->hi = BuildTree( temp, master, midpoint, hisize, conf );
                have_hi = true;
            }
            temp = temp->t.alias;
        }
        if( have_lo == false ) {
            tree->lo = BuildTree( NULL, master, offset, losize, conf );
        }
        if( have_hi == false ) {
            tree->hi = BuildTree( NULL, master, midpoint, hisize, conf );
        }
        if( tree->hi->has_name ) {
            tree->has_name = true;
        }
        if( tree->lo->has_name ) {
            tree->has_name = true;
        }
    }
    return( tree );
}


extern  void    BurnRegTree( reg_tree *tree )
/*******************************************/
{
    if( tree != NULL ) {
        BurnRegTree( tree->lo );
        BurnRegTree( tree->hi );
        if( tree->regs != NULL ) {
            FreeRegSet( tree->regs );
        }
        FreeRegTree( tree );
    }
}


static  void    TrimTree( reg_tree *tree )
/****************************************/
{
    if( tree->lo != NULL ) {
        if( tree->lo->has_name == false ) {
            BurnRegTree( tree->lo );
            tree->lo = NULL;
        } else {
            TrimTree( tree->lo );
        }
    }
    if( tree->hi != NULL ) {
        if( tree->hi->has_name == false ) {
            BurnRegTree( tree->hi );
            tree->hi = NULL;
        } else {
            TrimTree( tree->hi );
        }
    }
}


static  reg_tree        *CheckTree( reg_tree *tree )
/**************************************************/
{
    name        *alias;
    name        *temp;

    alias = tree->temp;
    temp = alias;
    for( ;; ) {
        temp = temp->t.alias;
        if( ( temp->t.temp_flags & VISITED ) == EMPTY ) {
            BurnRegTree( tree );
            tree = NULL;
            break;
        }
        if( temp == alias ) break;
    }
    return( tree );
}


static  void    CompressSets( reg_tree *tree )
/********************************************/
{
    hw_reg_set  *src;
    hw_reg_set  *dst;
    int         i;

    if( tree != NULL ) {
        CompressSets( tree->lo );
        CompressSets( tree->hi );
        if( tree->regs != NULL ) {
            dst = tree->regs;
            src = dst;
            for( i = REG_COUNT; i > 0; --i ) {
                if( !HW_CEqual( *src, HW_EMPTY ) ) {
                    *dst++ = *src;
                }
                ++src;
            }
            while( dst != src ) {
                HW_CAsgn( *dst++, HW_EMPTY );
            }
        }
    }
}


static  bool    PartIntersect( reg_tree *part,
                               reg_tree *whole,
                               hw_reg_set (*rtn)( hw_reg_set ) )
/****************************************************************/
{
    bool        change;
    int         i;
    int         j;
    hw_reg_set  *src;
    hw_reg_set  *dst;
    hw_reg_set  curr;
    hw_reg_set  tmp;

    change = false;
    if( whole->idx != RL_NUMBER_OF_SETS ) {
        if( part->idx == RL_NUMBER_OF_SETS ) {

            /* build a register for part consisting of the Hi/Lo parts of whole*/

            part->regs = AllocRegSet();
            part->idx = RL_;
            src = whole->regs;
            dst = part->regs;
            for( i = REG_COUNT; i > 0; --i ) {
                *dst++ = rtn( *src++ );
            }
        }

        /* check that all Hi/Lo parts of whole are contained in part*/

        src = whole->regs;
        for( i = REG_COUNT; i > 0; --i ) {
            if( !HW_CEqual( *src, HW_EMPTY ) ) {
                curr = rtn( *src );
                if( !HW_CEqual( curr, HW_EMPTY ) ) {
                    dst = part->regs;
                    for( j = REG_COUNT; j > 0; --j ) {
                        if( HW_Equal( *dst, curr ) ) {
                            break;
                        }
                        ++dst;
                    }
                    if( j == 0 ) {
                        HW_CAsgn( *src, HW_EMPTY );
                        change = true;
                    }
                } else {
                    HW_CAsgn( *src, HW_EMPTY );
                    change = true;
                }
            }
            ++src;
        }

        /* check that each reg in part is a Hi/Lo part in whole*/

        src = part->regs;
        for( i = REG_COUNT; i > 0; --i ) {
            curr = *src;
            if( !HW_CEqual( curr, HW_EMPTY ) ) {
                dst = whole->regs;
                for( j = REG_COUNT; j > 0; --j ) {
                    if( !HW_CEqual( *dst, HW_EMPTY ) ) {
                        tmp = rtn( *dst );
                        if( HW_Equal( curr, tmp ) ) break;
                    }
                    ++dst;
                }
                if( j == 0 ) {
                    HW_CAsgn( *src, HW_EMPTY );
                    change = true;
                }
            }
            ++src;
        }

    }
    return( change );
}


static  bool    Intersect( reg_tree *tree )
/*****************************************/
{
    bool        change;

    change = false;
    if( tree->hi != NULL ) {
        change |= PartIntersect( tree->hi, tree, &HighOffsetReg );
        change |= Intersect( tree->hi );
    }
    if( tree->lo != NULL ) {
        change |= PartIntersect( tree->lo, tree, &LowOffsetReg );
        change |= Intersect( tree->lo );
    }
    return( change );
}


extern  void    BurnNameTree( reg_tree *tree )
/********************************************/
{
    BurnRegTree( tree );
}


extern  void    RegTreeInit( void )
/*********************************/
{
    InitFrl( &RegFrl );
    InitFrl( &TreeFrl );
}


extern  void    BuildNameTree( conflict_node *conf )
/**************************************************/
{
    name        *temp;
    reg_tree    *tree;

    temp = conf->name;
    if( temp->n.class == N_TEMP && temp->t.alias != temp ) {
        tree = BuildTree( temp, temp, temp->v.offset, temp->n.size, conf );
        tree = CheckTree( tree );
        if( tree != NULL ) {
            TrimTree( tree );
        }
    } else {
        tree = NULL;
    }
    conf->tree = tree;
}


extern  void    BuildRegTree( conflict_node *conf )
/*************************************************/
{
    name        *temp;
    reg_tree    *tree;

    temp = conf->name;
    if( temp->n.class == N_TEMP && temp->t.alias != temp ) {
        tree = BuildTree( temp, temp, temp->v.offset, temp->n.size, conf );
        tree = CheckTree( tree );
        if( tree != NULL ) {
            BuildPossible( tree );
            TrimTree( tree );
            CheckBigPointer( tree );
            while( Intersect( tree ) ) {
            }
            CompressSets( tree );
        }
        conf->tree = tree;
    } else {
        SimpleTree( conf );
    }
}
