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
#include <stdarg.h>
#include <string.h>
#include <io.h>
#include "defgen.h"
#include "semantic.h"
#include "ytab.h"
#include "output.h"

typedef struct {
    char        ptrcnt;
    char        arraycnt;
    char        *pbtype;                // type when this is a parameter
    char        pbarraycnt;             // array count when a parameter
    char        *pbrettype;             // type if this is a return value
} ConvInfo;

typedef struct {
    char        *ctype;
    ConvInfo    *info;
} FullConvInfo;

typedef struct {
    char        *ctype;
    char        *pbtype;
} SimpleConvInfo;

#define PTR_CONVERT( type, conv ) \
static ConvInfo type ## Conv[] = { \
    0,  0,      "REF " conv,    0, NULL, \
    0,  0,      NULL,           0, NULL  \
};

static ConvInfo CharConv[] = {
    0,  0,      "char",         0, "char",
    1,  0,      "REF String",   0, "String",
    0,  1,      "REF char",     1, NULL,
    1,  1,      "REF String",   1, NULL,
    0,  0,      NULL,           0, NULL
};

static ConvInfo VoidConv[] = {
    0,  0,      "",             0, NULL,
    1,  0,      "Blob",         0, NULL,
    0,  0,      NULL,           0, NULL
};

static ConvInfo LpvoidConv[] = {
    0,  0,      "Blob",         0, NULL,
    0,  0,      NULL,           0, NULL
};

static ConvInfo LpstrConv[] = {
    0,  0,      "REF String",   0, "String",
    0,  1,      "REF String",   1, NULL,
    0,  0,      NULL,           0, NULL
};

PTR_CONVERT( Lpbyte,    "char" )
PTR_CONVERT( Lpint,     "int" )
PTR_CONVERT( Lpword,    "Uint" )
PTR_CONVERT( Lplong,    "long" )
PTR_CONVERT( Lpdword,   "Ulong" )

static FullConvInfo     SpecialConversion[] = {
    "LPBYTE",   &LpbyteConv,
    "LPCSTR",   &LpstrConv,
    "LPDWORD",  &LpdwordConv,
    "LPINT",    &LpintConv,
    "LPLONG",   &LplongConv,
    "LPSTR",    &LpstrConv,
    "LPVOID",   &LpvoidConv,
    "LPWORD",   &LpwordConv,
    "char",     &CharConv,
    "void",     &VoidConv
};

static SimpleConvInfo   Conversion[] = {
    "ATOM",             "UInt",
    "BOOL",             "Boolean",
    "BYTE",             "char",
    "COLORREF",         "Ulong",
    "DWORD",            "Ulong",
    "HACCEL",           "UInt",
    "HANDLE",           "UInt",
    "HBITMAP",          "UInt",
    "HBRUSH",           "UInt",
    "HCURSOR",          "UInt",
    "HDC",              "UInt",
    "HDRVR",            "UInt",
    "HDWP",             "UInt",
    "HFILE",            "UInt",
    "HFONT",            "UInt",
    "HGDIOBJ",          "UInt",
    "HGLOBAL",          "UInt",
    "HHOOK",            "UInt",
    "HICON",            "UInt",
    "HINSTANCE",        "UInt",
    "HLOCAL",           "UInt",
    "HMENU",            "UInt",
    "HMETAFILE",        "UInt",
    "HMODULE",          "UInt",
    "HPALETTE",         "UInt",
    "HPEN",             "UInt",
    "HRGN",             "UInt",
    "HRSRC",            "UInt",
    "HSTR",             "UInt",
    "HTASK",            "UInt",
    "HWND",             "UInt",
    "LPARAM",           "long",
    "LRESULT",          "long",
    "UINT",             "UInt",
    "WORD",             "UInt",
    "WPARAM",           "int",
    "double",           "double",
    "float",            "real",
    "int",              "int",
    "long",             "long",
    "short",            "int",
    "signed",           "int",
    "signed char",      "char",
    "signed int",       "char",
    "signed long",      "char",
    "signed short",     "char",
    "unsigned",         "UInt",
    "unsigned char",    "char",
    "unsigned int",     "UInt",
    "unsigned long",    "Ulong",
    "unsigned short",   "UInt"
};

