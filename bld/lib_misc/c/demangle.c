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
* Description:  C++ name demangling.
*
****************************************************************************/


#define ZZ_LEN_3

#ifdef __DIP__
#define __NO_STACK_CHECKING__
#endif

#if 0 || defined( TEST ) || defined( DUMP )
#include <stdio.h>
#endif
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "watcom.h"
#include "walloca.h"
#include "demangle.h"

#include "clibext.h"


#if defined( __WATCOMC__ )
#define STRNCMP _fstrncmp
#else
#define STRNCMP strncmp
#endif

#define IMPORT_PREFIX_STR_L     "__imp_"
#define IMPORT_PREFIX_STR_U     "__IMP_"
#define IMPORT_PREFIX_LEN       ( sizeof( IMPORT_PREFIX_STR_L ) - 1 )
#define CHECK_IMPORT_PREFIX(s)  (s[0]=='_'&&(memcmp(s,IMPORT_PREFIX_STR_L,IMPORT_PREFIX_LEN)==0||memcmp(s,IMPORT_PREFIX_STR_U,IMPORT_PREFIX_LEN)==0))

#define RECURSE_CHECK           (100*sizeof(int))
#define AUTO_BUFFER_SIZE        80
#define SUPPRESS_LIMIT          2
#define NULL_CHAR               '\0'

#define TRUNCATED_HASH_LEN      4

#define TRUNCATED_PREFIX1       'T'
#define MANGLE_PREFIX1          'W'
#define MANGLE_PREFIX2          '?'
#define PREFIX_EMBEDDED         '?'
#define MEMBER_POINTER          'M'
#define UNDERSCORE              '_'
#define PERIOD                  '.'
#define SCOPE_PREFIX            ':'
#define TEMPLATE_PREFIX         ':'
#define TEMPLATE_INT            '0'
#define TEMPLATE_TYPE           '1'
#define POSITIVE_INT            'Z'
#define NEGATIVE_INT            'Y'
#define OPNAME_PREFIX           '$'
#define SYMBOL_SUFFIX           '$'
#define MANGLE_RECURSE_SUFFIX   '$'
#define TYPE_NAME_PREFIX        '$'
#define TYPE_NAME_SUFFIX        '$'
#define THIS_QUAL_PREFIX        '.'
#define ARRAY_PREFIX            '['
#define ARRAY_SUFFIX            ']'
#define FUNCTION_PREFIX         '('
#define FUNCTION_SUFFIX         ')'
#define OP_FUN_PREFIX           'O'
#define REL_FUN_PREFIX          'R'
#define SPEC_TYPE_PREFIX        'T'
#define WAT_FUN_PREFIX          'W'
#define ASGN_FUN_PREFIX         'A'

#define GROUP_C_PREFIX          'C'
#define CTOR_PREFIX1            'C'
#define OP_CONVERT_PREFIX1      'C'
#define CTOR_PREFIX2            'T'
#define OP_CONVERT_PREFIX2      'V'

#define GROUP_D_PREFIX          'D'
#define DTOR_PREFIX1            'D'
#define OP_DELETE_PREFIX1       'D'
#define ARRAY_DELETE_PREFIX1    'D'
#define DTOR_PREFIX2            'T'
#define OP_DELETE_PREFIX2       'L'
#define ARRAY_DELETE_PREFIX2    'A'

#define GROUP_N_PREFIX          'N'
#define OP_NEW_PREFIX1          'N'
#define ARRAY_NEW_PREFIX1       'N'
#define OP_NEW_PREFIX2          'W'
#define ARRAY_NEW_PREFIX2       'A'

#define mytoupper(c)    ((c < 'a' || c > 'z') ? c : c - 0x20)

typedef void *(*realloc_fn_t)( void *, size_t );

typedef void (*outfunPtr)(void **, dm_pts, pointer_int, char const *);

#define MAX_REPLICATE   10
typedef struct replicate_desc {
    char const      *ptr;
    size_t          len;
} replicate_desc;

// static R/W data
static realloc_fn_t     user_realloc;
static replicate_desc   replicate[MAX_REPLICATE];
static int              next_replicate;

typedef struct output_desc {
    outfunPtr       outfun;
    void            *cookie;
    char const      *input;
    char const      *end;
    char const      *scope_ptr;
    char            *output;
    size_t          size;
    size_t          count;
    size_t          index;
    size_t          pending_loc;
    size_t          scope_len;
    int             suppress_output;
    unsigned        scope_index;
    unsigned        ctdt_pending : 1;
    unsigned        cv_pending : 1;
    unsigned        scope_name : 1;
    unsigned        base_name : 1;
    unsigned        dllimport : 1;
} output_desc;

// the simple demangler uses these to output & count chars in the output buffer
// the parser callback uses these to extract the structure of the name
#define _output1( _a         ) (data->outfun)( &(data->cookie), _a,  0,  0 )
#define _output2( _a, _b     ) (data->outfun)( &(data->cookie), _a, _b,  0 )
#define _output3( _a, _b, _c ) (data->outfun)( &(data->cookie), _a, _b, _c )

typedef struct state_desc {
    size_t          prefix;
    size_t          suffix;
    char            right : 1;
} state_desc;

static char const _WCI86FAR dtorChar = '~';
static char const _WCI86FAR openParen = '(';
static char const _WCI86FAR closeParen = ')';
static char const _WCI86FAR signedPrefix[] = "signed ";
static char const _WCI86FAR operatorName[] = "operator ";
static char const _WCI86FAR deleteFunction[] = "delete";
static char const _WCI86FAR deleteArrayFunction[] = "delete []";
static char const _WCI86FAR anonymousEnum[] = "__anonymous_enum ";
static char const _WCI86FAR newFunction[] = "new";
static char const _WCI86FAR newArrayFunction[] = "new []";
static char const _WCI86FAR scopeSeparator[] = "::";
static char const _WCI86FAR templatePrefix[] = "<";
static char const _WCI86FAR templateSuffix[] = ">";
static char const _WCI86FAR templateSeparator[] = ",";
static char const _WCI86FAR arrayPrefix[] = "[";
static char const _WCI86FAR arraySuffix[] = "]";
static char const _WCI86FAR functionPrefix[] = "( ";
static char const _WCI86FAR functionSuffix[] = ")";
static char const _WCI86FAR functionSeparator[] = ", ";
static char const _WCI86FAR basedSuffix[] = ") ";
static char const _WCI86FAR basedStrPrefix[] = "\"";
static char const _WCI86FAR basedStrSuffix[] = "\"";
static char const _WCI86FAR basedSelf[] = "__self";
static char const _WCI86FAR basedVoid[] = "void";

// mangled character translations
typedef struct table_t {
    char const _WCI86FAR        *string;    // translated string
    char                        grouping;   // flavour of character
} table_t;

#define LOWER_TABLE_LIMIT           'A'

static table_t const _WCI86FAR translate_type_encoding[] = {
#define CHAR_UNUSED         0
#define CHAR_BASIC_TYPE     1
#define CHAR_POINTER        2
#define CHAR_MODIFIER       3
    /* 'A' */   { "char ",      CHAR_BASIC_TYPE },
    /* 'B' */   { "float ",     CHAR_BASIC_TYPE },
    /* 'C' */   { "char ",      CHAR_BASIC_TYPE },
    /* 'D' */   { "double ",    CHAR_BASIC_TYPE },
    /* 'E' */   { "... ",       CHAR_BASIC_TYPE },
    /* 'F' */   { "far ",       CHAR_MODIFIER },
    /* 'G' */   { "far16 ",     CHAR_MODIFIER },
    /* 'H' */   { "huge ",      CHAR_MODIFIER },
    /* 'I' */   { "int ",       CHAR_BASIC_TYPE },
    /* 'J' */   { "__based(",   CHAR_MODIFIER },
    /* 'K' */   { NULL,         CHAR_UNUSED },
    /* 'L' */   { "long ",      CHAR_BASIC_TYPE },
    /* 'M' */   { "::* ",       CHAR_POINTER },
    /* 'N' */   { "near ",      CHAR_MODIFIER },
    /* 'O' */   { NULL,         CHAR_UNUSED },
    /* 'P' */   { "* ",         CHAR_POINTER },
    /* 'Q' */   { "bool ",      CHAR_BASIC_TYPE },
    /* 'R' */   { "& ",         CHAR_POINTER },
    /* 'S' */   { "short ",     CHAR_BASIC_TYPE },
    /* 'T' */   { "long double ",CHAR_BASIC_TYPE },
    /* 'U' */   { "unsigned ",  CHAR_MODIFIER },
    /* 'V' */   { "void ",      CHAR_BASIC_TYPE },
    /* 'W' */   { "wchar_t ",   CHAR_BASIC_TYPE },
    /* 'X' */   { "const ",     CHAR_MODIFIER },
    /* 'Y' */   { "volatile ",  CHAR_MODIFIER },
    /* 'Z' */   { "__int64 ",   CHAR_BASIC_TYPE },
    /* '[' */   { NULL,         CHAR_UNUSED },
    /* '\' */   { NULL,         CHAR_UNUSED },
    /* ']' */   { NULL,         CHAR_UNUSED },
    /* '^' */   { NULL,         CHAR_UNUSED },
    /* '_' */   { "",           CHAR_BASIC_TYPE } // for ctor/dtor return type
};

static char _WCI86FAR * const _WCI86FAR operatorFunction[] = {
    /* A */ ">>",
    /* B */ "<<",
    /* C */ "!",
    /* D */ "[]",
    /* E */ "->",
    /* F */ "*",
    /* G */ "++",
    /* H */ "--",
    /* I */ "-",
    /* J */ "+",
    /* K */ "&",
    /* L */ "->*",
    /* M */ "/",
    /* N */ "%",
    /* O */ ",",
    /* P */ "()",
    /* Q */ "~",
    /* R */ "^",
    /* S */ "|",
    /* T */ "&&",
    /* U */ "||"
};
static char const _WCI86FAR * const _WCI86FAR relationalFunction[] = {
    /* A */ "==",
    /* B */ "!=",
    /* C */ "<",
    /* D */ "<=",
    /* E */ ">",
    /* F */ ">="
};
static char const _WCI86FAR * const _WCI86FAR assignmentFunction[] = {
    /* A */ "=",
    /* B */ "*=",
    /* C */ "+=",
    /* D */ "-=",
    /* E */ "/=",
    /* F */ "%=",
    /* G */ ">>=",
    /* H */ "<<=",
    /* I */ "&=",
    /* J */ "|=",
    /* K */ "^="
};
#define num_elements( __a ) (sizeof(__a) / sizeof(__a[0]))

