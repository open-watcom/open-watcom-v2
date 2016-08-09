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
* Description:  Decorate and/or truncate object file symbol names.
*
****************************************************************************/


#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "cgdefs.h"
#include "coderep.h"
#include "cgauxinf.h"
#include "utils.h"
#include "typedef.h"
#include "objout.h"
#include "feprotos.h"

#include "langenvd.h"
#if _TARGET & _TARG_PPC
  #define   __TGT_SYS   __TGT_SYS_PPC_NT
#elif _TARGET & _TARG_AXP
  #define   __TGT_SYS   __TGT_SYS_AXP_NT
#elif _TARGET & _TARG_MIPS
  #define   __TGT_SYS   __TGT_SYS_MIPS
#else
  #define   __TGT_SYS   __TGT_SYS_X86
#endif
#include "langenv.h"

#define SPEC_PREFIX             ".PREFIX_DATA."
#define DLLIMPORT_PREFIX        "__imp_"
#define PIC_RW_PREFIX           "__rw_"

#define TRUNC_SYMBOL_HASH_LEN        4
#define TRUNC_SYMBOL_LEN_WARN        120

static uint_32 objNameHash( uint_32 h, const char *s )
/****************************************************/
{
    uint_32 c;
    uint_32 g;

    // don't change this in a patch
    for(;;) {
        /* ( h & ~0x0ffffff ) == 0 is always true here */
        c = *s;
        if( c == 0 ) break;
        h = (h << 4) + c;
        g = h & ~0x0ffffff;
        h ^= g;
        h ^= g >> 24;
        ++s;
    }
    return( h );
}

static char *createFourCharHash( const char *mangle, char *buff, int ucase )
/**************************************************************************/
{
    uint_32 mangle_hash;
    int     i;
    char    c;

    c = ( ucase ) ? 'A' : 'a';
    mangle_hash = objNameHash( 0, mangle );
    for( i = 0; i < TRUNC_SYMBOL_HASH_LEN; i++ ) {
        int x;

        x = ( mangle_hash % 36 ) + '0';
        if( x > '9' ) {
            x += c - '0' - 10;
        }
        buff[TRUNC_SYMBOL_HASH_LEN - 1 - i] = x;
        mangle_hash /= 36;
    }
    buff[i] = '\0';
    return( buff );
}

static int copyBaseName( char fce, char *dst, int dst_len, const char *src, int src_len )
/***************************************************************************************/
{
    int         len;
    int         i;
    const char  *p;
    char        c;

    p = NULL;
    len = 0;
    for( i = 0; ( i < src_len ) && ( len < dst_len ); i++ ) {
        if(( p == NULL ) && ( len == dst_len - TRUNC_SYMBOL_HASH_LEN )) {
            p = src + i;
        }
        c = src[i];
        if( fce == '\0' ) {
            if( c == '\\' ) {
                continue;
            }
        } else if( fce == '!' ) {
            c = tolower( c );
        } else if( fce == '^' ) {
            c = toupper( c );
        }
        dst[len++] = c;
    }
    if( len < dst_len ) {
        return( len );
    } else if( i < src_len ) {
        char    buff[TRUNC_SYMBOL_HASH_LEN + 1];

        assert( p != NULL );
        if( p == NULL )
            p = src;
        createFourCharHash( p, buff, ( fce == '^' ) );
        if( ( dst[0] == 'W' ) && ( dst[1] == '?' ) ) {
            assert( len >= TRUNC_SYMBOL_HASH_LEN + 2 );
            memmove( dst + 2 + TRUNC_SYMBOL_HASH_LEN, dst + 2,
                                len - 2 - TRUNC_SYMBOL_HASH_LEN );
            memcpy( dst + 2, buff, TRUNC_SYMBOL_HASH_LEN );
            dst[0] = 'T';
            dst[len] = '\0';
        } else {
            assert( len >= TRUNC_SYMBOL_HASH_LEN );
            memcpy( dst + len - TRUNC_SYMBOL_HASH_LEN, buff, TRUNC_SYMBOL_HASH_LEN );
        }
        return( -len );
    } else {
        return( len );
    }
}

static char *xtoa( char *p, unsigned x )
/*******************************/
{
    if( x > 10 ) {
        p = xtoa( p, x/10 );
        x %= 10;
    }
    *p = x + '0';
    return( ++p );
}

