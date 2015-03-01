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
* Description:  Implements the function used to parse .COP headers:
*                   parse_header()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
****************************************************************************/

#include <stdint.h>
#include <string.h>

#include "copfunc.h"
#include "cophdr.h"

/* Function parse_header().
 * Determine if the current position of the input stream points to the
 * start of a valid same-endian version 4.1 binary device file and, if 
 * it does, advance the stream to the first byte following the header.
 *
 * Parameter:
 *      in_file points the input stream.
 *
 * Returns:
 *      dir_v4_1_se if the file is a same-endian version 4.1 directory file.
 *      se_v4_1_not_dir if the file is a same-endian version 4.1 device,
 *          driver, or font file.
 *      not_se_v4_1 if the file is not same-endian and/or not version 4.1.
 *      not_bin_dev if the file is not a binary device file at all.
 *      file_error if an error occurred while reading the file.
 */

cop_file_type parse_header( FILE * in_file )
{
    unsigned char   count;
    char            text_version[0x0b];
    uint16_t        version;

    /* Get the count and ensure it is 0x02. */

    count = fread_u8( in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    if( count != 0x02 ) {
        return( not_bin_dev );
    }

    /* Check for a same_endian version 4.1 header.
    *  Note: checking 0x0c00 would, presumably, identify a different-endian
    *  version 4.1 header, if that ever becomes necessary.
    */
        
    fread_buff( &version, sizeof( version ), in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    if( version != 0x000c ) {
        return( not_se_v4_1 );
    }

    /* Get the text_version_length and ensure it is 0x0b. */

    count = fread_u8( in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    if( count != 0x0b ) {
        return( not_bin_dev );
    }

    /* Verify the text_version. */

    fread_buff( text_version, 0x0b, in_file );
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }

    if( memcmp( text_version, "V4.1 PC/DOS", 0x0b ) ) {
        return( not_bin_dev );
    }

    /* Get the type. */

    count = fread_u8( in_file );

    /* If there is no more data, this is not a valid .COP file. */
    
    if( ferror( in_file ) || feof( in_file ) ) {
        return( file_error );
    }
    
    /* Valid header, more data exists, determine the file type. */

    if( count == 0x03 ) {
        return( se_v4_1_not_dir );
    }
    if( count == 0x04 ) {
        return( dir_v4_1_se ); 
    }

    /* Invalid file type: this cannot be a valid .COP file. */

    return( not_bin_dev );
}