#define check_element(i,a)  (i >= 0 && i < num_elements( a ))

typedef union key_desc {
    char        str[2];
    short       val;
} key_desc;
typedef struct assoc_desc {
    key_desc            u;
    char _WCI86FAR      *name;
} assoc_desc;

static assoc_desc const _WCI86FAR watcomObject[] = {
    { {'A','*'},  "__internal" },
    { {'B','I'},  "__onceonly" },
    { {'D','A'},  "__arrdtorblk" },
    { {'D','F'},  "__defarg" },
    { {'D','I'},  "__debuginfo" },
    { {'D','O'},  "__dtorobjblk" },
    { {'M','P'},  "__mbrptrthunk" },
    { {'S','I'},  "__staticinit" },
    { {'T','H'},  "__throwblk" },
    { {'T','I'},  "__typeid" },
    { {'T','S'},  "__typesig" },
    { {'V','A'},  "__rtti" },
    { {'V','B'},  "__vbtbl" },
    { {'V','F'},  "__vftbl" },
    { {'V','M'},  "__vmtbl" },
    { {'V','T'},  "__vfthunk" },
    { {'S','T'},  "__typstattab" },
    { {'C','M'},  "__stattabcmd" },
    { {'U','N'},  "<unique>" },
};
#define MAX_WATCOM_OBJECT       num_elements( watcomObject )

#if 0 || defined( TEST ) || defined( DUMP )
int no_errors;
unsigned errors;
#endif

static bool scoped_name( output_desc *data, state_desc *state );
static bool type_encoding( output_desc *data, state_desc *state );
static bool recursive_mangled_name( output_desc *data, state_desc *state );
static size_t terminateOutput( output_desc *data );

static void zapSpace( output_desc *data )
{
    if( data->output == NULL ) {
        /* count the characters mode */
        return;
    }
    if( data->index > data->size ) {
        return;
    }
    if( data->output[data->index - 1] != ' ' ) {
        return;
    }
    /* remove trailing space */
    data->count--;
    if( data->index != (data->count + 1) ) {
        size_t last = data->size;
        if( last > data->count )
            last = data->count;
        if( last >= data->index ) {
            memmove( &data->output[data->index - 1], &data->output[data->index], ( last - data->index ) + 1 );
        }
    }
    data->index--;
}

#define START_ADJUST    ( 1 << 8 )

static void emitChar( output_desc *data, char c )
{
    size_t adjust_size;
    size_t test_size;
    char *test_realloc;

    if( data->suppress_output > SUPPRESS_LIMIT ) {
        return;
    }
    data->count++;
    if( data->output == NULL ) {
        /* count the characters mode */
        return;
    }
    if( data->count == ( data->size - 1 ) ) {
        if( user_realloc != NULL ) {
            adjust_size = START_ADJUST;
            for(;;) {
                if( adjust_size == 0 ) {
                    return;
                }
                test_size = data->size + adjust_size;
                test_realloc = user_realloc( data->output, test_size );
                if( test_realloc != NULL ) break;
                adjust_size >>= 1;
            }
            data->output = test_realloc;
            data->size = test_size;
        }
    }
    if( data->index < data->size ) {
        if( data->index < (data->count - 1) ) {
            size_t last = data->size;
            if( last > data->count )
                last = data->count;
            memmove( &data->output[data->index + 1], &data->output[data->index], ( last - data->index ) - 1 );
        }
        data->output[data->index] = c;
        data->index++;
    }
}

static void emitStr( output_desc *data, const char _WCI86FAR *p )
{
    if( p ) {
        while( *p ) {
            emitChar( data, *p );
            ++p;
        }
    }
}

#define setEmitIndex( data, idx ) ((data)->index = (idx))

static void resetEmitIndex( output_desc *data, size_t offset_from_end )
{
    size_t index;

    index = data->count - offset_from_end;
    if( index < data->size ) {
        data->index = index;
    } else {
        data->index = data->size - 1;
    }
}

static void setSuppression( output_desc *data )
{
    if( data->suppress_output ) {
        data->suppress_output++;
    }
}

static void resetSuppression( output_desc *data )
{
    if( data->suppress_output ) {
        --data->suppress_output;
    }
}

static void forceSuppression( output_desc *data )
{
    if( data->suppress_output <= SUPPRESS_LIMIT ) {
        data->suppress_output += SUPPRESS_LIMIT+1;
    }
}

static void unforceSuppression( output_desc *data )
{
    if( data->suppress_output > SUPPRESS_LIMIT ) {
        data->suppress_output -= SUPPRESS_LIMIT+1;
    }
}

static void demangleEmit( void **cookie, dm_pts dp, pointer_int value, char const *ptr )
{
    output_desc *data = *((output_desc **)cookie);
    size_t      idx = (size_t)value;

    switch( dp ) {
    case DM_BASIC_TYPE:
    case DM_POINTER:
    case DM_ARRAY:
    case DM_FUNCTION:
    case DM_THIS_FUNCTION:
    case DM_UNMODIFIED_TYPE:
    case DM_BASED_ENCODING:
    case DM_MODIFIER_LIST:
    case DM_TEMPLATE_ARG:
    case DM_RECURSE_BEGIN:
    case DM_RECURSE_END:
    case DM_TEMPLATE_NAME:
    case DM_DESTRUCTOR:
    case DM_CONSTRUCTOR:
    case DM_OPERATOR_CONVERT:
    case DM_CTOR_DTOR_NAME:
    case DM_SCOPED_NAME:
    case DM_NAME:
    case DM_SCOPE:
    case DM_MANGLED_NAME:
    case DM_TRUNCATED_NAME:
        // ignore these for demangling
        break;
    case DM_TYPE_ENCODING:
        if( data->base_name ) {
            forceSuppression( data );
        }
        break;
    case DM_SET_INDEX:
        setEmitIndex( data, idx );
        break;
    case DM_RESET_INDEX:
        resetEmitIndex( data, idx );
        break;
    case DM_ZAP_SPACE:
        zapSpace( data );
        break;
    case DM_EMIT_SPACE:
        emitChar( data, ' ' );
        break;
    case DM_IDENTIFIER:
    case DM_COPY_STRING:
        if( data->base_name && data->ctdt_pending ) {
            unforceSuppression( data );
        }
        if( idx == 0 ) {
            emitStr( data, ptr );
        } else {
            while( idx-- > 0 ) {
                emitChar( data, *ptr++ );
            }
        }
        if( data->base_name && data->ctdt_pending ) {
            forceSuppression( data );
        }
        break;
    case DM_INTEGER:
        {
            char buff[12];
            ltoa( (long)value, buff, 10 );
            emitStr( data, buff );
        }
        break;
    case DM_ARRAY_SIZE:
        if( value != 0 ) {
            char buff[12];
            ultoa( (unsigned long)value, buff, 10 );
            emitStr( data, buff );
        }
        break;
    case DM_OPEN_PAREN:
        emitChar( data, openParen );
        break;
    case DM_CLOSE_PAREN:
        emitChar( data, closeParen );
        break;
    case DM_CHAR_ENCODING_SIGNED:
        emitStr( data, signedPrefix );
        break;
    case DM_ARRAY_PREFIX:
        emitStr( data, arrayPrefix );
        break;
    case DM_ARRAY_SUFFIX:
        emitStr( data, arraySuffix );
        break;
    case DM_FUNCTION_PREFIX:
        emitStr( data, functionPrefix );
        break;
    case DM_FUNCTION_SUFFIX:
        emitStr( data, functionSuffix );
        if( data->cv_pending ) {
            unforceSuppression( data );
        }
        break;
    case DM_FUNCTION_ARG_SEPARATOR:
        emitStr( data, functionSeparator );
        break;
    case DM_BASED_SUFFIX:
        emitStr( data, basedSuffix );
        break;
    case DM_BASED_STRING_PREFIX:
        emitStr( data, basedStrPrefix );
        break;
    case DM_BASED_SELF:
        emitStr( data, basedSelf );
        break;
    case DM_BASED_VOID:
        emitStr( data, basedVoid );
        break;
    case DM_BASED_STRING_SUFFIX:
        emitStr( data, basedStrSuffix );
        break;
    case DM_TEMPLATE_PREFIX:
        emitStr( data, templatePrefix );
        break;
    case DM_TEMPLATE_SUFFIX:
        emitStr( data, templateSuffix );
        break;
    case DM_TEMPLATE_ARG_SEPARATOR:
        emitStr( data, templateSeparator );
        break;
    case DM_ANONYMOUS_ENUM:
        emitStr( data, anonymousEnum );
        break;
    case DM_OPERATOR_PREFIX:
        emitStr( data, operatorName );
        break;
    case DM_OPERATOR_NEW:
        emitStr( data, newFunction );
        break;
    case DM_OPERATOR_NEW_ARRAY:
        emitStr( data, newArrayFunction );
        break;
    case DM_OPERATOR_DELETE:
        emitStr( data, deleteFunction );
        break;
    case DM_OPERATOR_DELETE_ARRAY:
        emitStr( data, deleteArrayFunction );
        break;
    case DM_DESTRUCTOR_CHAR:
        emitChar( data, dtorChar  );
        break;
    case DM_SCOPE_SEPARATOR:
        if( data->base_name ) {
            forceSuppression( data );
        }
        emitStr( data, scopeSeparator );
        break;
    case DM_CHAR_ENCODING:
        emitStr( data, translate_type_encoding[idx].string );
        break;
    case DM_WATCOM_OBJECT:
        emitStr( data, watcomObject[idx].name );
        break;
    case DM_OPERATOR_FUNCTION:
        emitStr( data, operatorFunction[idx] );
        break;
    case DM_RELATIONAL_FUNCTION:
        emitStr( data, relationalFunction[idx] );
        break;
    case DM_ASSIGNMENT_FUNCTION:
        emitStr( data, assignmentFunction[idx] );
        break;
    case DM_DECLSPEC_IMPORT:
        emitStr( data, "__declspec(dllimport) " );
        break;
    default:
#if 0 || defined( TEST )
        printf( "ERROR: demangleEmit unknown dm_pts(%d)\n", dp );
        ++errors;
#endif
        break;
    }
}

