/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2019 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Initialization of data ( e.g. int foo[] = { 0, 1, 2 }; )
*
****************************************************************************/

#include "cvars.h"
#include <limits.h>
#include "i64.h"
#include "cfeinfo.h"
#include "cgswitch.h"

#define DATA_QUAD_SEG_SIZE      (32 * 1024)
#define DATA_QUADS_PER_SEG      (DATA_QUAD_SEG_SIZE / sizeof( DATA_QUAD_LIST ))

#define MAX_DATA_QUAD_SEGS (LARGEST_DATA_QUAD_INDEX / DATA_QUADS_PER_SEG + 1)

/* use a double-linked list of dataquads to facilitate insertions */
typedef struct data_quad_list {
    DATA_QUAD               dq;
    target_size             size;
    struct data_quad_list   *prev, *next;
} DATA_QUAD_LIST;

static DATA_QUAD_LIST   *DataQuadSegs[MAX_DATA_QUAD_SEGS];/* segments for data quads*/
static DATA_QUAD_LIST   *CurDataQuad;
static int              DataQuadSegIndex;
static int              DataQuadIndex;
static DATA_QUAD_LIST   *NewDataQuad( void );
static void StaticInitializer(SYMPTR sym, SYM_HANDLE handle, TYPEPTR type,
                              TREEPTR tree);

bool DataQuadsAvailable( void )
{
    DATA_QUAD_LIST  *dql = DataQuadSegs[0];

    return( dql != NULL && dql->next != NULL );
}

void InitDataQuads( void )
{
    DataQuadIndex = DATA_QUADS_PER_SEG;
    DataQuadSegIndex = -1;
    memset( DataQuadSegs, 0, sizeof( DataQuadSegs ) );
    /* put a guard at the start */
    CurDataQuad = NewDataQuad();
    CurDataQuad->prev = NULL;
    CurDataQuad->next = NULL;
    CurDataQuad->size = 0;
}

void FreeDataQuads( void )
{
    unsigned    i;

    for( i = 0; i < MAX_DATA_QUAD_SEGS; i++ ) {
        if( DataQuadSegs[i] == NULL )
            break;
        FEfree( DataQuadSegs[i] );
        DataQuadSegs[i] = NULL;
    }
}

void *StartDataQuadAccess( void )
{
    void    *cur_dqp;

    if( DataQuadsAvailable() ) {
        cur_dqp = CurDataQuad;
        CurDataQuad = DataQuadSegs[0]->next;
        return( cur_dqp );              // indicate data quads exist
    }
    return( NULL );                     // indicate no data quads
}

void EndDataQuadAccess( void *p )
{
    CurDataQuad = p;
}

DATA_QUAD *NextDataQuad(target_size *len)
{
    DATA_QUAD   *dq_ptr;

    if( CurDataQuad == NULL )
        return( NULL );
    dq_ptr = &CurDataQuad->dq;
    if (len) *len = CurDataQuad->size;
    CurDataQuad = CurDataQuad->next;
    return( dq_ptr );
}

static DATA_QUAD_LIST *NewDataQuad( void )
{
    static DATA_QUAD_LIST   *DataQuadPtr;
    DATA_QUAD_LIST          *dql;

    if( DataQuadIndex >= (DATA_QUADS_PER_SEG - 1) ) {
        if( DataQuadSegIndex == MAX_DATA_QUAD_SEGS ) {
            CErr1( ERR_INTERNAL_LIMIT_EXCEEDED );
            CSuicide();
        }
        ++DataQuadSegIndex;
        DataQuadIndex = 0;
        DataQuadPtr = FEmalloc( DATA_QUAD_SEG_SIZE );
        DataQuadSegs[DataQuadSegIndex] = DataQuadPtr;
    }
    dql = DataQuadPtr;
    ++DataQuadIndex;
    ++DataQuadPtr;
    return( dql );
}

/* splits the dataquad pointed to by dql so that the current one
   will have size "size" and the new one "oldsize - size" */
static void SplitDataQuad( DATA_QUAD_LIST *dql, target_size size )
{
    DATA_QUAD_LIST  *ndql;
    DATA_QUAD       *ndq;
    DATA_QUAD       *dq;
    target_size     oldsize;

    ndql = NewDataQuad();
    ndql->next = dql->next;
    ndql->prev = dql;
    dql->next = ndql;
    if( ndql->next != NULL )
        ndql->next->prev = ndql;
    oldsize = dql->size;
    ndql->size = oldsize - size;
    dql->size = size;

    ndq = &ndql->dq;
    dq = &dql->dq;
    memcpy( ndq, dq, sizeof( *dq ) );

    if( dq->flags & Q_DATA ) {
        if (dq->type == QDT_CONST) {
            ndq->u.string.offset = dq->u.string.offset + size;
            size = 0;
        } else if (dq->type == QDT_CONSTANT)
            size = 0;
    }
    if( size != 0 ) {
        /* can't happen ! */
        CErr2p( ERR_FATAL_ERROR, "Bad initializer quad" );
        CSuicide();
    }
}

static void DeleteDataQuad( DATA_QUAD_LIST *dql )
{
    dql->prev->next = dql->next;
    if( dql->next != NULL ) {
        dql->next->prev = dql->prev;
    }
}

static void GenDataQuad( DATA_QUAD *dq, target_size size )
{
    DATA_QUAD_LIST  *dql;
    target_size     cursize;

    dql = CurDataQuad->next;
    if( dql != NULL ) {
        /* overwrite the current dataquad */
        cursize = dql->size;
        while( size > cursize && dql->next != NULL ) {
            DeleteDataQuad( dql );
            dql = dql->next;
            cursize += dql->size;
        }
        /* dql now takes up cursize bytes but was defined to
           take up dql->size bytes: split into a dataquad with
           dql->size - (cursize - size) bytes and one with
           cursize - size bytes.
        */
        if( size < cursize ) {
            SplitDataQuad( dql, dql->size - ( cursize - size ) );
        }
    } else {
        dql = NewDataQuad();
        CurDataQuad->next = dql;
        dql->prev = CurDataQuad;
        dql->next = NULL;
    }
    dql->size = size;
    memcpy( &dql->dq, dq, sizeof( DATA_QUAD ) );
    CurDataQuad = dql;
}