static char             ErrBuf[ 2048 ];
static SymbolList       SymbolInfo;

static int SimpleComp( const void *p1, const void *p2 ) {

    SimpleConvInfo      *ptr;

    ptr = (SimpleConvInfo *)p2;
    return( strcmp( p1, ptr->ctype ) );
}

static int SpecialComp( const void *p1, const void *p2 ) {

    FullConvInfo        *ptr;

    ptr = (FullConvInfo *)p2;
    return( strcmp( p1, ptr->ctype ) );
}


static BOOL ConvertType( char *type, int ptrcnt, int arraycnt,
                         PBTypeInfo *ret, BOOL is_rettype )
{
    SimpleConvInfo      *simple;
    FullConvInfo        *special;
    ConvInfo            *cur;

    /* search simple types */
    simple = bsearch( type, Conversion,
                     sizeof( Conversion ) / sizeof( SimpleConvInfo ),
                     sizeof( SimpleConvInfo ), SimpleComp );
    if( simple != NULL ) {
        if( is_rettype && ptrcnt + arraycnt > 0 ) return( TRUE );
        if( ptrcnt + arraycnt > 1 ) return( TRUE );
        ret->arraycnt = arraycnt;
        if( ptrcnt > 0 ) {
            ret->modifiers = "REF ";
        } else {
            ret->modifiers = "";
        }
        ret->type = simple->pbtype;
        return( FALSE );
    }
    /* search special types */
    special = bsearch( type, SpecialConversion,
                     sizeof( SpecialConversion ) / sizeof( FullConvInfo ),
                     sizeof( FullConvInfo ), SpecialComp );
    if( special == NULL ) return( TRUE );
    cur = special->info;
    while( cur->pbtype != NULL ) {
        if( cur->ptrcnt == ptrcnt && cur->arraycnt == arraycnt ) {
            ret->modifiers = "";
            if( is_rettype ) {
                if( cur->pbrettype == NULL ) break;
                ret->type = cur->pbrettype;
                ret->arraycnt = 0;
            } else {
                ret->type = cur->pbtype;
                ret->arraycnt = cur->pbarraycnt;
            }
            return( FALSE );
        }
        cur++;
    }
    return( TRUE );
}

static void FreeDefInfo( PBDefInfo *info ) {

    PBTypeInfo          *cur;

    while( info->parms != NULL ) {
        cur = info->parms;
        info->parms = cur->next;
        free( cur );
    }
    free( info );
}

static void FreeFuncInfo( ParamInfo *info ) {

    ParamInfo           *cur;

    while( info != NULL ) {
        if( info->parameters != NULL ) {
            FreeFuncInfo( info->parameters );
        }
        cur = info;
        info = info->next;
        if( cur->modifiers != NULL ) free( cur->modifiers );
        if( cur->type != NULL ) {
            if( cur->type->typename != NULL ) free( cur->type->typename );
            free( cur->type );
        }
        if( cur->name != NULL ) free( cur->name );
        free( cur );
    }
}

static char *fmtType( ParamInfo *info ) {
    unsigned    i;

    sprintf( ErrBuf, "%s", info->type->typename );
    for( i=0; i < info->ptrcnt; i++ ) {
        strcat( ErrBuf, "*" );
    }
    if( info->ptrcnt > 0 ) {
        strcat( ErrBuf, " " );
        for( i=0; i < info->arraycnt; i++ ) {
            strcat( ErrBuf, "[]" );
        }
    }
    return( ErrBuf );
}