// ==========================================================================
// everything before this point deals with output to the demangled name
// ==========================================================================

static bool typeChar( char c, int grouping )
{
    int i;

    i = c - LOWER_TABLE_LIMIT;
    if( check_element( i, translate_type_encoding ) ) {
        return( translate_type_encoding[i].grouping == grouping );
    }
    return( false );
}

static int check_recurse( void ) {
#ifdef __NO_STACK_CHECKING__
    return( 1 );
#else
    return( alloca( RECURSE_CHECK ) != NULL );
#endif
}

static char prevChar( output_desc *data )
{
    char c;

    c = *(data->input - 1);
    return( mytoupper( c ) );
}

static char currChar( output_desc *data )
{
    char c;

    c = *data->input;
    if( data->input == data->end ) {
        c = NULL_CHAR;
    } else {
        c = mytoupper( c );
    }
    return( c );
}

static void advanceChar( output_desc *data )
{
    if( data->input != data->end && *data->input ) {
        data->input++;
    }
}

static bool is_identifier( char c )
{
    if( isalnum( c ) || c == UNDERSCORE || c == PERIOD ) {
        return( true );
    } else {
        return( false );
    }
}

static unsigned char_to_digit( char c )
{
    if( c >= '0' && c <= '9' ) return( c - '0' );
    if( c >= 'A' && c <= 'Z' ) return( c - 'A' + 10 );
    if( c >= 'a' && c <= 'z' ) return( c - 'a' + 10 );
    return( (unsigned)-1 );
}

static bool base_32_num( output_desc *data, long *value )
{
    unsigned    dig;
    long        v;
    bool        rc = false;

    v = 0;
    for( ; (dig = char_to_digit( currChar( data ) )) < 32; ) {
        v = v * 32 + dig;
        advanceChar( data );
        rc = true;
    }
    *value = v;
    return( rc );
}

static bool base_10_num( output_desc *data, size_t *value )
{
    unsigned    dig;
    size_t      v;
    bool        rc = false;

    v = 0;
    for( ; (dig = char_to_digit( currChar( data ) )) < 10; ) {
        v = v * 10 + dig;
        advanceChar( data );
        rc = true;
    }
    *value = v;
    return( rc );
}

static bool get_zz_len( output_desc *data, size_t *pvalue )
{
    size_t      value;
    unsigned    uvalue;

    if( (uvalue = char_to_digit( currChar( data ) )) < 36 ) {
        advanceChar( data );
        value = uvalue;
        if( (uvalue = char_to_digit( currChar( data ) )) < 36 ) {
            advanceChar( data );
#ifdef ZZ_LEN_3
            value = value * 36 + uvalue;
            if( (uvalue = char_to_digit( currChar( data ) )) < 36 ) {
                advanceChar( data );
#endif
                *pvalue = value * 36 + uvalue;
                return( true );
#ifdef ZZ_LEN_3
            }
#endif
        }
    }
    return( false );
}

static bool basic_type( output_desc *data, state_desc *state )
{
    char  c;

    _output1( DM_BASIC_TYPE );
    c = currChar( data );
    if( typeChar( c, CHAR_BASIC_TYPE ) ) {
        // put a signed before signed char
        if( c == 'C' && prevChar( data ) != 'U' ) {
            _output2( DM_SET_INDEX, state->prefix );
            _output1( DM_CHAR_ENCODING_SIGNED );
        }
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output2( DM_CHAR_ENCODING, c - LOWER_TABLE_LIMIT );
        return( true );
    }
    return( false );
}

static bool pointer( output_desc *data, state_desc *state )
{
    char  c;

    _output1( DM_POINTER );
    c = currChar( data );
    if( typeChar( c, CHAR_POINTER ) ) {
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output2( DM_CHAR_ENCODING, c - LOWER_TABLE_LIMIT );
        if( c == MEMBER_POINTER ) {
            if( currChar( data ) == TYPE_NAME_PREFIX ) {
                advanceChar( data );
                if( !scoped_name( data, state ) ) {
                    return( false );
                }
                if( currChar( data ) != TYPE_NAME_PREFIX ) {
                    return( false );
                }
                advanceChar( data );
                _output1( DM_ZAP_SPACE );
            }
        }
        return( true );
    }
    return( false );
}

static bool dimension( output_desc *data, state_desc *state )
{
    size_t value;

    _output2( DM_RESET_INDEX, state->suffix );
    _output1( DM_ARRAY_PREFIX );
    if( base_10_num( data, &value ) ) {
        _output2( DM_ARRAY_SIZE, value );
    }
    if( currChar( data ) == ARRAY_SUFFIX ) {
        advanceChar( data );
        _output1( DM_ARRAY_SUFFIX );
        return( true );
    }
    return( false );
}

static bool array( output_desc *data, state_desc *state )
{
    _output1( DM_ARRAY );
    _output2( DM_RESET_INDEX, state->suffix );
    _output1( DM_ZAP_SPACE );
    if( state->right ) {
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_OPEN_PAREN );
        _output2( DM_RESET_INDEX, state->suffix );
        _output1( DM_CLOSE_PAREN );
        state->right = false;
    }
    while( currChar( data ) == ARRAY_PREFIX ) {
        advanceChar( data );
        if( !dimension( data, state ) ) {
            return( false );
        }
    }
    return( type_encoding( data, state ) );
}

static bool function( output_desc *data, state_desc *state )
{
    auto state_desc new_state;
    size_t          conv_offset = 0;
    int             first_arg = 0;

    _output1( DM_FUNCTION );
    _output2( DM_RESET_INDEX, state->suffix );
    if( data->cv_pending ) {
        _output1( DM_EMIT_SPACE );
        conv_offset = data->index;
    } else {
        _output1( DM_ZAP_SPACE );
    }
    if( state->right ) {
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_OPEN_PAREN );
        _output2( DM_RESET_INDEX, state->suffix );
        _output1( DM_CLOSE_PAREN );
        state->right = false;
    }
    new_state = *state;
    _output1( DM_FUNCTION_PREFIX );
    setSuppression( data );
    while( currChar( data ) != FUNCTION_SUFFIX ) {
        if( first_arg ) {
            _output2( DM_RESET_INDEX, state->suffix );
            _output1( DM_ZAP_SPACE );
            _output1( DM_FUNCTION_ARG_SEPARATOR );
        }
        new_state.prefix = data->index;
        if( !type_encoding( data, &new_state ) ) {
            return( false );
        }
        first_arg = 1;
    }
    advanceChar( data );
    _output2( DM_RESET_INDEX, state->suffix );
    if( !first_arg ) {
        _output1( DM_ZAP_SPACE );
    }
    resetSuppression( data );
    _output1( DM_FUNCTION_SUFFIX );
    if( data->cv_pending ) {
        _output2( DM_SET_INDEX, conv_offset );
    } else {
        _output2( DM_SET_INDEX, state->prefix );
    }
    new_state.prefix = data->index;
    if( type_encoding( data, &new_state ) ) {
        data->cv_pending = false;
        return( true );
    } else {
        return( false );
    }
}

static bool tq_function( output_desc *data, state_desc *state )
{
    char c;
    char const *tq_ptr;
    size_t tq_len;

    _output1( DM_THIS_FUNCTION );
    tq_ptr = data->input;
    tq_len = 0;
    c = currChar( data );
    while( typeChar( c, CHAR_MODIFIER ) ) {
        // note: doesn't handle based modifiers
        tq_len++;
        advanceChar( data );
        c = currChar( data );
    }
    if( c == FUNCTION_PREFIX ) {
        advanceChar( data );
        if( function( data, state ) ) {
            _output2( DM_RESET_INDEX, state->suffix );
            if( tq_len > 0 ) {
                _output1( DM_EMIT_SPACE );
                while( tq_len-- ) {
                    c = *tq_ptr++;
                    _output2( DM_CHAR_ENCODING, mytoupper( c ) - LOWER_TABLE_LIMIT );
                }
                _output1( DM_ZAP_SPACE );
            }
            return( true );
        }
    }
    return( false );
}

static bool unmodified_type( output_desc *data, state_desc *state )
{
    char c;

    _output1( DM_UNMODIFIED_TYPE );
    c = currChar( data );
    if( c == TYPE_NAME_PREFIX ) {
        advanceChar( data );
        if( scoped_name( data, state ) ) {
            if( currChar( data ) == TYPE_NAME_SUFFIX ) {
                advanceChar( data );
                return( true );
            }
        }
    } else if( c == FUNCTION_PREFIX ) {
        advanceChar( data );
        return( function( data, state ) );
    } else if( c == THIS_QUAL_PREFIX ) {
        advanceChar( data );
        return( tq_function( data, state ) );
    } else if( c == ARRAY_PREFIX ) {
        return( array( data, state ) );
    } else if( typeChar( c, CHAR_POINTER ) ) {
        if( pointer( data, state ) ) {
            state->right = true;
            return( type_encoding( data, state ) );
        }
    } else {
        return( basic_type( data, state ) );
    }
    return( false );
}