static void ZeroBytes( target_size n )
{
    DATA_QUAD   dq;

    if( n == 0 )
        return;
    dq.type = QDT_CONSTANT;
    dq.flags = Q_DATA;
    GenDataQuad( &dq, n );
}

static void RelSeekBytes( target_ssize n )
{
    DATA_QUAD_LIST  *dql;

    dql = CurDataQuad;
    while( n < 0 && n <= -(target_ssize)dql->size ) {
        n += dql->size;
        dql = dql->prev;
    }
    while( n > 0 && dql->next != NULL ) {
        dql = dql->next;
        n -= dql->size;
    }
    /* now -dql->size < n <= 0 or dql->next == NULL */
    if( n < 0 ) {
        SplitDataQuad( dql, dql->size + n );
    }
    CurDataQuad = dql;
    if( n > 0 ) {
        /* dql->next == NULL */
        ZeroBytes( n );
    }
}

#ifdef max
#undef max
#endif
#define max(a,b) ((a) > (b) ? (a) : (b))

#ifdef min
#undef min
#endif
#define min(a,b) ((a) < (b) ? (a) : (b))

/**
 * Evaluates to 1 if t is a struct-like object type
 */
#define structlike(t) (\
    (t)->decl_type == TYPE_STRUCT   || \
    (t)->decl_type == TYPE_UNION    || \
    (t)->decl_type == TYPE_FCOMPLEX || \
    (t)->decl_type == TYPE_DCOMPLEX || \
    (t)->decl_type == TYPE_LDCOMPLEX)

#define aggregate(t) (\
    (structlike(t)) ||\
    (t)->decl_type == TYPE_ARRAY)

#define boolean(t) (\
    (t)->decl_type == TYPE_BOOL    || (\
    ((t)->decl_type == TYPE_FIELD  ||  \
    (t)->decl_type == TYPE_UFIELD) &&  \
    (t)->u.f.field_type == TYPE_BOOL))

/* This should support more than the required 64 levels of nesting */
#define MAX_OBJ_ITER ((MAX_LEVEL) >> 3)

/**
 * This struct represents a chain from the base symbol all the way
 * down to the target object being initialized in an aggregate object.
 *
 * In non-aggregates it merely refers to the object being initialized.
 */
static struct object_iter {
    TYPEPTR type;                 /* Object type                      */
    target_ssize offset;          /* Offset from the base symbol      */
    unsigned char level;          /* Depth of this iter               */
    unsigned int  non_const;      /* Number of non-const initializers */
    bool list_start;              /* Starts an init. list             */
    union {
        target_ssize idx;         /* Array index                      */
        FIELDPTR f;               /* Struct field                     */
    } u;
} ObjectIter[MAX_OBJ_ITER];

/**
 * Initialize an iterator with the given type.
 *
 * If the type is an aggregate and has subaggregates, drill down into
 * the lowest subaggregate if requested.
 *
 * Returns the current depth within the object.
 */
static unsigned int ObjectIterInit(unsigned int level, TYPEPTR type,
                                   bool list_start, bool drill_down)
{
    TYPEPTR t;
    FIELDPTR f;

next:
    type = SkipTypeFluff(type);
    memset(ObjectIter + level, 0, sizeof *ObjectIter);
    ObjectIter[level].type  = type;
    ObjectIter[level].level = level;
    ObjectIter[level].list_start = list_start;
    if (level) ObjectIter[level].offset = ObjectIter[level - 1].offset;

    if (structlike(type)) {
        f = type->u.tag->u.field_list;
        if (!SizeOfArg(type)) {
            SetDiagType1(type);
            CErr1(ERR_INCOMPLETE_EXPR_TYPE);
            SetDiagPop();

            /* Suppress additional errors */
            type->u.tag->size = SizeOfArg(f->field_type);
        }

        while (f && !f->name[0]) {
            t = SkipTypeFluff(f->field_type);
            if (structlike(t))
                break;

            ObjectIter[level].offset += f->offset;
            f = f->next_field;
        }

        ObjectIter[level].u.f = f;
    }

    /* Drill down into subaggregates if requested */
    if (drill_down && aggregate(type)) {
        if (type->decl_type == TYPE_ARRAY)
            type = SkipTypeFluff(type->object);
        else type = SkipTypeFluff(ObjectIter[level].u.f->field_type);

        if (!aggregate(type))
            goto ret;

        if (++level > MAX_OBJ_ITER) {
            CErr1(ERR_INTERNAL_LIMIT_EXCEEDED);
            goto err;
        }

        list_start = false;
        goto next;
    }

ret:
    return level;

err:
    return UINT_MAX;
}

/**
 * Updates the given iterator inline to refer to the next object to be
 * initialized, and returns the iterator. If we reach the end of the
 * object, NULL is returned.
 */
static struct object_iter *ObjectIterNext(struct object_iter *target)
{
    TYPEPTR t = NULL;
    struct object_iter *ret = target;

    do {
        if (target->type->decl_type == TYPE_ARRAY) {
            ++target->u.idx;
            target->offset += (target_ssize)SizeOfArg(target->type->object);

            if (target->type->u.array->unspecified_dim ||
                target->u.idx < target->type->u.array->dimension)
                break;
        }

        if (structlike(target->type) && target->u.f &&
            target->type->decl_type != TYPE_UNION) {
            target->u.f = target->u.f->next_field;
            if (target->u.f)
                t = SkipTypeFluff(target->u.f->field_type);

            /* Skip non-structlike fields without names */
            while (target->u.f && !target->u.f->name[0] && !structlike(t)) {
                target->u.f = target->u.f->next_field;
                if (target->u.f)
                    t = SkipTypeFluff(target->u.f->field_type);
            }

            if (target->u.f) {
                target->offset = 0;
                if (target->level)
                    target->offset = ObjectIter[target->level - 1].offset;
                target->offset += (target_ssize)target->u.f->offset;
                break;
            }
        }

        /**
         * Return NULL to signify that we have reached the end.
         */
        if (!target->level) {
            ret = NULL;
            break;
        } else {
            ret = ObjectIter + target->level - 1;
            ret->non_const += target->non_const;
            target = ret;
        }
    } while (1);

