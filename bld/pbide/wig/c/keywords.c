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
#include <string.h>
#include <assert.h>
#include "global.h"
#include "types.h"
#include "sruinter.h"
#include "ytab.h"
#include "keywords.h"

keyword         Statements[] = {
        NULL, "end",            ST_END,         FALSE,
        NULL, "forward",        ST_FORWARD,     FALSE,
        NULL, "from",           ST_FROM,        FALSE,
        NULL, "function",       ST_FUNCTION,    FALSE,
        NULL, "global",         ST_GLOBAL,      FALSE,
        NULL, "library",        ST_LIBRARY,     FALSE,
        NULL, "on",             ST_ON,          FALSE,
        NULL, "private",        ST_PRIVATE,     FALSE,
        NULL, "protected",      ST_PROTECTED,   FALSE,
        NULL, "prototypes",     ST_PROTOTYPES,  FALSE,
        NULL, "public",         ST_PUBLIC,      FALSE,
        NULL, "ref",            ST_REF,         FALSE,
        NULL, "return",         ST_RETURN,      FALSE,
        NULL, "shared",         ST_SHARED,      FALSE,
        NULL, "subroutine",     ST_SUBROUTINE,  FALSE,
        NULL, "to",             ST_TO,          FALSE,
        NULL, "type",           ST_TYPE,        FALSE,
        NULL, "variables",      ST_VARIABLES,   FALSE,
        NULL, NULL,             0,              FALSE
};


/* C types used */
enum {
    TC_VOID,
    TC_CHAR,
    TC_CHAR_STAR,
    TC_INT,
    TC_UINT,
    TC_LONG,
    TC_ULONG,
    TC_FLOAT,
    TC_DOUBLE
};

