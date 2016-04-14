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
* Description:  Type checking routines.
*
****************************************************************************/


#include "cvars.h"
#include "cgswitch.h"
#include "pragdefn.h"
#include "i64.h"
#include "cfeinfo.h"


/*  return types from TypeCheck */
typedef enum {
    TCE_OK,                     /* types are OK */
    TCE_TYPE_MISMATCH,          /* types mismatch */
    TCE_PARM_COUNT_MISMATCH,    /* different # of parms */
    TCE_TYPE2_HAS_MORE_INFO,    /* OK, but type2 has more information */
} typecheck_err;

typedef enum {
    NO,         /* not compatible */
    PW,         /* pointers with inconsistent levels of indirection */
    PM,         /* pointers point to different objects (Mismatch) */
    PC,         /* might be compatible if integer value is 0 */
    OK,         /* compatible */
    PQ,         /* pointers to different qualified types */
    PT,         /* pointer truncated */
    PX,         /* pointer expanded   */
    PS,         /* pointer to different signed types */
    AC,         /* assignment compatible */
} cmp_type;

extern call_list    *CallNodeList;

#define __  NO

static cmp_type const   CompTable[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
/*               BL,CH,UC,SH,US,IN,UI,LO,UL,DL,DU,FL,DB,LD,FI,DI,LI,PO,AR,ST,UN,FU,FI,VO,EN,TY,UF,DD,PC,WC,FC,DC,LC, */
/* BOOL     */ { OK,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,__,__,__,__,AC,__,AC,__,AC,__,__,__,__,__,__, },
/* CHAR     */ { AC,OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* UCHAR    */ { AC,OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* SHORT    */ { AC,OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* USHORT   */ { AC,OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* INT      */ { AC,OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* UINT     */ { AC,OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* LONG     */ { AC,AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* ULONG    */ { AC,AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* LONG64   */ { AC,AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* ULONG64  */ { AC,AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,AC,__,__,__,PC,__,__,__,__,OK,__,OK,__,OK,__,__,__,__,__,__, },
/* FLOAT    */ { AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,OK,AC,AC,AC,AC,AC,__,__,__,__,__,AC,__,AC,__,AC,__,__,__,AC,AC,AC, },
/* DOUBLE   */ { AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,OK,AC,AC,AC,AC,__,__,__,__,__,AC,__,AC,__,AC,__,__,__,AC,AC,AC, },
/* LDOUBLE  */ { AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,OK,AC,AC,AC,__,__,__,__,__,__,__,__,__,__,__,__,__,AC,AC,AC, },
/* FIMAG    */ { __,__,__,__,__,__,__,__,__,__,__,AC,AC,AC,OK,AC,AC,__,__,__,__,__,__,__,__,__,__,__,__,__,AC,AC,AC, },
/* DIMAG    */ { __,__,__,__,__,__,__,__,__,__,__,AC,AC,AC,AC,OK,AC,__,__,__,__,__,__,__,__,__,__,__,__,__,AC,AC,AC, },
/* LDIMAG   */ { __,__,__,__,__,__,__,__,__,__,__,AC,AC,AC,AC,AC,OK,__,__,__,__,__,__,__,__,__,__,__,__,__,AC,AC,AC, },
/* POINTER  */ { PC,PC,PC,PC,PC,PC,PC,PC,PC,PC,PC,__,__,__,__,__,__,OK,__,__,__,__,PC,__,__,__,PC,__,__,__,__,__,__, },
/* ARRAY    */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__,__,__,__,__, },
/* STRUCT   */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__,__,__,__, },
/* UNION    */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__,__,__, },
/* FUNCTION */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__,__,__,__, },
/* FIELD    */ { AC,OK,OK,OK,OK,OK,OK,OK,OK,OK,OK,AC,AC,__,__,__,__,PC,__,__,__,__,OK,__,__,__,OK,__,__,__,__,__,__, },
/* VOID     */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, },
/* ENUM     */ { AC,OK,OK,OK,OK,OK,OK,OK,OK,OK,OK,AC,AC,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__, },
/* TYPEDEF  */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__, },
/* UFIELD   */ { AC,OK,OK,OK,OK,OK,OK,OK,OK,OK,OK,AC,AC,__,__,__,__,PC,__,__,__,__,OK,__,__,__,OK,__,__,__,__,__,__, },
/* DOTDOTDOT*/ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, },
/* PLAIN CHAR*/{ __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, },
/* WCHAR    */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__, },
/* FCOMPLEX */ { __,__,__,__,__,__,__,__,__,__,__,AC,AC,AC,AC,AC,AC,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,AC,AC, },
/* DCOMPLEX */ { __,__,__,__,__,__,__,__,__,__,__,AC,AC,AC,AC,AC,AC,__,__,__,__,__,__,__,__,__,__,__,__,__,AC,OK,AC, },
/* LDCOMPLEX*/ { __,__,__,__,__,__,__,__,__,__,__,AC,AC,AC,AC,AC,AC,__,__,__,__,__,__,__,__,__,__,__,__,__,AC,AC,OK, },
};


static cmp_type InUnion( TYPEPTR typ1, TYPEPTR typ2, bool reversed )
{
    FIELDPTR    field;

    /* typ1->decl_type == TYPE_UNION */
    if( typ2->decl_type == TYPE_UNION )
        return( NO );
    for( field = typ1->u.tag->u.field_list; field != NULL; field = field->next_field ) {
        if( reversed ) {
            if( IdenticalType( typ2, field->field_type ) ) {
                return( OK );
            }
        } else {
            if( IdenticalType( field->field_type, typ2 ) ) {
                return( OK );
            }
        }
    }
    return( NO );
}

static bool ChkParmPromotion( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    switch( typ->decl_type ) {
    case TYPE_BOOL:
    case TYPE_CHAR:
    case TYPE_UCHAR:
    case TYPE_SHORT:
        if( CompFlags.strict_ANSI ) {
            return( false );
        }
        break;
    case TYPE_USHORT:
#if TARGET_SHORT != TARGET_INT
        if( CompFlags.strict_ANSI )  {
            return( false );
        }
#endif
        break;
    case TYPE_FLOAT:
        return( false );
    default:
        break;
    }
    return( true );
}

TYPEPTR  SkipTypeFluff( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    SKIP_ENUM( typ );
    return( typ );
}

static cmp_type CompatibleStructs( TAGPTR tag1, TAGPTR tag2 )
{
    FIELDPTR    field1;
    FIELDPTR    field2;
    TYPEPTR     typ1;
    TYPEPTR     typ2;

    if( tag1 == tag2 )
        return( OK );
    if( tag1->size != tag2->size )
        return( NO );
    field1 = tag1->u.field_list;
    field2 = tag2->u.field_list;
    /* if either struct is undefined, let's be conservative */
    if( (field1 == NULL) || (field2 == NULL) )
        return( NO );
    for( ; field1 != NULL && field2 != NULL; ) {
        typ1 = field1->field_type;
        SKIP_TYPEDEFS( typ1 );
        typ2 = field2->field_type;
        SKIP_TYPEDEFS( typ2 );
        if( !IdenticalType( typ1, typ2 ) ) {
            if( ( typ1->decl_type == TYPE_STRUCT && typ2->decl_type == TYPE_STRUCT ) ||
                ( typ1->decl_type == TYPE_UNION && typ2->decl_type == TYPE_UNION ) ) {
                if( CompatibleStructs( typ1->u.tag, typ2->u.tag ) != OK ) {
                    return( NO );
                }
            } else {
                return( NO );
            }
        }
        field1 = field1->next_field;
        field2 = field2->next_field;
    }
    /* one list longer than other (possible with -zp4) */
    if( field1 != NULL || field2 != NULL )
        return( NO );
    return( OK );
}

static typecheck_err ChkCompatibleFunctionParms( TYPEPTR typ1, TYPEPTR typ2, bool topLevelCheck )
{
    TYPEPTR         *plist1;
    TYPEPTR         *plist2;
    TYPEPTR         p1;
    TYPEPTR         p2;
    int             parmno;
    typecheck_err   rc;

    rc = TCE_OK;        /* indicate functions are compatible */
    plist1 = typ1->u.fn.parms;
    plist2 = typ2->u.fn.parms;
    if( plist1 != plist2 ) {
        if( plist1 == NULL ) {
            plist1 = plist2;
            plist2 = NULL;
        }
        if( plist2 == NULL ) {
            for( parmno = 1; (p1 = *plist1++) != NULL; ++parmno ) {
                if( p1->decl_type == TYPE_DOT_DOT_DOT ) {
                    break;
                }
                if( !ChkParmPromotion( p1 ) ) {
                    if( topLevelCheck ) {
                        CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
                    }
                    rc = TCE_TYPE_MISMATCH;
                }
            }
        } else {
            p1 = *plist1++; p2 = *plist2++;
            for( parmno = 1; p1 != NULL && p2 != NULL; ++parmno ) {
                if( p1->decl_type == TYPE_DOT_DOT_DOT || p2->decl_type == TYPE_DOT_DOT_DOT ) {
                    break;
                }
                if( !IdenticalType( p1, p2 ) ) {
                    if( topLevelCheck ) {
                        SetDiagType2( p1, p2 );
                        CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
                        SetDiagPop();
                    }
                    rc = TCE_TYPE_MISMATCH;
                }
                p1 = *plist1++; p2 = *plist2++;
            }
            if( p1 != NULL && p1->decl_type == TYPE_DOT_DOT_DOT || p2 != NULL && p2->decl_type == TYPE_DOT_DOT_DOT ) {
                p1 = NULL;
                p2 = NULL;
            }
            if( p1 != NULL || p2 != NULL ) {
                if( topLevelCheck ) {
                    CErr1( ERR_PARM_COUNT_MISMATCH );
                }
                rc = TCE_PARM_COUNT_MISMATCH;
            }
        }
    }
    return( rc );
}

bool ChkCompatibleLanguage( type_modifiers typ1, type_modifiers typ2 )
{
    typ1 &= MASK_LANGUAGES;
    typ2 &= MASK_LANGUAGES;
    if( typ1 == typ2 ) {
        return( true );
    } else if( typ1 == 0 ) {
        return( DftCallConv == GetLangInfo( typ2 ) );
    } else if( typ2 == 0 ) {
        return( DftCallConv == GetLangInfo( typ1 ) );
    } else  {
        return( false );
    }
}

static cmp_type DoCompatibleType( TYPEPTR typ1, TYPEPTR typ2, int ptr_indir_level )
{
    cmp_type         ret_val;
    type_modifiers   typ1_flags, typ2_flags;

    typ1_flags = FLAG_NONE;
    typ2_flags = FLAG_NONE;
    ret_val = OK;
    for( ;; ) {   // * [] loop
        typ1 = SkipTypeFluff( typ1 ); // skip typedefs, go into enum base
        typ2 = SkipTypeFluff( typ2 );
        if( typ1 == typ2 )
            break;
        if( typ1->decl_type != typ2->decl_type )
            break;
        if( typ1->decl_type == TYPE_ARRAY ) {
            /* See C99, 6.7.5.2p5 */
            if( typ1->u.array->dimension && typ2->u.array->dimension ) {
                if( typ1->u.array->dimension != typ2->u.array->dimension ) {
                    ret_val = PM;
                }
            }
        } else if( typ1->decl_type == TYPE_POINTER ) {
            typ1_flags = typ1->u.p.decl_flags;
            typ2_flags = typ2->u.p.decl_flags;
            if( (typ1_flags & MASK_QUALIFIERS) != (typ2_flags & MASK_QUALIFIERS) ) {
                if( ret_val == OK ) {   //PM is a worse case
                    ret_val = PQ;
                }
            }
            if( (typ1_flags & MASK_ALL_MEM_MODELS) != (typ2_flags & MASK_ALL_MEM_MODELS) ) {
                if( ( (typ1_flags & MASK_ALL_MEM_MODELS) != FLAG_NONE    // if same as mem model ok
                  && (typ2_flags & MASK_ALL_MEM_MODELS) != FLAG_NONE )
                  || TypeSize( typ1 ) != TypeSize( typ2 ) ) {
                    return( NO );
                }
            }
            ++ptr_indir_level;
        } else {
            break;
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
    }
    if( typ1 != typ2 ) {    // if not equal see if diff by pointers
        if( ptr_indir_level > 0 ) {
            if( typ1->decl_type == TYPE_VOID || typ2->decl_type == TYPE_VOID ) {
                // allow  void ** with any ** (but warn about it)
                if( ( ptr_indir_level == 1 ) || !CompFlags.strict_ANSI ) {
                    if( ptr_indir_level > 1 ) {
                        ret_val = PM;
                    }
                    return( ret_val ); // void *  and  anything *
                }
            }
            if( typ1->decl_type == TYPE_POINTER && typ2->decl_type != TYPE_ARRAY ) {
                ret_val = PW;
                while( typ1->decl_type == TYPE_POINTER ) {
                    typ1 = SkipTypeFluff( typ1->object );
                    ++ptr_indir_level;
                }
            } else if( typ2->decl_type == TYPE_POINTER && typ1->decl_type != TYPE_ARRAY ) {
                ret_val = PW;
                while( typ2->decl_type == TYPE_POINTER ) {
                    typ2 = SkipTypeFluff( typ2->object );
                    ++ptr_indir_level;
                }
            }
        }
    }
    if( typ1->decl_type == typ2->decl_type ) {
        if( typ1->decl_type == TYPE_FUNCTION ) {
            typ1_flags = typ1->u.fn.decl_flags;
            typ2_flags = typ2->u.fn.decl_flags;
            if( !ChkCompatibleLanguage( typ1_flags, typ2_flags ) ) {
                ret_val = NO;
            } else if( ChkCompatibleFunctionParms( typ1, typ2, false ) != TCE_OK ) {
                ret_val = NO;
            } else if( !IdenticalType( typ1->object, typ2->object ) ) {
                ret_val = NO;
            }
        } else if( typ1->decl_type == TYPE_STRUCT || typ1->decl_type == TYPE_UNION ) {
           /* allow pointers to different structs */
           /* stop this for ANSI! */
            if( ( typ1 != typ2 ) ) {
                // Types are not the same
                // if extensions are enabled, then we can do a compatible struct test
                if( CompFlags.extensions_enabled ) {
                    if( CompatibleStructs( typ1->u.tag, typ2->u.tag ) != OK) {
                        if( ptr_indir_level > 0 ) {
                            if( ret_val != PW ) {
                                ret_val = PM;
                            } else {
                                ret_val = NO;
                            }
                        } else {
                            ret_val = NO;
                        }
                    }
                } else {
                    ret_val = NO;
                }
            }
        } else if( (TYPE_FIELD == typ1->decl_type) || (TYPE_UFIELD == typ1->decl_type) ) {
            if( typ2->u.f.field_width > typ1->u.f.field_width ) {
                ret_val = AC;
            }
        }
    } else if( typ1->decl_type == TYPE_UNION && ptr_indir_level > 0 ) {
        if( InUnion( typ1, typ2, false ) != OK ) {
            ret_val = NO;
        } else {
            ret_val = PM;
        }
    } else if( typ2->decl_type == TYPE_UNION && ptr_indir_level > 0 ) {
        if( InUnion( typ2, typ1, true ) != OK ) {
            ret_val = NO;
        } else {
            ret_val = PM;
        }
    } else if( typ1->decl_type == TYPE_ARRAY ) {
        TYPEPTR     typ = typ1->object;

        if( !IdenticalType( typ, typ2 ) ) {
            ret_val = NO;
            SKIP_ARRAYS( typ );
            if( IdenticalType( typ, typ2 ) ) {
                ret_val = PM;
            }
        }
    } else if( typ2->decl_type == TYPE_ARRAY ) {
        TYPEPTR     typ = typ2->object;

        if( !IdenticalType( typ, typ1 ) ) {
            ret_val = NO;
            SKIP_ARRAYS( typ );
            if( IdenticalType( typ, typ1 ) ) {
                ret_val = PM;
            }
        }
    } else if( typ1->decl_type >= TYPE_LAST_ENTRY || typ2->decl_type >= TYPE_LAST_ENTRY ) {
        ret_val = NO;
    } else if( ptr_indir_level == 0 ) {
        ret_val = CompTable[typ1->decl_type][typ2->decl_type];
    } else {
        ret_val = PM;
        switch( typ1->decl_type ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
        case TYPE_LONG:
        case TYPE_LONG64:
            if( typ2->decl_type == typ1->decl_type + 1 ) {
                ret_val = PS;
            }
            break;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
        case TYPE_ULONG:
        case TYPE_ULONG64:
            if( typ2->decl_type + 1 == typ1->decl_type ) {
                ret_val = PS;
            }
            break;
        default:
            break;
        }
    }
    return( ret_val );
}

#define SUBNOT( a, b, on )  ( ( (a&on)|(b&on) )^(a&on) )

static cmp_type CompatibleType( TYPEPTR typ1, TYPEPTR typ2, bool assignment, bool null_ptr  )
{
    cmp_type         ret_val;
    cmp_type         ret_pq;
    type_modifiers   typ1_flags, typ2_flags;
    int              ptr_indir_level;

    ptr_indir_level = 0;
    typ1_flags = FLAG_NONE;
    typ2_flags = FLAG_NONE;
    ret_pq = OK;
    typ1 = SkipTypeFluff( typ1 ); // skip typedefs go into enums base
    typ2 = SkipTypeFluff( typ2 );
    if( typ1->decl_type == TYPE_POINTER && typ2->decl_type == TYPE_POINTER ) {
        // top level pointer
        typ1_flags = typ1->u.p.decl_flags;
        typ2_flags = typ2->u.p.decl_flags;
        // Special dispensation: assigning null pointer constant is allowed even
        // when the pointer size doesn't match. Required for MS compatibility.
        if( assignment && !null_ptr ) {
            type_modifiers  subnot;

            subnot = SUBNOT( typ1_flags, typ2_flags, MASK_QUALIFIERS );
            if( subnot ) {  // allow void * =  unaligned *
                if( subnot & (MASK_QUALIFIERS & ~FLAG_UNALIGNED) ) {
                    ret_pq = PQ;
                } else if( subnot & FLAG_UNALIGNED) {
                    align_type align1;

                    align1 = GetTypeAlignment( typ1->object );
                    if( align1 > 1 ) {
                        ret_pq = PQ;
                    }
                }
            }
            if( (typ1_flags & MASK_ALL_MEM_MODELS) != (typ2_flags & MASK_ALL_MEM_MODELS) ) {
                target_size size1, size2;

                size1 = TypeSize( typ1 );
                size2 = TypeSize( typ2 );
                if( size1 < size2 ) {
                   ret_pq = PT;
                } else if( size1 > size2 ) {
                    ret_pq = PX;
                }
            }
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
        ++ptr_indir_level;
    }
    ret_val = DoCompatibleType( typ1, typ2, ptr_indir_level );
    if( ret_val == OK ) {
        ret_val = ret_pq;
    }
    return( ret_val );
}

void CompatiblePtrType( TYPEPTR typ1, TYPEPTR typ2, TOKEN opr )
{
    SetDiagType3( typ1, typ2, opr ); /* Called with source, target. */
    switch( CompatibleType( typ1, typ2, false, false ) ) {
    case PT:
    case PX:
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ) { // else fuck em
            CWarn1( WARN_QUALIFIER_MISMATCH, ERR_QUALIFIER_MISMATCH );
        }
        break;
    case PM:
    case NO:
        CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        break;
    case PS:
        CWarn1( WARN_SIGN_MISMATCH, ERR_SIGN_MISMATCH );
        break;
    case PW:
        CWarn1( WARN_INCONSISTENT_INDIRECTION_LEVEL, ERR_INCONSISTENT_INDIRECTION_LEVEL );
        break;
    case PC:
        CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
        break;
    case OK:
    case AC:
        break;
    }
    SetDiagPop();
}

static bool IsNullConst( TREEPTR tree )
{
    bool    rc = false;

    if( tree->op.opr == OPR_PUSHINT ) {
        uint64  val64 = LongValue64( tree );

        rc = ( U64Test( &val64 ) == 0 );
    }
    return( rc );
}

static TREEPTR reverse_parms_tree( TREEPTR parm )
{
    TREEPTR     prev;
    TREEPTR     next;

    prev = NULL;
    while( parm != NULL ) {
        next = parm->left;
        parm->left = prev;
        prev = parm;
        parm = next;
    }
    return( prev );
}

static void CompareParms( TYPEPTR *master, TREEPTR parms, bool reverse )
{
    TYPEPTR     typ1;
    int         parmno;
    TREEPTR     parm;

    if( reverse ) {
        parms = reverse_parms_tree( parms );
    }
    typ1 = *master++;
    if( typ1 != NULL ) {
        if( typ1->decl_type == TYPE_VOID ) { /* type func(void);  */
            typ1 = NULL;                     /* indicate no parms */
        }
    }
    for( parmno = 1, parm = parms; ( typ1 != NULL ) && ( parm != NULL ); parm = parm->left, ++parmno ) {
        SKIP_TYPEDEFS( typ1 );
        // TODO is crap needed or has it been done
        if( typ1->decl_type == TYPE_FUNCTION ) {
            typ1 = PtrNode( typ1, FLAG_NONE, SEG_CODE );
        } else if( typ1->decl_type == TYPE_ARRAY ) {
            typ1 = PtrNode( typ1->object, FLAG_WAS_ARRAY, SEG_DATA );
        }
        /* check compatibility of parms */
        ParmAsgnCheck( typ1, parm, parmno, false );
        typ1 = *master++;
        if( typ1 != NULL && typ1->decl_type == TYPE_DOT_DOT_DOT ) {
            typ1 = NULL;
            parm = NULL;
            break;
        }
    }
    if( typ1 != NULL || parm != NULL ) {     /* should both be NULL now */
#if _CPU == 386
        /* can allow wrong number of parms with -3s option */
        if( !CompFlags.register_conventions ) {
            CWarn1( WARN_PARM_COUNT_MISMATCH, ERR_PARM_COUNT_WARNING );
        } else {
            CErr1( ERR_PARM_COUNT_MISMATCH );
        }
#else
        CErr1( ERR_PARM_COUNT_MISMATCH );
#endif
    }
    if( reverse ) {
        reverse_parms_tree( parms );
    }
}

/* Check parameters of function that were called before a prototype was seen */
extern void ChkCallParms( void )
{
    call_list   *nextcall;
    call_list   *next;

    for( nextcall = CallNodeList; nextcall != NULL; nextcall = next ) {
        TREEPTR     callnode;
        TREEPTR     callsite;
        SYM_ENTRY   sym;
        TYPEPTR     typ;

        next = nextcall->next;
        callnode = nextcall->callnode;
        if( callnode != NULL ) {
            callsite = callnode->left;      // point to OPR_FUNCNAME node
            SymGet( &sym, callsite->op.u2.sym_handle );
            typ = sym.sym_type;
            SKIP_TYPEDEFS( typ );
            if( (sym.flags & SYM_TEMP) == 0 )
                SetDiagSymbol( &sym, callsite->op.u2.sym_handle );
            SetErrLoc( &nextcall->src_loc );
            if( typ->u.fn.parms != NULL ) {
                CompareParms( typ->u.fn.parms, callnode->right, ParmsToBeReversed( sym.mods, NULL ) );
            } else {
                // Unprototyped function called. Note that for indirect calls, there
                // is no symbol associated with the function and diagnostic information
                // is hence limited.
                if( sym.flags & SYM_TEMP ) {
                    CWarn1( WARN_NONPROTO_FUNC_CALLED_INDIRECT, ERR_NONPROTO_FUNC_CALLED_INDIRECT );
                } else {
                    CWarn2p( WARN_NONPROTO_FUNC_CALLED, ERR_NONPROTO_FUNC_CALLED, SymName( &sym, callsite->op.u2.sym_handle ) );
                }
            }
            InitErrLoc();
            if( (sym.flags & SYM_TEMP) == 0 ) {
                SetDiagPop();
            }
        }
        CMemFree( nextcall );
    }
}

#define MAXSIZE        TARGET_BITS

static bool AssRangeChk( TYPEPTR typ1, TREEPTR opnd2 )
{
    unsigned        high;

    if( opnd2->op.opr == OPR_PUSHINT ) {
        switch( typ1->decl_type ) {
        case TYPE_FIELD:
        case TYPE_UFIELD:
            high = 0xffffffffU >> (MAXSIZE - (typ1->u.f.field_width));
            if( opnd2->op.u2.ulong_value > high ) {
                if( (opnd2->op.u2.ulong_value | (high >> 1)) != ~0U ) {
                    return( false );
                }
            }
            break;
        case TYPE_CHAR:
            if( opnd2->op.u2.long_value > 127 ||
                opnd2->op.u2.long_value < -128 ) {
                return( false );
            }
            break;
        case TYPE_UCHAR:
            if( opnd2->op.u2.ulong_value > 0xff) {
                if( (opnd2->op.u2.ulong_value | (0xff >> 1)) != ~0U ) {
                    return( false );
                }
            }
            break;
        case TYPE_UINT:
#if TARGET_INT > TARGET_SHORT
            break;
#endif
            // fall throught
        case TYPE_USHORT:
            if( opnd2->op.u2.ulong_value > 0xffff ) {
                if( (opnd2->op.u2.ulong_value | (0xffff >> 1)) != ~0U ) {
                    return( false );
                }
            }
            break;
        case TYPE_INT:
#if TARGET_INT > TARGET_SHORT
            break;
#endif
            // fall throught
        case TYPE_SHORT:
            if( opnd2->op.u2.long_value > 32767 ||
                opnd2->op.u2.long_value < -32768 ) {
                return( false );
            }
            break;
        default:
            break;
        }
    } else if( opnd2->op.opr == OPR_PUSHFLOAT ) {
        if( typ1->decl_type == TYPE_FLOAT ) {
            if( atof( opnd2->op.u2.float_value->string ) > TARGET_FLT_MAX ) {
                return( false );
            }
        }
    }
    return( true );
}

static bool IsPtrtoFunc( TYPEPTR typ )
{
    bool    ret;

    ret = false;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_POINTER ) {
        typ = typ->object;
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_FUNCTION ) {
            ret = true;
        }
    }
    return( ret );
}

