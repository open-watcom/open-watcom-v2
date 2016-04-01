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
* Description:  insert information into error messages
*
****************************************************************************/


#include "ftnstd.h"
#include "errcod.h"
#include "namecod.h"
#include "stmtsw.h"
#include "global.h"
#include "types.h"
#include "ferror.h"
#include "insert.h"
#include "utility.h"
#include "rstutils.h"
#include "rststruc.h"


extern  void            MsgBuffer(uint,char *,...);

extern  char            *StmtKeywords[];

typedef struct class_entry {
    char        *class;
    uint        id;
} class_entry;

static  class_entry     ClassMsg[] = {
    "PARAMETER",        0,
    NULL,               MS_STMT_FUNC,   // statement function
    "SUBROUTINE",       0,
    "REMOTE BLOCK",     0,
    "FUNCTION",         0,
    "BLOCK DATA",       0,
    "PROGRAM",          0,
    NULL,               MS_SIMP_VAR,    // simple variable
    NULL,               MS_SP_ARG,      // subprogram argument
    NULL,               MS_SF_ARG,      // statement function argument
    NULL,               MS_ARRAY,       // array
    NULL,               MS_COM_VAR,     // variable in COMMON
    NULL,               MS_VAR_ARR,     // variable or array
    NULL,               MS_EXT_PROC,    // external subprogram
    NULL,               MS_INTR_FUNC,   // intrinsic function
    "COMMON BLOCK",     0,
    NULL,               MS_EQUIV_VAR,   // equivalenced variable
    NULL,               MS_STRUCT_DEFN, // structure definition
    NULL,               MS_GROUP_NAME,  // group name
    NULL,               MS_ALLOC_ARRAY, // allocated array
};

#define MAX_MSGLEN      64      // maximum length of MS_xxx in error.msg


char    *PrmCodTab[] = {
    "expression",
    "simple variable",
    "array element",
    "substrung array element",
    "array name",
    "subprogram name",
    "subprogram name",
    "alternate return specifier"
};

static  uint    SymClass( sym_id sym ) {
//======================================

    unsigned_16 class;
    unsigned_16 flags;

    flags = sym->u.ns.flags;
    class = flags & SY_CLASS;
    if( class == SY_PARAMETER ) return( NAME_PARAMETER );
    if( class == SY_COMMON ) return( NAME_COMMON );
    if( class == SY_SUBPROGRAM ) {
        if( flags & SY_INTRINSIC ) return( NAME_INTRINSIC );
        flags &= SY_SUBPROG_TYPE;
        if( flags == SY_FN_OR_SUB ) return( NAME_EXT_PROC );
        return( flags >> SY_SUBPROG_IDX );
    }
    if( flags & SY_SUB_PARM ) return( NAME_ARGUMENT );
    if( (flags & SY_SPECIAL_PARM) && !(StmtSw & SS_DATA_INIT) ) return( NAME_SF_DUMMY );
    if( flags & SY_IN_COMMON ) return( NAME_COMMON_VAR );
    if( flags & SY_SUBSCRIPTED ) {
        if( _Allocatable( sym ) ) return( NAME_ALLOCATED_ARRAY );
        return( NAME_ARRAY );
    }
    if( flags & SY_PS_ENTRY ) return( NAME_FUNCTION );
    if( flags & SY_IN_EQUIV ) return( NAME_EQUIV_VAR );
    return( NAME_VARIABLE );
}


static  char    *GetClass( uint idx, char *buff ) {
//=================================================

    if( ClassMsg[idx].class != NULL ) return( ClassMsg[idx].class );
    MsgBuffer( ClassMsg[idx].id, buff );
    return( &buff[1] ); // skip the leading blank
}


static  char    *StmtName( char *buff ) {
//=======================================

    STMT    stmt;

    stmt = StmtProc;
    if( StmtSw & SS_DATA_INIT ) {
        stmt = PR_DATA;
    }
    switch( stmt ) {
    case PR_ASNMNT:
        MsgBuffer( MS_ASSIGN, buff );
        return( &buff[1] );     // skip leading blank
    case PR_ARIF:
        MsgBuffer( MS_ARITH_IF, buff );
        return( &buff[1] );     // skip leading blank
    case PR_STMTFUNC:
        MsgBuffer( MS_STMT_FUNC_DEFN, buff );
        return( &buff[1] );     // skip leading blank
    default:
        return( StmtKeywords[ stmt ] );
    }
}


void    StmtExtension( int errcode ) {
//====================================

    char        stmt[MAX_MSGLEN+1];

    Extension( errcode, StmtName( stmt ) );
}


void    StmtErr( int errcode ) {
//==============================

    char        stmt[MAX_MSGLEN+1];

    Error( errcode, StmtName( stmt ) );
}