keyword         DataTypes[] = {
        NULL, "BOOLean",                TY_BOOLEAN,             TC_INT,
        NULL, "char",                   TY_CHAR,                TC_CHAR,
        NULL, "character",              TY_CHAR,                TC_CHAR,
        NULL, "double",                 TY_DOUBLE,              TC_DOUBLE,
        NULL, "int",                    TY_INT,                 TC_INT,
        NULL, "integer",                TY_INT,                 TC_INT,
        NULL, "long",                   TY_LONG,                TC_LONG,
        NULL, "real",                   TY_REAL,                TC_FLOAT,
        NULL, "string",                 TY_STRING,              TC_CHAR_STAR,
        NULL, "uint",                   TY_UINT,                TC_UINT,
        NULL, "unsignedint",            TY_UINT,                TC_UINT,
        NULL, "unsignedinteger",        TY_UINT,                TC_UINT,
        NULL, "ulong",                  TY_ULONG,               TC_ULONG,
        NULL, "unsignedlong",           TY_ULONG,               TC_ULONG,
#if(0)
        /* enumerate types */
        NULL, "alignment",              TY_ALIGNMENT,           TC_UINT,
        NULL, "arrangetypes",           TY_ARRANGETYPES,        TC_UINT,
        NULL, "border",                 TY_BORDER,              TC_UINT,
        NULL, "borderstyle",            TY_BORDERSTYLE,         TC_UINT,
        NULL, "button",                 TY_BUTTON,              TC_UINT,
        NULL, "converttype",            TY_CONVERTTYPE,         TC_UINT,
        NULL, "dragmodes",              TY_DRAGMODES,           TC_UINT,
        NULL, "dwbuffer",               TY_DWBUFFER,            TC_UINT,
        NULL, "dwitemstatus",           TY_DWITEMSTATUS,        TC_UINT,
        NULL, "fileaccess",             TY_FILEACCESS,          TC_UINT,
        NULL, "filelock",               TY_FILELOCK,            TC_UINT,
        NULL, "filemode",               TY_FILEMODE,            TC_UINT,
        NULL, "fillpattern",            TY_FILLPATTERN,         TC_UINT,
        NULL, "ffontcharset",           TY_FFONTCHARSET,        TC_UINT,
        NULL, "fontfamily",             TY_FONTFAMILY,          TC_UINT,
        NULL, "fontpitch",              TY_FONTPITCH,           TC_UINT,
        NULL, "grcolortype",            TY_GRCOLORTYPE,         TC_UINT,
        NULL, "grdatatype",             TY_GRDATATYPE,          TC_UINT,
        NULL, "grgraphtype",            TY_GRGRAPHTYPE,         TC_UINT,
        NULL, "grlegendtype",           TY_GRLEGENDTYPE,        TC_UINT,
        NULL, "grobjecttype",           TY_GROBJECTTYPE,        TC_UINT,
        NULL, "grresettype",            TY_GRRESETTYPE,         TC_UINT,
        NULL, "grscaletype",            TY_GRSCALETYPE,         TC_UINT,
        NULL, "grscalevalue",           TY_GRSCALEVALUE,        TC_UINT,
        NULL, "grsymboltype",           TY_GRSYMBOLTYPE,        TC_UINT,
        NULL, "grtictype",              TY_GRTICTYPE,           TC_UINT,
        NULL, "helpcommand",            TY_HELPCOMMAND,         TC_UINT,
        NULL, "icon",                   TY_ICON,                TC_UINT,
        NULL, "keycode",                TY_KEYCODE,             TC_UINT,
        NULL, "libdirtype",             TY_LIBDIRTYPE,          TC_UINT,
        NULL, "libexporttype",          TY_LIBEXPORTTYPE,       TC_UINT,
        NULL, "libimporttype",          TY_LIBIMPORTTYPE,       TC_UINT,
        NULL, "linestyle",              TY_LINESTYLE,           TC_UINT,
        NULL, "mailfiletype",           TY_MAILFILETYPE,        TC_UINT,
        NULL, "maillogonoption",        TY_MAILLOGONOPTION,     TC_UINT,
        NULL, "mailreadoption",         TY_MAILREADOPTION,      TC_UINT,
        NULL, "mailrecipienttype",      TY_MAILRECIPIENTTYPE,   TC_UINT,
        NULL, "mailreturncode",         TY_MAILRETURNCODE,      TC_UINT,
        NULL, "maskdatatype",           TY_MASKDATATYPE,        TC_UINT,
        NULL, "object",                 TY_OBJECT,              TC_UINT,
        NULL, "parmtype",               TY_PARMTYPE,            TC_UINT,
        NULL, "pointer",                TY_POINTER,             TC_UINT,
        NULL, "rowfocusind",            TY_ROWFOCUSIND,         TC_UINT,
        NULL, "saveastype",             TY_SAVEASTYPE,          TC_UINT,
        NULL, "seektype",               TY_SEEKTYPE,            TC_UINT,
        NULL, "seekpostype",            TY_SEEKPOSTYPE,         TC_UINT,
        NULL, "textcase",               TY_TEXTCASE,            TC_UINT,
        NULL, "toolbaralignment",       TY_TOOLBARALIGNMENT,    TC_UINT,
        NULL, "trigevent",              TY_TRIGEVENT,           TC_UINT,
        NULL, "userobjects",            TY_USEROBJECTS,         TC_UINT,
        NULL, "vtextalign",             TY_VTEXTALIGN,          TC_UINT,
        NULL, "windowstate",            TY_WINDOWSTATE,         TC_UINT,
        NULL, "windowtype",             TY_WINDOWTYPE,          TC_UINT,
        NULL, "writemode",              TY_WRITEMODE,           TC_UINT,

        /* system types */
        NULL, "application",            TY_APPLICATION,         TC_ULONG,
        NULL, "checkbox",               TY_CHECKBOX,            TC_ULONG,
        NULL, "commandbutton",          TY_COMMANDBUTTON,       TC_ULONG,
        NULL, "datawindow",             TY_DATAWINDOW,          TC_ULONG,
        NULL, "datawindowchild",        TY_DATAWINDOWCHILD,     TC_ULONG,
        NULL, "dragobject",             TY_DRAGOBJECT,          TC_ULONG,
        NULL, "drawobject",             TY_DRAWOBJECT,          TC_ULONG,
        NULL, "dropdownlistbox",        TY_DROPDOWNLISTBOX,     TC_ULONG,
        NULL, "dynamicdescriptionarea", TY_DYNAMICDESCRIPTIONAREA,TC_ULONG,
        NULL, "dynamicstagingarea",     TY_DYNAMICSTAGINGAREA,  TC_ULONG,
        NULL, "editmask",               TY_EDITMASK,            TC_ULONG,
        NULL, "error",                  TY_ERROR,               TC_ULONG,
        NULL, "function_object",        TY_FUNCTION_OBJECT,     TC_ULONG,
        NULL, "graph",                  TY_GRAPH,               TC_ULONG,
        NULL, "graphobject",            TY_GRAPHOBJECT,         TC_ULONG,
        NULL, "graxis",                 TY_GRAXIS,              TC_ULONG,
        NULL, "grdispattr",             TY_GRDISPATTR,          TC_ULONG,
        NULL, "groupbox",               TY_GROUPBOX,            TC_ULONG,
        NULL, "hscrollbar",             TY_HSCROLLBAR,          TC_ULONG,
        NULL, "line",                   TY_LINE,                TC_ULONG,
        NULL, "listbox",                TY_LISTBOX,             TC_ULONG,
        NULL, "mailfiledescription",    TY_MAILFILEDESCRIPTION, TC_ULONG,
        NULL, "mailmessage",            TY_MAILMESSAGE,         TC_ULONG,
        NULL, "mailrecipient",          TY_MAILRECIPIENT,       TC_ULONG,
        NULL, "mailsession",            TY_MAILSESSION,         TC_ULONG,
        NULL, "mdiclient",              TY_MDICLIENT,           TC_ULONG,
        NULL, "menu",                   TY_MENU,                TC_ULONG,
        NULL, "message",                TY_MESSAGE,             TC_ULONG,
        NULL, "multilineedit",          TY_MULTILINEEDIT,       TC_ULONG,
        NULL, "nonvisualobject",        TY_NONVISUALOBJECT,     TC_ULONG,
        NULL, "oval",                   TY_OVAL,                TC_ULONG,
        NULL, "picture",                TY_PICTURE,             TC_ULONG,
        NULL, "picturebutton",          TY_PICTUREBUTTON,       TC_ULONG,
        NULL, "powerobject",            TY_POWEROBJECT,         TC_ULONG,
        NULL, "radiobutton",            TY_RADIOBUTTON,         TC_ULONG,
        NULL, "rectangle",              TY_RECTANGLE,           TC_ULONG,
        NULL, "roundrectangle",         TY_ROUNDRECTANGLE,      TC_ULONG,
        NULL, "singlelineedit",         TY_SINGLELINEEDIT,      TC_ULONG,
        NULL, "statictext",             TY_STATICTEXT,          TC_ULONG,
        NULL, "structure",              TY_STRUCTURE,           TC_ULONG,
        NULL, "systemfunctions",        TY_SYSTEMFUNCTIONS,     TC_ULONG,
        NULL, "transaction",            TY_TRANSACTION,         TC_ULONG,
        NULL, "userobject",             TY_USEROBJECT,          TC_ULONG,
        NULL, "vscrollbar",             TY_VSCROLLBAR,          TC_ULONG,
        NULL, "window",                 TY_WINDOW,              TC_ULONG,
        NULL, "windowobject",           TY_WINDOWOBJECT,        TC_ULONG,
#endif
        NULL, NULL,                     0,                      0
};


