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
#include <stdlib.h>
#include <sys\types.h>
#include <sys\stat.h>
#include <fcntl.h>
#include <io.h>
#include "defgen.h"
#include "output.h"

static char             OutputBuf[ 2048 ];
static char             *BufEnd = OutputBuf;
static int              OutFileHdl;
static char             Buffer[512];

static void TextOut( char *fmt, ... ) {
    va_list     al;

    va_start( al, fmt );
    vsprintf( BufEnd, fmt, al );
    while( *BufEnd != '\0' ) BufEnd++;
    va_end( al );
}

static void WriteBuf() {
    write( OutFileHdl, OutputBuf, BufEnd - OutputBuf );
    BufEnd = OutputBuf;
    *BufEnd = '\0';
}

static void WriteText( char *fmt, ... ) {
    va_list     al;

    va_start( al, fmt );
    vsprintf( BufEnd, fmt, al );
    while( *BufEnd != '\0' ) BufEnd++;
    WriteBuf();
}


static BOOL OpenFile( char *fname ) {
    OutFileHdl = open( fname, O_WRONLY | O_CREAT | O_TRUNC | O_TEXT,
                        S_IRWXU | S_IRWXG | S_IRWXO );
    if( OutFileHdl == -1 ) {
        ReportError( "Unable to open output file '%s'", fname );
        return( TRUE );
    }
    return( FALSE );
}

static void CloseFile( void ) {
    close( OutFileHdl );
    BufEnd = OutputBuf;
    *BufEnd = '\0';
}


static void PrintSingleDef( char *fnname, PBDefInfo *pbdef ) {

    PBTypeInfo  *curtype;
    unsigned    parmcnt;
    unsigned    i;

    if( pbdef->type == PB_SUBROUTINE ) {
        TextOut( "Subroutine %s(", fnname );
    } else if( pbdef->type == PB_FUNCTION ) {
        TextOut( "Function %s%s %s(", pbdef->rettype.modifiers,
                pbdef->rettype.type, fnname );
    }
    curtype = pbdef->parms;
    parmcnt = 0;
    while( curtype != NULL ) {
        if( parmcnt != 0 ) {
            TextOut( "," );
        }
        if( curtype->type[0] == '\0' ) {
            /* do nothing */
        } else {
            TextOut( "%s %s %s", curtype->modifiers, curtype->type,
                    curtype->name );
        }
        for( i=0; i < curtype->arraycnt; i++ ) {
            TextOut( "[]" );
        }
        curtype = curtype->next;
        parmcnt++;
    }
    TextOut( " )" );
}

void PrintPBDef( SymbolList *info ) {

    PBDefInfo   *pbdef;

    if( !OpenFile( Config.out_file ) ) {
        pbdef = info->func;
        while( pbdef != NULL ) {
            PrintSingleDef( pbdef->name, pbdef );
            TextOut( " library \"%s\"\n", Config.dll_name );
            WriteBuf();
            pbdef = pbdef->next;
        }
        CloseFile();
    }
}

void CreateNonVisualObj( SymbolList *info ) {

    PBDefInfo   *pbdef;
    PBTypeInfo  *pbtype;

    if( !OpenFile( Config.nuo_file ) ) {
        WriteText( "$PBExportHeader$%s.sru\n", Config.nuo_name );
        WriteText( "forward\n" );
        WriteText( "global type %s from nonvisualobject\n", Config.nuo_name );
        WriteText( "end type\n" );
        WriteText( "end forward\n" );
        WriteText( "\n" );
        WriteText( "global type %s from nonvisualobject\n", Config.nuo_name );
        WriteText( "end type\n" );
        WriteText( "global %s %s\n", Config.nuo_name, Config.nuo_name );
        WriteText( "\n" );

        WriteText( "type prototypes\n" );
        pbdef = info->func;
        while( pbdef != NULL ) {
            PrintSingleDef( pbdef->name, pbdef );
            TextOut( " library \"%s\"\n", Config.dll_name );
            WriteBuf();
            pbdef = pbdef->next;
        }
        WriteText( "end prototypes\n\n" );

        //
        // prototype UO functions
        //
        WriteText( "forward prototypes\n" );
        pbdef = info->func;
        while( pbdef != NULL ) {
            TextOut( "public " );
            sprintf( Buffer, "f_%s", pbdef->name );
            PrintSingleDef( Buffer, pbdef );
            TextOut( "\n" );
            WriteBuf();
            pbdef = pbdef->next;
        }
        WriteText( "end prototypes\n\n" );

        //
        // Code for UO functions goes here
        //
        pbdef = info->func;
        while( pbdef != NULL ) {
            TextOut( "public " );
            sprintf( Buffer, "f_%s", pbdef->name );
            PrintSingleDef( Buffer, pbdef );
            TextOut( ";\n" );
            WriteBuf();
            if( pbdef->type == PB_SUBROUTINE ) {
                TextOut( "%s( ", pbdef->name );
            } else if( pbdef->type == PB_FUNCTION ) {
                TextOut( "return %s( ", pbdef->name );
            }
            pbtype = pbdef->parms;
            if( pbtype != NULL ) {
                for( ;; ) {
                    if( pbtype->type[0] != '\0' ) {
                        TextOut( pbtype->name );
                    }
                    pbtype = pbtype->next;
                    if( pbtype != NULL ) {
                        TextOut( ", " );
                    } else {
                        break;
                    }
                }
            }
            TextOut( " )\n" );
            WriteBuf();
            if( pbdef->type == PB_SUBROUTINE ) {
                WriteText( "end subroutine\n\n", pbdef->name );
            } else if( pbdef->type == PB_FUNCTION ) {
                WriteText( "end function\n\n", pbdef->name );
            }
            pbdef = pbdef->next;
        }

        WriteText( "on %s.create\n", Config.nuo_name );
        WriteText( "TriggerEvent( this, \"constructor\" )\n" );
        WriteText( "end on\n\n" );
        WriteText( "on %s.destroy\n", Config.nuo_name );
        WriteText( "TriggerEvent( this, \"destructor\" )\n" );
        WriteText( "end on\n" );
        CloseFile();
    }
}

