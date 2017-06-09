/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2017-2017 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  In-memory Dialog manipulation functions
*
****************************************************************************/


#include "wdeglbl.h"
#include "windlg.h"
#include "wdedlgut.h"


/*
 * copyString - copy from string to memory
 */
static WPCHAR copyString( WPCHAR mem, const char *str, int len )
{
#if defined( __WINDOWS__ )
    if( mem != NULL ) {
        _FARmemcpy( mem, str, len );
    }
    return( mem + len );
#else
    int i;

    if( mem != NULL ) {
        for( i = 0; i < len / 2; i++ ) {
            *(short *)mem = *(unsigned char *)str;
            mem += 2;
            str++;
        }
        return( mem );
    } else {
        return( mem + len );
    }
#endif

} /* copyString */

/*
 * DlgStringLength - get length of string
 */
size_t DlgStringLength( const char *str )
{
#if defined( __WINDOWS__ )
    return( SLEN( str ) );
#else
    if( str == NULL ) {
        return( SLEN( str ) );
    } else {
        int len;
        len = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS, str, -1, NULL, 0 );
        if( len == 0 || len == ERROR_NO_UNICODE_TRANSLATION ) {
            return( SLEN( str ) );
        }
        return( len * sizeof( WCHAR ) );
    }
#endif

} /* DlgStringLength */

/*
 * DlgCopyMBString - copy from string to memory
 */
WPCHAR DlgCopyMBString( WPCHAR mem, const char *str, int len )
{
    if( str == NULL )
        str = "";
#if defined( __WINDOWS__ )
    return( copyString( mem, str, len ) );
#else
    if( mem != NULL ) {
        int     len2;
        len2 = MultiByteToWideChar( CP_OEMCP, MB_ERR_INVALID_CHARS, str, -1, (LPWSTR)mem, len );
        len2 *= sizeof( WCHAR );
        if( len2 != len ) {
            return( copyString( mem, str, len ) );
        }
        return( mem + len );
    } else {
        return( copyString( mem, str, len ) );
    }
#endif

} /* DlgCopyMBString */


/*
 * DlgCopyWord - copy from Word to memory
 */
WPCHAR DlgCopyWord( WPCHAR mem, WORD word )
{
    *mem++ = word;
    *mem++ = word >> 8;
    return( mem );

} /* DlgCopyWord */

unsigned char DlgGetClassOrdinal( const char *classname )
{
    unsigned char   class_ordinal;

    if( classname == NULL ) {
        class_ordinal = 0;
    } else if( stricmp( classname, "combobox" ) == 0 ) {
        class_ordinal = 0x85;
    } else if( stricmp( classname, "scrollbar" ) == 0 ) {
        class_ordinal = 0x84;
    } else if( stricmp( classname, "listbox" ) == 0 ) {
        class_ordinal = 0x83;
    } else if( stricmp( classname, "static" ) == 0 ) {
        class_ordinal = 0x82;
    } else if( stricmp( classname, "edit" ) == 0 ) {
        class_ordinal = 0x81;
    } else if( stricmp( classname, "button" ) == 0 ) {
        class_ordinal = 0x80;
    } else {
        class_ordinal = 0;
    }
    return( class_ordinal );
}
