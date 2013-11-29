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
#include <stdlib.h>
#include <string.h>
#include <malloc.h>
#include <ctype.h>
#include <stdarg.h>
#include <assert.h>
#include "global.h"
#include "hashtab.h"
#include "srusuprt.h"
#include "sruinter.h"
#include "ytab.h"
#include "mem.h"
#include "keywords.h"
#include "lexxer.h"
#include "gen_cpp.h"
#include "options.h"
#include "error.h"
#include "list.h"
#include "filelist.h"

static sru_file         SRU;
static BOOL             inSubPgm;
static BOOL             setHeader;

#define PROTO_PRIME     257
#define OVERHEAD        16

/* required strings */
#define SRU_HEADER              "PBExportHeader$"
#define THIS_VARIABLE           "this_hdl"
#define THIS_TYPE               "ulong"
#define DECL_THIS_VAR           THIS_TYPE" "THIS_VARIABLE
#define LEFT_PAREN              "( "
#define RIGHT_PAREN             " )"
#define COMMA_DELIM             ", "
#define RETURN_STATEMENT        "return "
#define VARIABLE_SECTION        "type variables\n"
#define END_VARIABLE_SECTION    "end variables\n\n"
#define TYPE_PROTOTYPE_SECTION  "type prototypes\n"
#define END_PROTOTYPE_SECTION   "end prototypes\n\n"
#define FUNCTION                "function "
#define SUBROUTINE              "subroutine "
#define LIBRARY                 "library "
#define DLL_SUFFIX              ".dll"
#define NONAME_FILE             "noname"
#define REF_MODIFIER            "ref "
#define DQUOTE                  "\""
#define CONSTRUCTOR_EVENT       "constructor"
#define DESTRUCTOR_EVENT        "destructor"
#define ON_CONSTRUCTOR          "\non constructor\n"
#define ON_DESTRUCTOR           "on destructor\n"
#define END_ON                  "end on\n\n"
#define DES_CALL_TEMPLATE       "%s( this_hdl )\n"
#define CONS_CALL_TEMPLATE      "this_hdl = %s( )\n"
#define DES_DECL_TEMPLATE       "subroutine %s( ulong this_hdl ) library \"%s.dll\"\n"
#define CONS_DECL_TEMPLATE      "function ulong %s( ) library \"%s.dll\"\n"

static statement *replaceStatement( statement *locale, char *stmt );
static statement *insertStatement( statement *locale, char *stmt );
static statement *appendStatement( char *stmt );
static void freeStatement( statement *curr );

void yyerror( char *msg ) {
/*************************/

    msg = msg;
    GetToEOS();
}


static id_type isTypeKnown( TypeInfo *typ ) {
/********************************************/

/* checks hash table to determine if a type is known and if it is, returns it id
*/

    long        len;
    id_type     rc;
    keyword     *tmp;

    assert( typ );

    len = strlen( typ->name );
    tmp = FindHashEntry( SRU.typ_tab, HashString( typ->name, len ),
                         typ->name, len );
    if( tmp ) {
        rc = tmp->id;
        if( typ->isref ) {
            rc |= TY_REF_INDICATOR;
        }
        return( rc );
    }
    return( 0 );
}


static BOOL isConsDes( const char *name ) {
/*****************************************/

    assert( name );

    return( !stricmp( name, SRU.con_name ) || !stricmp( name, SRU.des_name ) );
}

void GetConstructor( char *uoname, char *buf ) {
/***********************************************/
    GenerateCoverFnName( uoname, "_CPP_CONSTRUCTOR", buf );
}

void GetDestructor( char *uoname, char *buf ) {
/**********************************************/
    GenerateCoverFnName( uoname, "_CPP_DESTRUCTOR", buf );
}

static void mkConsDesNames( void ) {
/**********************************/

/* puts together SRU constructor and destructor names */

    int         len;
    char        *classname;

    classname = GetClass();
    assert( classname );

    if( SRU.con_name ) {
        MemFree( SRU.con_name );
    }
    if( SRU.des_name ) {
        MemFree( SRU.des_name );
    }

    len = strlen(classname);
    SRU.con_name = MemMalloc( PB_NAME_LEN + 1 );
    GetConstructor( classname, SRU.con_name );
    SRU.des_name = MemMalloc( PB_NAME_LEN + 1 );
    GetDestructor( classname, SRU.des_name );
}


