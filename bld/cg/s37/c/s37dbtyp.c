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


#include <stdio.h>
#include <stdarg.h>
#include <ctype.h>
#include <string.h>
#include "standard.h"
#include "coderep.h"
#include "cgdefs.h"
#include "cgmem.h"
#include "offset.h"
#include "typedef.h"
#include "s37bead.h"
#include "s37dbg.h"

typedef int handle;
#include "s37dbtyp.def"

extern uint            Length(char*);
extern byte           *Copy(void*,void*,uint);
extern type_def       *TypeAddress(cg_type);
extern char           *DbgFmtInt( char *, offset );
extern char           *DbgFmtStr( char *, char *, id_len );
extern void            PutStream(handle ,byte *,uint );


extern    unsigned_16           TypeIdx;

static short int TTagCount;
static short int NTagCount;

static cdebug_type_any   *TypeHead;
static cdebug_type_any  **TypeNext;
static cdebug_type_name  *TNameHead;
static cdebug_type_name **TNameNext;

static char const Scopes[][7] = {
    "struct","union","enum"
};

enum {
    SCOPE_STRUCT = 1,
    SCOPE_UNION  = 2,
    SCOPE_ENUM   = 3,
    SCOPE_MAX    = 4
};

extern  dbg_type DBFtnType( char *nm, dbg_ftn_type tipe ){
/*********************************************************/
    nm = NULL;
    tipe = 0;
    return( 0 );
}

extern  dbg_type DBScalar( char *nm, cg_type tipe ){
/**************************************************/
    type_def           *tipe_addr;
    cdebug_type_scalar *new;
    dbg_type           tnum;

    tipe_addr = TypeAddress( tipe );
    new = CGAlloc( sizeof( *new ) );
    if( tipe_addr->refno == TY_DEFAULT ) { /* SCALAR_VOID */
        new->common.class = CDEBUG_TYPE_INTEGER;
        new->common.len = 4;
        new->sign = 0;
    }else{ /* int or float */
        new->common.len = tipe_addr->length;
        if( tipe_addr->attr & TYPE_FLOAT ) {
            new->common.class = CDEBUG_TYPE_FLOAT;
        } else {
            new->common.class = CDEBUG_TYPE_INTEGER;
            new->sign =  tipe_addr->attr & TYPE_SIGNED ? -1 : 0;
        }
    }
    tnum = AddType( new );
    if( *nm != '\0' ) {
        AddTName( nm, tnum );
    }
    return( tnum );
}

extern  dbg_type DBScope( char *nm ) {
/* mode scope name to a number other than DBG_NIL */
    dbg_type num;

    num = 0;
    while( num < 3 ){
        if( strcmp( Scopes[num], nm ) == 0 ){
            break;
        }
        ++num;
    }
    return( num+1 );
}

extern  name_entry      *DBBegName( char *nm, dbg_type scope ) {
/**************************************************************/

    name_entry  *name;
    id_entry    *id;
    uint        len;

    len = Length( nm );
    name = CGAlloc( sizeof( *name )  );
    id = CGAlloc( sizeof( *id )-1 +len );
    id->len = len;
    Copy( nm, id->name, len );
    name->id = id;
    name->scope = scope;
    name->refno = DBG_NIL_TYPE;
    return( name );
}

extern  dbg_type DBForward( name_entry *name ) {
/* assign a type number for undefined type***************/
    if( name->refno == DBG_NIL_TYPE ) {
        name->refno = ++TypeIdx;
    }
    return( name->refno );
}

extern  dbg_type  DBEndName( name_entry *name, dbg_type tipe ) {
/**************************************************************/
    cdebug_type_any *tptr;

    tptr = FindType( tipe );
    if( name->refno != DBG_NIL_TYPE ) { /* rename type to used name */
        tptr->common.refno = name->refno;
    }
    if( name->scope == DBG_NIL_TYPE  ) {
        AddTId( name->id, tptr->common.refno ); /*typedef ?*/
        CGFree( name->id );
    }else if( name->scope == SCOPE_ENUM ){
        tptr->enums.id = name->id;
    }else{
        tptr->tags.id = name->id;
    }
    CGFree( name );
    return( tptr->common.refno );
}