    return ret;
}

/**
 * Fold a chain of OPR_DOT operations
 */
static void DotFold(TREEPTR tree)
{
    TREEPTR t = tree->left, tp = tree->left;

    while (t && t->op.opr == OPR_DOT) {
        tree->right->op.u2.long_value += t->right->op.u2.long_value;
        tp = t;
        t  = t->left;
    }

    if (tree->left != t) {
        tp->left = NULL;
        FreeExprTree(tree->left);
        tree->left = t;
    }
}

/**
 * Returns true if the given tree can be considered a constant expression
 * for the purpose of initialization.
 *
 * This includes:
 *  - Integral constants
 *  - Address constants
 *  - Address constants +/- an Integral constant
 */
static bool IsInitConstExpr(TREEPTR opnd)
{
    DATA_TYPE type;
    SYM_ENTRY sym;

again:
    type = opnd->u.expr_type->decl_type;

    switch (opnd->op.opr) {
    case OPR_ADDROF:
    case OPR_CONVERT:
        if ((opnd = opnd->right))
            goto again;
        break;
    case OPR_DOT:
        DotFold(opnd);
        if ((opnd = opnd->left))
            goto again;
        break;
    case OPR_ADD:
    case OPR_SUB:
        if (IsConstLeaf(opnd->right)) {
            if ((opnd = opnd->left))
                goto again;
        }
        break;
    case OPR_PUSHSYM:
    case OPR_PUSHADDR:
        if (type == TYPE_FUNCTION)
            goto is_const;

        SymGet(&sym, opnd->op.u2.sym_handle);
        if (sym.attribs.stg_class == SC_STATIC ||
            sym.attribs.stg_class == SC_EXTERN ||
            sym.attribs.stg_class == SC_NONE)
            goto is_const;
        break;
    case OPR_PUSHINT:
    case OPR_PUSHFLOAT:
    case OPR_PUSHSTRING:
        goto is_const;
    }

    return false;

is_const:
    return true;
}

/**
 * Parse a struct/union field designator, and look the field up.
 *
 * On success, this function returns the new iterator level corresponding
 * to the field found in Buffer.
 *
 * On error, or if the field cannot be found, UINT_MAX is returned.
 */
static unsigned int StructUnionDesignator(struct object_iter *target)
{
    FIELDPTR f;
    TYPEPTR type, ftype;
    unsigned int level = target->level, ilevel = target->level;

    type = SkipTypeFluff(target->type);
    if (CurToken != T_ID) {
        CErr1(ERR_EXPECTING_ID);
        goto err;
    }

    target->u.f = type->u.tag->u.field_list;

next:
    for (f = target->u.f; f; f = f->next_field) {
        target->u.f = f;

        ftype = SkipTypeFluff(f->field_type);
        if (!*f->name && structlike(ftype)) {
            if (++level >= MAX_OBJ_ITER) {
                CErr1(ERR_INTERNAL_LIMIT_EXCEEDED);
                goto err;
            }

            if (ObjectIterInit(level, ftype, 0, 0) == UINT_MAX)
                goto err;
            target = ObjectIter + level;
            goto next;
        }

        if (*f->name && !strcmp(f->name, Buffer)) {
            target->u.f    = f;
            target->offset = 0;
            if (level)
                target->offset = ObjectIter[level - 1].offset;
            target->offset += (target_ssize)f->offset;
            goto ret;
        }
    }

    if (level > ilevel) {
        --level;
        target = ObjectIter + level;
        goto next;
    }

    CErr3p(ERR_NAME_NOT_FOUND_IN_STRUCT, Buffer, type->u.tag->name);

err:
    return UINT_MAX;

ret:
    NextToken();
    return level;
}

/**
 * Parse an array member designator.
 *
 * Returns true on success and the index of the designated
 * member in idx.
 */
static bool ArrayDesignator(target_ssize *idx)
{
    TREEPTR t;
    bool ret = false;

    t = SingleExpr();
    FoldExprTree(t);
    if (IsConstLeaf(t)) {
        CastConstValue(t, TYPE_INT);
        *idx = t->op.u2.long_value;
        ret  = true;
    } else CErr1(ERR_NOT_A_CONSTANT_EXPR);

    FreeExprTree(t);
    MustRecog(T_RIGHT_BRACKET);
    return ret;
}

/**
 * Returns true if the given field referred to by the
 * given iterator level is an incomplete array type.
 */
static bool IsIncompleteArrayMember(struct object_iter *target)
{
    FIELDPTR f;
    bool ret = false;

    f = target->u.f;
    if (f->field_type->decl_type != TYPE_ARRAY ||
        !f->field_type->u.array->unspecified_dim)
        goto ret;
    ret = true;

    if (target->offset == SizeOfArg(target->type) && !f->next_field) {
        if (stdc_version >= C99 && target->type->decl_type == TYPE_STRUCT) {
            if (!CompFlags.extensions_enabled)
                CErr2p(ERR_CANT_INIT_FLEX_ARRAY_MEMBER, f->name);
            else ret = false;
            goto ret;
        }
    }

    CErr2p(ERR_INCOMPLETE_TYPE, f->name);

ret:
    return ret;
}

/**
 * Render any previous assignments to the selected target void, and return
 * the tail of the updated assignment list.
 */