static bool based_encoding( output_desc *data, state_desc *state )
{
    char   c;
    size_t len;

    _output1( DM_BASED_ENCODING );
    c = currChar( data );
    switch( c ) {
    case 'S':
        advanceChar( data );
        _output1( DM_BASED_SELF );
        _output1( DM_BASED_SUFFIX );
        return( true );
    case 'V':
        advanceChar( data );
        _output1( DM_BASED_VOID );
        _output1( DM_BASED_SUFFIX );
        return( true );
    case 'L':
        advanceChar( data );
        if( get_zz_len( data, &len ) ) {
            _output1( DM_BASED_STRING_PREFIX );
            _output3( DM_IDENTIFIER, len, data->input );
            while( len-- ) {
                advanceChar( data );
            }
            _output1( DM_BASED_STRING_SUFFIX );
            _output1( DM_BASED_SUFFIX );
            return( true );
        }
        break;
    case 'A':
    case 'F':
        advanceChar( data );
        if( currChar( data ) == TYPE_NAME_PREFIX ) {
            auto state_desc new_state;

            advanceChar( data );
            new_state = *state;
            new_state.prefix = data->index;
            new_state.suffix = data->count - data->index;
            if( scoped_name( data, &new_state ) ) {
                _output2( DM_RESET_INDEX, new_state.suffix );
                _output1( DM_ZAP_SPACE );
                _output1( DM_BASED_SUFFIX );
                return( true );
            }
        }
        break;
    }
    return( false );
}

static bool modifier_list( output_desc *data, state_desc *state )
{
    char c;

    _output1( DM_MODIFIER_LIST );
    c = currChar( data );
    while( typeChar( c, CHAR_MODIFIER ) ) {
        advanceChar( data );
        if( c == 'U' && currChar( data ) == 'A' ) {
            // don't emit unsigned before default char
        } else {
            _output2( DM_SET_INDEX, state->prefix );
            _output2( DM_CHAR_ENCODING, c - LOWER_TABLE_LIMIT );
        }
        if( c == 'J' ) {
            if( !based_encoding( data, state ) ) {
                return( false );
            }
        }
        c = currChar( data );
    }
    return( true );
}

static bool type_encoding( output_desc *data, state_desc *state )
{
    bool ret;
    char c;

    ret = false;
    _output1( DM_TYPE_ENCODING );
    c = currChar( data );
    if( typeChar( c, CHAR_MODIFIER ) ) {
        if( modifier_list( data, state ) ) {
            ret = unmodified_type( data, state );
        }
    } else {
        ret = unmodified_type( data, state );
    }
#if 0   // afs; this isn't the right place for this but we should output this
    if( ret && data->dllimport ) {
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_DECLSPEC_IMPORT );
        data->dllimport = 0;
    }
#endif
    return( ret );
}

static bool template_arg( output_desc *data, state_desc *state )
{
    char c;

    _output1( DM_TEMPLATE_ARG );
    c = currChar( data );
    if( c == TEMPLATE_INT ) {
        long value;
        advanceChar( data );
        if( base_32_num( data, &value ) ) {
            c = currChar( data );
            if( c == POSITIVE_INT ) {
                advanceChar( data );
                _output2( DM_SET_INDEX, state->prefix );
                _output2( DM_INTEGER, value );
                return( true );
            } else if( c == NEGATIVE_INT ) {
                advanceChar( data );
                _output2( DM_SET_INDEX, state->prefix );
                _output2( DM_INTEGER, -value );
                return( true );
            }
        }
    } else if( c == TEMPLATE_TYPE ) {
        advanceChar( data );
        return( type_encoding( data, state ) );
    } else if( c == PREFIX_EMBEDDED ) {
        advanceChar( data );
        if( check_recurse() ) {
            return( recursive_mangled_name( data, state ) );
        }
    }
    return( false );
}

static bool template_name( output_desc *data, state_desc *state )
{
    auto state_desc new_state;
    int             first_arg = 0;
    char            c;

    _output1( DM_TEMPLATE_NAME );
    new_state = *state;
    _output2( DM_RESET_INDEX, state->suffix );
    _output1( DM_TEMPLATE_PREFIX );
    c = currChar( data );
    while( c == TEMPLATE_INT || c == TEMPLATE_TYPE || c == PREFIX_EMBEDDED ) {
        if( first_arg ) {
            _output2( DM_RESET_INDEX, state->suffix );
            _output1( DM_ZAP_SPACE );
            _output1( DM_TEMPLATE_ARG_SEPARATOR );
        }
        new_state.prefix = data->index;
        if( !template_arg( data, &new_state ) ) {
            return( false );
        }
        first_arg = 1;
        c = currChar( data );
    }
    _output2( DM_RESET_INDEX, state->suffix );
    if( !first_arg ) {
        _output1( DM_ZAP_SPACE );
    }
    _output1( DM_TEMPLATE_SUFFIX );
    return( true );
}

static bool watcom_object( output_desc *data, state_desc *state )
{
    key_desc srch;
    int      i;
    size_t   len;
    char     c;

    c = currChar( data );
    if( c == 'A' ) {
        advanceChar( data );
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output2( DM_WATCOM_OBJECT, 0 );
    } else {
        srch.str[0] = c;
        advanceChar( data );
        srch.str[1] = currChar( data );
        advanceChar( data );
        for( i = 1 ; i < MAX_WATCOM_OBJECT ; i++ ) {
            if( srch.val == watcomObject[i].u.val ) {
                _output2( DM_SET_INDEX, state->prefix );
                _output2( DM_WATCOM_OBJECT, i );
                break;
            }
        }
        // check to make sure we found something
        if( i == MAX_WATCOM_OBJECT ) {
            _output2( DM_SET_INDEX, state->prefix );
            _output2( DM_WATCOM_OBJECT, 0 );
        }
    }
    if( get_zz_len( data, &len ) ) {
        while( len-- ) {
            advanceChar( data );
        }
        if( currChar( data ) == TYPE_NAME_SUFFIX ) {
            advanceChar( data );
            return( true );
        }
    }
    return( false );
}

static bool special_type_names( output_desc *data, state_desc *state )
{
    char c;

    c = currChar( data );
    if( c == 'E' ) {
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_ANONYMOUS_ENUM );
        for( ; (c = currChar( data )) != NULL_CHAR; ) {
            advanceChar( data );
            if( c == TYPE_NAME_SUFFIX ) {
                return( true );
            }
        }
    }
    return( false );
}

static bool operator_function( output_desc *data, state_desc *state )
{
    int i;

    i = currChar( data ) - LOWER_TABLE_LIMIT;
    if( check_element( i, operatorFunction ) ) {
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_OPERATOR_PREFIX );
        _output2( DM_OPERATOR_FUNCTION, i );
        return( true );
    }
    return( false );
}

static bool relational_function( output_desc *data, state_desc *state )
{
    int i;

    i = currChar( data ) - LOWER_TABLE_LIMIT;
    if( check_element( i, relationalFunction ) ) {
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_OPERATOR_PREFIX );
        _output2( DM_RELATIONAL_FUNCTION, i );
        return( true );
    }
    return( false );
}

static bool assignment_function( output_desc *data, state_desc *state )
{
    int i;

    i = currChar( data ) - LOWER_TABLE_LIMIT;
    if( check_element( i, assignmentFunction ) ) {
        advanceChar( data );
        _output2( DM_SET_INDEX, state->prefix );
        _output1( DM_OPERATOR_PREFIX );
        _output2( DM_ASSIGNMENT_FUNCTION, i );
        return( true );
    }
    return( false );
}

static bool op_new( output_desc *data, state_desc *state )
{
    _output2( DM_SET_INDEX, state->prefix );
    _output1( DM_OPERATOR_PREFIX );
    _output1( DM_OPERATOR_NEW );
    return( true );
}

static bool op_new_array( output_desc *data, state_desc *state )
{
    _output2( DM_SET_INDEX, state->prefix );
    _output1( DM_OPERATOR_PREFIX );
    _output1( DM_OPERATOR_NEW_ARRAY );
    return( true );
}

static bool dtor( output_desc *data, state_desc *state )
{
    _output2( DM_SET_INDEX, state->prefix );
    _output1( DM_DESTRUCTOR_CHAR );
    _output1( DM_DESTRUCTOR );
    data->pending_loc = data->count - data->index;
    data->ctdt_pending = true;
    return( true );
}

static bool op_delete( output_desc *data, state_desc *state )
{
    _output2( DM_SET_INDEX, state->prefix );
    _output1( DM_OPERATOR_PREFIX );
    _output1( DM_OPERATOR_DELETE );
    return( true );
}

static bool op_delete_array( output_desc *data, state_desc *state )
{
    _output2( DM_SET_INDEX, state->prefix );
    _output1( DM_OPERATOR_PREFIX );
    _output1( DM_OPERATOR_DELETE_ARRAY );
    return( true );
}

static bool ctor( output_desc *data, state_desc *state )
{
    state = state;
    _output1( DM_CONSTRUCTOR );
    data->pending_loc = data->count - data->index;
    data->ctdt_pending = true;
    return( true );
}

static bool user_conversion( output_desc *data, state_desc *state )
{
    _output2( DM_SET_INDEX, state->prefix );
    _output1( DM_OPERATOR_PREFIX );
    _output1( DM_OPERATOR_CONVERT );
    data->cv_pending = true;
    return( true );
}