/*  C/C++ conversion type tables */

static char *convTable[] = {    "void", "char", "char *", "short", "unsigned short",
                                "long", "unsigned long", "float", "double" };

//static char *refConvTable[] = { "void *", "char *", "char *", "int *",
//                              "unsigned int *", "long *", "unsigned long *",
//                              "float *", "double *" };

static keyword *findTypeFromId( id_type id ) {
/********************************************/

    int         x;

    id = _BaseType( id );
    assert( _IsIdType( id ) );

    /* find the apropriate type and return its record, should be implemented
       more efficiently at a later date
    */
    x = 0;
    while( DataTypes[x].key != NULL ) {
        if( DataTypes[x].id == id ) {
            return( &DataTypes[x] );
        }
        x++;
    }
    assert( FALSE );
    return( NULL ); /* never reach this point hopefully */
}


unsigned GetStmntCnt( void ) {
/*****************************/
    return( sizeof( Statements ) / sizeof( keyword ) );
}

static id_type getThunk( id_type id ) {
/**************************************/

    keyword     *rk;

    assert( _BaseType( id ) <= TY_LAST_TYPE );

    if( !_BaseType( id ) ) {
        return( id );
    }

    rk = findTypeFromId( _BaseType( id ) );
    assert( rk );

    return( rk->thunk );
}