static TREEPTR AssignOverride(struct object_iter *target, TREEPTR head,
                              TYPEPTR type)
{
    int offset = target->offset;
    TREEPTR t, tp;

    /* Overriding a union member overrides the whole union */
    if (target->type->decl_type == TYPE_UNION) {
        type   = target->type;
        offset = target->offset - target->u.f->offset;
    }

    for (t = head->left, tp = head; t; tp = t, t = t->left) {
        if (!t->right || !t->right->right)
            continue;

        if (t->op.u2.long_value < offset ||
            t->op.u2.long_value >= offset + SizeOfArg(type))
            continue;

        if (!aggregate(type)) {
            if (t->op.u2.long_value != offset ||
                type->decl_type != t->right->u.expr_type->decl_type ||
                type->object    != t->right->u.expr_type->object)
                continue;

            if ((type->decl_type == TYPE_FIELD || type->decl_type == TYPE_UFIELD) &&
                memcmp(&type->u.f, &t->right->u.expr_type->u.f, sizeof type->u.f))
                continue;
        }

        CWarn1(WARN_INITIALIZER_OVERRIDDEN, ERR_INITIALIZER_OVERRIDDEN);
        tp->left = t->left;
        t->left  = NULL;
        FreeExprTree(t);
        t = tp;
    }

    return t ? t : tp;
}

/**
 * Parse a designator list
 *
 * This takes tokens all the way up to T_EQUALS, and handles initializer
 * overrides. The ahead and atail parameters should be the head and tail
 * of the assignment list, which is automatically updated when overrides
 * to whole aggregates occur.
 *
 * Returns an status integer: -1 on error, 0 if no designator was present,
 * and 1 if a designator was parsed successfully. The updated iterator
 * is returned in *out.
 */
static int DesignatorList(struct object_iter **out, TREEPTR ahead, TREEPTR *atail)
{
    int ret = 0;
    target_ssize idx;
    struct object_iter *tmp, *base = *out, *target = *out;
    TYPEPTR type = SkipTypeFluff(base->type);
    unsigned int level = base->level;

    if (stdc_version < C99 || (CurToken != T_DOT && CurToken != T_LEFT_BRACKET))
        goto ret;

    /* Start from the current object being initialized */
    while (level && !target->list_start) {
        tmp = ObjectIter + --level;
        tmp->non_const += target->non_const;
        target = tmp;
        type = target->type;
    }

    while (CurToken == T_DOT || CurToken == T_LEFT_BRACKET) {
        if (CurToken == T_LEFT_BRACKET) {
            NextToken();
            if (!ArrayDesignator(&idx))
                goto err;

            if (type->decl_type != TYPE_ARRAY) {
                CErr1(ERR_EXPR_MUST_BE_ARRAY);
                goto err;
            }

            if (!type->u.array->unspecified_dim &&
                idx >= type->u.array->dimension) {
                CErr1(ERR_INIT_INDEX_EXCEEDS_ARRAY_BOUNDS);
                goto err;
            }

            if (type->u.array->unspecified_dim && idx < 0) {
                CErr1(ERR_INVALID_DIMENSION);
                goto err;
            }

            base = target;
            type = SkipTypeFluff(type->object);
            target->offset = 0;
            target->u.idx  = idx;
            if (level) target->offset  = ObjectIter[level - 1].offset;
            if (idx)   target->offset += idx * SizeOfArg(type);
        } else if (CurToken == T_DOT) {
            NextToken();
            if (!structlike(type)) {
                CErr1(ERR_MUST_BE_STRUCT_OR_UNION);
                goto err;
            }

            if ((level = StructUnionDesignator(target)) == UINT_MAX)
                goto err;

            target = ObjectIter + level;
            if (IsIncompleteArrayMember(target))
                goto err;

            base = target;
            type = SkipTypeFluff(target->u.f->field_type);
        }

        if (aggregate(type)) {
            if (++level >= MAX_OBJ_ITER) {
                CErr1(ERR_INTERNAL_LIMIT_EXCEEDED);
                goto err;
            }

            if ((level = ObjectIterInit(level, type, 0, 0)) == UINT_MAX)
                goto err;

            target = ObjectIter + level;
            base = target;
        }
    }

    ret = 1;
    MustRecog(T_EQUAL);
    if (aggregate(type) && CurToken == T_LEFT_BRACE) {
        target->list_start = true;
        *atail = AssignOverride(target, ahead, target->type);
    }

ret:
    *out = target;
    return ret;

err:
    while (CurToken != T_EQUAL      && CurToken != T_RIGHT_BRACE &&
           CurToken != T_SEMI_COLON && CurToken != T_EOF)
        NextToken();
    MustRecog(T_EQUAL);
    return -1;
}

/**
 * Create an expression assigning the given value to the target.
 * This is analagous to: *(type *)((char *)&sym + offset) = value
 */
static TREEPTR Assign(SYMPTR sym, SYM_HANDLE handle,
                      struct object_iter *target, TREEPTR value,
                      TYPEPTR type, TYPEPTR value_type)
{
    TYPEPTR tmp;
    TREEPTR t = NULL, t2;
    target_ssize idx = 0;
    bool is_ua = false;
    unsigned int e;

    /* Take the rvalue */
    value = RValue(value);
    if (value->op.opr == OPR_ERROR)
        goto err;

    type = SkipTypeFluff(type);
    if (boolean(type) && !boolean(value_type)) {
        value      = BoolConv(value);
        value_type = value->u.expr_type;
    }

    /* String literal */
    if (value->op.opr == OPR_PUSHSTRING) {
        value->op.u2.string_handle->ref_count++;
        if (target->type->decl_type == TYPE_ARRAY &&
            type->decl_type != TYPE_POINTER)
            type = target->type;

        if (type->decl_type == TYPE_ARRAY)
            value->u.expr_type = value_type;
    }

    /* Ensure types are compatible */
    e = ErrCount;
    ParmAsgnCheck(type, value, 0, true);
    if (e < ErrCount)
        goto err;

    /* Fixup targets with incomplete array types */
    if (target->type->decl_type == TYPE_ARRAY) {
        idx = target->u.idx;
        if (type->decl_type == TYPE_ARRAY)
            idx = type->u.array->dimension;

        if (target->type->u.array->unspecified_dim) {
            if (!target->type->u.array->dimension) {
                tmp          = target->type;
                target->type = ArrayNode(type);
                target->type->u.array->unspecified_dim = 1;

                if (tmp == sym->sym_type) {
                    sym->sym_type = target->type;
                    SymReplace(sym, handle);
                }
            }

            target->type->u.array->dimension = max(
                target->type->u.array->dimension,
                idx + 1
            );

            is_ua = true;
        } else target->u.idx = idx;
    }

    /* Assign the rvalue to the symbol */
    t = VarLeaf(sym, handle);
    t->u.expr_type = sym->sym_type;

    if (target->offset) {
        /* Make a pointer to target + offset */
        t = ExprNode(t, OPR_ADD, IntLeaf(target->offset));
        t->u.expr_type         = GetType(TYPE_POINTER);
        t->u.expr_type->object = type;
        t->op.u2.result_type   = t->u.expr_type;
    }

    if (target->offset ||
        (aggregate(target->type) && !aggregate(value->u.expr_type))) {
        /* Dereference it */
        t = ExprNode(t, OPR_POINTS, NULL);
        t->u.expr_type       = type;
        t->op.u2.result_type = type;
    }

    t = ExprNode(t, OPR_EQUALS, value);
    t->u.expr_type = type;
    t->op.u2.result_type = type;

    /* Retain the offset for static storage */
    t2 = IntLeaf(target->offset);
    t2->right = t;
    t = t2;

    /* If target has undefined dimensions, update target's index */
    if (is_ua) target->u.idx = idx;
    return t;

err:
    if (t) FreeExprTree(t);
    return NULL;
}