static bool IsPointer( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    return( typ->decl_type == TYPE_POINTER );
}

void ParmAsgnCheck( TYPEPTR typ1, TREEPTR opnd2, int parmno, bool asgn_check )
{
    TYPEPTR        typ2;

    if( opnd2->op.opr == OPR_ERROR )
        return;

    // Fold RHS expression so that we can properly check for null
    // pointers or out of range constants
    FoldExprTree( opnd2 );
    typ2 = opnd2->u.expr_type;

    SetDiagType2( typ1, typ2 );
    switch( CompatibleType( typ1, typ2, true, IsNullConst( opnd2 ) ) ) {
    case NO:
        if( parmno ) {
            CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
        } else {
            CErr1( ERR_TYPE_MISMATCH );
        }
        break;
    case PT:
        if( asgn_check ) {
            if( !IsPtrConvSafe( opnd2, typ1, typ2 ) ) {
                CWarnP1( parmno, WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
            }
        } else if( parmno ) {
            CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
        } else {
            CErr1( ERR_TYPE_MISMATCH );
        }
        break;
    case PX:
        if( asgn_check ) {
            if( IsPtrtoFunc( typ1 ) ) {
                if( parmno ) {
                    CErr2( ERR_PARM_POINTER_TYPE_MISMATCH, parmno );
                } else {
                    CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
                }
            }
        } else if( parmno ) {
            CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
        } else {
            CErr1( ERR_TYPE_MISMATCH );
        }
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ) {
            if( parmno ) {
                CWarn2( WARN_QUALIFIER_MISMATCH, ERR_PARM_QUALIFIER_MISMATCH, parmno );
            } else {
                CWarn1( WARN_QUALIFIER_MISMATCH, ERR_QUALIFIER_MISMATCH );
            }
        }
        break;
    case PM:
        if( parmno ) {
            CErr2( ERR_PARM_POINTER_TYPE_MISMATCH, parmno );
        } else {
            CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        }
        break;
    case PS:
        if( parmno ) {
            CWarn2( WARN_SIGN_MISMATCH, ERR_PARM_SIGN_MISMATCH, parmno );
        } else {
            CWarn1( WARN_SIGN_MISMATCH, ERR_SIGN_MISMATCH );
        }
        break;
    case PW:
        if( parmno ) {
            CWarn2( WARN_PARM_INCONSISTENT_INDIRECTION_LEVEL, ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL, parmno );
        } else {
            CWarn1( WARN_INCONSISTENT_INDIRECTION_LEVEL, ERR_INCONSISTENT_INDIRECTION_LEVEL );
        }
        break;
    case PC:
        if( asgn_check ) {  /* Allow only "... *p = int 0";  */
            if( IsPointer( typ1 ) && opnd2->op.opr == OPR_PUSHINT ) {
                if( opnd2->op.u2.long_value != 0 ) {
                    CWarnP1( parmno, WARN_NONPORTABLE_PTR_CONV, ERR_NONPORTABLE_PTR_CONV );
                }
            } else {
                CWarnP1( parmno, WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
            }
        } else if( parmno ) {
            CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
        } else {
            CErr1( ERR_TYPE_MISMATCH );
        }
        break;
    case AC:
        if( asgn_check ) {
            bitfield_width  fsize_1 = 0, fsize_2 = 0;

            if( TypeSizeEx( typ2, &fsize_2 ) > TypeSizeEx( typ1, &fsize_1 ) ) {
                CWarnP1( parmno, WARN_LOSE_PRECISION, ERR_LOSE_PRECISION );
            }
            if( fsize_2 > fsize_1 ) {
                CWarnP1( parmno, WARN_LOSE_PRECISION, ERR_LOSE_PRECISION );
            }
        } else {
            if( parmno ) {
                CErr2( ERR_PARM_TYPE_MISMATCH, parmno );
            } else {
                CErr1( ERR_TYPE_MISMATCH );
            }
            break;
        }
    case OK:
        if( asgn_check ) {
            if( !AssRangeChk( typ1, opnd2 ) ) {
                CWarnP1( parmno, WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
        }
        break;
    }
    SetDiagPop();
}

void TernChk( TYPEPTR typ1, TYPEPTR typ2 )
{
    SetDiagType3( typ1, typ2, T_QUESTION );
    switch( CompatibleType( typ1, typ2, false, false ) ) {
    case NO:
        CErr1( ERR_TYPE_MISMATCH );
        break;
    case PT:
    case PX:
        CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ) {
            CWarn1( WARN_QUALIFIER_MISMATCH,ERR_QUALIFIER_MISMATCH );
        }
        break;
    case PM:
        CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        break;
    case PS:
        CWarn1( WARN_SIGN_MISMATCH, ERR_SIGN_MISMATCH );
        break;
    case PW:
        CWarn1( WARN_INCONSISTENT_INDIRECTION_LEVEL, ERR_INCONSISTENT_INDIRECTION_LEVEL );
        break;
    case PC:
        CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
        break;
    case OK:
    case AC:
        break;
    }
    SetDiagPop();
}

void ChkRetType( TREEPTR tree )
{
    TYPEPTR     ret_type;
    TYPEPTR     func_type;

    if( tree->op.opr == OPR_ERROR )
        return;
    ret_type = TypeOf( tree );
    func_type = CurFunc->sym_type->object;
    if( func_type->decl_type == TYPE_VOID ) {
        CErr2p( ERR_NOT_EXPECTING_RETURN_VALUE, CurFunc->name );
    } else if( ret_type->decl_type == TYPE_POINTER ) {
        if( ret_type->u.p.segid == SEG_STACK ) {
            CWarn1( WARN_RET_ADDR_OF_AUTO, ERR_RET_ADDR_OF_AUTO );
        }
    }
    /* check that the types are compatible */
    ParmAsgnCheck( func_type, tree, 0, true );
}


static typecheck_err TypeCheck( TYPEPTR typ1, TYPEPTR typ2, SYMPTR sym )
{
    int                 pointer_type;
    type_modifiers      ptr_mask;
    typecheck_err       retcode;

    pointer_type = 0;
    retcode = TCE_OK;
    /* "char *s" and "char s[]" differs only by FLAG_WAS_ARRAY, ignore it too */
    if( TargetSwitches & BIG_DATA ) {
        ptr_mask = ~(FLAG_FAR  | FLAG_WAS_ARRAY | MASK_LANGUAGES);
    } else {
        ptr_mask = ~(FLAG_NEAR | FLAG_WAS_ARRAY | MASK_LANGUAGES);
    }
    for( ;; ) {
        typ1 = SkipTypeFluff( typ1 ); // skip typedefs, go into enum base
        typ2 = SkipTypeFluff( typ2 );
        /* this compare was moved here */
        /* ptr to typedef struct failed when this was before typedef skips */
        if( typ1 == typ2 )
            return( retcode );
        if( typ1->decl_type != typ2->decl_type ) {
            if( pointer_type ) {
                /* by popular demand, I disabled the questionable feature to accept
                   void *foo(void);
                   int *foo(void) {return NULL};
                   - Bart Oldeman, 2002/10/24 */
                if( typ1->decl_type == TYPE_ARRAY ) {
                    return( TypeCheck( typ1->object, typ2, sym ) );
                }
                if( typ2->decl_type == TYPE_ARRAY ) {
                    return( TypeCheck( typ1, typ2->object, sym ) );
                }
            }
            break;
        }
        if( typ1->decl_type == TYPE_POINTER ) {
            pointer_type = 1;
            if( (typ1->u.p.decl_flags & ptr_mask) != (typ2->u.p.decl_flags & ptr_mask) ) {
                break;
            }
        }
        if( TypeSize(typ1) != TypeSize(typ2) ) {
            if( TypeSize(typ1) == 0 ) {
                retcode = TCE_TYPE2_HAS_MORE_INFO;
            } else if( TypeSize(typ2) == 0 ) {
                retcode = TCE_OK;
            } else {
                break;
            }
            if( typ1->decl_type != TYPE_ARRAY ) {
                return( retcode );
            }
        }
        if( ( TYPE_FIELD == typ1->decl_type ) || ( TYPE_UFIELD == typ1->decl_type ) ) {
            if( typ1->u.f.field_width != typ2->u.f.field_width ) {
                break;
            }
        }
        if( typ1->decl_type == TYPE_STRUCT || typ1->decl_type == TYPE_UNION ) {
            /* must be the same tag to be identical, if they are the
               same tag, then typ1 == typ2 which is checked above */
            break;
        }
        if( typ1->decl_type == TYPE_FUNCTION ) {
            retcode = ChkCompatibleFunctionParms( typ1, typ2, false );
            if( retcode != TCE_OK )
                return( retcode );
            if( typ1->object == NULL || typ2->object == NULL ) {
                return( TCE_OK );
            }
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
        if( typ1 == NULL )
            break;
        if( typ2 == NULL ) {
            break;
        }
    }
    if( typ1 != typ2 ) {
        return( TCE_TYPE_MISMATCH );
    }
    return( TCE_OK );                /* indicate types are identical */
}

bool IdenticalType( TYPEPTR typ1, TYPEPTR typ2 )
{
    typecheck_err   rc;

    rc = TypeCheck( typ1, typ2, NULL );
    return( ( rc == TCE_OK ) || ( rc == TCE_TYPE2_HAS_MORE_INFO ) );
}


bool VerifyType( TYPEPTR new, TYPEPTR old, SYMPTR sym )
{
    switch( TypeCheck( new, old, sym ) ) {
    case TCE_TYPE2_HAS_MORE_INFO:
        /* new= void *, old= something * */
        /* indicate want old definition  */
        return( true );
    case TCE_TYPE_MISMATCH:
    case TCE_PARM_COUNT_MISMATCH:
        CErr2p( ERR_TYPE_DOES_NOT_AGREE, sym->name );
    default:
        break;
    }
    return( false );
}


bool ChkCompatibleFunction( TYPEPTR typ1, TYPEPTR typ2, bool topLevelCheck )
{
    return( ChkCompatibleFunctionParms( typ1, typ2, topLevelCheck ) == TCE_OK );
}