extern  dbg_type DBCharBlock( unsigned_32 len ){
/*******************************************************/
    len = 0;
    return( 0 );
}

extern  dbg_type DBIndCharBlock( bck_info *len, cg_type len_type, int off ){
/**************************************************************************/
    len = NULL;
    len_type = 0;
    off = 0;
    return( 0 );
}

extern  dbg_type  DBDereference( cg_type ptr_type, dbg_type base ) {
/******************************************************************/
    ptr_type = 0;
    base = 0;
    return( 0 );
}

extern  dbg_type  DBFtnArray( bck_info *bk, cg_type lo_bound,
                cg_type num_elts, int dim_off,dbg_type base ) {
/**************************************************************/
    bk = NULL;
    lo_bound = 0;
    num_elts = 0;
    dim_off = 0;
    base = 0;
    return( 0 );
}

extern  dbg_type  DBArray( dbg_type idx, dbg_type base ){
/*******************************************************/
    idx = base = 0;
    return( 0 );
}

extern  dbg_type DBIntArray( unsigned_32 hi, dbg_type base ) {
/********** Make cdebug array type **************************/
    cdebug_type_array *new;
    cdebug_type_any *bptr;

    new = CGAlloc( sizeof( *new ) );
    new->common.class = CDEBUG_TYPE_ARRAY;
    new->base = base;
    bptr = FindType( base );
    new->common.len = (hi+1) * bptr->common.len;
    new->length = (hi+1);
    return( AddType( new ) );
}

extern  dbg_type DBSubRange( signed_32 lo, signed_32 hi, dbg_type base ){
/***********************************************************************/
    lo=hi=base=0;
    return( 0 );
}

extern  dbg_type DBPtr( cg_type ptr_tipe, dbg_type base ){
    cdebug_type_pointer *new;
    type_def           *tipe_addr;

    tipe_addr = TypeAddress( ptr_tipe );
    new = CGAlloc( sizeof( *new ) );
    new->common.len = tipe_addr->length;
    new->common.class = CDEBUG_TYPE_POINTER;
    new->base = base;
    return( AddType( new ) );
}


extern  cdebug_type_tags *DBBegStruct( cg_type tipe, bool is_struct ) {
/**************************************/
    cdebug_type_tags   *new;

    new = CGAlloc( sizeof( *new ) );
    new->common.class =  is_struct ? CDEBUG_TYPE_STRUCT
                                   : CDEBUG_TYPE_UNION;
    new->common.len = TypeAddress( tipe )->length;
    new->index = 0;
    new->list = NULL;
    new->id = NULL;
    return( new );
}

extern  void    DBAddField( cdebug_type_tags *st, unsigned_32 off,
                            char *nm, dbg_type  base ) {
/******************************************************/

    DBAddBitField( st, off, 0, 0, nm, base );
}

extern  void    DBAddBitField( cdebug_type_tags *st, unsigned_32 off,
                    byte b_strt, byte b_len, char *nm, dbg_type base ) {
/******************************************************************/

    cdebug_member_entry  *field;
    cdebug_member_entry  *curr;
    cdebug_member_entry **owner;
    uint                  len;

    len = Length( nm );
    field = CGAlloc( sizeof( cdebug_member_entry ) - 1 + len );
    field->id.len = len;
    Copy( nm, field->id.name, len );
    if( b_len != 0 ){
        field->tref = MkBitType( b_strt, b_len );
    }else{
        field->tref = base;
    };
    field->off  = off;
    field->b_strt = b_strt;
    field->b_len  = b_len;
    owner = &st->list;
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
        if( off < curr->off ) break;
        if( (off == curr->off) && (b_strt <= curr->b_strt) ) break;
        owner = &curr->next;
    }
    field->next = curr;
    *owner = field;
    st->index++;
}

extern  dbg_type        DBEndStruct( cdebug_type_tags *st ) {
/*******************************************************/
    return( AddType( st ) );
}

