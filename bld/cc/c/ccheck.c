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


#include "cvars.h"
#include "cgswitch.h"


/*  return types from TypeCheck */
enum {
   TC_OK,                       /* types are OK */
   TC_TYPE_MISMATCH,            /* types mismatch */
   TC_PARM_COUNT_MISMATCH,      /* different # of parms */
   TC_TYPE2_HAS_MORE_INFO,      /* OK, but type2 has more information */
   TC_PARM_TYPE_MISMATCH        /* parms have different types */
};

typedef enum {
   NO,          /* not compatible */
   PW,          /* pointers with inconsistent levels of indirection */
   PM,          /* pointers point to different objects (Mismatch) 16-may-91 */
   PC,          /* might be compatible if integer value is 0 */
   OK,          /* compatible */
   PQ,          /* pointers to different qualified types */
   PT,          /* pointer truncated */
   PX,          /* pointer expanded   */
   PS,          /* pointer to different signed types */
   AC,          /* assignment compatible */
}cmp_type;

#define __  NO

local  cmp_type const __FAR CompTable[22][22] = {
/*               CH,UC,SH,US,IN,UI,LO,UL,DL,DU,FL,DB,PO,AR,ST,UN,FU,FI,VO,EN,TY,UF  */
/* CHAR     */ { OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* UCHAR    */ { OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* SHORT    */ { OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* USHORT   */ { OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* INT      */ { OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* UINT     */ { OK,OK,OK,OK,OK,OK,AC,AC,AC,AC,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* LONG     */ { AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* ULONG    */ { AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* LONG64   */ { AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* ULONG64  */ { AC,AC,AC,AC,AC,AC,OK,OK,OK,OK,AC,AC,PC,__,__,__,__,OK,__,OK,__,OK  },
/* FLOAT    */ { AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,OK,AC,__,__,__,__,__,AC,__,AC,__,AC  },
/* DOUBLE   */ { AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,AC,OK,__,__,__,__,__,AC,__,AC,__,AC  },
/* POINTER  */ { PC,PC,PC,PC,PC,PC,PC,PC,PC,PC,__,__,OK,__,__,__,__,__,__,__,__,__  },
/* ARRAY    */ { __,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__,__  },
/* STRUCT   */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__,__  },
/* UNION    */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__,__  },
/* FUNCTION */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__,__,__,__,__  },
/* FIELD    */ { OK,OK,OK,OK,OK,OK,OK,OK,OK,OK,AC,AC,__,__,__,__,__,OK,__,__,__,OK  },
/* VOID     */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__  },
/* ENUM     */ { OK,OK,OK,OK,OK,OK,OK,OK,OK,OK,AC,AC,__,__,__,__,__,__,__,OK,__,__  },
/* TYPEDEF  */ { __,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,__,OK,__  },
/* UFIELD   */ { OK,OK,OK,OK,OK,OK,OK,OK,OK,OK,AC,AC,__,__,__,__,__,OK,__,__,__,OK  }
};

static cmp_type CompatibleType( TYPEPTR typ1, TYPEPTR typ2, int assignment );
static cmp_type DoCompatibleType( TYPEPTR typ1, TYPEPTR typ2, int top_level );

static cmp_type InUnion( TYPEPTR typ1, TYPEPTR typ2, int reversed )
{
    FIELDPTR    field;

    /* typ1->decl_type == TYPE_UNION */
    if( typ2->decl_type == TYPE_UNION ) return( NO );
    field = typ1->u.tag->u.field_list;
    while( field != NULL ) {
        if( reversed ) {
            if( IdenticalType( typ2, field->field_type ) ) return( OK );
        } else {
            if( IdenticalType( field->field_type, typ2 ) ) return( OK );
        }
        field = field->next_field;
    }
    return( NO );
}

void ChkParmPromotion( TYPEPTR *plist )    /* 25-nov-94 */
{
    TYPEPTR     typ;
    int         parm_count;

    parm_count = 1;
    for(;;) {
        typ = *plist++;
        if( typ == NULL ) break;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        switch( typ->decl_type ) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
            if( CompFlags.strict_ANSI ){
                CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
            }
            break;
        case TYPE_USHORT:
            #if TARGET_SHORT != TARGET_INT
                if( CompFlags.strict_ANSI )  {
                    CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
                }
            #endif
            break;
        case TYPE_FLOAT:
            CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
            break;
        }
    }
    return;
}

TYPEPTR  SkipTypeFluff( TYPEPTR typ ){
    while( typ->decl_type == TYPE_TYPEDEF ) {
        typ = typ->object;
    }
    if( typ->decl_type == TYPE_ENUM ) typ = typ->object;
    return( typ );
}


void ChkParmList( TYPEPTR *plist1 ,TYPEPTR *plist2 ){
/**************************************************/
    int         parm_count;
    cmp_type    cmp;
    TYPEPTR     ptype1;
    TYPEPTR     ptype2;

    parm_count = 1;
    for(;;) {
        ptype1 = *plist1;
        ptype2 = *plist2;
        if( ptype1 == NULL  ||  ptype2 == NULL ) break;
        cmp = DoCompatibleType( ptype1, ptype2, 0 );
        switch( cmp ){
        case NO:
        case PT:
        case PX:
        case AC:
        case PC:
        case PM:                                    /* 16-may-91 */
            CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
            break;
        case PQ:
            if( !CompFlags.no_check_qualifiers ){ // else fuck em
                CWarn2( WARN_QUALIFIER_MISMATCH,
                        ERR_PARM_QUALIFIER_MISMATCH, parm_count );
            }
            break;
        case PS:
            CWarn2( WARN_SIGN_MISMATCH,
                 ERR_PARM_SIGN_MISMATCH,parm_count );
            break;
        case PW:
            CErr2( ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL, parm_count );
            break;
        }
        ++plist1;
        ++plist2;
        ++parm_count;
    }
    if( ptype1 != ptype2  ) {
        CErr1( ERR_PARM_COUNT_MISMATCH );
    }
}

static cmp_type CompatibleStructs( TAGPTR tag1, TAGPTR tag2 )
{
    FIELDPTR    field1;
    FIELDPTR    field2;
    TYPEPTR     typ1;
    TYPEPTR     typ2;

    if( tag1 == tag2 )  return( OK );
    if( tag1->size != tag2->size )  return( NO );
    field1 = tag1->u.field_list;
    field2 = tag2->u.field_list;
    for(;;) {
        if( field1 == NULL ) break;
        if( field2 == NULL ) break;
        typ1 = field1->field_type;
        while( typ1->decl_type == TYPE_TYPEDEF ) typ1 = typ1->object;
        typ2 = field2->field_type;
        while( typ2->decl_type == TYPE_TYPEDEF ) typ2 = typ2->object;
        if( ! IdenticalType( typ1, typ2 ) ) {
            if( (typ1->decl_type == TYPE_STRUCT  &&
                 typ2->decl_type == TYPE_STRUCT )    ||
                (typ1->decl_type == TYPE_UNION   &&
                 typ2->decl_type == TYPE_UNION  )    ) {
                    if( CompatibleStructs( typ1->u.tag, typ2->u.tag )
                            != OK ) {
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
    if( field1 != NULL || field2 != NULL ) return( NO );
    return( OK );
}

local int CheckDefaultArgPromotion( TYPEPTR *plist )    /* 25-nov-94 */
{
    TYPEPTR     typ;

    for(;;) {
        typ = *plist++;
        if( typ == NULL ) break;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        switch( typ->decl_type ) {
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
            if( CompFlags.strict_ANSI ) return( TC_TYPE_MISMATCH );
            break;
        case TYPE_USHORT:
            #if TARGET_SHORT != TARGET_INT
                if( CompFlags.strict_ANSI ) return( TC_TYPE_MISMATCH );
            #endif
            break;
        case TYPE_FLOAT:
            return( TC_TYPE_MISMATCH );
            break;
        }
    }
    return( TC_OK );
}


local int CompatibleFunction( TYPEPTR typ1, TYPEPTR typ2 )
{
    TYPEPTR     *plist1;
    TYPEPTR     *plist2;
    int         parm_count;
    cmp_type    cmp;

    plist1 = typ1->u.parms;
    plist2 = typ2->u.parms;
    if( plist1 != plist2 ) {
        if( plist1 == NULL ) {
            return CheckDefaultArgPromotion( plist2 );
        } else if( plist2 == NULL ) {
            return CheckDefaultArgPromotion( plist1 );
        }
        parm_count = 1;
        for(;;) {
            if( *plist1 == NULL  &&  *plist2 == NULL ) break;
            if( *plist1 == NULL  ||  *plist2 == NULL ) {
                return( TC_PARM_COUNT_MISMATCH );
            }
            if( CompFlags.strict_ANSI ) {               /* 22-nov-94 */
                if( ! IdenticalType( *plist1, *plist2 ) ) {
                    return( TC_PARM_TYPE_MISMATCH + parm_count );
                }
            } else{
                cmp = DoCompatibleType( *plist1, *plist2, 0 );
                switch( cmp ){
                case NO:
                case PT:
                case PX:
                case AC:
                case PC:
                case PM:                                    /* 16-may-91 */
                case PS:
                    CErr2( ERR_PARM_TYPE_MISMATCH, parm_count );
                    return( TC_PARM_TYPE_MISMATCH + parm_count );
                    break;
                case PQ:
                    if( !CompFlags.no_check_qualifiers ){ // else fuck em
                        CWarn2( WARN_QUALIFIER_MISMATCH,
                            ERR_PARM_QUALIFIER_MISMATCH, parm_count );
                    }
                    break;
                case PW:
                    CErr2( ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL, parm_count );
                    return( TC_PARM_TYPE_MISMATCH + parm_count );
                    break;
                }
            }
            ++plist1;
            ++plist2;
            ++parm_count;
        }
    }
    return( TC_OK );        /* indicate functions are compatible */
}

#define PTR_FLAGS (FLAG_MEM_MODEL|QUAL_FLAGS)
#define QUAL_FLAGS (FLAG_CONST|FLAG_VOLATILE|FLAG_UNALIGNED)

#if 0 // save till i figure out what to do
static cmp_type DoIndenticalType( TYPEPTR typ1, TYPEPTR typ2 ){
//  don't let anything screwy like x*[4] int = x*int
    cmp_type        ret_val;
    type_modifiers  typ1_flags, typ2_flags;
    int             size1,size2;

    typ1_flags = FLAG_NONE;
    typ2_flags = FLAG_NONE;
    ret_val = OK;
    for( ;; ) {   // * [] loop
        if( typ1 == typ2 )break;
        typ1 = SkipTypeFluff( typ1 ); // let typedefs and enums reduce
        typ2 = SkipTypeFluff( typ2 );
        if( typ1->decl_type != typ2->decl_type )break;
        switch( typ1->decl_type ){
        case TYPE_CHAR:
        case TYPE_UCHAR:
        case TYPE_SHORT:
        case TYPE_USHORT:
        case TYPE_INT:
        case TYPE_UINT:
        case TYPE_LONG:
        case TYPE_ULONG:
        case TYPE_LONG64:
        case TYPE_ULONG64:
        case TYPE_FLOAT:
        case TYPE_DOUBLE:
        case TYPE_DOT_DOT_DOT:
        case TYPE_VOID:
        case TYPE_ENUM:
        case TYPE_PLAIN_CHAR:
        case TYPE_WCHAR:
            goto done_check;
            break;
        case TYPE_POINTER:
            typ1_flags = typ1->u.p.decl_flags;
            typ2_flags = typ2->u.p.decl_flags;
            if( (typ1_flags & QUAL_FLAGS)!=(typ2_flags & QUAL_FLAGS) ) {
                if( ret_val == OK ){ //PT is a worse case
                    ret_val = PQ;
                }
            }
            if( (typ1_flags & FLAG_MEM_MODEL)!=(typ2_flags & FLAG_MEM_MODEL) ){
                if( (typ1_flags & FLAG_MEM_MODEL) == FLAG_NONE
                 || (typ2_flags & FLAG_MEM_MODEL) == FLAG_NONE  ){
                 // check out ambient model
                    size1 = TypeSize( typ1 );
                    size2 = TypeSize( typ2 );
                    if( size1 != size2 ){
                        ret_val = PT;
                    }
                }else{
                    ret_val = PT;
                }
            }
            break;
        case TYPE_ARRAY:
             size1 = typ1->u.array->dimension;
             size2 = typ1->u.array->dimension;
             if( size1 != size2 ){
                ret_val = NO;
                goto done_check;
             }
             break;
        case TYPE_STRUCT:
        case TYPE_UNION:
            ret_val = NO;
            goto done_check;
        case TYPE_FUNCTION:
            if( (typ1_flags & FLAG_LANGUAGES) != (typ2_flags & FLAG_LANGUAGES) ){
                ret_val = NO;
            }else if( CompatibleFunction( typ1, typ2 ) != TC_OK  ){
                ret_val = NO;
            }
            goto done_check;
        default:
            ;//assert
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
    }
done_check:
    return( ret_val );
}
#endif
static cmp_type DoCompatibleType( TYPEPTR typ1, TYPEPTR typ2, int top_level )
{
    cmp_type         ret_val;
    type_modifiers   typ1_flags, typ2_flags;

    typ1_flags = FLAG_NONE;
    typ2_flags = FLAG_NONE;
    ret_val = OK;
    typ1 = SkipTypeFluff( typ1 ); // skip typedefs go into enums base
    typ2 = SkipTypeFluff( typ2 );
    for( ;; ) {   // * [] loop
        if( typ1 == typ2 )break;
        typ1 = SkipTypeFluff( typ1 ); // skip typedefs go into enums base
        typ2 = SkipTypeFluff( typ2 );
        if( typ1->decl_type != typ2->decl_type )break;
        if( typ1->decl_type != TYPE_ARRAY && typ1->decl_type != TYPE_POINTER )break;
        if( typ1->decl_type==TYPE_POINTER ){
            typ1_flags = typ1->u.p.decl_flags;
            typ2_flags = typ2->u.p.decl_flags;
            if( (typ1_flags & QUAL_FLAGS)!=(typ2_flags & QUAL_FLAGS) ) {
                if( ret_val == OK ){ //PT is a worse case
                    ret_val = PQ;
                }
            }
            if( (typ1_flags & FLAG_MEM_MODEL)!=(typ2_flags & FLAG_MEM_MODEL) ){
                if( (typ1_flags & FLAG_MEM_MODEL) != FLAG_NONE   // if same as mem model ok
                 && (typ2_flags & FLAG_MEM_MODEL) != FLAG_NONE
                 ||  TypeSize( typ1 ) != TypeSize( typ2 )  ){
                    return( NO );
                }
            }
            ++top_level;
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
    }
    if( typ1->decl_type == TYPE_VOID || typ2->decl_type == TYPE_VOID ){
    // allow  void ** with any **
        if( top_level==1 || !CompFlags.strict_ANSI ){
            return( ret_val ); // void *  and  anything *
        }
    }
    if( typ1 != typ2 ){   // if not equal see if diff by pointers
        if( top_level > 0 ){ //for PW both types must start as pointers
            if( typ1->decl_type == TYPE_POINTER
               && typ2->decl_type != TYPE_ARRAY ){
                ret_val = PW;
                while( typ1->decl_type == TYPE_POINTER ){
                    typ1 = typ1->object;
                    typ1 = SkipTypeFluff( typ1 );
                    ++top_level;
                }
            }else if( typ2->decl_type == TYPE_POINTER
                && typ1->decl_type != TYPE_ARRAY ){
                ret_val = PW;
                while( typ2->decl_type == TYPE_POINTER ){
                    typ2 = typ2->object;
                    typ2 = SkipTypeFluff( typ2 );
                    ++top_level;
                }
            }
        }
    }
    if( typ1->decl_type==typ2->decl_type ) {
        if( typ1->decl_type == TYPE_FUNCTION ) {
            if( (typ1_flags & FLAG_LANGUAGES) != (typ2_flags & FLAG_LANGUAGES) ){
                ret_val = NO;
            }else if( (typ1!=typ2) && (CompatibleFunction( typ1, typ2 )!=TC_OK)  ){
                ret_val = NO;
            }
        }else if( typ1->decl_type == TYPE_STRUCT  || typ1->decl_type == TYPE_UNION ) {
           /* 11-jul-90: allow pointers to different structs */
            if( (typ1!=typ2) && (CompatibleStructs( typ1->u.tag, typ2->u.tag )!=OK) ) {
                if( CompFlags.extensions_enabled && (top_level>0) ){
                    if( ret_val != PW ){
                        ret_val = PM;
                    }else{
                        ret_val = NO;
                    }
                }else{
                    ret_val = NO;
                }
            }
        }
    }else if( typ1->decl_type == TYPE_UNION ) {
        if( InUnion( typ1, typ2, 0 ) != OK ){
            ret_val = NO;
        }
    }else if( typ2->decl_type == TYPE_UNION ) {
        if( InUnion( typ2, typ1, 1 )!= OK ){
            ret_val = NO;
        }
    }else if( typ1->decl_type == TYPE_ARRAY ){
        if( !IdenticalType( typ1->object, typ2 ) ){
            ret_val = NO;
        }
    }else if( typ2->decl_type == TYPE_ARRAY ){
        if( !IdenticalType( typ2->object, typ1 ) ){
            ret_val = NO;
        }
    }else if( typ1->decl_type >= TYPE_DOT_DOT_DOT  ||     /* 15-may-92 */
            typ2->decl_type >= TYPE_DOT_DOT_DOT ) {
            ret_val = NO;
    }else if( top_level == 0 ){
        ret_val = CompTable[ typ1->decl_type ][ typ2->decl_type ];
    }else{
        ret_val = NO;
        switch( typ1->decl_type ){
        case TYPE_CHAR:
        case TYPE_SHORT:
        case TYPE_INT:
        case TYPE_LONG:
        case TYPE_LONG64:
            if( typ2->decl_type == typ1->decl_type+1 ){
                ret_val = PS;
            }else if( TypeSize( typ1 ) ==   TypeSize( typ2 ) ){
                ret_val = PM;
            }
            break;
        case TYPE_UCHAR:
        case TYPE_USHORT:
        case TYPE_UINT:
        case TYPE_ULONG:
        case TYPE_ULONG64:
            if( typ2->decl_type+1 == typ1->decl_type ){
                ret_val = PS;
            }else if( TypeSize( typ1 ) ==   TypeSize( typ2 ) ){
                ret_val = PM;
            }
            break;
        }
    }
    return( ret_val );
}
#define SUBNOT( a, b, on )  ( ( (a&on)|(b&on) )^(a&on) )
static cmp_type CompatibleType( TYPEPTR typ1, TYPEPTR typ2, int assignment )
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
    if( typ1->decl_type == TYPE_POINTER && typ2->decl_type == TYPE_POINTER ){
    // top level pointer
        typ1_flags = typ1->u.p.decl_flags;
        typ2_flags = typ2->u.p.decl_flags;
        if( assignment ){
            type_modifiers subnot;

            subnot = SUBNOT(typ1_flags, typ2_flags, QUAL_FLAGS);
            if( subnot ){  // allow void * =  unaligned *
                if( subnot & (QUAL_FLAGS & ~FLAG_UNALIGNED)){
                    ret_pq = PQ;
                }else if( subnot & FLAG_UNALIGNED){
                    int align1;

                    align1 = GetTypeAlignment( typ1->object );
                    if( align1 > 1 ){
                        ret_pq = PQ;
                    }
                }
            }
            if( (typ1_flags & FLAG_MEM_MODEL)!=(typ2_flags & FLAG_MEM_MODEL) ){
                int size1, size2;

                size1 = TypeSize( typ1 );
                size2 = TypeSize( typ2 );
                if( size1 < size2 ){
                   ret_pq = PT;
                }else if( size1 > size2 ){
                    ret_pq = PX;
                }
            }
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
        ++top_level;
    }
    ret_val = DoCompatibleType( typ1, typ2, top_level );
    if( ret_val == OK ){
        ret_val = ret_pq;
    }
    return( ret_val );
}

void CompatiblePtrType( TYPEPTR typ1, TYPEPTR typ2 )
{
    switch( CompatibleType( typ1, typ2, 0 ) ){
    case PT:                                        /* 31-aug-89 */
    case PX:
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ){ // else fuck em
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
}

static void CompareParms( TYPEPTR *master,
                          TREEPTR *passed,
                          int source_fno,
                          int call_line )
{
    TYPEPTR     typ;
    TYPEPTR     typ2;
    int         parm_num;
    TREEPTR     parm;
    FNAMEPTR    flist;
    char        *filename;

    cmp_type    cmp;

    typ = *master;
    if( typ != NULL ) {                     /* 27-feb-90 */
        if( typ->decl_type == TYPE_VOID ) { /* type func(void); */
            typ = NULL;                     /* indicate no parms */
        }
    }
    ErrLine = call_line;
    flist  = FileIndexToFName( source_fno );
    if( CompFlags.ef_switch_used ){
        filename = FNameFullPath( flist );
    }else{
        filename = flist->name;
    }
    parm_num = 1;
    while( typ != NULL  &&  *passed != 0 ) {
        SymLoc = filename;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        //TODO is crap needed or has it been done
        if( typ->decl_type == TYPE_FUNCTION ) {
            typ = PtrNode( typ, FLAG_NONE, SEG_CODE );
        } else if( typ->decl_type == TYPE_ARRAY ) {
            typ = PtrNode( typ->object,
                            FLAG_WAS_ARRAY,
                           SEG_DATA );
        }
        parm = *passed;
        typ2 = parm->expr_type;
        // typ2 will be NULL if parm is OPR_ERROR in which case an error
        // has already been generated
        if( typ2 != NULL ) {
            /* check compatibility of parms */
            cmp = CompatibleType( typ, typ2, 1 );
            switch( cmp ){
            case NO:
            case PT:
            case PX:
            case AC:
                CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
                break;
            case PQ:
                if( !CompFlags.no_check_qualifiers ){ // else fuck em
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
            case PC:
                if( parm->right->op.opr == OPR_PUSHINT ) {
                    if( TypeSize(typ) != TypeSize(typ2) ) {
                        CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
                    }else if( parm->right->op.ulong_value != 0 ) {
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
            }
        }
        ++master;
        typ = *master;
        ++passed;
        if( typ == NULL ) break;
        if( typ->decl_type == TYPE_DOT_DOT_DOT ) return;
        ++parm_num;
    }
    if( typ != NULL || *passed != 0 ) {     /* should both be NULL now */
#if _CPU == 386
        /* can allow wrong number of parms with -3s option; 06-dec-91 */
        if( ! CompFlags.register_conventions ) {
            SymLoc = filename;
            CWarn1( WARN_PARM_COUNT_MISMATCH, ERR_PARM_COUNT_WARNING );
            SymLoc = NULL;
            return;
        }
#endif
        SymLoc = filename;
//          CWarn1( WARN_PARM_COUNT_MISMATCH, ERR_PARM_COUNT_MISMATCH );
        CErr1( ERR_PARM_COUNT_MISMATCH );           /* 18-feb-90 */
    }
    SymLoc = NULL;
}

extern  call_list *CallNodeList;

extern void ChkCallParms( void )
{
    call_list  *nextcall;
    TREEPTR     *actualparmlist;

    actualparmlist = (TREEPTR *)&ValueStack[0];
    nextcall = CallNodeList;
    while( nextcall != NULL ){
        call_list  *next;
        TREEPTR     callnode;
        TREEPTR     callsite;
        SYM_ENTRY   sym;
        TYPEPTR     typ;

        callnode = nextcall->callnode;
        if( callnode != NULL ){
            callsite = callnode->left;      // point to OPR_FUNCNAME node
            SymGet( &sym, callsite->op.sym_handle );
            typ = sym.sym_type;
            while( typ->decl_type == TYPE_TYPEDEF )  typ = typ->object;
            if( typ->u.parms != NULL ) {
                unsigned    parm_count;
                TREEPTR     parm;

                parm = callnode->right;
                parm_count = 0;
                while( parm != NULL ) {
                    actualparmlist[parm_count] = parm;
                    ++parm_count;
                    parm = parm->left;
                }
                actualparmlist[parm_count] = NULL;
                if( ParmsToBeReversed( sym.attrib, NULL ) ) {
                    int         i, j;
                    TREEPTR     tmp;

                    j = parm_count - 1;
                    for( i = 0; i < j; ++i, --j ) {
                        tmp = actualparmlist[i];
                        actualparmlist[i] = actualparmlist[j];
                        actualparmlist[j] = tmp;
                    }
                }
                CompareParms( typ->u.parms, actualparmlist,
                                    nextcall->source_fno,
                                    nextcall->srclinenum );
            }
        }
        next = nextcall->next;
        CMemFree( nextcall  );
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
            high = 0xfffffffful >> MAXSIZE-typ1->u.f.field_width;
            if( opnd2->op.ulong_value > high ){
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
            if( opnd2->op.ulong_value > 0xff){
                if( (opnd2->op.ulong_value | (0xff >> 1)) != ~0UL ) {
                    CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                }
            }
            break;
        case TYPE_UINT:
            if( sizeof( target_uint ) > 2 ) break;
        case TYPE_USHORT:
            if( opnd2->op.ulong_value > 0xffff ){
                if( (opnd2->op.ulong_value | (0xffff >> 1)) != ~0UL ) {
                    CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
                }
            }
            break;
        case TYPE_INT:
            if( sizeof( target_uint ) > 2 ) break;
        case TYPE_SHORT:
            if( opnd2->op.long_value > 32767 ||
                opnd2->op.long_value < -32768L ) {
                CWarn1( WARN_CONSTANT_TOO_BIG, ERR_CONSTANT_TOO_BIG );
            }
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

static bool IsPtrtoFunc( TYPEPTR typ  ){
    int ret;
    ret = FALSE;
    while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
    if( typ->decl_type == TYPE_POINTER ) {
        typ = typ->object;
        while( typ->decl_type == TYPE_TYPEDEF ) typ = typ->object;
        if( typ->decl_type == TYPE_FUNCTION ) {
            ret = TRUE;
        }
    }
    return( ret );
}

void AsgnCheck( TYPEPTR typ1, TREEPTR opnd2 )
{
    TYPEPTR        typ2;

    if( opnd2->op.opr == OPR_ERROR )  return;
    typ2 = opnd2->expr_type;

    switch( CompatibleType( typ1, typ2, 1 ) ) {
    case NO:
        CErr1( ERR_TYPE_MISMATCH );
        break;
    case PT:                                        /* 31-aug-89 */
        CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
        break;
    case PX:
        if( IsPtrtoFunc( typ1 ) ){
            CWarn1( WARN_POINTER_TYPE_MISMATCH,ERR_POINTER_TYPE_MISMATCH );
        }
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ){ // else fuck em
            CWarn1( WARN_QUALIFIER_MISMATCH, ERR_QUALIFIER_MISMATCH );
        }
        break;
    case PM:                                    /* 16-may-91 */
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
        if( opnd2->op.opr == OPR_PUSHINT ) {
            if( opnd2->op.long_value != 0 ) {
                CWarn1( WARN_NONPORTABLE_PTR_CONV,
                    ERR_NONPORTABLE_PTR_CONV );
            }
        } else {
            CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
        }
        break;
    case OK:
    case AC:
        AssRangeChk( typ1, opnd2 );
    }
}

void ParmAsgnCheck( TYPEPTR typ1, TREEPTR opnd2, int parm_num )
{
//TODO merge up with AsgnCheck & ChkCalls
    TYPEPTR        typ2;

    if( opnd2->op.opr == OPR_ERROR )  return;
    typ2 = opnd2->expr_type;

    switch( CompatibleType( typ1, typ2, 1 ) ) {
    case NO:
        CErr2( ERR_PARM_TYPE_MISMATCH, parm_num );
        break;
    case PT:                                        /* 31-aug-89 */
        CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
        break;
    case PX:
        if( IsPtrtoFunc( typ1 ) ){
            CWarn1( WARN_POINTER_TYPE_MISMATCH,ERR_POINTER_TYPE_MISMATCH );
        }
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ){ // else fuck em
            CWarn2( WARN_QUALIFIER_MISMATCH,
                    ERR_PARM_QUALIFIER_MISMATCH, parm_num );
        }
        break;
    case PM:                                    /* 16-may-91 */
        CWarn2( WARN_POINTER_TYPE_MISMATCH,
                ERR_PARM_POINTER_TYPE_MISMATCH, parm_num );
        break;
    case PS:                                    /* 16-may-91 */
        CWarn2( WARN_SIGN_MISMATCH,
                ERR_PARM_SIGN_MISMATCH, parm_num );
        break;
    case PW:
        CWarn2( WARN_PARM_INCONSISTENT_INDIRECTION_LEVEL,
                ERR_PARM_INCONSISTENT_INDIRECTION_LEVEL, parm_num );
                break;
        break;
    case PC:
        if( opnd2->op.opr == OPR_PUSHINT ) {
            if( opnd2->op.long_value != 0 ) {
                CWarn1( WARN_NONPORTABLE_PTR_CONV,
                    ERR_NONPORTABLE_PTR_CONV );
            }
        } else {
            CWarn1( WARN_PCTYPE_MISMATCH, ERR_PCTYPE_MISMATCH );
        }
        break;
    case OK:
    case AC:
        AssRangeChk( typ1, opnd2 );
    }
}

void TernChk( TYPEPTR typ1, TYPEPTR typ2 )
{
    cmp_type    cmp;

    cmp = CompatibleType( typ1, typ2, 0 );
    switch( cmp ){
    case NO:
        CErr1( ERR_TYPE_MISMATCH );
        break;
    case PT:                                        /* 31-aug-89 */
    case PX:
        CWarn1( WARN_POINTER_TRUNCATION, ERR_POINTER_TRUNCATION );
        break;
    case PQ:
        if( !CompFlags.no_check_qualifiers ){ // else fuck em
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

    if( tree->op.opr == OPR_ERROR )  return;
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
    AsgnCheck( func_type, tree );
}


int IdenticalType( TYPEPTR typ1, TYPEPTR typ2 )
{
    int rc;

    rc = TypeCheck( typ1, typ2 );
    return( rc == TC_OK  ||  rc == TC_TYPE2_HAS_MORE_INFO );
}


int VerifyType( TYPEPTR new, TYPEPTR old, SYMPTR sym )
{
    int rc;

    rc = TypeCheck( new, old );
    switch( rc ) {
    case TC_OK:                     /* OK */
        break;
    case TC_TYPE_MISMATCH:          /* types didn't match */
        while( new->decl_type == TYPE_TYPEDEF ) new = new->object;
        while( old->decl_type == TYPE_TYPEDEF ) old = old->object;
        /*
            types won't match for:

            extern char a[];        char a[200];
            char a[200];            extern char a[];
        */
        if(( new->decl_type == TYPE_ARRAY )                 &&
           ( old->decl_type == TYPE_ARRAY )                 &&
           ( IdenticalType( new->object, old->object ) ) ) {
            if( TypeSize(new) != 0 ) {
                if( TypeSize(old) == 0 ) {
                    /* let it go but indicate that the new type holds */
                }
            } else {
                if( TypeSize(old) != 0 ) {
                    /* let it go */
                    return( 1 );
                    break;
                }
            }
        }
        CErr2p( ERR_TYPE_DOES_NOT_AGREE, sym->name );
        break;
    case TC_PARM_COUNT_MISMATCH:    /* parm count mismatch */
        CErr1( ERR_PARM_COUNT_MISMATCH );
        break;
    case TC_TYPE2_HAS_MORE_INFO:    /* OK, new= void *, old= something *;*/
        return( 1 );        /* indicate want old definition */
        break;
    default:        /* parm type mismatch */
        CErr2( ERR_PARM_TYPE_MISMATCH, rc - TC_PARM_TYPE_MISMATCH );
        break;
    }
    return( 0 );
}


local int TypeCheck( TYPEPTR typ1, TYPEPTR typ2 )
{
    int                 pointer_type;
    type_modifiers      mask;
    int                 retcode;

    pointer_type = 0;
    mask = ~FLAG_NONE;                      /* 23-jun-89 */
    if( TargetSwitches & BIG_DATA )  mask = ~FLAG_FAR;
    for( ;; ) {
        typ1 = SkipTypeFluff( typ1 );
        typ2 = SkipTypeFluff( typ2 );
        /* this compare was moved here 20-sep-88 */
        /* ptr to typedef struct failed when this was before typedef skips*/
        if( typ1 == typ2 ) return( TC_OK );
        if( typ1->decl_type != typ2->decl_type ) {
            if( pointer_type ) {
                if( typ2->decl_type == TYPE_VOID ) return( TC_OK );
                if( typ1->decl_type == TYPE_VOID ) {
                    return( TC_TYPE2_HAS_MORE_INFO );
                }
                if( typ1->decl_type == TYPE_ARRAY ) {
                    return( TypeCheck( typ1->object, typ2 ) );
                }
                if( typ2->decl_type == TYPE_ARRAY ) {
                    return( TypeCheck( typ1, typ2->object ) );
                }
            }
            return( TC_TYPE_MISMATCH );
        }
        if( typ1->decl_type == TYPE_POINTER ){
            pointer_type = 1;
            if( (typ1->u.p.decl_flags & mask) != (typ2->u.p.decl_flags & mask) ){
                 return( TC_TYPE_MISMATCH );
            }
        }
        if( TypeSize(typ1) != TypeSize(typ2) ) {
            if( typ1->decl_type == TYPE_ARRAY ) {       /* 09-mar-94 */
                if( TypeSize(typ1) == 0 ) {
                    return( TC_TYPE2_HAS_MORE_INFO );
                }
                if( TypeSize(typ2) == 0 ) {
                    return( TC_OK );
                }
            }
            return( TC_TYPE_MISMATCH );
        }
        if( typ1->decl_type == TYPE_STRUCT  ||
            typ1->decl_type == TYPE_UNION ) {
            /* must be the same tag to be identical, if they are the
               same tag, then typ1 == typ2 which is checked above */
            return( TC_TYPE_MISMATCH );
        }
        if( typ1->decl_type == TYPE_FUNCTION ) {
            retcode = CompatibleFunction( typ1, typ2 );
            if( retcode != TC_OK ) return( retcode );
            if( typ1->object == NULL  ||  typ2->object == NULL ) {
                return( TC_OK );
            }
        }
        typ1 = typ1->object;
        typ2 = typ2->object;
        if( typ1 == NULL ) break;
        if( typ2 == NULL ) break;
    }
    if( typ1 != typ2 ) return( TC_TYPE_MISMATCH );
    return( TC_OK );                /* indicate types are identical */
}