void SemFunction( ParamInfo *finfo ) {
    BOOL        rc;
    ParamInfo   *cur;
    PBTypeInfo  *pbtype;
    PBDefInfo   *pbdef;
    unsigned    parmcnt;
    unsigned    genparmcnt;
    char        *type;

    genparmcnt = 0;
    if( finfo->modifiers->m_pascal && strcmp( finfo->name, "LibMain" )
        && strcmp( finfo->name, "WEP" ) && !finfo->err ) {
        pbdef = malloc( sizeof( PBDefInfo ) + strlen( finfo->name ) );
        memset( pbdef, 0, sizeof( PBDefInfo ) );
        strcpy( pbdef->name, finfo->name );
        if( !strcmp( finfo->type->typename, "void" ) && finfo->ptrcnt == 0 ) {
            pbdef->type = PB_SUBROUTINE;
        } else {
            pbdef->type = PB_FUNCTION;
            rc = ConvertType( finfo->type->typename, finfo->ptrcnt,
                              0, &(pbdef->rettype), TRUE );
            if( rc ) {
                type = fmtType( finfo );
                ReportWarning( "Can't convert return type '%s' of function %s",
                             type, finfo->name );
                FreeFuncInfo( finfo );
                FreeDefInfo( pbdef );
                return;
            }
        }
        cur = finfo->parameters;
        parmcnt = 0;
        while( cur != NULL ) {
            if( cur->err ) {
                FreeFuncInfo( finfo );
                FreeDefInfo( pbdef );
                return;
            }
            if( cur->parameters != NULL ) {
                ReportWarning( "Function pointers are not supported as parameters" );
                FreeFuncInfo( finfo );
                FreeDefInfo( pbdef );
                return;
            }
            if( cur->name != NULL ) {
                pbtype = malloc( sizeof( PBTypeInfo ) + strlen( cur->name ) );
                memset( pbtype, 0, sizeof( PBTypeInfo ) );
                strcpy( pbtype->name, cur->name );
            } else {
                pbtype = malloc( sizeof( PBTypeInfo ) + 5 );
                memset( pbtype, 0, sizeof( PBTypeInfo ) );
                sprintf( pbtype->name, "p%d", genparmcnt );
                genparmcnt++;
            }
            rc = ConvertType( cur->type->typename, cur->ptrcnt,
                             cur->arraycnt, pbtype, FALSE );
            if( rc ) {
                type = fmtType( cur );
                ReportWarning( "Can't convert type of parameter %d '%s' in function %s",
                                parmcnt + 1, type, finfo->name );
                FreeFuncInfo( finfo );
                FreeDefInfo( pbdef );
                free( pbtype );
                return;
            }
            if( pbdef->lastparm == NULL ) {
                pbdef->parms = pbtype;
            } else {
                pbdef->lastparm->next = pbtype;
            }
            pbdef->lastparm = pbtype;
            cur = cur->next;
            parmcnt ++;
        }
        if( SymbolInfo.lastfunc == NULL ) {
            SymbolInfo.func = pbdef;
        } else {
            SymbolInfo.lastfunc->next = pbdef;
        }
        SymbolInfo.lastfunc = pbdef;
    }
    FreeFuncInfo( finfo );
}

void DoOutput( void ) {
    PBDefInfo   *cur;

    PrintPBDef( &SymbolInfo );
    if( Config.nuo_name != NULL ) {
        CreateNonVisualObj( &SymbolInfo );
    }
    while( SymbolInfo.func != NULL ) {
        cur = SymbolInfo.func;
        SymbolInfo.func = SymbolInfo.func->next;
        FreeDefInfo( cur );
    }
}

ParamInfo *SemCreateParam( TypeInfo *tinfo, char *name, int ptrcnt,
                           int arraycnt )
{
    ParamInfo   *ret;

    ret = malloc( sizeof( ParamInfo ) );
    memset( ret, 0, sizeof( ParamInfo ) );
    ret->type = tinfo;
    ret->name = name;
    ret->ptrcnt = ptrcnt;
    ret->arraycnt = arraycnt;
    return( ret );
}

ModifierInfo *SemAddSpecifier( ModifierInfo *table, int tok ) {
    if( table == NULL ) {
        table = malloc( sizeof( ModifierInfo ) );
        memset( table, 0, sizeof( ModifierInfo ) );
    }
    switch( tok ) {
    case T_NEAR:
        table->m_near = TRUE;
        break;
    case T_CONST:
        table->m_const = TRUE;
        break;
    case T_FAR:
        table->m_far = TRUE;
        break;
    case T_HUGE:
        table->m_huge = TRUE;
        break;
    case T_CDECL:
        table->m_cdecl = TRUE;
        break;
    case T_PASCAL:
        table->m_pascal = TRUE;
        break;
    case T_FORTRAN:
        table->m_fortran = TRUE;
        break;
    case T_SYSCALL:
        table->m_syscall = TRUE;
        break;
    case T_EXPORT:
        table->m_export = TRUE;
        break;
    }
    return( table );
}
