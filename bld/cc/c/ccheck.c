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


/*  return types from TypeCheck */
enum {
    TC_OK,                      /* types are OK */
    TC_TYPE_MISMATCH,           /* types mismatch */
    TC_PARM_COUNT_MISMATCH,     /* different # of parms */
    TC_TYPE2_HAS_MORE_INFO,     /* OK, but type2 has more information */
    TC_PARM_TYPE_MISMATCH       /* parms have different types */
};

typedef enum {
    NO,         /* not compatible */
    PW,         /* pointers with inconsistent levels of indirection */
    PM,         /* pointers point to different objects (Mismatch) 16-may-91 */
    PC,         /* might be compatible if integer value is 0 */
    OK,         /* compatible */
    PQ,         /* pointers to different qualified types */
    PT,         /* pointer truncated */
    PX,         /* pointer expanded   */
    PS,         /* pointer to different signed types */
    AC,         /* assignment compatible */
} cmp_type;

typedef enum {
    VC_CONVERT, /* "promote" void * to the type it is compared with */
    VC_WARN     /* warn for mismatching void * in function pointer argument lists */
} voidptr_cmp_type;

#define __  NO

extern struct aux_info *GetLangInfo( type_modifiers flags );

local  cmp_type const   CompTable[TYPE_LAST_ENTRY][TYPE_LAST_ENTRY] = {
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

local   int     TypeCheck( TYPEPTR typ1, TYPEPTR typ2 );
local   bool    IsPointer( TYPEPTR typ );


static cmp_type InUnion( TYPEPTR typ1, TYPEPTR typ2, int reversed )
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

static int ChkParmPromotion( TYPEPTR *plist, int topLevelCheck )    /* 25-nov-94 */
{
    TYPEPTR     typ;
    int         parm_count;

    parm_count = 1;
    for( ;; ) {
        typ = *plist++;
        if( typ == NULL )
            break;
        SKIP_TYPEDEFS( typ );
        switch( typ->decl_type ) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
            if( CompFlags.strict_ANSI ) {
                if( topLevelCheck ) {
                    CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
                }
                return( TC_TYPE_MISMATCH );
            }
            break;
        case TYPE_USHORT:
#if TARGET_SHORT != TARGET_INT
            if( CompFlags.strict_ANSI )  {
                if( topLevelCheck ) {
                    CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
                }
                return( TC_TYPE_MISMATCH );
            }
#endif
            break;
        case TYPE_FLOAT:
            if( topLevelCheck ) {
                CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
            }
            return( TC_TYPE_MISMATCH );
        default:
            break;
        }
    }
    return( TC_OK );
}