static TREEPTR AssignTarget(SYMPTR sym, SYM_HANDLE handle,
                            struct object_iter *target,
                            TREEPTR value)
{
    target_size len;
    TREEPTR t;
    TYPEPTR type, vtype, tmp;

    if (value->op.opr == OPR_ERROR)
        goto err;

    /* Get the type of field being assigned, and the type of value */
    type  = target->type;
    vtype = SkipTypeFluff(value->u.expr_type);

    if (type->decl_type == TYPE_ARRAY)
        type = SkipTypeFluff(type->object);
    else if (structlike(type) && target->u.f)
        type = SkipTypeFluff(target->u.f->field_type);

    if (target->type->decl_type == TYPE_ARRAY && value->op.opr == OPR_PUSHSTRING) {
        if (type->decl_type != TYPE_POINTER) {
            /* Truncate string literals, complete char arrays */
            len = value->u.expr_type->u.array->dimension;

            if (target->type->u.array->unspecified_dim) {
                tmp = target->type;
                target->type = ArrayNode(target->type->object);
                target->type->u.array->dimension = len;
                target->type->u.array->unspecified_dim = 0;
                if (sym->sym_type == tmp) {
                    sym->sym_type = target->type;
                    SymReplace(sym, handle);
                }
            }

            if (len && len - 1 > target->type->u.array->dimension) {
                CWarn1(WARN_LIT_TOO_LONG, ERR_LIT_TOO_LONG);
                len = target->type->u.array->dimension;
                value->u.expr_type->u.array->dimension = len;
            }
        }
    }

    /* Initializers for static objects must be constant */
    FoldExprTree(value);
    if (!IsInitConstExpr(value)) {
        target->non_const++;
        if (!SymLevel || sym->attribs.stg_class == SC_STATIC) {
            CErr1(ERR_NOT_A_CONSTANT_EXPR);
            goto err;
        }
    }

    if ((t = Assign(sym, handle, target, value, type, vtype)))
        return t;

err:
    FreeExprTree(value);
    return NULL;
}

/**
 * Parse an initializer list for a scalar type.
 *
 * Returns a scalar expression representing the initializer value
 */
static TREEPTR InitializerListScalar(void)
{
    TREEPTR t;
    unsigned int braces = 0;

    /**
     * initializer-list contains initializer which may contain
     * an initializer-list in braces. Since scalars have no
     * sub-objects, this implies that so long as there is only
     * one effective expression, and the braces are balanced, the
     * expression can be enclosed in a theoretically infinite set
     * of braces. Odd, but it fits the grammar.
     */
    CWarn1(WARN_BRACES_AROUND_SCALAR_INIT, ERR_BRACES_AROUND_SCALAR_INIT);
    while (CurToken == T_LEFT_BRACE) {
        ++braces;
        NextToken();
    }

    t = ScalarExpr(SingleExpr());
    if (CurToken == T_COMMA)
        NextToken();

    if (CurToken != T_RIGHT_BRACE && CurToken != T_SEMI_COLON &&
        CurToken != T_EOF)
        CErr1(ERR_TOO_MANY_INITS);

    MustRecog(T_RIGHT_BRACE);
    while (--braces < UINT_MAX && CurToken == T_RIGHT_BRACE)
        MustRecog(T_RIGHT_BRACE);

    return t;
}

/**
 * Parse an initializer list, filling in the assignments tree with
 * assignment expressions from the initializer list.
 *
 * Returns the number of non-constant initializers found in the list,
 * or UINT_MAX on error.
 */
