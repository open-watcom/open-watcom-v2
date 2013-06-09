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
* Description:  Implements the functions declared in cfdir.h:
*                   get_compact_entry()
*                   get_extended_entry()
*                   skip_compact_entry()
*                   skip_extended_entry()
*
* Note:         The Wiki should be consulted for any term whose meaning is
*               not apparent. This should help in most cases.
*
****************************************************************************/

#include <stdlib.h>
#if !defined( __UNIX__ )
#include <direct.h>
#endif
#include "wgml.h"
#include "cfdir.h"
#include "common.h"

/* Extern function definitions. */

/*  Function get_compact_entry().
 *  Reads and returns the current entry.
 *
 *  Parameters:
 *      in_file points to the length-byte of the item name of the item to be
 *          skipped.
 *      entry is intended to contain the current entry.
 *
 *  Modified Parameter:
 *      entry will contain the values from the current entry on SUCCESS.
 *      entry will contain either the values on entry or a mixture of the
 *          values on entry with the values from the current entry on
 *          FAILURE, depending on where the failure occurs.
 *
 *  Returns:
 *      FAILURE on a file error or on EOF.
 *      SUCCESS otherwise.
 *
 *  Note: the defined_name and the member_name fields will be terminated with
 *        '\0' if SUCCESS is returned.
 */

int get_compact_entry( FILE * in_file, directory_entry * entry )
{
    int count;

    /* Get the length of the defined_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Ensure the length is not zero or too long for the buffer. */

    if( (count == 0) || (count > DEFINED_NAME_MAX) ) {
        return( FAILURE );
    }

    /* Get the defined_name. */

    fread( entry->defined_name, count, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Make it a null-terminated string. */

    entry->defined_name[count] = '\0';

    /* Get the length of the member_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Ensure the length is not zero or too long for the buffer. */

    if( (count == 0) || (count > NAME_MAX) ) {
        return( FAILURE );
    }

    /* Get the member_name. */

    fread( entry->member_name, count, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Make it a null-terminated string. */

    entry->member_name[count] = '\0';

    return( SUCCESS );
}

/*  Function get_extended_entry().
 *  Reads and returns the current entry.
 *
 *  Parameters:
 *      in_file points to the length-byte of the item name of the item to be
 *          skipped.
 *      entry is intended to contain the current entry.
 *
 *  Modified Parameter:
 *      entry will contain the values from the current entry on SUCCESS.
 *      entry will contain either the values on entry or a mixture of the
 *          values on entry with the values from the current entry on
 *          FAILURE, depending on where the failure occurs.
 *
 *  Returns:
 *      FAILURE on a file error or on EOF.
 *      SUCCESS otherwise.
 *
 *  Note: the defined_name and the member_name fields will be terminated with
 *        '\0' if SUCCESS is returned.
 */

int get_extended_entry( FILE * in_file, directory_entry * entry )
{
    uint8_t count;

    /* Get the length of the defined_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Ensure the length is not zero or too long for the buffer. */

    if( (count == 0) || (count > DEFINED_NAME_MAX) ) {
        return( FAILURE );
    }

    /* Get the defined_name. */

    fread( entry->defined_name, count, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Make it a null-terminated string. */

    entry->defined_name[count] = '\0';

    /* Skip the marker. */

    fseek( in_file, sizeof( uint16_t ), SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Get the length of the member_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Ensure the length is not zero or too long for the buffer. */

    if( (count == 0) || ((uint16_t) count > NAME_MAX) ) {
        return( FAILURE );
    }

    /* Get the member_name. */

    fread( &(entry->member_name), count, 1, in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Make it a null-terminated string. */

    entry->member_name[count] = '\0';

    /* Skip the preview. */

    fseek( in_file, sizeof( uint16_t ), SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    return( SUCCESS );
}

/*  Function skip_compact_entry().
 *  Skips the current entry.
 *
 *  Parameter:
 *      in_file points to the length-byte of the item name of the item to be
 *          skipped.
 *
 *  Returns:
 *      FAILURE on a file error or on EOF.
 *      SUCCESS otherwise.
 */

int skip_compact_entry( FILE * in_file )
{
    uint8_t count;

    /* Get the length of the defined_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Skip the defined_name. */

    fseek( in_file, count, SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Get the length of the member_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Skip the member_name. */

    fseek( in_file, count, SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    return( SUCCESS );
}

/*  Function skip_extended_entry().
 *  Skips the current entry.
 *
 *  Parameter:
 *      in_file points to the length-byte of the item name of the item to be
 *          skipped.
 *
 *  Returns:
 *      FAILURE on a file error or on EOF.
 *      SUCCESS otherwise.
 */

int skip_extended_entry( FILE * in_file )
{
    uint8_t count;

    /* Get the length of the defined_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Skip the defined_name. */

    fseek( in_file, count, SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Skip the marker. */

    fseek( in_file, sizeof( uint16_t ), SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Get the length of the member_name. */
    
    count = fgetc( in_file );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Skip the member_name. */

    fseek( in_file, count, SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    /* Skip the preview. */

    fseek( in_file, sizeof( uint16_t ), SEEK_CUR );
    if( ferror( in_file ) || feof( in_file ) ) return( FAILURE );

    return( SUCCESS );
}