static int GetExtName( cg_sym_handle sym, char *buffer, int max_len )
/*******************************************************************/
{
    const char           *src;
    char                 *dst;
    const char           *p;
    const char           *prefix;
    const char           *sufix;
    char                 *dst_basename;
    char                 *tmp_suffix;
    char                 c;
    int                  base_len;
    int                  dst_len;
    const char           *pattern;

    pattern = FEExtName( sym, EXTN_PATTERN );
    c = '\0';
    base_len = 0;
    prefix = pattern;
    for( p = pattern; *p != '\0'; p++ ) {
        if(( *p == '*' ) || ( *p == '!' ) || ( *p == '^' )) {
            if( c == '\0' ) {
                prefix = p;
                c = *p;
            }
        } else if( ( c != '\0' ) || ( *p == '#' ) ) {
            break;
        } else if( *p == '\\' ) {
            p++;
        }
    }
    if( c == '\0' )
        base_len = p - pattern;
    sufix = p;
    // add prefix to output buffer
    dst = buffer;
    for( src = pattern; src != prefix; ++src ) {
        if( *src != '\\' ) {
            *(dst++) = *src;
        }
    }
    *dst = '\0';
    // add sufix to output buffer
    dst_basename = dst;
    for( src = sufix; *src != '\0'; ++src ) {
        if( *src == '#' ) {
            unsigned    size;

            size = (unsigned)(pointer_int)FEExtName( sym, EXTN_PRMSIZE );
            if( size != (unsigned)-1 ) {
                *(dst++) = '@';
                dst = xtoa( dst, size );
            }
        } else {
            if( *src == '\\' )
                ++src;
            *(dst++) = *src;
        }
    }
    *dst = '\0';
    // max base name length
    dst_len = max_len - ( dst - buffer );
    if( dst_len > 0 ) {
        int     sufix_len;
        int     len;

        if( base_len != 0 ) {
            // alias
            src = pattern;
        } else {
            src = FEExtName( sym, EXTN_BASENAME );
            base_len = strlen( src );
        }
        // shift sufix to the end of buffer
        sufix_len = dst - dst_basename;
        tmp_suffix = buffer + max_len - sufix_len;
        memmove( tmp_suffix, dst_basename, sufix_len + 1 );
        // copy + truncate base symbol name
        len = copyBaseName( c, dst_basename, dst_len, src, base_len );
        if( len < 0 ) {
            FEMessage( MSG_SYMBOL_TOO_LONG, sym );
        } else {
            // shift sufix to the end of symbol name
            memcpy( dst_basename + len, tmp_suffix, sufix_len + 1 );
        }
    } else {
       // TODO: error prefix + sufix >= max_len
       assert( 0 );
    }
    return( 0 );
}

void    DoOutObjectName( cg_sym_handle sym,
                        void (*outputter)( const char *, void * ),
                        void *data, import_type kind )
/*******************************************************************/
{
    char        *dst;
    char        buffer[TS_MAX_OBJNAME + TRUNC_SYMBOL_HASH_LEN];
    unsigned    pref_len;

    dst = buffer;
    switch( kind ) {
    case SPECIAL:
        pref_len = (sizeof( SPEC_PREFIX )-1);
        dst = CopyStr( SPEC_PREFIX, dst );
        break;
    case DLLIMPORT:
        pref_len = (sizeof( DLLIMPORT_PREFIX )-1);
        dst = CopyStr( DLLIMPORT_PREFIX, dst );
        break;
    case PIC_RW:
        pref_len = (sizeof( PIC_RW_PREFIX )-1);
        dst = CopyStr( PIC_RW_PREFIX, dst );
        break;
    default:
        pref_len = 0;
    }
    GetExtName( sym, dst, TS_MAX_OBJNAME - 1 - pref_len );
    outputter( buffer, data );
}

extern bool SymIsExported( cg_sym_handle sym ) {
/*******************************************/

    bool        exported;

    exported = false;
    if( sym != NULL ) {
        if( FEAttr( sym ) & FE_DLLEXPORT ) {
            exported = true;
        } else if( *(call_class*)FindAuxInfoSym( sym, CALL_CLASS ) & DLL_EXPORT ) {
            exported = true;
        }
    }
    return( exported );
}
