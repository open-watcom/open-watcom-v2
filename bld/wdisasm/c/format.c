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


#include <string.h>
#include <malloc.h>
#include "disasm.h"


#define _Printable( p )         ( (unsigned)(p-32) < (unsigned)(127 - 32))

static  char            *FixString[] = {
        "DW      ",
        "DW      ",
        "DW      SEG ",
        "DD      ",
        "DW      ",
/* Phar lap specific */
        "DD      ",
        "DP      "
};

static char *           unixFixString[] = {
        ".value  ",
        ".value  ",
        ".value  ",
        ".long   ",
        ".value  ",
        ".long   ",
        ".long   ",
};


static  void            DoDefBytes( char * );


char *stpcpy( char *dest, const char *src )
/*****************************************/
/* like strcpy, but return pointer to the null terminator of dest */
{
    while( *dest = *src ) {
        ++dest;
        ++src;
    }
    return( dest );
}

static void DoByteSpacing()
{
    if( !(Options & FORM_ASSEMBLER) ) EmitBlanks( DataLen*3 );
}

void  FormatLine( fixup *fix, char *label, char is_data, char in_pcode )
/**********************************************************************/
{
    char                *name;
    int                 blanks;
    fixup_class         class;
    char                *grp_name;
    char                *p;
    uint_32             to_add;
    char                is_ins;

    is_ins = FALSE;
    if( fix != NULL ) {
        name = FindLabel( BAD_OFFSET, fix->imp_address, fix->target );
        if( name == NULL ) {
            name = "";
        }
        class = fix->class & 7;
        /* CL_LONG, CL_LONG_POINTER ???? */
        p = TxtBuff;
        if( class == CL_BASE && _Class( fix->target ) == TYPE_GROUP ) {
            p = stpcpy( p, ( DO_UNIX ? unixFixString : FixString )[ 0 ] );
        } else {
            p = stpcpy( p, ( DO_UNIX ? unixFixString : FixString )[ class ] );
            grp_name = GetGroupName( fix );
            if( grp_name != NULL ) {
                p = stpcpy( p, grp_name );
                *p++ = ':';
            }
        }
        p = stpcpy( p, name );
        switch( _Class( fix->target ) ) {
        case TYPE_SEGMENT:
            to_add = fix->seg_address;
            break;
        case TYPE_COMDAT:
        case TYPE_COMDEF:
        case TYPE_IMPORT:
        case TYPE_GROUP:
            to_add = fix->imp_address;
            break;
        default:
            to_add = 0;
            break;
        }
        if( to_add != 0 ) {
            MyIToHS( p, to_add );
        }
    } else {
        TxtBuff[ 0 ] = '\0';
        if( Segment->data_seg ) {
            FormatData( TxtBuff );
        } else if( is_data ) {   /* code pretending to be data (scan tables) */
#ifdef __PCODE__
            if( in_pcode < 3  ||  ( Options & FORM_ASSEMBLER ) ) {
                DoDefBytes( TxtBuff );
            } else {
                FormatPcode( TxtBuff );
            }
#else
            in_pcode = in_pcode;
            DoDefBytes( TxtBuff );
#endif
        } else if( LabelInInstr() && ( Options & FORM_ASSEMBLER ) ) {
            DoDefBytes( TxtBuff );
        } else {
            while( LabelOnInstr() ) {
                EmitLoc();
                DoByteSpacing();
                Emit( GetLabel() );
                EmitLine( ":" );
            }
            FormatIns( TxtBuff, &CurrIns, Options );
            is_ins = TRUE;
        }
    }
    EmitLoc();
    if( label == NULL ) {
        EmitBytes();
        blanks = LABEL_LEN;
    } else {
        blanks = LABEL_LEN - strlen( label );
        if( blanks <= 0 ) {
            DoByteSpacing();
            Emit( label );
            if( !is_data ) {
                EmitLine( ":" );
            } else if( DO_UNIX ) {
                EmitLine( ":" );
            } else {
                EmitLine( " LABEL BYTE" );
            }
            EmitLoc();
        }
        EmitBytes();
        if( blanks <= 0 ) {
            blanks = LABEL_LEN;
        } else {
            Emit( label );
            if( DO_UNIX || ((Options & FORM_ASSEMBLER) && is_ins) ) {
                Emit( ":" );
                --blanks;
            }
        }
    }
    EmitBlanks( blanks );
    EmitLine( TxtBuff );
}


static void FormatData( char *buf )
/*********************************/

{
    char                *p;
    int                 i;
    int                 value;

    if( Options & FORM_ASSEMBLER ) {
        DoDefBytes( buf );
    } else {
        p = buf;
        *p++ = '-';
        *p++ = ' ';
        for( i = 0; i < DataBytes; ++i ) {
            value = DataString[ i ];
            if( !_Printable( value ) ) {
                value = '.';
            }
            *p++ = value;
        }
        *p = '\0';
    }
}


char * DBstring()
/***************/
{
    if( DO_UNIX ) {
        return( ".byte   " );
    } else {
        return( "DB      " );
    }
}

static  void  DoDefBytes( char *buf )
/***********************************/

{
    char                *p;
    int                 i;

    if( DataBytes == 0 ) {
        buf[ 0 ] = '\0';
    } else {
        p = stpcpy( buf, DBstring() );
        i = 0;
        for( ;; ) {
            DoIToHS( p, DataString[ i ], 2 );
            p += strlen( p ); /* FIXME: DoIToHS should return length encoded */
            ++i;
            if( i == DataBytes ) break;
            *p++ = ',';
        }
    }
}
