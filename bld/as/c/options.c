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


#include "as.h"
#include <ctype.h>
#include "preproc.h"
#include "banner.h"

#ifdef AS_ALPHA
as_flags        AsOptions = OBJ_COFF;   // COFF is default.
#else
as_flags        AsOptions = 0;          // ELF is default.
#endif

char            *AsIncPath = NULL;

static char     **ppDefines = NULL;
static int      maxNumPredefines;

static bool optionsPredefine( const char *str ) {
//***********************************************
// Sets up an array of define strings for PP_Define later on when it's init'ed.

    static unsigned     idx;
    unsigned            i;
    size_t              length;
    const char          *s;
    bool                got_equal, got_macro;

    if( !*str ) return( FALSE );
    if( ppDefines == NULL ) {
        length = maxNumPredefines * sizeof( char * );
        ppDefines = MemAlloc( length );
        memset( ppDefines, 0, length );
        idx = 0;
    } else {
        ++idx;
    }
    ppDefines[idx] = MemAlloc( strlen( str ) + 1 );
    got_equal = FALSE;
    got_macro = FALSE;
    s = str;
    i = 0;
    for( ;; ) {
        if( *s == '=' ) {
            if( got_equal || !got_macro ) return( FALSE );
            got_equal = TRUE;
            ppDefines[idx][i] = ' ';
        } else {
            got_macro = TRUE;
            ppDefines[idx][i] = *s;
            if( *s == '\0' ) break;
        }
        ++i;
        ++s;
    }
    return( TRUE );
}

extern void OptionsPPDefine( void ) {
//***********************************

    unsigned    idx = 0;
    char        *str;

    if( !ppDefines ) return;
    str = ppDefines[idx++];
    while( str ) {
        PP_Define( str );
        str = ppDefines[idx++];
    }
}

extern void OptionsFini( void ) {
//*******************************

    unsigned    idx = 0;
    char        *str;

    if( ppDefines ) {
        str = ppDefines[idx++];
        while( str ) {
            MemFree( str );
            str = ppDefines[idx++];
        }
        MemFree( ppDefines );
    }

    if( AsIncPath ) MemFree( AsIncPath );
}

extern bool OptionsInit( int argc, char **argv ) {
//************************************************

    char        *s;

#ifdef AS_ALPHA
    s = "__WASAXP__=" BANSTR( _BANVER ) ;
#elif defined( AS_PPC )
    s = "__WASPPC__=" BANSTR( _BANVER ) ;
#elif defined( AS_MIPS )
    s = "__WASMPS__=" BANSTR( _BANVER ) ;
#endif
    maxNumPredefines = argc + 2; // version macro and extra null at the end
    if( !optionsPredefine( s ) )
        goto errInvalid;

    while( *argv ) {
        if( argv[0][0] == '-' || argv[0][0] == '/' ) {
            s = &argv[0][2];
            switch( argv[0][1] ) {
            case 'b':
            case 'B':
                // ignore the -bt=NT crap
                break;
            case 'd':
            case 'D':
                if( isdigit( *s ) ) {
                    DebugLevel = strtoul( s, &s, 10 );
                } else if( !optionsPredefine( s ) ) {
                    goto errInvalid;
                }
                break;
            case 'e':
                if( *s == '\0' ) goto errInvalid;
                ErrorLimit = strtoul( s, &s, 10 );
                if( *s != '\0' ) goto errInvalid;
                break;
            case 'f':
            case 'F':
                switch( *s ) {
                case 'o':
                    ++s;
                    if( *s == '=' ) ++s;
                    if( *s == '\0' ) goto errInvalid;
                    ObjSetObjFile( s );
                    break;
                default:
                    goto errInvalid;
                }
                break;
            case 'h':
            case '?':
                _SetOption( PRINT_HELP );
                break;
            case 'i':
            case 'I':
                if( *s == '=' ) {
                    ++s;
                }
                if( *s == '\0' ) break;
                if( AsIncPath ) { // Additional /i switch
                    AsIncPath = MemRealloc( AsIncPath, strlen( AsIncPath ) +
                        strlen( s ) + 2 );      // for ';' and EOL
                    strcat( AsIncPath, ";" );
                    strcat( AsIncPath, s );
                } else { // First /i switch
                    AsIncPath = MemAlloc( strlen( s ) + 1 );
                    strcpy( AsIncPath, s );
                }
                break;
            case 'o':
                switch( *s ) {
                case 'c':
                    _SetOption( OBJ_COFF );
                    break;
                case 'e': // ELF
                    _UnsetOption( OBJ_COFF );
                    break;
                default:
                    goto errInvalid;
                }
                if( *++s != '\0' ) goto errInvalid;
                break;
            case 'q':
                _SetOption( BE_QUIET );
                break;
            case 'w':
                if( isdigit( *s ) ) {
                    WarningLevel = strtoul( s, &s, 10 );
                } else {
                    switch( *s ) {
                    case 'e':
                        _SetOption( WARNING_ERROR );
                        break;
                    default:
                        goto errInvalid;
                    }
                }
                break;
            case 'z':
                switch( *s ) {
                case 'q':
                    _SetOption( BE_QUIET );
                    break;
                default:
                    goto errInvalid;
                }
                break;
#ifdef AS_DEBUG_DUMP
            case 'v':
                while( *s ) {
                    switch( *s ) {
                    case 'p':
                        _SetOption( DUMP_PARSE_TREE );
                        break;
                    case 't':
                        _SetOption( DUMP_INS_TABLE );
                        break;
                    case 'i':
                        _SetOption( DUMP_INSTRUCTIONS );
                        break;
                    case 's':
                        _SetOption( DUMP_SYMBOL_TABLE );
                        break;
                    case 'l':
                        _SetOption( DUMP_LEXER_BUFFER );
                        break;
                    case 'd':
                        _SetOption( DUMP_DEBUG_MSGS );
                        break;
                    default:
                        goto errInvalid;
                    }
                    s++;
                }
                break;
#endif
            default:
            errInvalid:
                Banner();
                AsOutMessage( stderr, AS_MSG_ERROR );
                AsOutMessage( stderr, INVALID_OPTION, *argv );
                fputc( '\n', stderr );
                return( FALSE );
                break;
            }
            memcpy( argv, argv+1, sizeof( *argv ) * argc );
        } else {
            argv++;
        }
        argc--;
        maxNumPredefines--;
    }
    return( TRUE );
}