void FreeSru( void ) {
/*********************/

    statement   *curr;

    if( SRU.name ) {
        MemFree( SRU.name );
    }
    if( SRU.con_name ) {
        MemFree( SRU.con_name );
    }
    if( SRU.name ) {
        MemFree( SRU.des_name );
    }
    while( SRU.head_stmt ) {
        curr = SRU.head_stmt;
        SRU.head_stmt = curr->next;
        freeStatement( curr );
    }
    DestroyHashTable( SRU.type_prots );
    DestroyHashTable( SRU.typ_tab );
    memset( &SRU, 0, sizeof( sru_file ) ); /* clear all records */
}


void InitSru( void ) {
/********************/

    long        x;

    memset( &SRU, 0, sizeof( sru_file ) );
    inSubPgm = FALSE;
    setHeader = FALSE;
    SRU.type_prots = NewHashTable( PROTO_PRIME );
    SRU.name = MemStrDup( NONAME_FILE );
    SetDefaultAccess( ST_PUBLIC );
    SetBaseName( SRU.name );
    if( Options & OPT_GEN_C_CODE ) {
        SRU.flags |= CONSTRUCTOR_DEFINED;
        SRU.flags |= DESTRUCTOR_DEFINED;
    }
    mkConsDesNames();

    /* loads types into hash table */
    SRU.typ_tab = NewHashTable( HASH_PRIME );
    x = 0;
    while( DataTypes[x].key != NULL ) {
        InsertHashValue( SRU.typ_tab, DataTypes[x].key,
                        strlen( DataTypes[x].key ), &DataTypes[x] );
        x++;
    }
}


void SetDefaultAccess( id_type type ) {
/**************************************/

    if( inSubPgm ) {
        return;
    }
    SRU.curr_access = type;
}

void SetHeader( char *name, char *ext ) {
/***************************************/

    unsigned            headerlen;

    ext = ext;
    headerlen = strlen( SRU_HEADER );
    if( inSubPgm || setHeader || strnicmp(name, SRU_HEADER, headerlen ) ) {
        return;
    }
    assert( name );
    assert( ext );

    setHeader = TRUE;

    if( SRU.name ) {
        MemFree( SRU.name );
    }
    SRU.name = MemStrDup( name );
    strcpy( SRU.name, name + headerlen );
    SetBaseName( SRU.name );
    mkConsDesNames();
}


void UserCode( void ) {
/*********************/

    if( !inSubPgm ) {
        return;
    }

    SRU.subprog->data.sp.user_code++;
}


void SetComment( void ) {
/***********************/

    if( !inSubPgm ) {
        return;
    }
    SRU.subprog->data.sp.user_code++; /* do not put any WIG code in */
}


void SetReturn( void ) {
/**********************/

    if( !inSubPgm ) {
        return;
    }
    SRU.flags |= RETURN_STMT_PRESENT;
}

static void copyTypeInfo( TypeInfo *dst, TypeInfo *src ) {
/*********************************************************/
    if( src->name == NULL ) {
        dst->name = NULL;
    } else {
        dst->name = MemStrDup( src->name );
    }
    dst->isref = src->isref;
}

void SetFunction( TypeInfo *ret, char *fname ) {
/***********************************************/

    if( inSubPgm ) {
        return;
    }

    assert( fname );
    assert( ret );

    copyTypeInfo( &( SRU.curr.sp.ret_type ), ret );
    SRU.curr.sp.name = MemStrDup( fname );
    SRU.curr.sp.typ = ST_FUNCTION;
    SRU.curr_typ = SRU_SUBPROG;
    SRU.curr.sp.typ_id = isTypeKnown( ret ); /* we must first set sp info */
    if( !SRU.curr.sp.typ_id ) {
        /* issue warning if unknown type is found and set fake bit */
        /* only issue a warning for forward definitions so we don't issue
         * the same warning twice */
        if( SRU.sections->data.sec.primary == ST_FORWARD ) {
            Warning( UNKNOWN_TYPE, ret->name, fname );
        }
        SRU.curr.sp.fake = TRUE;
    }
}


void SetSubroutine( char *sname ) {
/*********************************/

    if( inSubPgm ) {
        return;
    }
    assert( sname );

    SRU.curr.sp.ret_type.name = NULL;
    SRU.curr.sp.ret_type.isref = FALSE;
    SRU.curr.sp.name = MemStrDup( sname );
    SRU.curr.sp.typ = ST_SUBROUTINE;
    SRU.curr.sp.subroutine = TRUE;
    SRU.curr_typ = SRU_SUBPROG;
}