TYPEPTR  SkipTypeFluff( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_ENUM ) {
        typ = typ->object;
    }
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
    for( ;; ) {
        if( field1 == NULL )
            break;
        if( field2 == NULL )
            break;
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
            } else {                                /* 11-jul-90 */
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

int ChkCompatibleFunction( TYPEPTR typ1, TYPEPTR typ2, int topLevelCheck )
{
    TYPEPTR     *plist1;
    TYPEPTR     *plist2;
    int         parm_count;

    plist1 = typ1->u.fn.parms;
    plist2 = typ2->u.fn.parms;
    if( plist1 != plist2 ) {
        if( plist1 == NULL ) {
            return( ChkParmPromotion( plist2, topLevelCheck ) );
        } else if( plist2 == NULL ) {
            return( ChkParmPromotion( plist1, topLevelCheck ) );
        }
        parm_count = 1;
        for( ;; ) {
            if( *plist1 == NULL && *plist2 == NULL )
                break;
            if( *plist1 == NULL  ||  *plist2 == NULL ) {
                if( topLevelCheck ) {
                    CErr1( ERR_PARM_COUNT_MISMATCH );
                }
                return( TC_PARM_COUNT_MISMATCH );
            }
            if( ! IdenticalType( *plist1, *plist2 ) ) {
                if( topLevelCheck ) {
                    SetDiagType2( *plist1, *plist2 );
                    CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
                    SetDiagPop();
                }
                return( TC_PARM_TYPE_MISMATCH + parm_count );
            }
            ++plist1;
            ++plist2;
            ++parm_count;
        }
    }
    return( TC_OK );        /* indicate functions are compatible */
}

int ChkCompatibleLanguage( type_modifiers typ1, type_modifiers typ2 )
{
    typ1 &= FLAG_LANGUAGES;
    typ2 &= FLAG_LANGUAGES;
    if( typ1 == typ2 ) {
        return( 1 );
    } else if( typ1 == 0 ) {
        return( DftCallConv == GetLangInfo( typ2 ) );
    } else if( typ2 == 0 ) {
        return( DftCallConv == GetLangInfo( typ1 ) );
    } else  {
        return( 0 );
    }
}

#define PTR_FLAGS (FLAG_MEM_MODEL|QUAL_FLAGS)
#define QUAL_FLAGS (FLAG_CONST|FLAG_VOLATILE|FLAG_UNALIGNED)

static cmp_type DoCompatibleType( TYPEPTR typ1, TYPEPTR typ2, int top_level,
                                  voidptr_cmp_type voidptr_cmp )
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
        if( typ1->decl_type != TYPE_ARRAY && typ1->decl_type != TYPE_POINTER )
            break;
        if( typ1->decl_type == TYPE_ARRAY && typ2->decl_type == TYPE_ARRAY ) {
            /* See C99, 6.7.5.2p5 */
            if( typ1->u.array->dimension && typ2->u.array->dimension ) {
                if( typ1->u.array->dimension != typ2->u.array->dimension ) {
                    ret_val = PM;
                }
            }
        }
        if( typ1->decl_type==TYPE_POINTER ) {
            typ1_flags = typ1->u.p.decl_flags;
            typ2_flags = typ2->u.p.decl_flags;
            if( (typ1_flags & QUAL_FLAGS)!=(typ2_flags & QUAL_FLAGS) ) {
                if( ret_val == OK ) {   //PT is a worse case
                    ret_val = PQ;
                }
            }
            if( (typ1_flags & FLAG_MEM_MODEL) != (typ2_flags & FLAG_MEM_MODEL) ) {
                if( ( (typ1_flags & FLAG_MEM_MODEL) != FLAG_NONE    // if same as mem model ok
                    && (typ2_flags & FLAG_MEM_MODEL) != FLAG_NONE )
                    ||  TypeSize( typ1 ) != TypeSize( typ2 ) ) {
                    return( NO );
                }
            }
            ++top_level;
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
    }
    if( typ1 != typ2 ) {    // if not equal see if diff by pointers
        if( typ1->decl_type == TYPE_VOID || typ2->decl_type == TYPE_VOID ) {
            // allow  void ** with any ** (but warn about it)
            if( top_level==1 || !CompFlags.strict_ANSI ) {
                if( voidptr_cmp == VC_WARN || (top_level > 1) ) {
                    CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
                }
                return( ret_val ); // void *  and  anything *
            }
        }
        if( top_level > 0 ) {   //for PW both types must start as pointers
            if( typ1->decl_type == TYPE_POINTER
               && typ2->decl_type != TYPE_ARRAY ) {
                ret_val = PW;
                while( typ1->decl_type == TYPE_POINTER ) {
                    typ1 = typ1->object;
                    typ1 = SkipTypeFluff( typ1 );
                    ++top_level;
                }
            } else if( typ2->decl_type == TYPE_POINTER
                && typ1->decl_type != TYPE_ARRAY ) {
                ret_val = PW;
                while( typ2->decl_type == TYPE_POINTER ) {
                    typ2 = typ2->object;
                    typ2 = SkipTypeFluff( typ2 );
                    ++top_level;
                }
            }
        }
    }
    if( typ1->decl_type==typ2->decl_type ) {
        if( typ1->decl_type == TYPE_FUNCTION ) {
            typ1_flags = typ1->u.fn.decl_flags;
            typ2_flags = typ2->u.fn.decl_flags;
            if( !ChkCompatibleLanguage( typ1_flags, typ2_flags ) ) {
                ret_val = NO;
            } else {
                /* check to see if the two functions have identical parameters
                   and return types */
                if( ChkCompatibleFunction( typ1, typ2, 0 ) != TC_OK  ||
                    !IdenticalType( typ1->object, typ2->object ) ) {
                    CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
                }
            }
        } else if( typ1->decl_type == TYPE_STRUCT  || typ1->decl_type == TYPE_UNION ) {
           /* 11-jul-90: allow pointers to different structs */
           /* 29-oct-03: stop this for ANSI! */
            if( ( typ1 != typ2 ) ) {
                // Types are not the same
                // if extensions are enabled, then we can do a compatible struct test
                if( CompFlags.extensions_enabled ) {
                    if( CompatibleStructs( typ1->u.tag, typ2->u.tag ) != OK) {
                        if( top_level > 0 ) {
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
        } else if( (TYPE_FIELD == typ1->decl_type) || (TYPE_UFIELD == typ1->decl_type) ) { /* CarlYoung 31-Oct-03 */
            if( typ2->u.f.field_width > typ1->u.f.field_width ) {
                ret_val = AC;
            }
        }
    } else if( typ1->decl_type == TYPE_UNION && top_level > 0 ) {
        if( InUnion( typ1, typ2, 0 ) != OK ) {
            ret_val = NO;
        } else {
            ret_val = PM;
        }
    } else if( typ2->decl_type == TYPE_UNION && top_level > 0 ) {
        if( InUnion( typ2, typ1, 1 )!= OK ) {
            ret_val = NO;
        } else {
            ret_val = PM;
        }
    } else if( typ1->decl_type == TYPE_ARRAY ) {
        TYPEPTR     typ = typ1->object;

        if( !IdenticalType( typ, typ2 ) ) {
            ret_val = NO;
            while( typ->decl_type == TYPE_ARRAY ) {
                typ = typ->object;
            }
            if( IdenticalType( typ, typ2 ) ) {
                ret_val = PM;
            }
        }
    } else if( typ2->decl_type == TYPE_ARRAY ) {
        TYPEPTR     typ = typ2->object;

        if( !IdenticalType( typ, typ1 ) ) {
            ret_val = NO;
            while( typ->decl_type == TYPE_ARRAY ) {
                typ = typ->object;
            }
            if( IdenticalType( typ, typ1 ) ) {
                ret_val = PM;
            }
        }
    } else if( typ1->decl_type >= TYPE_LAST_ENTRY  ||
            typ2->decl_type >= TYPE_LAST_ENTRY ) {
        ret_val = NO;
    } else if( top_level == 0 ) {
        ret_val = CompTable[ typ1->decl_type ][ typ2->decl_type ];
    } else {
        ret_val = NO;
        switch( typ1->decl_type ) {
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
        case TYPE_LONG:
        case TYPE_LONG64:
            if( typ2->decl_type == typ1->decl_type+1 ) {
                ret_val = PS;
            } else if( TypeSize( typ1 ) ==   TypeSize( typ2 ) ) {
                ret_val = PM;
            }
            break;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
        case TYPE_ULONG:
        case TYPE_ULONG64:
            if( typ2->decl_type+1 == typ1->decl_type ) {
                ret_val = PS;
            } else if( TypeSize( typ1 ) ==   TypeSize( typ2 ) ) {
                ret_val = PM;
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
    int              top_level;

    top_level = 0;
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

            subnot = SUBNOT( typ1_flags, typ2_flags, QUAL_FLAGS );
            if( subnot ) {  // allow void * =  unaligned *
                if( subnot & (QUAL_FLAGS & ~FLAG_UNALIGNED) ) {
                    ret_pq = PQ;
                } else if( subnot & FLAG_UNALIGNED) {
                    int align1;

                    align1 = GetTypeAlignment( typ1->object );
                    if( align1 > 1 ) {
                        ret_pq = PQ;
                    }
                }
            }
            if( (typ1_flags & FLAG_MEM_MODEL)!=(typ2_flags & FLAG_MEM_MODEL) ) {
                int size1, size2;

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
        ++top_level;
    }
    ret_val = DoCompatibleType( typ1, typ2, top_level, VC_CONVERT );
    if( ret_val == OK ) {
        ret_val = ret_pq;
    }
    return( ret_val );
}

void CompatiblePtrType( TYPEPTR typ1, TYPEPTR typ2 )
{
    SetDiagType2( typ2, typ1 ); /* Called with source, target. */
    switch( CompatibleType( typ1, typ2, FALSE, FALSE ) ) {
    case PT:                                        /* 31-aug-89 */
    case PX:
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ) { // else fuck em
            CWarn1( WARN_QUALIFIER_MISMATCH, ERR_QUALIFIER_MISMATCH );
        }
        break;
    case PM:                                    /* 16-may-91 */
    case NO:
        CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        break;
    case PS:
        CWarn1( WARN_SIGN_MISMATCH, ERR_SIGN_MISMATCH );
        break;
    case PW:
        CWarn1( WARN_INCONSISTENT_INDIRECTION_LEVEL,
                    ERR_INCONSISTENT_INDIRECTION_LEVEL );
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
    bool    rc = FALSE;

    if( tree->op.opr == OPR_PUSHINT ) {
        uint64  val64 = LongValue64( tree );

        rc = !U64Test( &val64 );
    }
    return( rc );
}

static void CompareParms( TYPEPTR *master, TREEPTR parm, source_loc *src_loc )
{
    TYPEPTR     typ;
    TYPEPTR     typ2;
    int         parm_num;
    cmp_type    cmp;

    typ = *master++;
    if( typ != NULL ) {                     /* 27-feb-90 */
        if( typ->decl_type == TYPE_VOID ) { /* type func(void); */
            typ = NULL;                     /* indicate no parms */
        }
    }
    parm_num = 1;
    while( ( typ != NULL ) && ( parm != NULL ) ) {
        SKIP_TYPEDEFS( typ );
        // TODO is crap needed or has it been done
        if( typ->decl_type == TYPE_FUNCTION ) {
            typ = PtrNode( typ, FLAG_NONE, SEG_CODE );
        } else if( typ->decl_type == TYPE_ARRAY ) {
            typ = PtrNode( typ->object, FLAG_WAS_ARRAY, SEG_DATA );
        }
        typ2 = parm->expr_type;
        // typ2 will be NULL if parm is OPR_ERROR in which case an error
        // has already been generated
        if( typ2 != NULL ) {
            /* check compatibility of parms */
            SetErrLoc( src_loc );
            SetDiagType2 ( typ2, typ );
            cmp = CompatibleType( typ, typ2, TRUE, IsNullConst( parm ) );
            switch( cmp ) {
            case NO:
            case PT:
            case PX:
            case AC:
                CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
                break;
            case PQ:
                if( !CompFlags.no_check_qualifiers ) { // else fuck em
                    CWarn2( WARN_QUALIFIER_MISMATCH,
                        ERR_PARM_QUALIFIER_MISMATCH, parm_num );
                }
                break;
            case PM:                                    /* 16-may-91 */
                CWarn2( WARN_POINTER_TYPE_MISMATCH,
                        ERR_PARM_POINTER_TYPE_MISMATCH, parm_num );
                break;
            case PS:
                CWarn2( WARN_SIGN_MISMATCH,
                        ERR_PARM_SIGN_MISMATCH, parm_num );
                break;
            case PW:
                CWarn2( WARN_PARM_INCONSISTENT_INDIRECTION_LEVEL,
                        ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL, parm_num );
                break;
            case PC:  /* Allow only "void *p = int 0";  */
                if( IsPointer( typ ) && parm->right->op.opr == OPR_PUSHINT ) {
                    if( TypeSize(typ) != TypeSize(typ2) ) {
                        CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
                    } else if( parm->right->op.ulong_value != 0 ) {
                        CWarn1( WARN_NONPORTABLE_PTR_CONV,
                                ERR_NONPORTABLE_PTR_CONV );
                    }
                } else {
                    if( TypeSize(typ->object) == TypeSize(typ2->object) ) {
                        CWarn2( WARN_POINTER_TYPE_MISMATCH,
                             ERR_PARM_POINTER_TYPE_MISMATCH, parm_num );
                    } else {
                        CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
                    }
                }
                break;
            case OK:
                break;
            }
            SetDiagPop();
            InitErrLoc();
        }
        typ = *master++;
        if( typ != NULL && typ->decl_type == TYPE_DOT_DOT_DOT )
            return;
        parm = parm->left;
        ++parm_num;
    }
    if( typ != NULL || parm != NULL ) {     /* should both be NULL now */
        SetErrLoc( src_loc );
#if _CPU == 386
        /* can allow wrong number of parms with -3s option; 06-dec-91 */
        if( !CompFlags.register_conventions ) {
            CWarn1( WARN_PARM_COUNT_MISMATCH, ERR_PARM_COUNT_WARNING );
            return;
        }
#endif
        CErr1( ERR_PARM_COUNT_MISMATCH );           /* 18-feb-90 */
    }
}

extern  call_list *CallNodeList;

static TREEPTR reverse_parms_tree( TREEPTR parm ) {
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

/* Check parameters of function that were called before a prototype was seen */
extern void ChkCallParms( void )
{
    call_list   *nextcall;

    nextcall = CallNodeList;
    while( nextcall != NULL ) {
        call_list  *next;
        TREEPTR     callnode;
        TREEPTR     callsite;
        SYM_ENTRY   sym;
        TYPEPTR     typ;

        callnode = nextcall->callnode;
        if( callnode != NULL ) {
            callsite = callnode->left;      // point to OPR_FUNCNAME node
            SymGet( &sym, callsite->op.sym_handle );
            typ = sym.sym_type;
            SKIP_TYPEDEFS( typ );
            if( !(sym.flags & SYM_TEMP) )
                SetDiagSymbol( &sym, callsite->op.sym_handle );
            if( typ->u.fn.parms != NULL ) {
                TREEPTR     parms;
                bool        reverse;

                parms = callnode->right;
                reverse = ( ParmsToBeReversed( sym.attrib, NULL ) && ( parms != NULL ) );
                if( reverse ) {
                    parms = reverse_parms_tree( parms );
                }
                CompareParms( typ->u.fn.parms, parms, &nextcall->src_loc );
                if( reverse ) {
                    reverse_parms_tree( parms );
                }
            } else {
                // Unprototyped function called. Note that for indirect calls, there
                // is no symbol associated with the function and diagnostic information
                // is hence limited.
                SetErrLoc( &nextcall->src_loc );
                if( sym.flags & SYM_TEMP ) {
                    CWarn( WARN_NONPROTO_FUNC_CALLED_INDIRECT,
                            ERR_NONPROTO_FUNC_CALLED_INDIRECT );
                } else {
                    CWarn( WARN_NONPROTO_FUNC_CALLED,
                            ERR_NONPROTO_FUNC_CALLED, SymName( &sym, callsite->op.sym_handle ) );
                }
            }
            if( !(sym.flags & SYM_TEMP) ) {
                SetDiagPop();
        }
        }
        next = nextcall->next;
        CMemFree( nextcall );
        nextcall = next;
    }
}

#define MAXSIZE        (sizeof( long )*8)
static void AssRangeChk( TYPEPTR typ1, TREEPTR opnd2 )
{
    unsigned       long high;
    if( opnd2->op.opr == OPR_PUSHINT ) {
        switch( typ1->decl_type ) {
        case TYPE_FIELD:
        case TYPE_UFIELD:
            high = 0xfffffffful >> (MAXSIZE - (typ1->u.f.field_width));
            if( opnd2->op.ulong_value > high ) {
                if( (opnd2->op.ulong_value | (high >> 1)) != ~0UL ) {
                    CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                }
            }
            break;
        case TYPE_CHAR:
            if( opnd2->op.long_value > 127 ||
                opnd2->op.long_value < -128 ) {
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
            break;
        case TYPE_UCHAR:
            if( opnd2->op.ulong_value > 0xff) {
                if( (opnd2->op.ulong_value | (0xff >> 1)) != ~0UL ) {
                    CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                }
            }
            break;
        case TYPE_UINT:
            if( sizeof( target_uint ) > 2 ) {
                break;
            }
            // fall throught
        case TYPE_USHORT:
            if( opnd2->op.ulong_value > 0xffff ) {
                if( (opnd2->op.ulong_value | (0xffff >> 1)) != ~0UL ) {
                    CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                }
            }
            break;
        case TYPE_INT:
            if( sizeof( target_uint ) > 2 ) {
                break;
            }
            // fall throught
        case TYPE_SHORT:
            if( opnd2->op.long_value > 32767 ||
                opnd2->op.long_value < -32768L ) {
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
            break;
        default:
            break;
        }
    } else if( opnd2->op.opr == OPR_PUSHFLOAT ) {
        if( typ1->decl_type == TYPE_FLOAT ) {
            if( atof( opnd2->op.float_value->string ) > TARGET_FLT_MAX ) {
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
        }
    }
}

static bool IsPtrtoFunc( TYPEPTR typ )
{
    int ret;
    ret = FALSE;
    SKIP_TYPEDEFS( typ );
    if( typ->decl_type == TYPE_POINTER ) {
        typ = typ->object;
        SKIP_TYPEDEFS( typ );
        if( typ->decl_type == TYPE_FUNCTION ) {
            ret = TRUE;
        }
    }
    return( ret );
}

static bool IsPointer( TYPEPTR typ )
{
    SKIP_TYPEDEFS( typ );
    return( typ->decl_type == TYPE_POINTER );
}

void ParmAsgnCheck( TYPEPTR typ1, TREEPTR opnd2, int parm_num )
{
// TODO merge up with  ChkCalls
    TYPEPTR        typ2;

    if( opnd2->op.opr == OPR_ERROR )
        return;

    // Fold RHS expression so that we can properly check for null
    // pointers or out of range constants
    FoldExprTree( opnd2 );
    typ2 = opnd2->expr_type;

    SetDiagType2( typ2, typ1 );
    switch( CompatibleType( typ1, typ2, TRUE, IsNullConst( opnd2 ) ) ) {
    case NO:
        if( parm_num == 0 ) {
            CErr1( ERR_TYPE_MISMATCH );
        } else {
            CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
        }
        break;
    case PT:                                        /* 31-aug-89 */
        if( !IsPtrConvSafe( opnd2, typ1, typ2 ) ) {
            CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
        }
        break;
    case PX:
        if( IsPtrtoFunc( typ1 ) ) {
            CWarn1( WARN_POINTER_TYPE_MISMATCH,ERR_POINTER_TYPE_MISMATCH );
        }
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ) { // else fuck em
            if( parm_num == 0 ) {
                CWarn1( WARN_QUALIFIER_MISMATCH, ERR_QUALIFIER_MISMATCH );
            } else {
                CWarn2( WARN_QUALIFIER_MISMATCH,
                        ERR_PARM_QUALIFIER_MISMATCH, parm_num );
            }
        }
        break;
    case PM:                                    /* 16-may-91 */
        if( parm_num == 0 ) {
            CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        } else {
            CWarn2( WARN_POINTER_TYPE_MISMATCH,
                    ERR_PARM_POINTER_TYPE_MISMATCH, parm_num );
        }
        break;
    case PS:                                    /* 16-may-91 */
        if( parm_num == 0 ) {
            CWarn1( WARN_SIGN_MISMATCH, ERR_SIGN_MISMATCH );
        } else {
            CWarn2( WARN_SIGN_MISMATCH,
                    ERR_PARM_SIGN_MISMATCH, parm_num );
        }
        break;
    case PW:
        if( parm_num == 0 ) {
            CWarn1( WARN_INCONSISTENT_INDIRECTION_LEVEL,
                    ERR_INCONSISTENT_INDIRECTION_LEVEL );
        } else {
            CWarn2( WARN_PARM_INCONSISTENT_INDIRECTION_LEVEL,
                    ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL, parm_num );
        }
        break;
    case PC:  /* Allow only "void *p = int 0";  */
        if( IsPointer( typ1 ) && opnd2->op.opr == OPR_PUSHINT ) {
            if( opnd2->op.long_value != 0 ) {
                CWarn1( WARN_NONPORTABLE_PTR_CONV,
                         ERR_NONPORTABLE_PTR_CONV );
            }
        } else {
            CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
        }
        break;
    case AC:
        /* CarlYoung 31-Oct-03 */
        {
            unsigned long   fsize_1 = 0, fsize_2 = 0;

            if( TypeSizeEx( typ2, &fsize_2 ) > TypeSizeEx( typ1, &fsize_1 ) ) {
                CWarn1( WARN_LOSE_PRECISION, ERR_LOSE_PRECISION );
            }
            if( fsize_2 > fsize_1 ) {
                CWarn1( WARN_LOSE_PRECISION, ERR_LOSE_PRECISION );
            }
        }
    case OK:
        AssRangeChk( typ1, opnd2 );
        break;
    }
    SetDiagPop();
}

void TernChk( TYPEPTR typ1, TYPEPTR typ2 )
{
    cmp_type    cmp;

    cmp = CompatibleType( typ1, typ2, FALSE, FALSE );
    switch( cmp ) {
    case NO:
        CErr1( ERR_TYPE_MISMATCH );
        break;
    case PT:                                        /* 31-aug-89 */
    case PX:
        CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ) { // else fuck em
            CWarn1( WARN_QUALIFIER_MISMATCH,ERR_QUALIFIER_MISMATCH );
        }
        break;
    case PM:                                    /* 16-may-91 */
        CWarn1( WARN_POINTER_TYPE_MISMATCH, ERR_POINTER_TYPE_MISMATCH );
        break;
    case PS:                                    /* 16-may-91 */
        CWarn1( WARN_SIGN_MISMATCH, ERR_SIGN_MISMATCH );
        break;
    case PW:
        CWarn1( WARN_INCONSISTENT_INDIRECTION_LEVEL,
                 ERR_INCONSISTENT_INDIRECTION_LEVEL );
        break;
    case PC:
        CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
        break;
    case OK:
    case AC:
        break;
    }
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
        if( ret_type->u.p.segment == SEG_STACK ) {
            CWarn1( WARN_RET_ADDR_OF_AUTO, ERR_RET_ADDR_OF_AUTO );
        }
    }
    /* check that the types are compatible */
    ParmAsgnCheck( func_type, tree, 0 );
}


int IdenticalType( TYPEPTR typ1, TYPEPTR typ2 )
{
    int     rc;

    rc = TypeCheck( typ1, typ2 );
    return( ( rc == TC_OK ) || ( rc == TC_TYPE2_HAS_MORE_INFO ) );
}


int VerifyType( TYPEPTR new, TYPEPTR old, SYMPTR sym )
{
    int     rc;

    rc = TypeCheck( new, old );
    switch( rc ) {
    case TC_OK:                     /* OK */
        break;
    case TC_TYPE_MISMATCH:          /* types didn't match */
        CErr2p( ERR_TYPE_DOES_NOT_AGREE, sym->name );
        break;
    case TC_PARM_COUNT_MISMATCH:    /* parm count mismatch */
        CErr1( ERR_PARM_COUNT_MISMATCH );
        break;
    case TC_TYPE2_HAS_MORE_INFO:    /* OK, new= void *, old= something *;*/
        return( 1 );                /* indicate want old definition */
    default:                        /* parm type mismatch */
        CErr2( ERR_PARM_TYPE_MISMATCH, rc - TC_PARM_TYPE_MISMATCH );
        break;
    }
    return( 0 );
}


local int TypeCheck( TYPEPTR typ1, TYPEPTR typ2 )
{
    int                 pointer_type;
    type_modifiers      ptr_mask;
    int                 retcode;

    pointer_type = 0;
    retcode = TC_OK;
    /* "char *s" and "char s[]" differs only by FLAG_WAS_ARRAY, ignore it too */
    if( TargetSwitches & BIG_DATA ) {
        ptr_mask = ~(FLAG_FAR  | FLAG_WAS_ARRAY | FLAG_LANGUAGES);
    } else {
        ptr_mask = ~(FLAG_NEAR | FLAG_WAS_ARRAY | FLAG_LANGUAGES);
    }
    for( ;; ) {
        typ1 = SkipTypeFluff( typ1 ); // skip typedefs, go into enum base
        typ2 = SkipTypeFluff( typ2 );
        /* this compare was moved here 20-sep-88 */
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
                    return( TypeCheck( typ1->object, typ2 ) );
                }
                if( typ2->decl_type == TYPE_ARRAY ) {
                    return( TypeCheck( typ1, typ2->object ) );
                }
            }
            return( TC_TYPE_MISMATCH );
        }
        if( typ1->decl_type == TYPE_POINTER ) {
            pointer_type = 1;
            if( (typ1->u.p.decl_flags & ptr_mask) != (typ2->u.p.decl_flags & ptr_mask) ) {
                return( TC_TYPE_MISMATCH );
            }
        }
        if( TypeSize(typ1) != TypeSize(typ2) ) {
            if( TypeSize(typ1) == 0 ) {
                retcode = TC_TYPE2_HAS_MORE_INFO;
            } else if( TypeSize(typ2) == 0 ) {
                retcode = TC_OK;
            } else {
                return( TC_TYPE_MISMATCH );
            }
            if( typ1->decl_type != TYPE_ARRAY ) {
                return( retcode );
            }
        }
        /* CarlYoung 31-Oct-03 */
        if( ( TYPE_FIELD == typ1->decl_type ) || ( TYPE_UFIELD == typ1->decl_type ) ) {
            if( typ1->u.f.field_width != typ2->u.f.field_width ) {
                return( TC_TYPE_MISMATCH );
            }
        }
        if( typ1->decl_type == TYPE_STRUCT || typ1->decl_type == TYPE_UNION ) {
            /* must be the same tag to be identical, if they are the
               same tag, then typ1 == typ2 which is checked above */
            return( TC_TYPE_MISMATCH );
        }
        if( typ1->decl_type == TYPE_FUNCTION ) {
            retcode = ChkCompatibleFunction( typ1, typ2, 0 );
            if( retcode != TC_OK )
                return( retcode );
            if( typ1->object == NULL  ||  typ2->object == NULL ) {
                return( TC_OK );
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
    if( typ1 != typ2 )
        return( TC_TYPE_MISMATCH );
    return( TC_OK );                /* indicate types are identical */
}