extern  cdebug_type_enums *DBBegEnum( cg_type  tipe ) {
/***************************************************/

    cdebug_type_enums   *new;

    new = CGAlloc( sizeof( *new ) );
    new->common.class = CDEBUG_TYPE_ENUMS;
    new->common.len = TypeAddress( tipe )->length;
    new->index = 0;
    new->list = NULL;
    new->tref = DBScalar( "", tipe );
    new->id = NULL;
    return( new );
}

extern  void DBAddConst( cdebug_type_enums *en, char *nm, signed_32  val ) {
/*********************************************************************/

    cdebug_enum  *cons;
    cdebug_enum  *curr;
    cdebug_enum **owner;
    uint          len;

    len = Length( nm );
    cons = CGAlloc( sizeof( *cons ) - 1 + len );
    cons->id.len = len;
    Copy( nm, cons->id.name, len );
    cons->val = val;
    owner = &en->list;
    for(;;) {
        curr = *owner;
        if( curr == NULL ) break;
        if( val < curr->val ) break;
        owner = &curr->next;
    }
    cons->next = curr;
    *owner = cons;
    en->index++;
}

extern  dbg_type DBEndEnum( cdebug_type_enums *en ){
/*******Add enum type******************************/
    return( AddType( en ) );
}


extern  proc_list       *DBBegProc(  cg_type call_type,  dbg_type  ret ) {
/****start proc type ****************************************************/

    proc_list   *pr;

    call_type = 0;
    pr = CGAlloc( sizeof( proc_list ) );
    pr->num = 0;
    pr->list = NULL;
    pr->call = NULL;
    pr->ret = ret;
    return(pr);
}


extern  void    DBAddParm( proc_list *pr, dbg_type tipe ) {
/*********************************************************/
#if 0  /* not sure if needed */
    parm_entry  *parm;
    parm_entry  **owner;

    parm = CGAlloc( sizeof( parm_entry ) );
    pr->num++;
    owner = &pr->list;
    while( *owner != NULL ) {
        owner = &(*owner)->next;
    }
    *owner = parm;
    parm->tipe = tipe;
    parm->next = NULL;
#else
    pr = NULL;
    tipe = 0;
#endif
}


extern  dbg_type        DBEndProc( proc_list  *pr ) {
/*****Add proc type*********************************/

    parm_entry  *parm;
    parm_entry  *old;

    parm = pr->list;
    while( parm != NULL ){
        old = parm;
        parm = parm->next;
        CGFree( old );
    }
    CGFree( pr );
    return( 0 );
}


static dbg_type AddType( cdebug_type_any *new ) {
/** Add type to type list and bump type index */
    new->common.next = *TypeNext;
    new->common.refno = ++TypeIdx;
    *TypeNext = new;
    ++TTagCount;
    TypeNext = &new->common.next;
    return( TypeIdx );
}

static cdebug_type_any *FindType( dbg_type refno ) {
/** Look up type refno if no there return NULL  ***/
    cdebug_type_any *curr;

    curr = TypeHead;
    while( curr != NULL ) {
        if( curr->common.refno == refno ) {
            break;
        }
        curr = curr->common.next;
    }
    return( curr );
}

static void AddTName( char *name, dbg_type tref ) {
/** Add name to type name list and bump type name index */
    cdebug_type_name *new;
    uint              len;

    len = Length( name );
    new = CGAlloc( sizeof( *new )-1 + len );
    new->tref = tref;
    new->id.len = len;
    Copy( name, new->id.name, len );
    new->next = *TNameNext;
    *TNameNext = new;
    TNameNext = &new->next;
    NTagCount++;   /* bump type name count */
}

static void  AddTId( id_entry *id, dbg_type tref ) {
/** Add id to type name list and bump type name index */
    cdebug_type_name *new;

    new = CGAlloc( sizeof( *new )-1 + id->len );
    new->tref = tref;
    new->id.len = id->len;
    Copy( id->name, new->id.name, id->len );
    new->next = *TNameNext;
    *TNameNext = new;
    TNameNext = &new->next;
    NTagCount++;   /* bump type name count */
}

