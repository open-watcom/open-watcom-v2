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
* Description:  Implements the functions common to the research programs:
*                   display_char()
*                   display_hex_block()
*                   display_hex_char()
*                   display_hex_line()
*                   parse_cmdline()
*                   res_initialize_globals()
*
****************************************************************************/

#define __STDC_WANT_LIB_EXT1__  1 // Activates "Safe C" functions.
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cmdlhelp.h"
#include "common.h"

/* Global variables. */

#define global
#include "research.h"

/* Local data definition. */ 

static char     hexchar[] = "0123456789ABCDEF";

/*  Function definitions. */

/*  Function display_char().
 *  If isgraph() indicates that in_char is displayable, returns a space in
 *  out_chars[0] and in_char in out_chars[1]. Otherwise returns values in
 *  out_chars which, when output together, provide a hexadecimal representation
 *  of in_char. Since isgraph() is used, the space character will appear in
 *  hexadecimal.
 *
 *  Parameters:
 *      out_chars points to a two-char array 
 *      in_char contains the value to be output
 *
 *  Value Returned:
 *      The values indicated above are returned in the out_chars.
 */

void display_char( char * out_chars, char in_char )
{
    if ( isgraph( in_char ) ) {
        out_chars[0] = ' ';
        out_chars[1] = in_char;
    } else {
        out_chars[0] = hexchar[ ( in_char >> 4 ) & 0x0f ];
        out_chars[1] = hexchar[ in_char & 0x0f ];
    }
    
    return;
}

/*  Function display_hex_block().
 *  Given a pointer to a block of bytes and the number of bytes it contains,
 *  display the data in a format similar to that of wdump -b to stdout.
 *
 *  Parameters:
 *      in_data points to the first byte in the block.
 *      in_length contains the number of bytes in the block.
 */

void display_hex_block( uint8_t * in_data, uint16_t in_count )
{
    uint16_t    i;
    char        data_buffer[16];
    char        data_display[69];

    for( i = 0; i < in_count; i += 16 ) {
        if( i + 16 > in_count ) {
            memset(data_buffer, 0x20, 16);
            memcpy_s(data_buffer, 16, &in_data[i], in_count - i );
        } else {
            memcpy_s(data_buffer, 16, &in_data[i], 16 );
        }
        display_hex_line( data_display, data_buffer );
        printf_s( "%s\n", data_display );
    }

    return;
}

/*  Function display_hex_char().
 *  Returns values in out_chars which, when output as a null-terminated string,
 *  provide a hexadecimal representation of in_char. 
 *
 *  Parameters:
 *      out_chars points to a three-char array.
 *      in_char contains the value to be output.
 *
 *  Value Returned:
 *      out_chars contains a string representing the char in hex.
 */
 
void display_hex_char( char * out_chars, char in_char )
{
    out_chars[0] = hexchar[ ( in_char >> 4 ) & 0x0f ];
    out_chars[1] = hexchar[ in_char & 0x0f ];
    out_chars[2] = '\0';
    
    return;    
}

/*  Function display_hex_line().
 *  Constructs out_chars from in_chars. The effect is to replace a line of 16
 *  byte values with a line of 68 bytes:
 *      8 groups of a two byte representation of a byte value plus a space.
 *      1 space, producing the "two spaces in the middle" pattern.
 *      8 groups of a two byte representation of a byte value plus a space.
 *      3 spaces, producing four spaces between the hex and byte displays.
 *      16 values, each of which is the value passed in if isprint() indicates
 *          it is printable, or a space otherwise.
 *  The result is the sort of hex display produced by wdump -b. The 69th byte
 *  is '\0': out_chars is intended to be displayable as a character string.
 *
 *  Parameters:
 *      out_chars must point to an array of at least 69 bytes.
 *      in_chars must point to an array of 16 bytes.
 */
 
void display_hex_line( char * out_chars, char * in_chars )
{
    uint8_t i;

    /* Process the first eight input values. */

    for( i = 0; i < 8; i++) {
        out_chars[3*i] = hexchar[ ( in_chars[i] >> 4 ) & 0x0f ];
        out_chars[3*i + 1] = hexchar[ in_chars[i] & 0x0f ];
        out_chars[3*i + 2] = ' ';
    }

    /* Insert the second space between the columns. */

    out_chars[24] = ' ';

    /* Process the second eight input values. */
    
    for( i = 8; i < 16; i++) {
        out_chars[3*i + 1] = hexchar[ ( in_chars[i] >> 4 ) & 0x0f ];
        out_chars[3*i + 2] = hexchar[ in_chars[i] & 0x0f ];
        out_chars[3*i + 3] = ' ';
    }

    /* Insert three more spaces. */

    out_chars[49] = ' ';
    out_chars[50] = ' ';
    out_chars[51] = ' ';

    /* Now append the input values, if printable. */

    for( i = 0; i < 16; i++ ) {
        if ( isprint( in_chars[i] ) ) {
            out_chars[i+52] = in_chars[i];
        } else {
           out_chars[i+52] = ' ';
        }
    }

    /* Make out_chars a character string. */

    out_chars[68] = '\0';    

    return;
}

/*  Function parse_cmdline().
 *  Extract the path entered by the user into tgt_path. Doublequotes used
 *  to allow embedded spaces are removed.
 *
 *  Parameter:
 *      cmdline contains the command line without the program name.
 *
 *  Global Used:
 *      tgt_path is loaded with the directory provided to the program, with any
 *          doublequotes removed.
 *
 *  Returns:
 *      FAILURE on any error.
 *      SUCCESS if all goes well.
 */

int parse_cmdline( char * cmdline )
{
    char *      end;
    char        opt;
    ptrdiff_t   len;

    /* Find the parameter -- there should only be one. */
        
    cmdline = skip_spaces( cmdline );

    /* In case someone finds a way to enter an all-whitespace command line. */
    
    if( *cmdline == '\0' ) {
        print_usage();
        return ( FAILURE );
    }

    /* Verify that parameter is not adorned. */
 
    opt = *cmdline;
    if( opt == '-'  ||  opt == dos_switch_char  ||  opt == '@' ) {
        print_usage();
        return( FAILURE );
    }

    /* Find the length of the parameter. */

    end = cmdline;
    end = FindNextWS( end );
    len = end - cmdline;

    /* In case someone managed to enter a zero-length path. */
        
    if( len <= 0)
    {
        print_usage();
        return( FAILURE );
    }

    /* To allow space for the final null character */

    len++;

    /* Allocate memory for the global pointer tgt_path. */
        
    tgt_path = malloc( len );
    if( tgt_path == NULL ) return( FAILURE );

    /* Copy the parameter into tgt_path. */
    /* It will be null-terminated per documentation of strncpy_s(). */

    strncpy_s( tgt_path, len, cmdline, len-1 );

    /* Remove doublequotes, if present. */

    if(opt == '\"') UnquoteFName( tgt_path, len, tgt_path );

    /* We are done. */

    return( SUCCESS );

}

/*  Function res_initialize_globals().
 *  Initialize the global variable common to the research functions.
 */

void res_initialize_globals( void )
{
    tgt_path = NULL;
}