void StartSubProgram( void ) {
/****************************/

/* do some house cleaning before we start processing subprograms */

    if( inSubPgm || SRU.sections ) {
        return;
    }

    if( SRU.var_sec == NULL && !(Options & OPT_GEN_C_CODE) ) {
        SRU.var_sec = appendStatement( VARIABLE_SECTION );
        appendStatement( END_VARIABLE_SECTION );
    }
    if( SRU.type_sec == NULL ) {
        SRU.type_sec = appendStatement( TYPE_PROTOTYPE_SECTION );
        appendStatement( END_PROTOTYPE_SECTION );
    }
}


void EndSubProgram( void ) {
/**************************/

/* determine wether to munge the current subprogram to make an
   external call, and then closes this subprogram
*/

    char        *line;
    var_rec     *finger;
    int         size;
    char        *ret;
    char        *name;
    statement   *curr;

    if( !inSubPgm ) {
        return;
    }

    inSubPgm = FALSE;

    assert( SRU.subprog );

    /* determine whether we should munge the  subprogram */
    if( SRU.subprog->data.sp.user_code > 1 ) {
        return;
    } else if( !(SRU.subprog->data.sp.typ_id && SRU.subprog->data.sp.ret_stmt)
                && SRU.subprog->data.sp.user_code ) {
        return;
    }

    /* munge subprogram */

    /* calculate size of statement */
    name = SRU.subprog->data.sp.name;
    size = strlen( name );
    size += sizeof(LEFT_PAREN) + sizeof(RIGHT_PAREN) + sizeof(THIS_VARIABLE);
    finger = SRU.subprog->data.sp.parm_list;
    while( finger ) {
        size += strlen( finger->name ) + sizeof( COMMA_DELIM );
        finger = finger->next;
    }
    if( !SRU.subprog->data.sp.subroutine ) {
        size += sizeof( RETURN_STATEMENT );
    }

    /* allocate memory and create function call */
    line = alloca( size );
    if( !SRU.subprog->data.sp.subroutine ) {
        ret = RETURN_STATEMENT;
    } else {
        ret = "";
    }

    finger = SRU.subprog->data.sp.parm_list;
    if( Options & OPT_GEN_C_CODE ) {
        sprintf( line, "%s%s%s", ret, name, LEFT_PAREN );
    } else {
        sprintf( line, "%s%s%s%s", ret, name, LEFT_PAREN, THIS_VARIABLE );
        if( finger ) {
            strcat( line, COMMA_DELIM );
        }
    }
    while( finger ) {
        strcat( line, finger->name );
        if( finger->next ) {
            strcat( line, COMMA_DELIM );
        }
        finger = finger->next;
    }
    strcat( line, RIGHT_PAREN );
    strcat( line, "\n" );

    /* determine where to place the new call, and place it there */
    if( SRU.subprog->data.sp.typ_id && (SRU.subprog->data.sp.user_code == 1) &&
        SRU.subprog->data.sp.ret_stmt ) {
        curr = SRU.subprog;
        while( curr && ( curr->next != SRU.subprog->data.sp.ret_stmt ) ) {
            curr = curr->next;
        }
        if( curr ) {
            replaceStatement( curr, line );
        } else {
            insertStatement( SRU.subprog, line );
        }
    } else {
        insertStatement( SRU.subprog, line );
    }
}


void FiniParmList( void ) {
/*************************/

    /* currently we do nothing */
}


void AddParm( TypeInfo *typ, char *tname, ArrayInfo *array ) {
/*************************************************************/

/* add parameter to current function's list */

    var_rec     *ptmp;

    if( inSubPgm ) {
         return;
    }

    assert( tname );
    assert( typ );

    ptmp = MemMalloc( sizeof( var_rec ) );
    memset( ptmp, 0, sizeof( var_rec ) );
    if( SRU.curr.sp.last_parm ) {
        SRU.curr.sp.last_parm->next = ptmp;
    } else {
        SRU.curr.sp.parm_list = ptmp;
    }
    SRU.curr.sp.last_parm = ptmp;
    copyTypeInfo( &ptmp->type, typ );
    ptmp->typ_id = isTypeKnown( typ );
    if( !ptmp->typ_id ) {
        ptmp->fake = TRUE;
    }
    ptmp->next = NULL;
    ptmp->name = MemStrDup( tname );
    if( array == NULL ) {
        ptmp->array = NULL;
    } else {
        ptmp->array = MemMalloc( sizeof( ArrayInfo ) );
        memcpy( ptmp->array, array, sizeof( ArrayInfo ) );
    }
}


