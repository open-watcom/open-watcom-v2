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


#include <ctype.h>
#include <malloc.h>
#include <process.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "wdismsg.h"
#include "disasm.h"

#define CMD_LINE_SIZE   128
#if !defined( __QNX__ )
#   define _OBJ "obj"
#else
#   define _OBJ "o"
#endif

static  char            *rootName;
static  char            *objName;
static  char            *lstName;
static  bool            lstReqd;
static  char            objExt[] = { _OBJ };
static  bool            srcReqd;


/*
 * Static function prototypes
 */
static  void            initOptions( void );
static  void            parseOptions( char * );
static  char            *skipBlanks( char * );
static  char            *skipToDelim( char * );
static  char            *skipFName( char * );
static  void            openFiles( void );


int  main()
/*********/

{
    char                *cmd;
    void               (*parse_object)( void );

#if defined( _M_I86SM ) || defined( _M_I86MM )
    _heapgrow();    /* grow the near heap */
#endif
    if( !MsgInit() ) return( EXIT_FAILURE );
    cmd = AllocMem( CMD_LINE_SIZE );
    getcmd( cmd );
    InitOutput();
    initOptions();
    parseOptions( cmd );
    openFiles();
    InitObj();
    parse_object = InitORL() ? ParseObjectORL : ParseObjectOMF;
    for( ;; ) {
        InitTables();
        ModNameORL = rootName;
        parse_object();
        if( Mod == NULL ) {
            break;
        }
        if( srcReqd ) {
            if( SrcName == NULL ) {
                if( CommentName != NULL ) {
                    SrcName = CommentName;
                } else if( Mod->name != NULL ) {
                    SrcName = Mod->name;
                } else {
                    SrcName = rootName;
                }
            }
            OpenSource();
        }
        DisAssemble();
        if( Source != NULL ) {
            CloseTxt( Source );
        }
        SrcName = NULL;         /* if another module, get name from obj file */
    }
    if( UseORL ) FiniORL();
    CloseBin( ObjFile );
    CloseTxt( Output );
    MsgFini();
    return( 0 );
}


static  void  initOptions()
/*************************/

{
    UnMangle = true;
    ImpDump = false;
    ExpDump = false;
    lstReqd = false;
    srcReqd = false;
    Options = 0;
    Source = NULL;
    objName = NULL;
    lstName = NULL;
    SrcName = NULL;
    CommentName = NULL;
    CodeName = NULL;
    Output = stdout;
}


static  void  parseOptions( char *cmd )
/*************************************/
{
    char                *ptr;
    char                opt;

    cmd = skipBlanks( cmd );
    if( *cmd == '\0' || *cmd == '?' ) {
        SysHelp();                      /* no name specified */
    } else {
        ptr = skipToDelim( cmd );
        while( *cmd != '\0' ) {         /* parse options */
            ptr = skipToDelim( cmd );
            if( *cmd != '-'             /* object file name */
#if !defined( __QNX__ )
                && *cmd != '/'
#endif
                ) {
                if( objName == NULL ) {
                    ptr = skipFName( cmd );
                    objName = ParseName( cmd, ptr );
                } else {
                    SysError( ERR_INV_CMMD_OPTION, false );
                }
            } else {
                opt = tolower( *(++cmd) );
                switch( opt ) {
                case 'a':
                    Options |= FORM_ASSEMBLER;
                    if( tolower( *( cmd + 1 ) ) == 'u' ) {
                        cmd++;
                        Options |= FORM_UNIX;
                    }
                    break;
                case 'b':
                    Options |= FORM_INDEX_IN;
                    break;
                case 'e':
                    ImpDump = true;
                    break;
                case 'p':
                    ExpDump = true;
                    break;
                case 'r':
                    Options |= FORM_REG_UPPER;
                    break;
                case 'u':
                    Options |= FORM_NAME_UPPER;
                    break;
                case 'l':
                    lstReqd = true;
                    ++cmd;
                    if( *cmd == '=' || *cmd == '@' || *cmd == '#' ) {
                        ++cmd;
                        ptr = skipFName( cmd );
                        lstName = ParseName( cmd, ptr );
                    }
                    break;
                case 's':
                    srcReqd = true;
                    ++cmd;
                    if( *cmd == '=' || *cmd == '@' || *cmd == '#' ) {
                        ++cmd;
                        ptr = skipFName( cmd );
                        SrcName = ParseName( cmd, ptr );
                    }
                    break;
                case 'i':
                    ++cmd;
                    if( *cmd == '=' || *cmd == '@' || *cmd == '#' ) {
                        ++cmd;
                    }
                    if( isalpha( *cmd ) || *cmd == '_' ) {
                        IntLblStart = toupper( *cmd );
                    }
                    break;
                case 'c':
                    ++cmd;
                    if( *cmd == '=' || *cmd == '@' || *cmd == '#' ) {
                        CodeName = ParseName( ++cmd, ptr );
                        break;
                    } /* else - fall through */
                case 'w':
                    Options |= FORM_DO_WTK;
                    break;
                case 'm':
                    UnMangle = false;
                    break;
                default:
                    SysError( ERR_INV_CMMD_OPTION, false );
                }
            }
            cmd = skipBlanks( ptr );
        }
        if( objName == NULL ) {
            SysError( ERR_NO_OBJ_SPEC, false );
        }
    }
}


static  void  openFiles( void )
/*****************************/

{
/*  C.L. May 22, 91 */
    char        sp_buf[ _MAX_PATH2 ];
    char        *ext;
    size_t      len;

    _splitpath2( objName, sp_buf, NULL, NULL, &rootName, &ext );
    len = strlen( rootName ) + 1;
    rootName = memcpy( AllocMem( len ), rootName, len );
    if( ext[ 0 ] == '\0' ) {
        /* search with '.obj' or '.o' extension first */
        ObjFile = OpenBinRead( MakeFileName( objName, objExt ) );
        if( ObjFile == NULL ) {
            /* try without extension */
            ObjFile = OpenBinRead( objName );
        }
    } else {
        /* extension specified */
        ObjFile = OpenBinRead( objName );
    }
/**/
    if( ObjFile == NULL ) {
//        ObjFile = OpenBinRead( MakeFileName( objName, objExt ) );
//        if( ObjFile == NULL ) {
            SysError( ERR_NOT_OPEN_OBJ, true );
//        }
    }
    if( lstReqd ) {
        if( lstName == NULL ) {
            Output = OpenTxtWrite( MakeFileName( rootName, "lst" ) );
        } else {
            Output = OpenTxtWrite( lstName );
        }
        if( Output == NULL ) {
            CloseBin( ObjFile );
            SysError( ERR_NOT_OPEN_LIST, true );
        }
    }
}


static  char  *skipBlanks( char *str )
/************************************/
{
    while( isspace( *str ) ) {
        ++str;
    }
    return( str );
}


static  char  *skipToDelim( char *str )
/*************************************/
{
    do {
        ++str;
#if defined( __QNX__ )
    } while( *str != '-' && !isspace( *str ) && *str );
#else
    } while( *str != '/' && *str != '-' && !isspace( *str ) && *str );
#endif
    return( str );
}


static  char  *skipFName( char *str )
/***********************************/
{
    do {
        ++str;
#if defined( __QNX__ )
    } while( !isspace( *str ) && *str );
#else
    } while( *str != '/' && !isspace( *str ) && *str );
#endif
    return( str );
}