static unsigned int InitializerList(SYMPTR sym, SYM_HANDLE handle,
                                    TREEPTR assignments)
{
    bool brace;
    int designator;
    TYPEPTR type;
    TREEPTR t, head = assignments;
    unsigned int non_const = 0, level;
    struct object_iter *target, *tmp;

    /* Initialize the root iterator */
    type   = SkipTypeFluff(sym->sym_type);
    level  = ObjectIterInit(0, type, 1,
                            CurToken != T_LEFT_BRACE &&
                            CurToken != T_DOT        &&
                            CurToken != T_LEFT_BRACKET);
    target = ObjectIter + level;
    if (level == UINT_MAX)
        goto err;

    while (CurToken != T_RIGHT_BRACE && CurToken != T_SEMI_COLON &&
           CurToken != T_EOF) {

        /* Bail if we'd go off the end of the object */
        if (!target) {
            CErr1(ERR_TOO_MANY_INITS);
            goto err;
        }

        if ((designator = DesignatorList(&target, head, &assignments)) < 0)
            goto err;

        if (structlike(target->type)) {
            if (IsIncompleteArrayMember(target))
                goto err;
        }

        type  = target->type;
        level = target->level;
        brace = false;

        if (CurToken == T_LEFT_BRACE) {
            brace = true;
            NextToken();

            if (CurToken == T_RIGHT_BRACE || CurToken == T_COMMA) {
                CErr1(ERR_EMPTY_INITIALIZER_LIST);
                goto err;
            }
        }

        /* Delve deeper into aggregates */
        if (aggregate(type)) {
            if (type->decl_type == TYPE_ARRAY)
                type = SkipTypeFluff(type->object);
            else type = SkipTypeFluff(ObjectIter[level].u.f->field_type);

            if (aggregate(type)) {
                level = ObjectIterInit(level + 1, type,
                                       brace && !designator, !brace);

                if (level == UINT_MAX)
                    goto err;
                target = ObjectIter + level;
                if (brace) continue;
            } else brace = false;
        }

        /**
         * Backup our iterator in case the expression contains an
         * initializer.
         */
        tmp = CMemAlloc((level + 1) * sizeof *target);
        memcpy(tmp, ObjectIter, (level + 1) * sizeof *target);
        t = brace ? InitializerListScalar() : SingleExpr();
        memcpy(ObjectIter, tmp, (level + 1) * sizeof *target);
        CMemFree(tmp);

        if (t && (t = AssignTarget(sym, handle, target, t))) {
            if (stdc_version >= C99)
                assignments = AssignOverride(target, head, t->right->u.expr_type);
            assignments->left = t;
            assignments = t;
        }

        if (CurToken == T_COMMA)
            NextToken();

        /**
         * If we hit the end of the list, unwind to the start of the
         * previous list, or the root, depending on how many right
         * braces we encounter.
         */
        while (CurToken == T_RIGHT_BRACE && level) {
            while (level && !target->list_start) {
                non_const += target->non_const;
                target     = ObjectIter + --level;
            }

            if (level) {
                NextToken();
                if (CurToken == T_COMMA)
                    NextToken();

                non_const += target->non_const;
                target     = ObjectIter + --level;
            } else break;
        }

        /* Advance to the next field/object unless we have a designator */
        if (CurToken != T_DOT && CurToken != T_LEFT_BRACKET)
            target = ObjectIterNext(target);
    }

done:
    if (non_const != UINT_MAX)
        non_const += ObjectIter->non_const;

    /* Fixup arrays */
    type = SkipTypeFluff(sym->sym_type);
    if (type->decl_type == TYPE_ARRAY && type->u.array->unspecified_dim) {
        type = ArrayNode(type->object);
        type->u.array->dimension       = ObjectIter->u.idx;
        type->u.array->unspecified_dim = false;

        if (sym->sym_type->decl_type == TYPE_TYPEDEF) {
            type = TypeNode(TYPE_TYPEDEF, type);
            type->u.typedefn = sym->sym_type->u.typedefn;
        }

        sym->sym_type = type;
        SymReplace(sym, handle);
    }

    while (CurToken != T_SEMI_COLON && CurToken != T_EOF &&
          (non_const != UINT_MAX ? CurToken != T_RIGHT_BRACE : 1))
        NextToken();
    return non_const;

err:
    non_const = UINT_MAX;
    goto done;
}

/**
 * Initialize an object with a static initializer.
 *
 * If the target object has static storage, initialize the object directly.
 * If the target has auto storage, create a static initializer object and
 * assign it to the target.
 */