void StartSection( id_type ptyp, id_type styp ) {
/***********************************************/

    if( inSubPgm ) {
         return;
    }

    SRU.curr.sec.primary = ptyp;
    SRU.curr.sec.secondary = styp;
    SRU.curr_typ = SRU_SECTION;
    if( styp == ST_VARIABLES ) {
        if( ptyp == ST_SHARED ) {
            SetDefaultAccess( ST_PRIVATE );
        } else if( ptyp == ST_TYPE ) {
            SetDefaultAccess( ST_PUBLIC );
        }
    }
}


void EndSection(void) {
/*********************/

    if( inSubPgm ) {
         return;
    }

    if( SRU.sections ) {
        SRU.sections = SRU.sections->link;
    }
}

void AddDataMethod2( TypeInfo *typ, List *varlist ) {
/*****************************************************/
    AddDataMethod( SRU.curr_access, typ, varlist );
}

void AddDataMethod( id_type access_type, TypeInfo *typ, List *varlist ) {
/************************************************************************/

    unsigned            varcnt;
    VarInfo             *info;

    if( inSubPgm ) {
         return;
    }

    assert( varlist );
    assert( typ );

    SRU.curr.vars.varlist = varlist;
    copyTypeInfo( &(SRU.curr.vars.type), typ );
    SRU.curr.vars.access_id = access_type;
    SRU.curr.vars.typ_id = isTypeKnown( &SRU.curr.vars.type );
    if( !SRU.curr.vars.typ_id ) {
        SRU.curr.vars.fake = TRUE;
        // NYI -  put out a good error msg
        varcnt = GetListCount( SRU.curr.vars.varlist );
        for( ; varcnt > 0; varcnt-- ) {
            info = GetListItem( SRU.curr.vars.varlist, varcnt - 1 );
            Warning( UNKNOWN_DATA_TYPE, info->name, typ->name );
        }
    } else {
        SRU.curr.vars.fake = FALSE;
    }
    SRU.curr_typ = SRU_VARIABLE;
}


void RegisterEvent( char *event ) {
/*********************************/

    assert( event );

    if( !stricmp( event, CONSTRUCTOR_EVENT ) ) {
        SRU.flags |= CONSTRUCTOR_DEFINED;
    } else if( !stricmp( event, DESTRUCTOR_EVENT ) ) {
        SRU.flags |= DESTRUCTOR_DEFINED;
    }
}


static void dumpStatements( void ) {
/**********************************/

/* loop through statements and create new sru file by dumping them out */

    FILE        *fout;
    statement   *curr;

    /* open file */
    fout = WigOpenFile( GetOutputFile(), "wt" );
    if( !fout ) {
        Error( FILE_OPEN_ERR, GetOutputFile() );
    }

    /* loop and udmp */
    curr = SRU.head_stmt;
    while( curr ) {
        assert( curr->stmt );
        fprintf( fout, "%s", curr->stmt );
        SRU.head_stmt = curr->next;
        if( curr->keep ) {
            curr = curr->next;
        } else {
            /* free if not required for back end */
            freeStatement( curr );
            curr = SRU.head_stmt;
        }
    }
    WigCloseFile( fout );
}