static bool op_name( output_desc *data, state_desc *state )
{
    char c;

    c = currChar( data );
    switch( c ) {
    case GROUP_C_PREFIX:
        advanceChar( data );
        c = currChar( data );
        if( c == CTOR_PREFIX2 ) {
            advanceChar( data );
            return( ctor( data, state ) );
        } else if( c == OP_CONVERT_PREFIX2 ) {
            advanceChar( data );
            return( user_conversion( data, state ) );
        }
        break;
    case GROUP_D_PREFIX:
        advanceChar( data );
        c = currChar( data );
        if( c == DTOR_PREFIX2 ) {
            advanceChar( data );
            return( dtor( data, state ) );
        } else if( c == OP_DELETE_PREFIX2 ) {
            advanceChar( data );
            return( op_delete( data, state ) );
        } else if( c == ARRAY_DELETE_PREFIX2 ) {
            advanceChar( data );
            return( op_delete_array( data, state ) );
        }
        break;
    case GROUP_N_PREFIX:
        advanceChar( data );
        c = currChar( data );
        if( c == OP_NEW_PREFIX2 ) {
            advanceChar( data );
            return( op_new( data, state ) );
        } else if( c == ARRAY_NEW_PREFIX2 ) {
            advanceChar( data );
            return( op_new_array( data, state ) );
        }
        break;
    case ASGN_FUN_PREFIX:
        advanceChar( data );
        return( assignment_function( data, state ) );
        break;
    case REL_FUN_PREFIX:
        advanceChar( data );
        return( relational_function( data, state ) );
        break;
    case OP_FUN_PREFIX:
        advanceChar( data );
        return( operator_function( data, state ) );
        break;
    case SPEC_TYPE_PREFIX:
        advanceChar( data );
        return( special_type_names( data, state ) );
        break;
    case WAT_FUN_PREFIX:
        advanceChar( data );
        return( watcom_object( data, state ) );
        break;
    }
    return( false );
}

static bool name( output_desc *data, state_desc *state )
{
    char c;

    c = currChar( data );
    if( isdigit( c ) ) {
        size_t     len;
        char const *id;
        int        i;

        i = c - '0';
        // in case C++ compiler and demangler don't match; don't crash
        if( i < 0 || i >= next_replicate ) {
            id = "@";
            len = 1;
        } else {
            id = replicate[i].ptr;
            len = replicate[i].len;
        }
        if( data->ctdt_pending ) {
            _output2( DM_RESET_INDEX, data->pending_loc );
            _output1( DM_CTOR_DTOR_NAME );
            _output3( DM_IDENTIFIER, len, id );
            data->ctdt_pending = false;
        }
        if( data->scope_name && data->scope_index == 0 ) {
            data->scope_name = false;
            data->scope_ptr = id;
            data->scope_len = len;
        }
        _output2( DM_SET_INDEX, state->prefix );
        _output3( DM_IDENTIFIER, len, id );
        _output1( DM_EMIT_SPACE );
        advanceChar( data );
        return( true );
    } else {
        char const *ptr = data->input;
        size_t  len = 0;
        while( is_identifier( c ) ) {
            len++;
            advanceChar( data );
            c = currChar( data );
        }
        if( data->scope_name && data->scope_index == 0 ) {
            data->scope_name = false;
            data->scope_ptr = ptr;
            data->scope_len = len;
        }
        if( c == SYMBOL_SUFFIX ) {
            advanceChar( data );
            if( data->ctdt_pending ) {
                _output2( DM_RESET_INDEX, data->pending_loc );
                _output1( DM_CTOR_DTOR_NAME );
                _output3( DM_IDENTIFIER, len, ptr );
                data->ctdt_pending = false;
            }
            _output2( DM_SET_INDEX, state->prefix );
            _output3( DM_IDENTIFIER, len, ptr );
            _output1( DM_EMIT_SPACE );
            if( next_replicate < MAX_REPLICATE ) {
                replicate[next_replicate].ptr = ptr;
                replicate[next_replicate].len = len;
                next_replicate++;
            }
            return( true );
        }
    }
    return( false );
}

static bool sym_name( output_desc *data, state_desc *state )
{
    _output1( DM_NAME );
    if( currChar( data ) == OPNAME_PREFIX ) {
        advanceChar( data );
        return( op_name( data, state ) );
    } else {
        return( name( data, state ) ) ;
    }
}

static bool scope( output_desc *data, state_desc *state, size_t *symbol_length )
{
    char c;

    _output1( DM_SCOPE );
    if( data->scope_name && data->scope_index > 0 ) {
        data->scope_index--;
    }
    c = currChar( data );
    if( c == PREFIX_EMBEDDED ) {
        advanceChar( data );
        if( check_recurse() ) {
            return( recursive_mangled_name( data, state ) );
        }
    } else if( c == TEMPLATE_PREFIX ) {
        auto state_desc new_state;

        advanceChar( data );
        new_state = *state;
        new_state.suffix = data->count - state->prefix - *symbol_length;
        return( template_name( data, &new_state ) );
    } else {
        int rc = sym_name( data, state );
        *symbol_length = data->index - state->prefix - 1;
        return( rc );
    }
    return( false );
}

static bool scoped_name( output_desc *data, state_desc *state )
{
    size_t symbol_length;

    _output1( DM_SCOPED_NAME );
    if( sym_name( data, state ) ) {
        symbol_length = data->index - state->prefix - 1;
        while( currChar( data ) == SCOPE_PREFIX ) {
            advanceChar( data );
            _output2( DM_SET_INDEX, state->prefix );
            if( currChar( data ) != TEMPLATE_PREFIX ) {
                _output1( DM_SCOPE_SEPARATOR );
            }
            if( !scope( data, state, &symbol_length ) ) {
                return( false );
            }
            _output1( DM_ZAP_SPACE );
        }
        return( true );
    }
    return( false );
}

static bool mangled_name( output_desc *data )
{
    auto state_desc new_state;

    _output1( DM_MANGLED_NAME );
    new_state.prefix = 0;
    new_state.suffix = 0;
    new_state.right = false;
    if( scoped_name( data, &new_state ) ) {
        _output2( DM_RESET_INDEX, 0 );
        _output1( DM_ZAP_SPACE );
        return( type_encoding( data, &new_state ) );
    }
    return( false );
}

static bool full_mangled_name( output_desc *data )
{
    unsigned advances;

    advances = 1;
    if( ( data->end - data->input ) >= IMPORT_PREFIX_LEN ) {
        if( CHECK_IMPORT_PREFIX( data->input ) ) {
            data->input += IMPORT_PREFIX_LEN;
            data->dllimport = 1;
        }
    }
    switch( currChar( data ) ) {
    case TRUNCATED_PREFIX1:
        advances += TRUNCATED_HASH_LEN;
        _output1( DM_TRUNCATED_NAME );
        /* fall through */
    case MANGLE_PREFIX1:
        advanceChar( data );
        if( currChar( data ) == MANGLE_PREFIX2 ) {
            for( ; advances != 0; --advances ) {
                advanceChar( data );
                if( currChar( data ) == NULL_CHAR ) {
                    return( false );
                }
            }
            return( mangled_name( data ) );
        }
        break;
    }
    return( false );
}

static void do_demangle( output_desc *data )
{
#if 0 || defined( TEST )
    char const *input = data->input;

    if( !full_mangled_name( data ) ) {
        if( ! no_errors ) {
            ++errors;
            printf( "ERROR: full_mangled_name failed\n" );
            printf( "in:-->%s<--\n", input );
            printf( "remaining:-->%s<--\n", data->input );
            terminateOutput( data );
            printf( "output:-->%s<--\n", data->output );
        }
    } else {
        if( currChar( data ) != NULL_CHAR ) {
            ++errors;
            printf( "ERROR: full_mangled_name failed to consume all\n" );
            printf( "-->%s<--\n", input );
        }
    }
#else
    full_mangled_name( data );
#endif
}

static void do_copy( output_desc *data )
{
    char const  *ptr;
    size_t      len;
    char        c;

    ptr = data->input;
    len = 0;
    c = currChar( data );
    while( c != NULL_CHAR ) {
        advanceChar( data );
        len++;
        c = currChar( data );
    }
    _output3( DM_COPY_STRING, len, ptr );
}

static void init_globals( realloc_fn_t reallocator )
{
    next_replicate = 0;
    user_realloc = reallocator;
}

static void init_descriptor( output_desc *data,
                             outfunPtr ofn, void *cookie,
                             char const *input, size_t len,
                             char *buff, size_t buff_size )
{
    data->outfun = ofn;
    data->cookie = cookie;
    data->end = NULL;
    if( len != 0 ) {
        /* length of mangled name is known */
        data->end = input + len;
    }
    data->input = input;
    data->output = buff;
    data->size = buff_size;
    data->scope_ptr = NULL;
    data->count = 0;
    data->index = 0;
    data->pending_loc = 0;
    data->scope_len = 0;
    data->suppress_output = 1;
    data->scope_index = 0;
    data->ctdt_pending = 0;
    data->cv_pending = 0;
    data->scope_name = 0;
    data->base_name = 0;
    data->dllimport = 0;
}

static size_t terminateOutput( output_desc *data )
{
    size_t outlen;

    outlen = data->count;
    if( data->output != NULL && outlen > 0 && data->size > 0 ) {
        /* name may have been truncated */
        if( outlen > data->size - 1  ) {
            outlen = data->size - 1;
        }
        /* remove trailing space */
        if( outlen > 0 && data->output[outlen - 1] == ' ' )
            --outlen;
        data->output[outlen] = '\0';
    }
    /* size does not include '\0' */
    return( outlen );
}

static size_t demangle_recursive( char const *input, char *buff, size_t buff_size )
{
    realloc_fn_t save_reallocator;
    auto output_desc data;

    save_reallocator = user_realloc;
    user_realloc = NULL;
    init_descriptor( &data, &demangleEmit, &data, input, 0, buff, buff_size );
#if 0 || defined( TEST )
    if( !mangled_name( &data ) ) {
        ++errors;
        printf( "ERROR: mangled_name failed\n" );
        printf( "-->%s<--\n", input );
    }
#else
    mangled_name( &data );
#endif
    terminateOutput( &data );
    user_realloc = save_reallocator;
    return( data.input - input );
}

static bool recursive_mangled_name( output_desc *data, state_desc *state )
{
    auto char buff[AUTO_BUFFER_SIZE];

    _output1( DM_RECURSE_BEGIN );
    data->input += demangle_recursive( data->input, buff, sizeof( buff ) );
    _output1( DM_RECURSE_END );
    _output2( DM_SET_INDEX, state->prefix );
    _output3( DM_COPY_STRING, 0, buff );
    return( true );
}