static dbg_type MkBitType( byte start, byte len ){
/**** make a cdebug bitfield type****************/
    cdebug_type_field *new;
    unsigned long mask;

    new = CGAlloc( sizeof( *new ) );
    new->common.class = CDEBUG_TYPE_FIELD;
    new->common.len = 0;
    new->len = len;
    new->shift = sizeof(mask)*8-(start+len);
    mask = 0xffffffff;
    mask <<= sizeof(mask)*8-len;
    mask >>= start;
    new->mask = mask;
    return( AddType( new ) );
}

extern  void    DBTypInit( void ) {
/*****************************/

    TypeIdx   = 0;  /* type refno      */
    TTagCount = 0;  /* type tag count  */
    NTagCount = 0;   /* type name count */
    TypeHead = NULL;
    TypeNext = &TypeHead;
    TNameHead = NULL;
    TNameNext = &TNameHead;
}

extern  void    DbgTypeTags( handle dbgfile ) {
/*** write dbg info on types  *************/

    TTags( dbgfile, TypeHead, TTagCount );
    NTags( dbgfile, TNameHead, NTagCount );
}

static void TTags( handle dbgfile, cdebug_type_any *list, int index ) {
/**** Put out type count and type tags************/
    char             tagbuff[1+4+4+4];
    cdebug_type_any *old;
    int              size;

    tagbuff[0] = 't';
    DbgFmtInt( &tagbuff[1], index );
    PutStream( dbgfile, tagbuff, 5 );
    tagbuff[0] = 'T';
    while( list != NULL ) {
        DbgFmtInt( &tagbuff[1], list->common.refno );
        DbgFmtInt( &tagbuff[5], list->common.len );
        DbgFmtInt( &tagbuff[9], list->common.class );
        PutStream( dbgfile, tagbuff, 13 );
        switch( list->common.class ) {
        case CDEBUG_TYPE_INTEGER: /* #bits & sign */
            DbgFmtInt( &tagbuff[1], list->common.len*8 );
            DbgFmtInt( &tagbuff[5], list->scalar.sign );
            PutStream( dbgfile, &tagbuff[1], 8 );
            size = sizeof( cdebug_type_scalar );
            break;
        case CDEBUG_TYPE_FLOAT: /* precision */
            DbgFmtInt( &tagbuff[1], (list->common.len-1)*2 );
            PutStream( dbgfile, &tagbuff[1], 4 );
            size = sizeof( cdebug_type_scalar );
            break;
        case CDEBUG_TYPE_FIELD:
            DbgFmtInt( &tagbuff[1], list->field.len );
            DbgFmtInt( &tagbuff[5], list->field.shift );
            DbgFmtInt( &tagbuff[9], list->field.mask );
            PutStream( dbgfile, &tagbuff[1], 12 );
            size = sizeof( cdebug_type_field );
            break;
        case CDEBUG_TYPE_POINTER:
            DbgFmtInt( &tagbuff[1], list->pointer.base );
            PutStream( dbgfile, &tagbuff[1], 4 );
            size = sizeof( cdebug_type_pointer );
            break;
        case CDEBUG_TYPE_ARRAY:
            DbgFmtInt( &tagbuff[1], list->array.base );
            DbgFmtInt( &tagbuff[5], list->array.length );
            PutStream( dbgfile, &tagbuff[1], 8 );
            size = sizeof( cdebug_type_array );
            break;
        case CDEBUG_TYPE_STRUCT:
            DbgStruct( dbgfile, list );
            size = sizeof( cdebug_type_tags );
            break;
        case CDEBUG_TYPE_UNION:
            DbgUnion( dbgfile, list );
            size = sizeof( cdebug_type_tags );
            break;
        case CDEBUG_TYPE_ENUMS:
            DbgEnum( dbgfile, list );
            size = sizeof( cdebug_type_enums );
            break;
        }
        old = list;
        list = list->common.next;
        CGFree( old );
    }

}

