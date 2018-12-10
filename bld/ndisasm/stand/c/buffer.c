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
#include <string.h>
#include <ctype.h>
#include "dis.h"
#include "global.h"
#include "buffer.h"
#include "print.h"
#include "formasm.h"
#include "labproc.h"


static unsigned         OutputPos = 0;
static char             Buffer[1024] = {0};
static char             IntermedBuffer[30] = {0};

static char             *buffer_pos = Buffer;

char *FmtHexNum( char *buff, unsigned prec, dis_value value )
{
    char        *src;
    char        *dst;
    const char  *fmt;
    int         len;
    bool        masm_src;

    masm_src = ( (DFormat & DFF_ASM) && IsMasmOutput() );
    if( ( value.u._32[I64LO32] == 0 ) && ( value.u._32[I64HI32] == 0 ) && ( prec == 0 ) ) {
        strcpy( buff, ( masm_src ) ? "0" : "0x0" );
    } else {
        if( value.u._32[I64HI32] == 0 ) {
            fmt = ( masm_src ) ? "0%*.*lxH" : "0x%*.*lx";
            len = 0;
        } else {
            fmt = ( masm_src ) ? "0%*.*lx" : "0x%*.*lx";
            if( prec > 8 ) {
                len = sprintf( buff, fmt, prec - 8, prec - 8, value.u._32[I64HI32] );
                prec = 8;
            } else {
                len = sprintf( buff, fmt, 0, 0, value.u._32[I64HI32] );
            }
            fmt = ( masm_src ) ? "%*.*lxH" : "%*.*lx";
        }
        sprintf( buff + len, fmt, prec, prec, value.u._32[I64LO32] );
        if( masm_src ) {
            /* don't need the extra leading zero, squeeze it out */
            for ( src = dst = buff; *src != '\0'; src++ ) {
                if ( dst != buff || src[0] != '0' || !isdigit( src[1] ) ) {
                    *dst = *src;
                    dst++;
                }
            }
            *dst = '\0';
            if ( buff[1] == 'H' ) {
                buff[1] = '\0';
            }
        }
    }
    return( buff );
}

char *FmtLabelNum( char *buff, uint_32 value )
{
    sprintf( buff, "%c$%lu", LabelChar, value );
    return( buff );
}

static void write_char( char c )
{
    if( buffer_pos == Buffer + sizeof( Buffer ) ) {
        BufferPrint();
    }
    *buffer_pos++ = c;
}

static void write_string( const char *str )
{
    while( *str != '\0' ) {
        write_char( *str++ );
    }
}

void BufferAlignToTab( unsigned pos )
// align the buffer to a particular tab position
{
    unsigned            num_tabs;

    if( pos * TAB_WIDTH < OutputPos ) {
        BufferConcatNL();
        num_tabs = pos;
    } else {
        num_tabs = ( pos * TAB_WIDTH - OutputPos + TAB_WIDTH - 1 ) / TAB_WIDTH;
    }
    while( num_tabs-- > 0 ) {
        write_char( '\t' );
    }
    OutputPos = pos * TAB_WIDTH;
}

static void updateOutputPosChar( char c )
// update the position of the last character as it will be seen in output
{
    if( c == '\n' ) {
        OutputPos = 0;
    } else if( c == '\t' ) {
        OutputPos = ((OutputPos / 8) + 1) * 8;
    } else if( c != '\0' ) {
        OutputPos++;
    }
}

static void updateOutputPos( const char *string )
// update the position of the last character as it will be seen in output
{
    if( string != NULL ) {
        for( ; *string != '\0'; ++string ) {
            updateOutputPosChar( *string );
        }
    }
}

void BufferConcat( const char *string )
// concatenate a string on the end of the buffer
{
    write_string( string );
    updateOutputPos( string );
}

void BufferConcatChar( char c )
// concatentate a character on the end of the buffer
{
    write_char( c );
    updateOutputPosChar( c );
}

void BufferConcatNL( void )
// concatentate a \n on the end of the buffer
{
    write_char( '\n' );
    OutputPos = 0;
}

size_t BufferMsg( int resourceid )
// concatenate a message from the resource file
{
    char        buff[MAX_RESOURCE_SIZE];

    if( MsgGet( resourceid, buff ) ) {
        BufferConcat( buff );
        return( strlen( buff ) );
    }
    return( 0 );
}

void BufferPrint( void )
// print the buffer to current output destination
{
    PrintBuffer( Buffer, buffer_pos - Buffer );
    buffer_pos = Buffer;
}

void BufferHex( unsigned prec, dis_value value )
{
    FmtHexNum( IntermedBuffer, prec, value );
    BufferConcat( IntermedBuffer );
}

void BufferHexU32( unsigned prec, uint_32 value )
{
    dis_value   dvalue;

    dvalue.u._32[I64LO32] = value;
    dvalue.u._32[I64HI32] = 0;
    FmtHexNum( IntermedBuffer, prec, dvalue );
    BufferConcat( IntermedBuffer );
}

void BufferHex2( unsigned char value )
{
    sprintf( IntermedBuffer, "%02X", value );
    BufferConcat( IntermedBuffer );
}

void BufferHex4( unsigned short value )
{
    sprintf( IntermedBuffer, "%04X", value );
    BufferConcat( IntermedBuffer );
}

void BufferHex8( uint_32 value )
{
    sprintf( IntermedBuffer, "%08X", value );
    BufferConcat( IntermedBuffer );
}


void BufferDecimal( long value )
{
    sprintf( IntermedBuffer, "%ld", value );
    BufferConcat( IntermedBuffer );
}

void BufferUnsigned( unsigned long value )
{
    sprintf( IntermedBuffer, "%lu", value );
    BufferConcat( IntermedBuffer );
}

void BufferQuoteText( const char *text, char quote )
{
    BufferConcatChar( quote );
    BufferConcat( text );
    BufferConcatChar( quote );
}

void BufferQuoteName( const char *name )
{
    if( NeedsQuoting( name ) ) {
        BufferQuoteText( name, '`' );
    } else {
        BufferConcat( name );
    }
}

void BufferLabelNum( uint_32 value )
{
    FmtLabelNum( IntermedBuffer, value );
    BufferConcat( IntermedBuffer );
}

void BufferLinePrefixAddress( dis_sec_offset off, bool is32bit )
{
    if( is32bit ) {
        BufferHex8( off );
    } else {
        BufferHex4( off );
    }
}

void BufferLinePrefixData( unsigned_8 *data, dis_sec_offset off, dis_sec_offset total, unsigned item_size, unsigned len )
{
    unsigned    done;
    union ptr {
        unsigned_8      u8;
        unsigned_16     u16;
        unsigned_32     u32;
    }           *p;

    p = (union ptr *)( data + off );
    total -= off;
    BufferConcatChar( ' ' );
    for( done = 0; done < len; done += item_size ) {
        BufferConcatChar( ' ' );
        if( done < total ) {
            if( item_size == 1 ) {
                BufferHex2( p->u8 );
            } else if( item_size == 2 ) {
                BufferHex4( p->u16 );
            } else if( item_size == 4 ) {
                BufferHex8( p->u32 );
            }
            p = (union ptr *)( (char *)p + item_size );
        } else {
            if( item_size == 1 ) {
                BufferConcat( "  " );
            } else if( item_size == 2 ) {
                BufferConcat( "    " );
            } else if( item_size == 4 ) {
                BufferConcat( "        " );
            }
        }
    }
}