static statement *insertTypePrototype( statement *func, statement *locale ) {
/***************************************************************************/

/* insert a prototype of the external function into the types protoype section*/

    char        *line;
    char        *name;
    char        *libname;
    char        *sp;
    char        *ret;
    var_rec     *finger;
    statement   *rc;
    long        size;
    TypeInfo    *typ;
    char        typbuf[64];
    TypeInfo    tmptype;

    assert( func );
    assert( locale );

    name =  func->data.sp.name;

    /* determine return type */
    if( !func->data.sp.subroutine ) {
        typ = &func->data.sp.ret_type;
        sp = FUNCTION;
        if( _IsRefType( func->data.sp.typ_id ) ) {
            strcpy( typbuf, REF_MODIFIER );
            strcat( typbuf, typ->name );
            ret = typbuf;
            if( !func->data.sp.typ_id ) {
                func->data.sp.fake = TRUE;
            }
        } else {
            ret = typ->name;
        }
    } else {
        ret = "";
        sp = SUBROUTINE;
    }

    /* calculate size of statement */
    libname = GetLibName();
    size = strlen( name ) + strlen( sp ) + strlen( ret ) + strlen( libname );
    size += sizeof( DECL_THIS_VAR ) + sizeof( LIBRARY  ) + sizeof( LEFT_PAREN )
          + sizeof( RIGHT_PAREN ) + sizeof( DLL_SUFFIX ) + sizeof( DQUOTE )
          + sizeof( DQUOTE ) + OVERHEAD;

    finger = func->data.sp.parm_list;
    while( finger ) {
        size += sizeof( COMMA_DELIM ) + strlen( finger->name ) + 3;
        if( _IsRefType( finger->typ_id ) ) {
            size += sizeof( REF_MODIFIER );
        }
        size += strlen( finger->type.name );
        finger = finger->next;
    }


    /* build the statement */
    line = alloca( size );
    finger = func->data.sp.parm_list;
    if( Options & OPT_GEN_C_CODE ) {
        sprintf( line, "%s %s %s %s ", sp, ret, name, LEFT_PAREN );
    } else {
        sprintf( line, "%s %s %s %s %s", sp, ret, name, LEFT_PAREN,
                                        DECL_THIS_VAR );
        if( finger ) {
            strcat( line, COMMA_DELIM );
        }
    }
    while( finger ) {
        if( _IsRefType( finger->typ_id ) ) {
            strcat( line, REF_MODIFIER );
        }
        strcat( line, finger->type.name );
        strcat( line, " " );
        strcat( line, finger->name );
        if( finger->next ) {
            strcat( line, COMMA_DELIM );
        }
        finger = finger->next;
    }
    strcat( line, RIGHT_PAREN );
    strcat( line, " " );
    strcat( line, LIBRARY );
    strcat( line, DQUOTE );
    strcat( line, libname );
    strcat( line, DLL_SUFFIX );
    strcat( line, DQUOTE );
    strcat( line, "\n" );

    /* insert statement into our statement linked list */
    rc = insertStatement( locale, line );

    // beware - when pointers get added to the sp_header structure they
    //          must be explicitly copied here or else things will get freed
    //          twice.  This code should be rewritten. DRW
    memcpy( &(rc->data), &(func->data), sizeof( spec ) );
    rc->data.sp.name = MemStrDup( name );
    copyTypeInfo( &rc->data.sp.ret_type, &func->data.sp.ret_type );
    finger = func->data.sp.parm_list;
    SRU.curr.sp.parm_list = NULL;
    SRU.curr.sp.last_parm = NULL;
    tmptype.name = THIS_TYPE;
    tmptype.isref = FALSE;
    AddParm( &tmptype, THIS_VARIABLE, NULL );
    while( finger ) {
        AddParm( &finger->type, finger->name, finger->array );
        finger = finger->next;
    }
    rc->data.sp.parm_list = SRU.curr.sp.parm_list;
    rc->data.sp.last_parm = SRU.curr.sp.last_parm;
    rc->typ = SRU_SUBPROG;
    return( rc );
}


static void resolveTypePrototypes( void ) {
/*****************************************/

/* adds required protoypes to the protoype section */

    statement   *finger;
    statement   *rc;
    long        len;
    char        *name;
    char        *libname;

    if( !SRU.type_sec ) {
        return;
    }

    finger = SRU.forward_prots;

    /* loop through the chain of prototypes and add them if necessary */
    while( finger ) {
        name = finger->data.sp.name;
        len = strlen( name );

        /* check in hash table for function name */
        if( !FindHashEntry(SRU.type_prots, HashString(name, len), name, len) ) {

            /* add to prototype section */
            rc = insertTypePrototype( finger, SRU.type_sec );
            rc->link = SRU.cpp_prots;
            SRU.cpp_prots = rc;
            rc->keep = TRUE;
            InsertHashValue( SRU.type_prots, rc->data.sp.name,
                             strlen( rc->data.sp.name ), rc );
        }
        finger = finger->link;
    }

    /* generate constructors and destructor prototypes if necessary */
    libname = GetLibName();
    len = max( sizeof( DES_DECL_TEMPLATE ) + strlen( SRU.des_name ),
               sizeof( CONS_DECL_TEMPLATE ) + strlen( SRU.con_name ) );
    name = alloca( len + strlen( libname ) + 1 );
    if( !( SRU.flags & DESTRUCTOR_DEFINED ) ) {
        sprintf( name, DES_DECL_TEMPLATE, SRU.des_name, libname );
        insertStatement( SRU.type_sec, name );
    }
    if( !( SRU.flags & CONSTRUCTOR_DEFINED ) ) {
        sprintf( name, CONS_DECL_TEMPLATE, SRU.con_name, libname );
        insertStatement( SRU.type_sec, name );
    }
}


