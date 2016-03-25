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


// array of bits set for chars in alternate char-set
extern unsigned char    _ti_alt_map[32];
// array of char mappings
extern char             ti_char_map[256];

// macros for getting/setting bits in alt-char map
#define ti_alt_map( x )     ( _ti_alt_map[( x ) / 8] != 0 && (( _ti_alt_map[( x ) / 8] >> ( ( x ) % 8 ) ) & 1) )
#define ti_alt_map_set( x ) ( _ti_alt_map[( x ) / 8] |= ( 1 << ( ( x ) % 8 ) ) )


typedef enum {
    TIX_FAIL,
    TIX_NOFILE,
    TIX_DEFAULT,
    TIX_OK
} tix_status;

extern tix_status   ti_read_tix( bool );
extern int          ui_tix_missing( const char *name );
