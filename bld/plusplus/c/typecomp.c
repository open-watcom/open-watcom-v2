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


#include "plusplus.h"
#include "carve.h"
#include "template.h"
#include "pragdefn.h"
#include "initdefs.h"

typedef struct tc_data TC_DATA;
struct tc_data {
    TC_DATA             *next;
    TYPE                type1;
    TYPE                type2;
    unsigned            arg_index;
    type_exclude        mask;
    enum {
        FUNCTION,
        MEMBER_POINTER
    }                   flavour;
};

typedef enum {
    FOLLOW_OF,
    FOLLOW_STACK
} TC_STATE;

#define BLOCK_TC_DATA           32
static carve_t carveTC_DATA;

static TC_DATA *pushTC_DATA( TC_DATA **h )
{
    return StackCarveAlloc( carveTC_DATA, h );
}

static void popTC_DATA( TC_DATA **h )
{
    TC_DATA *e;

    e = *h;
    *h = e->next;
    CarveFree( carveTC_DATA, e );
}

static bool typeCompareCurrent( TC_DATA **h, TYPE type1, TYPE type2, type_exclude mask )
{
    type_flag   flag1;
    type_flag   flag2;
    arg_list    *args1;
    arg_list    *args2;
    TC_DATA     *top;

    if( type1 == NULL ) {
        return( false );
    }
    if( type2 == NULL ) {
        return( false );
    }
    if( type1->id != type2->id ) {
        return( false );
    }
    flag1 = type1->flag;
    flag2 = type2->flag;
    switch( type1->id ) {
    case TYP_SINT:
    case TYP_VOID:
        break;
    case TYP_ERROR:
    case TYP_BOOL:
    case TYP_CHAR:
    case TYP_SCHAR:
    case TYP_UCHAR:
    case TYP_WCHAR:
    case TYP_SSHORT:
    case TYP_USHORT:
    case TYP_UINT:
    case TYP_SLONG:
    case TYP_ULONG:
    case TYP_SLONG64:
    case TYP_ULONG64:
    case TYP_FLOAT:
    case TYP_DOUBLE:
    case TYP_LONG_DOUBLE:
    case TYP_POINTER:
    case TYP_DOT_DOT_DOT:
        if( flag1 != flag2 ) {
            return( false );
        }
        break;
    case TYP_CLASS:
        if( flag1 != flag2 || ( flag1 & TF1_UNBOUND ) == 0 ) {
            return( false );
        }
        return( TemplateUnboundSame( type1, type2 ) );
    case TYP_ENUM:
        return( false );
    case TYP_TYPEDEF:
        if( flag1 != flag2 ) {
            return( false );
        }
        if( type1->u.t.sym != type2->u.t.sym ) {
            return( false );
        }
        if( type1->u.t.scope != type2->u.t.scope ) {
            return( false );
        }
        break;
    case TYP_BITFIELD:
        if( flag1 != flag2 ) {
            return( false );
        }
        if( type1->u.b.field_start != type2->u.b.field_start ) {
            return( false );
        }
        if( type1->u.b.field_width != type2->u.b.field_width ) {
            return( false );
        }
        break;
    case TYP_FUNCTION:
        if( mask & TC1_FUN_LINKAGE ) {
            flag1 &= ~(TF1_PLUSPLUS | TF1_FN_IGNORE);
            flag2 &= ~(TF1_PLUSPLUS | TF1_FN_IGNORE);
        } else {
            flag1 &= ~TF1_FN_IGNORE;
            flag2 &= ~TF1_FN_IGNORE;
        }
        if( flag1 != flag2 ) {
            return( false );
        }
        if( ( type1->u.f.pragma != type2->u.f.pragma )
          &&( ! PragmasTypeEquivalent( type1->u.f.pragma, type2->u.f.pragma ) ) ) {
            return( false );
        }
        args1 = type1->u.f.args;
        args2 = type2->u.f.args;
        if( args1 != args2 ) {
            if( args1 == NULL ) {
                return( false );
            }
            if( args2 == NULL ) {
                return( false );
            }
            if( args1->num_args != args2->num_args ) {
                return( false );
            }
            if( args1->qualifier != args2->qualifier ) {
                return( false );
            }
            if( args1->num_args > 0 ) {
                top = pushTC_DATA( h );
                top->type1 = type1;
                top->type2 = type2;
                top->arg_index = args1->num_args;
                top->flavour = FUNCTION;
                top->mask = mask | TC1_PTR_FUN;
            }
        }
        break;
    case TYP_ARRAY:
        if( flag1 != flag2 ) {
            return( false );
        }
        if( type1->u.a.array_size != type2->u.a.array_size ) {
            return( false );
        }
        break;
    case TYP_MODIFIER:
        if( flag1 != flag2 ) {
            return( false );
        }
        if( type1->u.m.base != type2->u.m.base ) {
            return( false );
        }
        if( type1->u.m.pragma != type2->u.m.pragma ) {
            return( false );
        }
        break;
    case TYP_MEMBER_POINTER:
        if( flag1 != flag2 ) {
            return( false );
        }
        top = pushTC_DATA( h );
        top->type1 = type1;
        top->type2 = type2;
        top->arg_index = 0;
        top->flavour = MEMBER_POINTER;
        top->mask = mask | TC1_PTR_FUN;
        break;
    case TYP_GENERIC:
        if( flag1 != flag2 ) {
            return( false );
        }
        if( type1->u.g.index != type2->u.g.index ) {
            return( false );
        }
        break;
    case TYP_TYPENAME:
        return( !strcmp( type1->u.n.name, type2->u.n.name ) );
    default:
#ifndef NDEBUG
        CFatal( "unknown type being compared" );
#else
        return( false );
#endif
    }
    return( true );
}