int IsPtrType( id_type id ) {
/***************************/

    if( _IsRefType( id ) ) {
        return( TRUE );
    }
    switch( getThunk( _BaseType( id ) ) ) {
    case( TC_CHAR ):
    case( TC_INT ):
    case( TC_UINT ):
    case( TC_LONG ):
    case( TC_ULONG ):
    case( TC_FLOAT ):
    case( TC_DOUBLE ):
        return( FALSE );
    default:
        return( TRUE );
    }
}

static char *convertType( id_type id ) {
/****************************************/
    return( convTable[ getThunk( _BaseType( id ) ) ] );
}


static void formatType( char *typbuf, char *arraybuf, char *name,
                        char *type, BOOL isref, ArrayInfo *array ) {
/********************************************************************/

    *arraybuf = '\0';
    if( array != NULL ) {
        if( array->flags & ARRAY_DYNAMIC ) {
            sprintf( typbuf, "%s %s", type, name );
            strcpy( arraybuf, "[]" );
        } else if( array->flags & ARRAY_SIMPLE ) {
            sprintf( typbuf, "%s %s", type, name );
            sprintf( arraybuf, "[%d]", array->elemcnt );
        }
    } else if( isref ) {
        sprintf( typbuf, "%s *%s", type, name );
    } else {
        sprintf( typbuf, "%s %s", type, name );
    }
}

char *ConvertRetType( id_type id ) {
/***********************************/
    return( convertType( id ) );
}

void ConvertVarType( char *typebuf, char *arraybuf, id_type id, ArrayInfo *array ) {
/***************************************************************************/

    char        *type;

    if( _BaseType( id ) == TY_STRING ) {
        type = "String";
    } else {
        type = convertType( id );
    }
    formatType( typebuf, arraybuf, "", type, _IsRefType( id ), array );
}

void ConvertParmType( char *buf, char *name, id_type id, ArrayInfo *array ) {
/**************************************************************************/

    char        *type;
    char        arraybuf[30];

    if( _BaseType( id ) == TY_STRING && array == NULL && _IsRefType( id ) ) {
        type = "char";
    } else {
        type = convertType( id );
    }
    formatType( buf, arraybuf, name, type, _IsRefType( id ), array );
    strcat( buf, arraybuf );
}

#if(0)
char *TypeConvert( id_type id ) {
/**********************************/

    assert( _BaseType( id ) <= TY_LAST_TYPE );

    if( _IsRefType( id ) ) {
        return( refConvTable[ getThunk( _BaseType( id ) ) ] );
    } else {
        return( convTable[ getThunk( _BaseType( id ) ) ] );
    }
}


char *FindType( id_type id ) {
/*******************************/

    keyword     *rk;

    id = _BaseType( id );
    assert( _IsIdType( id ) );

    rk = findTypeFromId( id );
    assert( rk );

    return( rk->key );
}
#endif


char    *FindStmt( id_type id ) {
/********************************/

    long        x;

    x = 0;
    while( Statements[x].key != NULL ) {
        if( Statements[x].id == id ) {
            return( Statements[x].key );
        }
        x++;
    }
    assert( FALSE );
    return( NULL ); /* never reach this point hopefully */
}