static void StaticInitializer(SYMPTR sym, SYM_HANDLE handle, TYPEPTR type,
                              TREEPTR tree)
{
    SYM_ENTRY isym;
    SYM_HANDLE ihandle = 0;
    TREEPTR t, value;
    TYPEPTR vtype;
    DATA_QUAD dq, *pdq;
    DATA_QUAD_LIST *cdq;
    target_ssize size, offset;
#ifdef _LONG_DOUBLE_
    long_double ld;
#endif

    CompFlags.initializing_data = true;

    /* Create a symbol for our target */
    size = (target_ssize)SizeOfArg(type);
    dq.type             = QDT_STATIC;
    dq.flags            = Q_DATA;
    dq.u.var.sym_handle = handle;
    dq.u.var.offset     = 0;

    /* For non-static targets, create a static initializer object */
    if (SymLevel && sym->attribs.stg_class != SC_STATIC) {
        ihandle = MakeNewSym(&isym, 'X', type, SC_STATIC);
        isym.mods = FLAG_CONST;
        isym.flags |= SYM_INITIALIZED | SYM_ASSIGNED | SYM_TEMP;
        SetSegment(&isym);
        SetSegAlign(&isym);
        SymReplace(&isym, ihandle);
        dq.u.var.sym_handle = ihandle;
    }

    /**
     * Add a 'zero' data quad the size of our target object. The
     * initializer data quads will split this quad appropriately.
     */
    GenDataQuad(&dq, 0);
    ZeroBytes(size);
    RelSeekBytes(-size);

    for (t = (tree && tree->left) ? tree->left : tree; t; t = t->left, pdq = NULL) {
        if (!t->right || !t->right->right)
            continue;

        value   = t->right->right;
        vtype   = SkipTypeFluff(t->right->op.u2.result_type);
        offset  = t->op.u2.long_value;
        size    = (target_ssize)TypeSize(vtype);
        RelSeekBytes(offset);

        memset(&dq, 0, sizeof dq);
        dq.type  = (enum quad_type)vtype->decl_type;
        dq.flags = Q_DATA;
        pdq      = NULL;

        while (value && value->op.opr == OPR_CONVERT) value = value->right;
        if (value && value->op.opr == OPR_ADDROF)     value = value->right;
        if (!value) continue;

        if (value->op.opr == OPR_PUSHSTRING)
            dq.type = QDT_STRING;

        if (value->op.opr == OPR_PUSHSYM || value->op.opr == OPR_PUSHADDR) {
            if (dq.type == QDT_ARRAY) {
                if (t->u.expr_type->decl_type == TYPE_POINTER)
                    dq.type = QDT_POINTER;
            } else if (vtype->object && vtype->object->decl_type == TYPE_FUNCTION)
                dq.type = QDT_FUNCTION;
            else if (dq.type != QDT_FUNCTION)
                dq.type = QDT_POINTER;
        }

        switch (dq.type) {
        case QDT_LONG64:
        case QDT_ULONG64:
            CastConstValue(value, dq.type);
            dq.u.ulong64 = value->op.u2.ulong64_value;
            break;
        case QDT_FLOAT:
        case QDT_FIMAGINARY:
        case QDT_DOUBLE:
        case QDT_DIMAGINARY:
        case QDT_LONG_DOUBLE:
        case QDT_LDIMAGINARY:
            switch (dq.type) {
            case QDT_DOUBLE:
            case QDT_DIMAGINARY:
                CastConstValue(value, TYPE_DOUBLE);
                break;
            case QDT_LONG_DOUBLE:
            case QDT_LDIMAGINARY:
                CastConstValue(value, TYPE_LONG_DOUBLE);
                break;
            default:
                CastConstValue(value, TYPE_FLOAT);
                break;
            }

#ifdef _LONG_DOUBLE_
            ld = value->op.u2.float_value->ld;
            __iLDFD( &ld, &dq.u.double_value );
#else
            dq.u.double_value = value->op.u2.float_value->ld.u.value;
#endif
            break;
        case QDT_FIELD:
        case QDT_UFIELD:
            dq.type = (enum quad_type)vtype->u.f.field_type;
            size    = (target_ssize)TypeSize(GetType(vtype->u.f.field_type));

            /* Look for a previous data quad for this field */
            cdq = CurDataQuad;
            if (cdq && cdq->next && cdq->next->dq.type != QDT_CONSTANT) {
                if (cdq->next->size == size)
                    pdq = &cdq->next->dq;
            }

            CastConstValue(value, vtype->u.f.field_type);
            if (vtype->u.f.field_type == TYPE_LONG64 ||
                vtype->u.f.field_type == TYPE_ULONG64) {
                dq.u.ulong64 = value->op.u2.ulong64_value;

                if (vtype->u.f.field_width < 32) {
                    dq.u.ulong64.u._32[I64LO32] &= (1 << vtype->u.f.field_width) - 1;
                    dq.u.ulong64.u._32[I64HI32] = 0;
                } else if (vtype->u.f.field_width == 32) {
                    dq.u.ulong64.u._32[I64HI32] = 0;
                } else if (vtype->u.f.field_width > 32) {
                    dq.u.ulong64.u._32[I64HI32] &= (1 << (vtype->u.f.field_width - 32)) - 1;
                }

                U64ShiftL(&dq.u.ulong64, vtype->u.f.field_start, &dq.u.ulong64);
                if (pdq) {
                    pdq->u.ulong64.u._32[I64LO32] &= ~dq.u.ulong64.u._32[I64LO32];
                    pdq->u.ulong64.u._32[I64HI32] &= ~dq.u.ulong64.u._32[I64HI32];
                    pdq->u.ulong64.u._32[I64LO32] |= dq.u.ulong64.u._32[I64LO32];
                    pdq->u.ulong64.u._32[I64HI32] |= dq.u.ulong64.u._32[I64HI32];
                }
            } else {
                dq.u.ulong_values[0] = value->op.u2.ulong_value;
                dq.u.ulong_values[0] &= (1 << vtype->u.f.field_width) - 1;
                dq.u.ulong_values[0] <<= vtype->u.f.field_start;

                if (pdq) {
                    pdq->u.ulong_values[0] &= ~dq.u.ulong_values[0];
                    pdq->u.ulong_values[0] |= dq.u.ulong_values[0];
                }
            }
            break;
        case QDT_STRING:
            size = TARGET_POINTER;
            if (t->right->right->u.expr_type->decl_type == TYPE_ARRAY) {
                size = SizeOfArg(t->right->right->u.expr_type);
                dq.type = QDT_CONST;
            }

            dq.u.string.offset = 0;
            dq.u.string.handle = value->op.u2.string_handle;
            break;
        case QDT_FUNCTION:
            dq.flags |= Q_CODE_POINTER;
            dq.u.var.sym_handle = value->op.u2.func.sym_handle;
        case QDT_POINTER:
            dq.type = QDT_POINTER;
            size    = TARGET_POINTER;

            if (vtype->u.p.decl_flags & (FLAG_FAR | FLAG_HUGE) ||
                TypeSize(vtype) == TARGET_FAR_POINTER) {
                dq.flags |= Q_FAR_POINTER;
                size = TARGET_FAR_POINTER;
            } else if (vtype->u.p.decl_flags & FLAG_NEAR) {
                dq.flags |= Q_NEAR_POINTER;
                size = TARGET_NEAR_POINTER;
            }

            switch (value->op.opr) {
            case OPR_DOT:
                dq.u.var.sym_handle = value->left->op.u2.sym_handle;
                dq.u.var.offset     = value->right->op.u2.long_value;
                break;
            case OPR_ADD:
                dq.u.var.offset     = value->right->op.u2.long_value;
                dq.u.var.sym_handle = value->left->op.u2.sym_handle;
                break;
            case OPR_SUB:
                dq.u.var.offset     = -value->right->op.u2.long_value;
                dq.u.var.sym_handle = value->left->op.u2.sym_handle;
                break;
            default:
                if (value->op.opr != OPR_PUSHINT) {
                    if (!dq.u.var.sym_handle)
                        dq.u.var.sym_handle = value->op.u2.sym_handle;

                    if (value->left && value->left->op.opr == OPR_PUSHINT)
                        dq.u.var.offset = value->left->op.u2.long_value;
                } else dq.u.ulong64 = value->op.u2.ulong64_value;
            }
            break;
        default:
            CastConstValue(value, dq.type);
            dq.u.ulong64 = value->op.u2.ulong64_value;
        }

        if (!pdq) {
            GenDataQuad(&dq, size);
            RelSeekBytes(-size);
        }

        RelSeekBytes(-offset);
    }

    /* Seek to the end of the data quad list */
    while (CurDataQuad->next)
        CurDataQuad = CurDataQuad->next;

    /* Assign the static initializer to the target object */
    if (SymLevel && sym->attribs.stg_class != SC_STATIC) {
        t = ExprNode(VarLeaf(sym, handle), OPR_EQUALS, VarLeaf(&isym, ihandle));
        t->right->op.opr     = OPR_PUSHSYM;
        t->u.expr_type       = isym.sym_type;
        t->op.u2.result_type = isym.sym_type;
        AddStmt(t);
    }

    CompFlags.initializing_data = false;
}