bool TypeCompareExclude( TYPE type1, TYPE type2, type_exclude mask )
/******************************************************************/
{
    TC_STATE    state;
    type_flag   flag1;
    type_flag   flag2;
    void        *base1;
    void        *base2;
    TC_DATA     *stack;

    if( type1 == type2 ) {
        return( true );
    }
    stack = NULL;
    state = FOLLOW_OF;
    for(;;) {
        if( type1 == NULL || type2 == NULL ) break;

        // tweak type1 and type2 to ignore minor distinctions
        type1 = TypeModExtract( type1, &flag1, &base1, mask|TC1_NOT_MEM_MODEL );
        type2 = TypeModExtract( type2, &flag2, &base2, mask|TC1_NOT_MEM_MODEL );
        if( type1 == NULL || type2 == NULL ) break;

        if( mask & TC1_PTR_FUN ) {
            type_flag   extra;

            // tweak type1 and type2 to ignore more major distinction
            if( ( type1->id == TYP_POINTER )
              &&( type2->id == TYP_FUNCTION ) ) {
                type2 = TypeModFlagsEC( MakePointerTo( type2 ), &extra );
                flag2 |= extra;
            } else if( ( type2->id == TYP_POINTER )
                     &&( type1->id == TYP_FUNCTION ) ) {
                type1 = TypeModFlagsEC( MakePointerTo( type1 ), &extra );
                flag1 |= extra;
            }
        }

        if( ( type1->id == TYP_CLASS )
         && ( type1->flag & TF1_UNBOUND )
         && ( type1->of != NULL ) ) {
            type1 = type1->of;
        }
        if( ( type2->id == TYP_CLASS )
         && ( type2->flag & TF1_UNBOUND )
         && ( type2->of != NULL ) ) {
            type2 = type2->of;
        }

        // compare type1 and type2
        flag1 &= ~TF1_MOD_IGNORE;
        flag2 &= ~TF1_MOD_IGNORE;
        if( flag1 != flag2 ) {
            break;
        } else if( (flag1 & TF1_BASED) &&
                   !TypeBasesEqual( flag1, base1, base2 ) ) {
            break;
        } else if( type1 == type2 ) {
            state = FOLLOW_STACK;
        } else {
            if( !typeCompareCurrent( &stack, type1, type2, mask ) ) {
                break;
            }
        }

        // advance the type pointers
        for( ; ; ) {
            TC_DATA     *top;
            unsigned    arg_index;

            switch( state ) {
            case FOLLOW_OF:
                type1 = type1->of;
                type2 = type2->of;
                break;
            case FOLLOW_STACK:
                top = stack;
                if( top == NULL ) {
                    return( true );
                }
                state = FOLLOW_OF;
                switch( top->flavour ) {
                case FUNCTION:
                    arg_index = --top->arg_index;
                    type1 = top->type1->u.f.args->type_list[arg_index];
                    type2 = top->type2->u.f.args->type_list[arg_index];
                    mask  = top->mask;
                    if( arg_index == 0 ) {
                        popTC_DATA( &stack );
                    }
                    break;
                case MEMBER_POINTER:
                    type1 = top->type1->u.mp.host;
                    type2 = top->type2->u.mp.host;
                    mask  = top->mask;
                    popTC_DATA( &stack );
                    break;
                }
                break;
            }
            if( type1 == type2 ) {
                state = FOLLOW_STACK;
                continue;
            }
            break;
        }
    }
    while( stack != NULL ) {
        popTC_DATA( &stack );
    }
    return( false );
}

bool TypesSameExclude( TYPE type1, TYPE type2, type_exclude mask )
/****************************************************************/
{
    return( TypeCompareExclude( type1, type2, mask | TC1_PTR_FUN ) );
}

bool TypesIdentical( TYPE type1, TYPE type2 )
/*******************************************/
{
    return TypeCompareExclude( type1, type2, TC1_PTR_FUN | TC1_NOT_ENUM_CHAR );
}

static void typeCompareInit(    // INITIALIZATION
    INITFINI* defn )            // - definition
{
    defn = defn;
    carveTC_DATA = CarveCreate( sizeof( TC_DATA ), BLOCK_TC_DATA );
}

static void typeCompareFini(    // COMPLETION
    INITFINI* defn )            // - definition
{
    defn = defn;
    DbgStmt( CarveVerifyAllGone( carveTC_DATA, "TC_DATA" ) );
    CarveDestroy( carveTC_DATA );
}

INITDEFN( type_compare, typeCompareInit, typeCompareFini );
