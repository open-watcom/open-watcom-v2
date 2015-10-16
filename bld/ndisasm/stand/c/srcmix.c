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


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "dis.h"
#include "global.h"
#include "orl.h"
#include "srcmix.h"
#include "buffer.h"
#include "mydwarf.h"
#include "memfuncs.h"

#include "clibext.h"


extern char *   ObjFileName;
extern char *   CommentString;
extern dis_format_flags DFormat;

char *                  SourceFileName = NULL;
char *                  SourceFileInObject = NULL;
char *                  SourceFileInDwarf = NULL;
bool                    source_mix = FALSE;
FILE *                  SourceFile = NULL;
orl_linnum *            lines = NULL;
orl_table_index         numlines;
orl_table_index         currline;
int                     lineInFile;

static enum {
    NO_LINES,
    ORL_LINES,
    DWARF_LINES,
} line_type;

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

static int compareLines( const void *first, const void *second )
{
    orl_linnum *l1 = (orl_linnum *)first;
    orl_linnum *l2 = (orl_linnum *)second;

    // make sure the "fake" function label line numbers filter to the end
    if( l1->linnum == 0 ) {
        return 1;
    }
    if( l2->linnum == 0 ) {
        return -1;
    }

    if( l1->off > l2->off ) {
        return 1;
    }
    if( l1->off < l2->off ) {
        return -1;
    }
    return 0;
}

static orl_linnum *SortLineNums( orl_linnum *lines, orl_table_index numlines )
{
    orl_linnum *newlines;

    newlines = (orl_linnum *)MemAlloc( numlines * sizeof( orl_linnum ) );
    memcpy( newlines, lines, numlines * sizeof( orl_linnum ) );
    qsort( newlines, numlines, sizeof( orl_linnum ), compareLines );

    return newlines;
}

extern void GetSourceFile( section_ptr sec )
{
    char        path[_MAX_PATH2];
    char *      drive;
    char *      dir;
    char *      file_name;
    char *      extension;
    orl_linnum *templines;

    numlines = ORLSecGetNumLines( sec->shnd );
    if( numlines == 0 ) {
        numlines = GetDwarfLines( sec );
        if( numlines == 0 ) {
            if( DFormat & DFF_ASM ) {
                BufferConcat( CommentString );
            }
            BufferMsg( NO_LINE_NUMS );
            BufferConcatNL();
            BufferPrint();
            line_type = NO_LINES;
            return;
        } else {
            line_type = DWARF_LINES;
        }
    } else {
        lines = ORLSecGetLines( sec->shnd );
        line_type = ORL_LINES;
    }
    templines = SortLineNums( lines , numlines );
    if( line_type == DWARF_LINES ) {
        MemFree( lines );
    }
    lines = templines;
    currline = 0;
    lineInFile = 0;

    if( SourceFileName ) {
        SourceFile = fopen( SourceFileName, "r" );
        if( SourceFile != NULL ) {
            return;
        }
        _splitpath2( SourceFileName, path, &drive, &dir, &file_name, &extension );
        MemFree( SourceFileName );
        SourceFileName = MemAlloc( strlen(drive) + strlen(dir) + strlen(file_name) + 5 );

        _makepath( SourceFileName, drive, dir, file_name, ".c" );
        SourceFile = fopen( SourceFileName, "r" );
        if( SourceFile != NULL ) {
            return;
        }
        _makepath( SourceFileName, drive, dir, file_name, ".cpp" );
        SourceFile = fopen( SourceFileName, "r" );
        if( SourceFile != NULL ) {
            return;
        }
        _makepath( SourceFileName, drive, dir, file_name, ".for" );
        SourceFile = fopen( SourceFileName, "r" );
        if( SourceFile != NULL ) {
            return;
        }
        _makepath( SourceFileName, drive, dir, file_name, ".asm" );
        SourceFile = fopen( SourceFileName, "r" );
        if( SourceFile != NULL ) {
            return;
        }
        _makepath( SourceFileName, drive, dir, file_name, ".*" );
        NoSource( SourceFileName );
        MemFree( SourceFileName );
        SourceFileName = NULL;
        return;
    }

    if( SourceFileInObject ) {
        SourceFile = fopen( SourceFileInObject, "r" );
        if( SourceFile != NULL ) {
            return;
        }
    }

    if( SourceFileInDwarf ) {
        SourceFile = fopen( SourceFileInDwarf, "r" );
        if( SourceFile != NULL ) {
            return;
        }
    }

    _splitpath2( ObjFileName, path, &drive, &dir, &file_name, &extension );
    SourceFileName = MemAlloc( strlen(drive) + strlen(dir) + strlen(file_name) + 5 );

    _makepath( SourceFileName, drive, dir, file_name, ".c" );
    SourceFile = fopen( SourceFileName, "r" );
    if( SourceFile != NULL ) {
        return;
    }
    _makepath( SourceFileName, drive, dir, file_name, ".cpp" );
    SourceFile = fopen( SourceFileName, "r" );
    if( SourceFile != NULL ) {
        return;
    }
    _makepath( SourceFileName, drive, dir, file_name, ".for" );
    SourceFile = fopen( SourceFileName, "r" );
    if( SourceFile != NULL ) {
        return;
    }
    _makepath( SourceFileName, drive, dir, file_name, ".asm" );
    SourceFile = fopen( SourceFileName, "r" );
    if( SourceFile != NULL ) {
        return;
    }
    _makepath( SourceFileName, drive, dir, file_name, ".*" );
    NoSource( SourceFileName );
    MemFree( SourceFileName );
    SourceFileName = NULL;
}

static char *getNextLine( void )
{
    long int len, pos;
    char *buff;
    int c;

    pos = ftell( SourceFile );
    len = 0;
    do{
        c=fgetc( SourceFile );
        len++;
    }while( c != '\n' && c != EOF );

    buff = MemAlloc( len+1 );
    if( c == EOF ) {
        buff[0]='\0';
    } else {
        fseek( SourceFile, pos, SEEK_SET );
        fgets( buff, len+1, SourceFile );
    }
    lineInFile++;
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

extern void MixSource( orl_sec_offset offset )
{
    orl_linnum *line_entry;

    if( SourceFile ){
        line_entry = &(lines[currline]);
        while( currline < numlines && line_entry->off == offset
                        && line_entry->linnum != 0 ) {
            BufferConcatNL();
            while( lineInFile < line_entry->linnum ){
                if( DFormat & DFF_ASM ) {
                    BufferConcat( CommentString );
                    BufferPrint();
                }
                printLine();
            }
            currline++;
            line_entry = &(lines[currline]);
        }
    }
}

extern void EndSourceMix( void )
{
    if( SourceFile ) {
        fclose( SourceFile );
        SourceFile = NULL;
    }
    if( lines ) {
        MemFree( lines );
    }
}