/**
 * Ensure aggregates in auto storage are implicitly initialized to 0.
 */
static void InitializeAutoVar(SYMPTR sym, SYM_HANDLE handle, TYPEPTR type,
                              TREEPTR tree)
{
    TREEPTR t;
    target_ssize offset;
    target_size  size;
    unsigned char *state;
    unsigned int assigned = 0, mask, i;

    if (!(state = CMemAlloc(1 + SizeOfArg(type) / CHAR_BIT)))
        goto ret;

    for (t = tree->left; t; t = t->left) {
        if (!t->right || !t->right->right)
            continue;

        if ((offset = t->op.u2.long_value) < 0)
            continue;

        size = min(SizeOfArg(t->right->left->u.expr_type), SizeOfArg(type));
        for (i = 0; i < size; i++, offset++) {
            mask = 1 << (offset % CHAR_BIT);
            if (!(state[offset / CHAR_BIT] & mask)) {
                state[offset / CHAR_BIT] |= mask;
                assigned++;
            }
        }
    }

    CMemFree(state);
    if (assigned < SizeOfArg(type))
        StaticInitializer(sym, handle, type, NULL);

ret:
    return;
}

/**
 * Parse an initializer list, or assignment expression
 */
void Initializer(SYMPTR sym, SYM_HANDLE handle)
{
    TYPEPTR type;
    TREEPTR t, t2, tree = NULL;
    SYM_ENTRY isym;
    SYM_HANDLE ihandle;
    unsigned int non_const = 0;

    if (sym->flags & SYM_INITIALIZED) {
        CErrSymName(ERR_VAR_ALREADY_INITIALIZED, sym, handle);
        goto err;
    }

    /* Objects defined at file scope have static storage */
    if ((sym->attribs.stg_class == SC_AUTO ||
         sym->attribs.stg_class == SC_REGISTER) && !SymLevel)
        CErr1(ERR_INV_STG_CLASS_FOR_GLOBAL);

    /* Don't warn about unreferenced syms with extern linkage */
    if (!SymLevel && sym->attribs.stg_class == SC_NONE)
        sym->flags |= SYM_IGNORE_UNREFERENCE;

    /* Block scope identifiers with linkage can't have initializers */
    if (sym->attribs.stg_class != SC_NONE     &&
        sym->attribs.stg_class != SC_AUTO     &&
        sym->attribs.stg_class != SC_STATIC   &&
        sym->attribs.stg_class != SC_REGISTER && SymLevel) {
        CErr1(ERR_CANT_INITIALIZE_EXTERN_VAR);
        goto err;
    }

    if (CurToken == T_LEFT_BRACE) {
        NextToken();
        if (CurToken == T_RIGHT_BRACE || CurToken == T_COMMA) {
            CErr1(ERR_EMPTY_INITIALIZER_LIST);
            goto err;
        }

        tree = LeafNode(OPR_NOP);
        if ((non_const = InitializerList(sym, handle, tree)) == UINT_MAX)
            goto err;
        MustRecog(T_RIGHT_BRACE);

        type = SkipTypeFluff(sym->sym_type);
        if (stdc_version >= C99 && aggregate(type) &&
            sym->attribs.stg_class == SC_AUTO)
            InitializeAutoVar(sym, handle, type, tree);
    } else {
        /**
         * Aggregate types must have had a list, unless we've got auto
         * storage. Char arrays may be initialized from a compatible
         * literal string with or without braces.
         */
        tree = CommaExpr();
        type = SkipTypeFluff(sym->sym_type);

        if (aggregate(type) && sym->attribs.stg_class != SC_AUTO) {
            if (type->decl_type != TYPE_ARRAY ||
                (type->decl_type == TYPE_ARRAY &&
                 tree->op.opr != OPR_PUSHSTRING)) {
                CErr1(ERR_NEED_BRACES);
                goto err;
            }
        }

        (void)ObjectIterInit(0, type, 0, 0);
        ObjectIter->u.f = NULL;
        if (!(tree = AssignTarget(sym, handle, ObjectIter, tree)))
            goto ret;
        tree = ExprNode(tree, OPR_NOP, NULL);
    }

    if (SymLevel && sym->attribs.stg_class != SC_STATIC) {
        /* Add and unlink assignment statement(s) */
        for (t = tree->left; t; t = t->left) {
            if (!t->right || !t->right->right) continue;

            type = t->right->left->u.expr_type;
            if (aggregate(type) && t->right->right->op.opr == OPR_PUSHSTRING) {
                t2 = t->left;
                t->left = NULL;
                t->op.u2.long_value = 0;

                /* Create storage for the string literal */
                ihandle = MakeNewSym(&isym, 'S', type, SC_STATIC);
                isym.mods = FLAG_CONST;
                isym.flags |= SYM_INITIALIZED | SYM_ASSIGNED | SYM_TEMP;
                SetSegment(&isym);
                SetSegAlign(&isym);
                SymReplace(&isym, ihandle);
                StaticInitializer(&isym, ihandle, type, t);
                t->left = t2;

                /* Copy it to the target */
                t2 = ExprNode(t->right->left, OPR_EQUALS, VarLeaf(&isym, ihandle));
                t2->right->op.opr     = OPR_PUSHSYM;
                t2->u.expr_type       = isym.sym_type;
                t2->op.u2.result_type = isym.sym_type;
                AddStmt(t2);
            } else AddStmt(t->right);

            t->right = NULL;
        }
    } else StaticInitializer(sym, handle, type, tree);

    /* Enure we set the segment, etc. */
    sym->flags |= SYM_ASSIGNED | SYM_INITIALIZED;
    if (sym->u.var.segid == SEG_NULL) {
        SetFarHuge(sym, false);
        SetSegment(sym);
        SetSegAlign(sym);
    }
    SymReplace(sym, handle);

ret:
    FreeExprTree(tree);
    return;

err:
    while (CurToken != T_COMMA && CurToken != T_SEMI_COLON && CurToken != T_EOF)
        NextToken();
    goto ret;
}