void    StmtIntErr( int errcode, int num ) {
//==========================================

    char        stmt[MAX_MSGLEN+1];

    Error( errcode, StmtName( stmt ), num );
}


void    StmtPtrErr( int errcode, void *ptr ) {
//============================================

    char        stmt[MAX_MSGLEN+1];

    Error( errcode, StmtName( stmt ), ptr );
}


void    NameWarn( int errcod, sym_id sym ) {
//=========================================

    char        buff[MAX_SYMLEN+1];

    STGetName( sym, buff );
    Warning( errcod, buff );
}


void    NameErr( int errcod, sym_id sym ) {
//=========================================

    char        buff[MAX_SYMLEN+1];

    STGetName( sym, buff );
    Error( errcod, buff );
}


void    NameStmtErr( int errcod, sym_id sym, STMT stmt ) {
//============================================================

    char        buff[MAX_SYMLEN+1];

    STGetName( sym, buff );
    Error( errcod, buff, StmtKeywords[ stmt ] );
}


void    PrmCodeErr( int errcode, int code ) {
//===========================================

    Error( errcode, PrmCodTab[ code ] );
}


void    NameExt( int errcod, sym_id sym ) {
//=========================================

    char        buff[MAX_SYMLEN+1];

    STGetName( sym, buff );
    Extension( errcod, buff );
}


void    NamNamErr( int errcod, sym_id var1, sym_id var2 ) {
//=========================================================

    char        buff1[MAX_SYMLEN+1];
    char        buff2[MAX_SYMLEN+1];

    STGetName( var1, buff1 );
    STGetName( var2, buff2 );
    Error( errcod, buff1, buff2 );
}


void    ClassNameErr( int errcod, sym_id sym ) {
//==============================================

    char        buff[MAX_SYMLEN+1];
    char        class[MAX_MSGLEN+1];

    STGetName( sym, buff );
    Error( errcod, GetClass( SymClass( sym ), class ), buff );
}


void    PrevDef( sym_id sym ) {
//=============================

    char        buff[MAX_SYMLEN+1];
    char        class[MAX_MSGLEN+1];

    STGetName( sym, buff );
    Error( VA_PREV_DEF_NAM, buff, GetClass( SymClass( sym ), class ) );
}


void    NameTypeErr( int errcod, sym_id sym ) {
//=============================================

    char        buff[MAX_SYMLEN+1];

    STGetName( sym, buff );
    Error( errcod, buff, TypeKW( sym->u.ns.u1.s.typ ) );
}


void    TypeTypeErr( int errcod, TYPE typ1, TYPE typ2 ) {
//=======================================================

    Error( errcod, TypeKW( typ1 ), TypeKW( typ2 ) );
}


void    TypeNameTypeErr( int errcod, TYPE typ1, sym_id sym, TYPE typ2 ) {
//=======================================================================

    char        buff[MAX_SYMLEN+1];

    STGetName( sym, buff );
    Error( errcod, TypeKW( typ1 ), buff, TypeKW( typ2 ) );
}


void    TypeErr( int errcod, TYPE typ ) {
//=======================================

    Error( errcod, TypeKW( typ ) );
}


void    KnownClassErr( int errcod, uint idx ) {
//=============================================

    char        class[MAX_MSGLEN+1];

    Error( errcod, GetClass( idx, class ) );
}


void    ClassErr( int errcod, sym_id sym ) {
//==========================================

    char        class[MAX_MSGLEN+1];

    Error( errcod, GetClass( SymClass( sym ), class ) );
}


void    OpndErr( int errcod ) {
//=============================

    char        *str;

    str = MkNodeStr( CITNode );
    Error( errcod, str );
    FrNodeStr( str );
}


void    IllName( sym_id sym ) {
//=============================

    char        buff[MAX_SYMLEN+1];
    char        class[MAX_MSGLEN+1];
    char        stmt[MAX_MSGLEN+1];

    STGetName( sym, buff );
    Error( VA_ILL_USE, GetClass( SymClass( sym ), class ), buff,
                       StmtName( stmt ) );
}


void    IllType( sym_id sym ) {
//=============================

    char        buff[MAX_SYMLEN+1];
    char        stmt[MAX_MSGLEN+1];

    STGetName( sym, buff );
    Error( TY_ILL_USE, buff, TypeKW( sym->u.ns.u1.s.typ ), StmtName( stmt ) );
}


void    StructErr( int errcode, sym_id sym ) {
//============================================

    char        struct_name[MAX_SYMLEN+1];

    STStructName( sym, struct_name );
    Error( errcode, struct_name );
}


void    FieldErr( int errcode, sym_id sym ) {
//===========================================

    char        field_name[MAX_SYMLEN+1];

    STFieldName( sym, field_name );
    Error( errcode, field_name );
}