static void resolveConstructor_Destructor( void ) {
/*************************************************/

/* generate constructors and destructors if necessary */

    char        *buff;

    if( !( SRU.flags & CONSTRUCTOR_DEFINED ) ) {
        buff = alloca( sizeof(CONS_CALL_TEMPLATE) + strlen(SRU.con_name) + 1 );
        appendStatement( ON_CONSTRUCTOR );
        sprintf( buff, CONS_CALL_TEMPLATE, SRU.con_name );
        appendStatement( buff );
        appendStatement( END_ON );
    }
    if( !( SRU.flags & DESTRUCTOR_DEFINED ) ) {
        buff = alloca( sizeof(DES_CALL_TEMPLATE) + strlen(SRU.des_name) + 1 );
        appendStatement( ON_DESTRUCTOR );
        sprintf( buff, DES_CALL_TEMPLATE, SRU.des_name );
        appendStatement( buff );
        appendStatement( END_ON );
    }
}

void DoPostProcessing( void ) {
/*****************************/
    resolveTypePrototypes();
}

void FiniSru( void ) {
/********************/

/* finish processing sru */

    resolveConstructor_Destructor();
    dumpStatements();
}


void GenCPPInterface( void ) {
/****************************/

    GenerateCPPFiles( &SRU );
}


void ProcessStatement( void ) {
/*****************************/

/* process an sru statement */

    statement   *stmt;
    statement   *sc;
    var_rec     *parm;

    /* create statement */
    FinishLine();
    stmt = appendStatement( GetParsedLine() );
    stmt->typ = SRU.curr_typ;
    memcpy( &( stmt->data ), &( SRU.curr ), sizeof( spec ) );

    /* depending on type, munge it */
    switch( SRU.curr_typ ) {
    case( SRU_OTHER ):
        /* check for return statements in sru */
        if( inSubPgm && ( SRU.flags & RETURN_STMT_PRESENT ) ) {
            SRU.subprog->data.sp.ret_stmt = stmt;
            SRU.flags &= ~RETURN_STMT_PRESENT;
        }
        break;
    case( SRU_SUBPROG ):
        /* make sure to add prototypes to apropriate section */
        if( inSubPgm ) {
            break;
        } else if( !SRU.sections ) {
            inSubPgm = TRUE;
        }
        parm = stmt->data.sp.parm_list;
        while( parm != NULL ) {
            if( parm->fake ) {
                stmt->data.sp.fake = TRUE;
                /* only issue a warning for forward definitions so we don't
                 * issue the same warning twice */
                if( SRU.sections->data.sec.primary == ST_FORWARD ) {
                    Warning( UNKNOWN_TYPE, parm->type.name,
                             stmt->data.sp.name );
                }
                break;
            }
            if( parm->array != NULL ) {
                if( parm->array->flags & ARRAY_MULTI_DIM ) {
                    stmt->data.sp.fake = TRUE;
                    if( SRU.sections->data.sec.primary == ST_FORWARD ) {
                        Warning( ERR_MULTI_DIM_PARM, parm->name,
                                 stmt->data.sp.name );
                    }
                } else if( parm->array->flags & ARRAY_RANGE ) {
                    stmt->data.sp.fake = TRUE;
                    if( SRU.sections->data.sec.primary == ST_FORWARD ) {
                        Warning( ERR_INDEX_DEF_PARM, parm->name,
                                 stmt->data.sp.name );
                    }
                }
            }
            parm = parm->next;
        }
        SRU.subprog = stmt;
        sc = SRU.sections;
        if( !inSubPgm && sc && ( sc->data.sec.secondary == ST_PROTOTYPES ) ) {
            if( sc->data.sec.primary == ST_FORWARD ) {
                stmt->link = SRU.forward_prots;
                SRU.forward_prots = stmt;
            } else if( sc->data.sec.primary == ST_TYPE ) {
                if( !(Options & OPT_GEN_C_CODE) && isConsDes(stmt->data.sp.name) ) {
                    break;
                }
                InsertHashValue( SRU.type_prots, stmt->data.sp.name,
                                 strlen( stmt->data.sp.name ), stmt );
                stmt->link = SRU.cpp_prots;
                SRU.cpp_prots = stmt;
                stmt->keep = TRUE;
            }
        }
        break;
    case( SRU_VARIABLE ):
        /* add global variables to appropriate section */
        if( inSubPgm ) {
            break;
        }
        if( SRU.sections->data.sec.primary == ST_SHARED ) {
            stmt->link = SRU.shared_vars;
            SRU.shared_vars = stmt;
        } else {
            stmt->link = SRU.obj_vars;
            SRU.obj_vars = stmt;
        }
        stmt->keep = TRUE;
        break;
    case( SRU_SECTION ):
        /* identify two special sections when they come up */
        if( inSubPgm ) {
            break;
        }
        stmt->link = SRU.sections;
        SRU.sections = stmt;
        if( stmt->data.sec.primary == ST_TYPE ) {
            if( stmt->data.sec.secondary == ST_PROTOTYPES ) {
                SRU.type_sec = stmt;
            } else if( stmt->data.sec.secondary == ST_VARIABLES ) {
                SRU.var_sec = stmt;
            }
        }
        break;
    default:
        assert( FALSE );
    }
    memset( &( SRU.curr ), 0, sizeof( spec ) );
    SRU.curr_typ = SRU_OTHER;
}