size_t __demangle_l(                            // DEMANGLE A C++ NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char *buff,                                 // - output buffer for demangled C++ name
    size_t buff_size )                          // - size of output buffer
{
    int                 mangled;
    size_t              outlen;
    auto output_desc    data;

    init_globals( NULL );
    mangled = __is_mangled( input, len );
    init_descriptor( &data, &demangleEmit, &data, input, len, buff, buff_size );
    if( mangled ) {
        do_demangle( &data );
    } else {
        do_copy( &data );
    }
    outlen = terminateOutput( &data );
    /* size does not include '\0' */
    return( outlen );
}

int __is_mangled( char const *name, size_t len )
{
    int offset;

    len = len;
    offset = 2;
    if( CHECK_IMPORT_PREFIX( name ) ) {
        name += IMPORT_PREFIX_LEN;
        offset += IMPORT_PREFIX_LEN;
    }
    switch( name[0] ) {
    case TRUNCATED_PREFIX1:
        offset += TRUNCATED_HASH_LEN;
        /* fall through */
    case MANGLE_PREFIX1:
        if( name[1] == MANGLE_PREFIX2 ) {
            return( offset );
        }
        break;
    }
    return( 0 );
}

#if !defined( __WLIB__ ) && !defined( __DISASM__ )

static mangled_type checkInternal( char const *n )
{
    if( n[0] == '.' ) {
        return( __MANGLED_INTERNAL );
    }
    if( n[0] == OPNAME_PREFIX && mytoupper( n[1] ) == WAT_FUN_PREFIX ) {
        return( __MANGLED_INTERNAL );
    }
    if( n[0] == OPNAME_PREFIX && mytoupper( n[1] ) == CTOR_PREFIX1 && mytoupper( n[2] ) == CTOR_PREFIX2 ) {
        return( __MANGLED_CTOR );
    }
    if( n[0] == OPNAME_PREFIX && mytoupper( n[1] ) == DTOR_PREFIX1 && mytoupper( n[2] ) == DTOR_PREFIX2 ) {
        return( __MANGLED_DTOR );
    }
    return( __MANGLED );
}

mangled_type __is_mangled_internal( char const *name, size_t len )
{
    unsigned offset;
    len = len;

    offset = 2;
    switch( name[0] ) {
    case TRUNCATED_PREFIX1:
        offset += TRUNCATED_HASH_LEN;
        /* fall through */
    case MANGLE_PREFIX1:
        if( name[1] == MANGLE_PREFIX2 ) {
            return( checkInternal( name + offset ) );
        }
        break;
    }
    return( __NOT_MANGLED );
}

bool __unmangled_name(                          // FIND UNMANGLED BASE NAME
    char const *name,                           // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char const **basep,                         // - location of C++ base name
    size_t *base_sizep )                        // - size of C++ base name
{                                               // return true if name mangled
    char const *end;
    int   mangled;
    size_t base_size;

    if( len == 0 ) {
        len = strlen( name );
    }
    mangled = __is_mangled( name, len );
    if( !mangled ) {
        *basep = name;
        *base_sizep = len;
        return( false );
    }
    end = name + len;
    name += mangled; // skip magic cookie
    *basep = name;
    base_size = 0;
    if( *name == OPNAME_PREFIX ) {
        base_size += 3;
        name++;
        if( mytoupper( *name ) == WAT_FUN_PREFIX ) {
            base_size++;
        }
    } else {
        for( ; name != end ; ++name ) {
            if( *name == SYMBOL_SUFFIX )
                break;
            base_size++;
        }
    }
    *base_sizep = base_size;
    return( true );
}

#if !defined( __WLINK__ )

#if !defined( __DIP__ )

#if defined( __WATCOMC__ )
_WCRTLINK
#endif
size_t __demangle_t(                            // DEMANGLE A C++ TYPE
    char const *input,                          // - mangled C++ type
    size_t len,                                 // - length of mangled type
    char *buff,                                 // - output buffer for demangled C++ type
    size_t buff_size )                          // - size of output buffer
{
    size_t outlen;
    auto output_desc data;
    auto state_desc new_state;

    init_globals( NULL );
    init_descriptor( &data, &demangleEmit, &data, input, len, buff, buff_size );
    data.suppress_output = 0;
    new_state.prefix = 0;
    new_state.suffix = 0;
    new_state.right = false;
    type_encoding( &data, &new_state );
    outlen = terminateOutput( &data );
    /* size does not include '\0' */
    return( outlen );
}

size_t __demangle_r(                            // DEMANGLE A C++ NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char **buffp,                               // - output buffer for demangled C++ name
    size_t buff_size,                           // - size of output buffer
    void * (*realloc)( void *, size_t ) )       // - size adjuster for output
{
    char                *buff;
    int                 mangled;
    size_t              outlen;
    auto output_desc    data;

    init_globals( realloc );
    mangled = __is_mangled( input, len );
    buff = NULL;
    if( buffp != NULL ) {
        buff = *buffp;
    }
    init_descriptor( &data, &demangleEmit, &data, input, len, buff, buff_size );
    data.suppress_output = 0;
    if( mangled ) {
        do_demangle( &data );
    } else {
        do_copy( &data );
    }
    outlen = terminateOutput( &data );
    if( buffp != NULL ) {
        *buffp = data.output;
    }
    /* size does not include '\0' */
    return( outlen );
}

#endif // !__DIP__

bool __scope_name(                               // EXTRACT A C++ SCOPE
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    unsigned index,                             // - scope wanted
    char const **scopep,                        // - location of C++ scope name
    size_t *scope_sizep )                       // - size of C++ scope name
{                                               // returns true on success
    int                 mangled;
    auto output_desc    data;

    init_globals( NULL );
    *scopep = NULL;
    *scope_sizep = 0;
    mangled = __is_mangled( input, len );
    if( !mangled ) {
        return( false );
    }
    init_descriptor( &data, &demangleEmit, &data, input, len, NULL, 0 );
    data.scope_name = true;
    data.scope_index = index;
    do_demangle( &data );
    if( data.scope_len != 0 ) {
        *scopep = data.scope_ptr;
        *scope_sizep = data.scope_len;
        return( true );
    }
    return( false );
}

size_t __demangled_basename(                    // CREATE DEMANGLED BASE NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    char *buff,                                 // - output buffer for demangled C++ base name
    size_t buff_size )                          // - size of output buffer
{                                               // return len of output
    int                 mangled;
    size_t              outlen;
    auto output_desc    data;

    init_globals( NULL );
    mangled = __is_mangled( input, len );
    init_descriptor( &data, &demangleEmit, &data, input, len, buff, buff_size );
    data.suppress_output = 0;
    data.base_name = 1;
    if( mangled ) {
        do_demangle( &data );
    } else {
        do_copy( &data );
    }
    outlen = terminateOutput( &data );
    /* size does not include '\0' */
    return( outlen );
}

size_t __mangle_operator(                       // MANGLE OPERATOR NAME
    char const *op,                             // - operator token
    size_t len,                                 // - length of operator token
    char *buff )                                // - output buffer for mangled operator name
                                                // return len of mangled operator name
{
    int i;

    if( len == 0 ) {
        len = strlen( op );
    }

    for( i = 0 ; i < num_elements( operatorFunction ) ; i++ ) {
        if( STRNCMP( op, operatorFunction[i], len ) == 0 ) {
            if( operatorFunction[i][len] == NULL_CHAR ) {
                *buff++ = OPNAME_PREFIX;
                *buff++ = OP_FUN_PREFIX;
                *buff++ = (char)( i + LOWER_TABLE_LIMIT );
                return( 3 );
            }
        }
    }
    for( i = 0 ; i < num_elements( relationalFunction ) ; i++ ) {
        if( STRNCMP( op, relationalFunction[i], len ) == 0 ) {
            if( relationalFunction[i][len] == NULL_CHAR ) {
                *buff++ = OPNAME_PREFIX;
                *buff++ = REL_FUN_PREFIX;
                *buff++ = (char)( i + LOWER_TABLE_LIMIT );
                return( 3 );
            }
        }
    }
    for( i = 0 ; i < num_elements( assignmentFunction ) ; i++ ) {
        if( STRNCMP( op, assignmentFunction[i], len ) == 0 ) {
            if( assignmentFunction[i][len] == NULL_CHAR ) {
                *buff++ = OPNAME_PREFIX;
                *buff++ = ASGN_FUN_PREFIX;
                *buff++ = (char)( i + LOWER_TABLE_LIMIT );
                return( 3 );
            }
        }
    }
    return( 0 );
}

#endif // !__WLINK__

#endif // !__WLIB__ && !__DISASM__

#if defined( TEST )

void __parse_mangled_name(                      // PARSE MANGLED NAME
    char const *input,                          // - mangled C++ name
    size_t len,                                 // - length of mangled name
    void *cookie,                               // - data to carry around
    outfunPtr ofn )                             // - function to invoke
{
    int                 mangled;
    auto output_desc    data;

    init_globals( NULL );
    mangled = __is_mangled( input, len );
    init_descriptor( &data, ofn, cookie, input, len, NULL, 0 );
    data.suppress_output = 0;
    if( mangled ) {
        do_demangle( &data );
    } else {
        do_copy( &data );
    }
}

#endif // TEST

#if 0 || defined( TEST ) || defined( DUMP )
void dump( output_desc *data )
{
    int i;

    printf( "input:   [%x]>%s<\n", data->input, data->input );
    printf( "end:     [%x]\n", data->end );
    printf( "output:  [%x]>%s<\n", data->output, data->output );
    printf( "realloc: [%x]\n", user_realloc );
    printf( "size: %d, count: %d, index: %d, suppress_output: %d\n",
            data->size,
            data->count,
            data->index,
            data->suppress_output );
    for( i = 0 ; i < next_replicate ; i++ ) {
        printf( "replicate#%d: ptr: [%x]>%s< len: %d\n",
                i,
                replicate[i].ptr,
                replicate[i].ptr,
                replicate[i].len );
    }
    printf( "pending_loc: %d, ctdt_pending: %d, cv_pending: %d, base_name: %d\n",
            data->pending_loc,
            data->ctdt_pending,
            data->cv_pending,
            data->base_name );
    printf( "scope_name: %d, scope_index: %d, scope_ptr: %x, scope_len %d\n",
            data->scope_name,
            data->scope_index,
            data->scope_ptr,
            data->scope_len );
}

