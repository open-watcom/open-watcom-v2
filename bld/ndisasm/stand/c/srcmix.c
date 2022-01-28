/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "dis.h"
#include "global.h"
#include "srcmix.h"
#include "buffer.h"
#include "disdwarf.h"
#include "memfuncs.h"
#include "pathgrp2.h"

#include "clibext.h"


char                    *SourceFileName = NULL;
const char              *SourceFileInObject = NULL;
char                    *SourceFileInDwarf = NULL;
bool                    source_mix = false;
FILE                    *SourceFile = NULL;

static enum {
    NO_LINES,
    ORL_LINES,
    DWARF_LINES,
} line_type;

static const char * const src_exts[] = {
    "c",
    "cpp",
    "for",
    "asm"
};

static void NoSource( char *file )
{
    if( DFormat & DFF_ASM ) {
        BufferConcat( CommentString );
    }
    BufferMsg( CANT_OPEN_SOURCE );
    BufferConcat( file );
    BufferConcatNL();
    BufferPrint();
}

static void OpenSourceFileExts( const char *fname )
{
    pgroup2     pg;
    char        *src_filename;
    int         i;

    _splitpath2( fname, pg.buffer, &pg.drive, &pg.dir, &pg.fname, &pg.ext );
    src_filename = MemAlloc( strlen( pg.drive ) + strlen( pg.dir ) + strlen( pg.fname ) + 5 );
    for( i = 0; i < sizeof( src_exts ) / sizeof( src_exts[0] ); i++ ) {
        _makepath( src_filename, pg.drive, pg.dir, pg.fname, src_exts[i] );
        SourceFile = fopen( src_filename, "r" );
        if( SourceFile != NULL ) {
            MemFree( src_filename );
            return;
        }
    }
    _makepath( src_filename, pg.drive, pg.dir, pg.fname, "*" );
    NoSource( src_filename );
    MemFree( src_filename );
}

static int compareLines( const void *l1, const void *l2 )
{
    // make sure the "fake" function label line numbers filter to the end
    if( ((orl_linnum)l1)->linnum == 0 ) {
        return( 1 );
    }
    if( ((orl_linnum)l2)->linnum == 0 ) {
        return( -1 );
    }

    if( ((orl_linnum)l1)->off > ((orl_linnum)l2)->off ) {
        return( 1 );
    }
    if( ((orl_linnum)l1)->off < ((orl_linnum)l2)->off ) {
        return( -1 );
    }
    return( 0 );
}

static orl_linnum SortLineNums( orl_linnum old_lines, orl_table_index inumlines )
{
    void    *new_lines;
    size_t  size;

    size = inumlines * ORL_STRUCT_SIZEOF( orl_linnum );
    new_lines = MemAlloc( size );
    memcpy( new_lines, old_lines, size );
    qsort( new_lines, inumlines, ORL_STRUCT_SIZEOF( orl_linnum ), compareLines );
    return( new_lines );
}

static char *getNextLine( void )
{
    long len, pos;
    char *buff;
    int c;

    pos = ftell( SourceFile );
    len = 0;
    do {
        c = fgetc( SourceFile );
        len++;
    } while( c != '\n' && c != EOF );

    buff = MemAlloc( len + 1 );
    if( c == EOF ) {
        buff[0]='\0';
    } else {
        fseek( SourceFile, pos, SEEK_SET );
        fgets( buff, len + 1, SourceFile );
    }
    return buff;
}

static void printLine( void )
{
    char *buff;

    buff = getNextLine();
    BufferConcat( buff );
    BufferPrint();
    MemFree( buff );
}

void GetSourceFile( state_lines *ls, section_ptr section )
{
    orl_linnum  lines;

    ls->numlines = ORLSecGetNumLines( section->shnd );
    if( ls->numlines == 0 ) {
        GetDwarfLines( ls, section );
        if( ls->numlines == 0 ) {
            if( DFormat & DFF_ASM ) {
                BufferConcat( CommentString );
            }
            BufferMsg( NO_LINE_NUMS );
            BufferConcatNL();
            BufferPrint();
            line_type = NO_LINES;
            return;
        }
        lines = ls->lines;
        line_type = DWARF_LINES;
    } else {
        lines = ORLSecGetLines( section->shnd );
        line_type = ORL_LINES;
    }
    ls->lines = SortLineNums( lines, ls->numlines );
    if( line_type == DWARF_LINES ) {
        MemFree( (void *)lines );
    }
    if( SourceFileName != NULL ) {
        SourceFile = fopen( SourceFileName, "r" );
        if( SourceFile == NULL ) {
            OpenSourceFileExts( SourceFileName );
        }
        return;
    }
    if( SourceFileInObject != NULL ) {
        SourceFile = fopen( SourceFileInObject, "r" );
        if( SourceFile != NULL ) {
            return;
        }
    }
    if( SourceFileInDwarf != NULL ) {
        SourceFile = fopen( SourceFileInDwarf, "r" );
        if( SourceFile != NULL ) {
            return;
        }
    }
    OpenSourceFileExts( ObjFileName );
}

void MixSource( state_lines *ls, dis_sec_offset offset, orl_table_index *currline, orl_table_index *lineInFile )
{
    orl_linnum      line_entry;
    orl_table_index curr_line;
    orl_table_index line_in_file;

    if( SourceFile != NULL ){
        curr_line = *currline;
        line_in_file = *lineInFile;
        line_entry = ls->lines + curr_line;
        for( ; curr_line < ls->numlines; curr_line++ ) {
            if( line_entry->off != offset || line_entry->linnum == 0 )
                break;
            BufferConcatNL();
            for( ; line_in_file < line_entry->linnum; line_in_file++ ) {
                if( DFormat & DFF_ASM ) {
                    BufferConcat( CommentString );
                    BufferPrint();
                }
                printLine();
            }
            line_entry++;
        }
        *currline = curr_line;
        *lineInFile = line_in_file;
    }
}

void FreeSourceLines( state_lines *ls )
{
    MemFree( (void *)ls->lines );
    ls->lines = NULL;
    ls->numlines = 0;
    ls->currlinesize = 0;
}

void EndSourceMix( state_lines *ls )
{
    if( SourceFile != NULL ) {
        fclose( SourceFile );
        SourceFile = NULL;
    }
    if( ls->lines != NULL ) {
        FreeSourceLines( ls );
    }
}