static void DbgStruct( handle dbgfile, cdebug_type_tags *tags ) {
/**** Put out structure member list info*************************/
    char                *tag, tagbuff[1+4+4+4+256];
    cdebug_member_entry *list, *old;
    int                  index;

    if( tags->id != NULL ){
        tag=DbgFmtStr( &tagbuff[0], tags->id->name, tags->id->len );
        CGFree( tags->id );
    }else{
        tag=DbgFmtInt( &tagbuff[0], 0 );
    }
    PutStream( dbgfile, tagbuff, tag-tagbuff );
    tagbuff[0] = 'f';
    DbgFmtInt( &tagbuff[1], tags->index );
    PutStream( dbgfile, tagbuff, 5 );
    list = tags->list;
    index = 0;
    tagbuff[0] = 'F';
    while( list != NULL ){
        DbgFmtInt( &tagbuff[1], ++index );
        DbgFmtInt( &tagbuff[5], list->tref  );
        DbgFmtInt( &tagbuff[9], list->off  );
        tag=DbgFmtStr( &tagbuff[13], list->id.name, list->id.len );
        PutStream( dbgfile, tagbuff, tag-tagbuff );
        old = list;
        list = list->next;
        CGFree( old );
    }

}

static void DbgUnion( handle dbgfile, cdebug_type_tags *tags ) {
/**** Put out union member list info*************************/
    char                *tag, tagbuff[1+4+4+4+256];
    cdebug_member_entry *list, *old;

    if( tags->id != NULL ){
        tag=DbgFmtStr( &tagbuff[0], tags->id->name, tags->id->len );
        CGFree( tags->id );
    }else{
        tag=DbgFmtInt( &tagbuff[0], 0 );
    }
    PutStream( dbgfile, tagbuff, tag-tagbuff );
    tagbuff[0] = 'u';
    DbgFmtInt( &tagbuff[1], tags->index );
    PutStream( dbgfile, tagbuff, 5 );
    list = tags->list;
    tagbuff[0] = 'U';
    while( list != NULL ){
        DbgFmtInt( &tagbuff[1], list->tref  );
        tag=DbgFmtStr( &tagbuff[5], list->id.name, list->id.len );
        PutStream( dbgfile, tagbuff, tag-tagbuff );
        old = list;
        list = list->next;
        CGFree( old );
    }

}

static void DbgEnum( handle dbgfile, cdebug_type_enums *enums ) {
/**** Put out union member list info*************************/
    char                *tag, tagbuff[1+4+4+4+256];
    cdebug_enum         *list, *old;

    if( enums->id != NULL ){
        tag=DbgFmtStr( &tagbuff[0], enums->id->name, enums->id->len );
        CGFree( enums->id );
    }else{
        tag=DbgFmtInt( &tagbuff[0], 0 );
    }
    PutStream( dbgfile, tagbuff, tag-tagbuff );
    tagbuff[0] = 'e';
    DbgFmtInt( &tagbuff[1], enums->index );
    PutStream( dbgfile, tagbuff, 5 );
    list = enums->list;
    tagbuff[0] = 'E';
    while( list != NULL ){
        tag=DbgFmtStr( &tagbuff[1], list->id.name, list->id.len );
        tag=DbgFmtInt( tag, list->val );
        PutStream( dbgfile, tagbuff, tag-tagbuff );
        old = list;
        list = list->next;
        CGFree( old );
    }

}

static void NTags( handle dbgfile, cdebug_type_name *list, int index ) {
/**** Put out type name count and type name tags************/
    char             *tag, tagbuff[1+4+256];
    cdebug_type_name *old;
    int              size;

    tagbuff[0] = 'n';
    DbgFmtInt( &tagbuff[1], index );
    PutStream( dbgfile, tagbuff, 5 );
    tagbuff[0] = 'N';
    while( list != NULL ) {
        DbgFmtInt( &tagbuff[1], list->tref );
        tag=DbgFmtStr( &tagbuff[5], list->id.name, list->id.len );
        PutStream( dbgfile, tagbuff, tag-tagbuff );
        old = list;
        size = sizeof( *list ) + list->id.len;
        list = list->next;
        CGFree( old );
    }

}
