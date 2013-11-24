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
#include <ctype.h>
#include "yacc.h"
#include "yaccins.h"
#include "alloc.h"

static FILE *tblout;

static char *opstr[] = {
    "_jl",          /* JLT */
    "_je",          /* JEQ */
    "_jg",          /* JGT */
    "_jle",         /* JLE */
    "_jne",         /* JNE */
    "_jge",         /* JGE */
    "ljmp",         /* JMP */
    "_lbl",         /* LBL */
    "_vcmp",        /* VCMP */
    "_tcmp",        /* TCMP */
    "_scan",        /* SCAN */
    "_call",        /* CALL */
    "_action",      /* ACTION */
    "_reduce"       /* REDUCE */
};

static unsigned opsize[] = {
    5,              /* JLT */
    5,              /* JEQ */
    5,              /* JGT */
    5,              /* JLE */
    5,              /* JNE */
    5,              /* JGE */
    3,              /* JMP */
    0,              /* LBL */
    3,              /* VCMP */
    2,              /* TCMP */
    3,              /* SCAN */
    3,              /* CALL */
    15,             /* ACTION */
    8               /* REDUCE */
};

typedef struct an_ins {
    char            opcode;
    short           offset;
} an_ins;

static an_ins *code;
static unsigned codeused, codeavail;

#define BLOCK   512

static int *lbladdr;

void writeobj( int maxlabel )
{
    tblout = fopen( "ytab.asm", "w" );
    if( tblout == NULL ) {
        msg( "cannot open 'ytab.asm'\n" );
    }
    fprintf( tblout, "INCLUDE ytabmac.inc\n" );
    fprintf( tblout, "_TEXT\tSEGMENT\n" );
    lbladdr = MALLOC( maxlabel, int );
    calcaddr();
    dumpcode();
    fprintf( tblout, "_TEXT\tENDS\n" );
    fprintf( tblout, "END\n" );
    fclose( tblout );
    FREE( code );
    FREE( lbladdr );
}


static calcaddr( void )
{
    an_ins *ins;
    unsigned j, insaddr;
    
    insaddr = 0;
    for( j = 0; j < codeused; ++j ) {
        ins = &code[j];
        if( ins->opcode == LBL )
            lbladdr[ins->offset] = insaddr;
        insaddr += opsize[ins->opcode];
    }
}

static dumpcode( void )
{
    an_ins *ins;
    int offset;
    unsigned j;
    
    fprintf( tblout, "L:" );
    for( j = 0; j < codeused; ++j ) {
        ins = &code[j];
        offset = ins->offset;
        fprintf( tblout, "\t%s", opstr[ins->opcode] );
        switch( ins->opcode ) {
        case VCMP: case TCMP:
            if( isprint( offset ) && offset != '\'' && offset != '\\' ) {
                fprintf( tblout, "\t'%c'", offset );
            } else {
                fprintf( tblout, "\t%d", offset );
            }
            break;
        case JLT: case JEQ: case JGT: case JLE: case JNE: case JGE: case JMP:
        case LBL: case CALL:
            if( offset > 0 ) {
                fprintf( tblout, "\tL+%d", lbladdr[offset] );
            } else {
                fprintf( tblout, "\tL%d", -offset );
            }
            break;
        case ACTION:
            fprintf( tblout, "\t%d,%d", PROLEN(offset), PRONUM(offset) );
            break;
        case REDUCE:
            fprintf( tblout, "\t%d,%d", PROLEN(offset), PROLHS(offset) );
            break;
        default:
            break;
        }
        fprintf( tblout, "\n" );
    }
}

void emitins( unsigned opcode, unsigned offset )
{
    an_ins *ins;

    need( 1 );
    ins = &code[codeused++];
    ins->opcode = opcode;
    ins->offset = offset;
}

static need( unsigned n )
{
    if( codeused + n > codeavail ) {
        codeavail += BLOCK;
        if( code ) {
            code = REALLOC( code, codeavail, an_ins );
        } else {
            code = MALLOC( codeavail, an_ins );
        }
    }
}