#endif

#if 0 || defined( TEST )
#define TRUNC_BUFFER    256
#define GUARD_CHAR      '@'
#define ALLOC_SIZE      2
#define BUF_SIZE        256

typedef struct test_stream {
    char *mangle;               // mangled name
    char *full_demangle;        // complete demangled name
    char *unmangled;            // unmangled name
    char *basename;             // demangled basename
} test_stream;

test_stream testVector[] = {
#if 1
    "W?foo$n(pn(uauaua)v)v",
        "void near foo( void (near *)( char, char, char ))",
        "foo",
        "foo",
    "W?a$ni",
        "int near a",
        "a",
        "a",
    "W?$CT:S$n()_",
        "near S::S()",
        "$CT",
        "S",
    "W?$DT:S$n()_",
        "near S::~S()",
        "$DT",
        "~S",
    "W?$DLn(pv)v",
        "void near operator delete( void * )",
        "$DL",
        "operator delete",
    "W?foo$n(iuipua)v",
        "void near foo( int, int unsigned, char * )",
        "foo",
        "foo",
    "W?$CV:opCONV$n()i",
        "near opCONV::operator int ()",
        "$CV",
        "operator int",
    "W?foo$n(pn$opCONV$$)pn$opCONV$$",
        "opCONV near * near foo( opCONV near * )",
        "foo",
        "foo",
    "W?foo$n(pn$opCONV$$)pn$1$",
        "opCONV near * near foo( opCONV near * )",
        "foo",
        "foo",
    "W?$NWn(ui)pnv",
        "void near * near operator new( int unsigned )",
        "$NW",
        "operator new",
    "W?bar$n($opnew$$)$1$",
        "opnew near bar( opnew )",
        "bar",
        "bar",
    "W?foo$n(pn[2][3][4]ipn[6]ua)v",
        "void near foo( int (near *)[2][3][4], char (near *)[6])",
        "foo",
        "foo",
    "W?bar$n(pn(uab)ipn(pn(d)it)v)v",
        "void near bar( int (near *)( char, float ), void (near *)( int (near *)( double ), long double ))",
        "bar",
        "bar",
    "W?pff$npn(db)ua",
        "char (near * near pff)( double, float )",
        "pff",
        "pff",
    "W?goo$nm$Base$$ni",
        "int near Base::* near goo",
        "goo",
        "goo",
    "W?goo$n(nm$Base$$ni)v",
        "void near goo( int near Base::* near )",
        "goo",
        "goo",
    "W?$Wvm07:H$$:E$$nx[]ui",
        "int unsigned const near __vmtbl[]",
        "$Wvm",
        "__vmtbl",
    "W?$Wvt0qset_v1:A$c$:A$set_v1$n(i)v$:V1$n(i)v",
        "void near V1::__vfthunk( int )",
        "$Wvt",
        "__vfthunk",
    "W?foo$:S$n(ua)i",
        "int near S::foo( char )",
        "foo",
        "foo",
    "W?foo$:S$n.x(ua)i",
        "int near S::foo( char ) const",
        "foo",
        "foo",
    "W?$CV:S$n()i",
        "near S::operator int ()",
        "$CV",
        "operator int",
    "W?$CV:S$n.x()i",
        "near S::operator int () const",
        "$CV",
        "operator int",
    "W?$WAA19ios:$opDTOR$$istream:c$$istream:$$opDTOR$?n()$n()pnv",
        "void near * near __internal()",
        "$WAA",
        "__internal",
    "W?_trmem_open$n(pn(ui)pnvpn(pnv)vpn(pnvui)pnvpn(pnvui)pnvpnvpn(pnvpnxuaui)vui)pn$_trmem_internal$$",
        "_trmem_internal near * near _trmem_open( void near * (near *)( int unsigned ), void (near *)( void near * ), void near * (near *)( void near "
        "*, int unsigned ), void near * (near *)( void near *, int unsigned ), void near *, void (near *)( void near *, char const near *, int unsigned ), int unsigned )",
        "_trmem_open",
        "_trmem_open",
    "W?s$n$Stack$::1ni0az?ok$n()v$",
        "Stack<int near,10,void near ok()> near s",
        "s",
        "s",
    "W?dummy$:Stack$::1ni0az?ok$n()vn()v",
        "void near Stack<int near,10,void near ok()>::dummy()",
        "dummy",
        "dummy",
    "W?$CT:Stack$::1ni0ay?ok$n()vn()_",
        "near Stack<int near,-10,void near ok()>::Stack()",
        "$CT",
        "Stack",
    "W?s$n$Inner$:Stack$::1ni0az?ok$n()v:xyz$$",
        "xyz::Stack<int near,10,void near ok()>::Inner near s",
        "s",
        "s",
    "W?s$n$Inner$:Stack$::1ni0az1n$xyz$::1ni$:abc$$",
        "abc::Stack<int near,10,xyz<int near > near >::Inner near s",
        "s",
        "s",
    "W?dummy$:Stack$::1ni0az?ok$n()v:xyz$n()v",
        "void near xyz::Stack<int near,10,void near ok()>::dummy()",
        "dummy",
        "dummy",
    "W?dummy$:Stack$::1ni0az?ok$n()v:xyz$::1nin()v",
        "void near xyz<int near >::Stack<int near,10,void near ok()>::dummy()",
        "dummy",
        "dummy",
    "W?a$:.1$:?foo$n()vn[]i",
        "int near void near foo()::.1::a[]",
        "a",
        "a",
    "W?cout$n$ostream$$",
        "ostream near cout",
        "cout",
        "cout",
    "W?$OB:ostream$n(pnxua)rn$ostream$$",
        "ostream near & near ostream::operator <<( char const near * )",
        "$OB",
        "operator <<",
    "W?endl$n(rn$ostream$$)rn$ostream$$",
        "ostream near & near endl( ostream near & )",
        "endl",
        "endl",
    "W?$OB:ostream$n(pn(rn$ostream$$)rn$ostream$$)rn$ostream$$",
        "ostream near & near ostream::operator <<( ostream near & (near *)( ostream near & ))",
        "$OB",
        "operator <<",
    "W?m$n()pjvv",
        "void __based(void) * near m()",
        "m",
        "m",
    "W?pv$npjvua",
        "char __based(void) * near pv",
        "pv",
        "pv",
    "W?ps$npjsua",
        "char __based(__self) * near ps",
        "ps",
        "ps",
    "W?pn$npjl03FOOuc",
        "char unsigned __based(\"FOO\") * near pn",
        "pn",
        "pn",
    "W?pa$npjf$s$uc",
        "char unsigned __based(s) * near pa",
        "pa",
        "pa",
    "W?pp$npja$b$ua",
        "char __based(b) * near pp",
        "pp",
        "pp",
    "W?bar$n(uaua)v",
        "void near bar( char, char )",
        "bar",
        "bar",
    "W?bar$n(cc)v",
        "void near bar( char signed, char signed )",
        "bar",
        "bar",
    "W?bar$n(ucuc)v",
        "void near bar( char unsigned, char unsigned )",
        "bar",
        "bar",
    "W?mpa$nm$S$$nxi",
        "int const near S::* near mpa",
        "mpa",
        "mpa",
    "W?mpb$nm$S$$nyi",
        "int volatile near S::* near mpb",
        "mpb",
        "mpb",
    "W?$Wmp05a$nxi$:S$n()pnxi",
        "int const near * near S::__mbrptrthunk()",
        "$Wmp",
        "__mbrptrthunk",
    "W?$Wmp05b$nyi$:S$n()pnyi",
        "int volatile near * near S::__mbrptrthunk()",
        "$Wmp",
        "__mbrptrthunk",
    "W?y$n$E$$",
        "E near y",
        "y",
        "y",
    "W?y$n$$TE$$",
        "__anonymous_enum near y",
        "y",
        "y",
    "W?setSymType$:KeySymbol$f($$TE$$)v",
        "void far KeySymbol::setSymType( __anonymous_enum )",
        "setSymType",
        "setSymType",
    "W?y$n$$TEextratext$$",
        "__anonymous_enum near y",
        "y",
        "y",
    "W?setSymType$:KeySymbol$f($$TEextra_stuff$$)v",
        "void far KeySymbol::setSymType( __anonymous_enum )",
        "setSymType",
        "setSymType",
    "W?foo$n(pn$_123456789012345678901234567890_1$$pn$_123456789012345678901234567890_2$$pn$_123456789012345678901234567890_3$$pn$_1234567890123456789"
    "01234567890_4$$pn$_123456789012345678901234567890_5$$pn$_123456789012345678901234567890_6$$pn$_123456789012345678901234567890_7$$)v",
        "void near foo( _123456789012345678901234567890_1 near *, _123456789012345678901234567890_2 near *, _123456789012345678901234567890_3 near *, "
        "_123456789012345678901234567890_4 near *, _123456789012345678901234567890_5 near *, _123456789012345678901234567890_6 near *, _123456789012345678901234567890_7 near * )",
        "foo",
        "foo",
    "W?$nan(ui)pnv",
        "void near * near operator new []( int unsigned )",
        "$na",
        "operator new []",
    "W?$dan(pnv)v",
        "void near operator delete []( void near * )",
        "$da",
        "operator delete []",
    "W?$Wcm012$nx[]uc",
        "char unsigned const near __stattabcmd[]",
        "$Wcm",
        "__stattabcmd",
    "W?$Wqq012$i",
        "int __internal",
        "$Wqq",
        "__internal",
    "W?$Wdi0vnql0f8@f__dlang_h_win_windows_h$na",
        "char near __debuginfo",
        "$Wdi",
        "__debuginfo",
    "W?e$nuz",
        "__int64 unsigned near e",
        "e",
        "e",
    "__imp_W?foo$n(iiapn(aas)i)i",
        "int near foo( int, int, char, int (near *)( char, char, short ))",
        "foo",
        "foo",
    "W?x$:$Wun0f1_dr061n1l8koys$:A$ni",
        "int near A::<unique>::x",
        "x",
        "x",
#endif
    "W?exprNodeAnalysis$:$Wun0e2o_eo9stv6l5hj$n(pn$ExprNode$$pnv)pn$1$",
        "ExprNode near * near <unique>::exprNodeAnalysis( ExprNode near *, void near * )",
        "exprNodeAnalysis",
        "exprNodeAnalysis",
#if 0 || defined( __INCLUDE_TRUNCATED_NAME )
    "T?hhhhfoo$n(pn$_123456789012345678901234567890_1$$pn$_123456789012345678901234567890_2$$pn$_123456789012345678901234567890_3$$pn$_123456789012345"
    "678901234567890_4$$pn$_123456789012345678901234567890_5$$pn$_123456789012345678901234567890_6$$pn$_12345678901",
        "near foo( _123456789012345678901234567890_1 near *, _123456789012345678901234567890_2 near *, _123456789012345678901234567890_3 near *, _1234"
        "56789012345678901234567890_4 near *, _123456789012345678901234567890_5 near *, _123456789012345678901234567890_6 near *, near * ",
        "foo",
        "foo",
    "T?hhhhxx$n$S$::?foo$n(pn$_123456789012345678901234567890_1$$pn$_123456789012345678901234567890_2$$pn$_123456789012345678901234567890_3$$pn$_12345"
    "6789012345678901234567890_4$$pn$_123456789012345678901234567890_5$$pn$_123456789012345678901234567890_6$$pn$_1",
        "near foo( _123456789012345678901234567890_1 near *, _12345678901234567890123456>::S near xx",
        "xx",
        "xx",
#endif
    NULL,
        NULL,
        NULL,
        NULL
};