static statement *mkStatement( char *stmt ) {
/*******************************************/

    statement   *ret;

    assert( stmt );

    ret = MemMalloc( sizeof( statement ) + strlen( stmt ) + 1 );
    memset( ret, 0, sizeof( statement ) );
    strcpy( ret->stmt, stmt );
    return( ret );
}


static statement *insertStatement( statement *locale, char *stmt ) {
/******************************************************************/

    statement   *link;

    assert( stmt );
    assert( locale );

    link = mkStatement( stmt );
    link->next = locale->next;
    locale->next = link;
    if( SRU.tail_stmt == locale ) {
        SRU.tail_stmt = link;
    }
    return( link );
}


static statement *appendStatement( char *stmt ) {
/***********************************************/

    statement   *link;

    assert( stmt );

    link = mkStatement( stmt );
    if( SRU.tail_stmt ) {
        SRU.tail_stmt->next = link;
    } else {
        SRU.head_stmt = link;
    }
    SRU.tail_stmt = link;
    return( link );
}


static statement *replaceStatement( statement *locale, char *stmt ) {
/*******************************************************************/

    statement   *link;

    assert( stmt );
    assert( locale );

    if( !locale->next ) {
        locale->next = mkStatement( stmt );
        SRU.tail_stmt = locale->next;
    } else {
        link = locale->next;
        locale->next = mkStatement( stmt );
        locale->next->next = link->next;
        if( link == SRU.tail_stmt ) {
            SRU.tail_stmt = locale->next;
        }
        if( !link->keep ) {
            freeStatement( link );
        }
    }
    return( locale->next );
}

static void freeVarList( void *cur ) {
/****************************************/
    MemFree( ((VarInfo *)cur)->name );
}

static void freeStatement( statement *curr ) {
/********************************************/

    var_rec     *parm;

    assert( curr );

    switch( curr->typ ) {
    case( SRU_SUBPROG ):
        while( curr->data.sp.parm_list ) {
            parm = curr->data.sp.parm_list;
            curr->data.sp.parm_list = parm->next;
            MemFree( parm->name );
            MemFree( parm->type.name );
            if( parm->array != NULL ) MemFree( parm->array );
            MemFree( parm );
        }
        MemFree( curr->data.sp.name );
        if( !curr->data.sp.subroutine ) {
            MemFree( curr->data.sp.ret_type.name );
        }
        break;
    case( SRU_VARIABLE ):
        MemFree( curr->data.vars.type.name );
        FreeList( curr->data.vars.varlist, freeVarList );
        break;
    case( SRU_SECTION ):
    case( SRU_OTHER ):
        break;
    default:
        assert( FALSE );
    }
    MemFree( curr );
}