void testEmit( void **cookie, dm_pts dp, size_t value, char const *ptr )
{
    int **argc = (int **)cookie;
    static const char *names[] = {
        #define DM_DEF( id )    "\tDM_" #id ": %d\n",
        DM_DEFS
        #undef DM_DEF
    };

    ptr = ptr;
    if( ((unsigned)dp) < DM_INVALID ) {
        if( **argc > 1 ) {
            printf( names[dp], value );
        }
#if 0 || defined( TEST )
    } else {
        ++errors;
        printf( "ERROR: testEmit unknown dm_pts(%d)\n", dp );
#endif
    }
}

static char *typeTestVector[] = {
    "$D$$", "D",
    "pn$D$$", "D near *",
    NULL, NULL,
};

void main( int argc )
{
    static char buff[TRUNC_BUFFER+2];   // allow for two guard chars
    size_t len;
    size_t trunc_len;
    int i;
    char *p;
    char const *q;

    printf( "To see all the results, invoke demangle.exe with a parameter\n" );
    printf( "When executing with a parameter, no page fault is success.\n" );
    printf( "Starting test...\n" );
    assert((( sizeof( typeTestVector ) / sizeof( typeTestVector[0] ) ) & 1 ) == 0 );
    errors = 0;
    for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
        len = __demangle_l( testVector[i].mangle, 0, NULL, 0 );
        if( argc > 1 ) {
            printf( "%s -> (%u)\n", testVector[i].mangle, len );
        }
    }
    for( trunc_len = TRUNC_BUFFER ; trunc_len > 0 ; trunc_len-- ) {
        for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
            buff[0] = buff[trunc_len+1] = GUARD_CHAR;
            len = __demangle_l( testVector[i].mangle, 0, buff+1, trunc_len );
            if( argc > 1 ) {
                printf( "%s -->%s<-- (%u)\n", testVector[i].mangle, buff+1, len );
            }
            if( len > trunc_len ) {
                ++errors;
                printf( "ERROR: returned length exceeds truncate length\n" );
            }
            if( buff[0] != GUARD_CHAR ) {
                printf( "ERROR: ran off start of output buffer\n" );
                ++errors;
            }
            if( buff[trunc_len+1] != GUARD_CHAR ) {
                printf( "ERROR: ran off end of output buffer\n" );
                ++errors;
            }
        }
        for( i = 0; typeTestVector[i] != NULL; i += 2 ) {
            buff[0] = buff[trunc_len+1] = GUARD_CHAR;
            len = __demangle_t( typeTestVector[i], 0, buff+1, trunc_len );
            if( argc > 1 ) {
                printf( "%s -->%s<-- (%u)\n", typeTestVector[i], buff+1, len );
            }
            if( memcmp( buff+1, typeTestVector[i+1], len ) ) {
                printf( "ERROR:\n%s\ndemangle should yield -->%s<--\n", typeTestVector[i], typeTestVector[i+1] );
                printf(             "          but yielded -->%s<-- (%u)\n", buff+1, len );
                ++errors;
            }
            if( len > trunc_len ) {
                ++errors;
                printf( "ERROR: returned length exceeds truncate length\n" );
            }
            if( buff[0] != GUARD_CHAR ) {
                printf( "ERROR: ran off start of output buffer\n" );
                ++errors;
            }
            if( buff[trunc_len+1] != GUARD_CHAR ) {
                printf( "ERROR: ran off end of output buffer\n" );
                ++errors;
            }
        }
    }
    if( errors ) {
        printf( "test failed!\n" );
        exit( 1 );
    }
    for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
        p = malloc( ALLOC_SIZE );
        len = __demangle_r( testVector[i].mangle, 0, &p, ALLOC_SIZE, realloc );
        if( argc > 1 ) {
            printf( "%s -->%s<-- (%u)\n", testVector[i].mangle, p, len );
        }
        free( p );
    }
    for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
        if( __unmangled_name( testVector[i].mangle, 0, &q, &len ) ) {
            char buff[BUF_SIZE];
            memcpy( buff, q, len );
            buff[len] = 0;
            if( argc > 1 ) {
                printf( "%s -->%s<-- (%u) (unmangled name)\n", testVector[i].mangle, buff, len );
            }
        } else {
            if( argc > 1 ) {
                printf( "%s (not mangled)\n", testVector[i].mangle );
            }
        }
    }
    for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
        if( argc > 1 ) {
            printf( "parsing -->%s<--\n", testVector[i].mangle );
        }
        __parse_mangled_name( testVector[i].mangle, 0, &argc, &testEmit );
    }
    for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
        p = malloc( ALLOC_SIZE );
        len = __demangle_r( testVector[i].mangle, 0, &p, ALLOC_SIZE, realloc );
        if( strcmp( p, testVector[i].full_demangle ) ) {
            printf( "ERROR:\n%s\ndemangle should yield -->%s<--\n", testVector[i].mangle, testVector[i].full_demangle );
            printf(             "          but yielded -->%s<-- (%u)\n", p, len );
            ++errors;
        }
        free( p );
        __unmangled_name( testVector[i].mangle, 0, &q, &len );
        if( strncmp( q, testVector[i].unmangled, len ) ) {
            char buff[BUF_SIZE];
            memcpy( buff, q, len );
            buff[len] = 0;
            printf( "%s\nunmangle should yield -->%s<--\n", testVector[i].mangle, testVector[i].unmangled );
            printf(     "          but yielded -->%s<-- (%u)\n", buff, len );
            ++errors;
        }
        p = malloc( BUF_SIZE );
        len = __demangled_basename( testVector[i].mangle, 0, p, BUF_SIZE );
        if( strncmp( p, testVector[i].basename, len ) ) {
            printf( "%s\nbasename should yield -->%s<--\n", testVector[i].mangle, testVector[i].basename );
            printf(     "          but yielded -->%s<-- (%u)\n", p, len );
            ++errors;
        }
        free( p );
    }
    if( errors ) {
        printf( "test failed!\n" );
        exit( 1 );
    }
    for( trunc_len = TRUNC_BUFFER ; trunc_len > 0 ; trunc_len-- ) {
        for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
            buff[0] = buff[trunc_len+1] = GUARD_CHAR;
            len = __demangle_l( testVector[i].mangle, 0, buff+1, trunc_len );
            if( len > trunc_len ) {
                printf( "ERROR: returned length exceeds truncate length\n" );
                ++errors;
            }
            if( buff[0] != GUARD_CHAR ) {
                printf( "ERROR: ran off start of output buffer\n" );
                ++errors;
            }
            if( buff[trunc_len+1] != GUARD_CHAR ) {
                printf( "ERROR: ran off end of output buffer\n" );
                ++errors;
            }
            if( errors ) {
                printf( "test failed!\n" );
                exit( 1 );
            }
        }
    }
    if( argc > 1 ) {
        no_errors = 1;
        // test truncated names
        for( i = 0 ; testVector[i].mangle != NULL ; ++i ) {
            char *p1;
            int j, max;
            max = strlen( testVector[i].mangle );
            p1 = malloc( max + 1 );
            memcpy( p1, testVector[i].mangle, max + 1 );
            for( j = max ; j > 0 ; j-- ) {
                p1[j] = '\0';
                p = malloc( ALLOC_SIZE );
                len = __demangle_r( p1, 0, &p, ALLOC_SIZE, realloc );
                printf( "truncated demangle yielded -->%s<-- (%u)\n", p, len );
                free( p );
            }
            free( p1 );
        }
    }
    printf( "...test completed.\n" );
    exit( 0 );
}
#endif

#if 0 || defined(UTIL)

#define BUF_SIZE 4096

void main( int argc, char **argv )
{
    char    buffer[BUF_SIZE];

    if( argc < 2) {
        printf( "Usage: demangle <mangled name>\n" );
        return;
    }
    __demangle_l( argv[1], strlen( argv[1] ), buffer, BUF_SIZE );
    puts( buffer );
}
#endif
