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
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "msdbg.h"
#include "pcobj.h"

#define MAX_LNAMES              (20)
#define MAX_SEGDEFS             (20)

#define FIRST_DEFINED_TYPE      (0x0200)

FILE *fp;

int obj_32;
int obj_metaware;

unsigned type_index = FIRST_DEFINED_TYPE;

unsigned curr_lname;
char *lnames[MAX_LNAMES+1];
unsigned curr_segdef;
char *segdefs[MAX_SEGDEFS+1];

char *record;
char *in_record;
unsigned record_len;
int dump_hex;

unsigned type_seg;
unsigned sym_seg;

typedef struct {
    uint_8      code;
    char        *name;
} xlat_t;

xlat_t translateType[] = {
{ MS_SL_BITFIELD,       "bitfield"      },
{ MS_SL_NEWTYPE,        "newtype"       },
{ MS_SL_STRING,         "string"        },
{ MS_SL_CONST,          "const"         },
{ MS_SL_LABEL,          "label"         },
{ MS_SL_PROCEDURE,      "procedure"     },
{ MS_SL_PARAMETER,      "parameter"     },
{ MS_SL_ARRAY,          "array"         },
{ MS_SL_STRUCTURE,      "structure"     },
{ MS_SL_POINTER,        "pointer"       },
{ MS_SL_SCALAR,         "scalar"        },
{ MS_SL_LIST,           "list"          },
{ MS_SL_BARRAY,         "barray"        },
{ MS_SL_FSTRING,        "fstring"       },
{ MS_SL_FARRIDX,        "farridx"       },
{ MS_SL_SKIP,           "skip"          },
{ MS_SL_BASED,          "based"         },
{ MS_BCL_NIL,           "nil"           },
{ MS_BCL_STRING,        "string"        },
{ MS_BCL_INDEX,         "index"         },
{ MS_BCL_UINT_16,       "uint_16"       },
{ MS_BCL_UINT_32,       "uint_32"       },
{ MS_BCL_INT_8,         "int_8"         },
{ MS_BCL_INT_16,        "int_16"        },
{ MS_BCL_INT_32,        "int_32"        },
{ MS_BCL_BASEDSEG,      "basedseg"      },
{ MS_BCL_BASEDVAL,      "basedval"      },
{ MS_BCL_BASEDSEGVAL,   "basedsegval"   },
{ MS_BCL_BASEDADDR,     "basedaddr"     },
{ MS_BCL_BASEDSEGADDR,  "basedsegaddr"  },
{ MS_BTL_TAG,           "tag"           },
{ MS_BTL_VARIANT,       "variant"       },
{ MS_BTL_BOOLEAN,       "bool"          },
{ MS_BTL_CHARACTER,     "char"          },
{ MS_BTL_INTEGER,       "int"           },
{ MS_BTL_UNSIGNED_INT,  "unsigned"      },
{ MS_BTL_SIGNED_INT,    "signed"        },
{ MS_BTL_REAL,          "real"          },
{ MS_OL_HUGE,           "huge"          },
{ MS_OL_C_NEAR,         "cdecl near"    },
{ MS_OL_C_FAR,          "cdecl far"     },
{ MS_OL_PACKED,         "packed"        },
{ MS_OL_UNPACKED,       "unpacked"      },
{ MS_OL_PLM_FAR,        "far"           },
{ MS_OL_PLM_NEAR,       "near"          },
{ MS_OL_FASTCALL_NEAR,  "fastcall near" },
{ MS_OL_FASTCALL_FAR,   "fastcall far"  },
{ MS_OL_INLINE,         "inline"        },
{ 0,                    NULL },
};

xlat_t translateSymbol[] = {
{ MS_SYM_BLOCK_START,   "block_start"   },
{ MS_SYM_PROCEDURE_START,"procedure_start"},
{ MS_SYM_END_RECORD,    "end_record"    },
{ MS_SYM_BP_RELATIVE,   "bp_relative"   },
{ MS_SYM_LOCAL_SYM,     "local_sym"     },
{ MS_SYM_CODE_LABEL,    "code_label"    },
{ MS_SYM_WITH_START,    "with_start"    },
{ MS_SYM_REGISTER_SYM,  "register_sym"  },
{ MS_SYM_CONSTANT_SYM,  "constant_sym"  },
{ MS_SYM_FORTRAN_ENTRY, "fortran_entry" },
{ MS_SYM_SKIP_RECORD,   "skip_record"   },
{ MS_SYM_CHANGE_DEF_SEG,"change_def_seg"},
{ MS_SYM_TYPEDEF_SYM,   "typedef_sym"   },
{ 0,                    NULL },
};

void fatal( char *p )
{
    puts( p );
    exit( EXIT_FAILURE );
}

int getb( void )
{
    int c;

    c = fgetc( fp );
    if( c == EOF ) fatal( "unexpected end of file encountered" );
    *in_record = c;
    ++in_record;
    return( c );
}

void *memalloc( size_t amt )
{
    void *p;

    p = malloc( amt );
    if( p == NULL ) fatal( "out of memory" );
    return( p );
}

void *memrealloc( void *p, size_t amt )
{
    p = realloc( p, amt );
    if( p == NULL ) fatal( "out of memory" );
    return( p );
}

void flushRec( unsigned len )
{
    int c;

    while( len != 0 ) {
        c = getb();
        --len;
    }
}

unsigned getw( void )
{
    int lo, hi;

    lo = getb();
    hi = getb();
    return( ( hi << 8 ) | lo );
}

unsigned long getl( void )
{
    unsigned lo, hi;

    lo = getw();
    hi = getw();
    return( (((unsigned long) hi) << 16 ) | lo );
}

unsigned geti( unsigned *len )
{
    int lo, hi;

    lo = getb();
    --*len;
    hi = 0;
    if( lo & 0x80 ) {
        --*len;
        hi = getb();
    }
    return( ( hi << 8 ) | lo );
}

void dumpCode( int x, xlat_t *tbl )
{
    do {
        if( x == tbl->code ) {
            printf( "%s ", tbl->name );
            return;
        }
        ++tbl;
    } while( tbl->name != NULL );
    printf( "?%02x ", x );
}

void dumpb( void )
{
    int c;

    c = getb();
    printf( "%02x ", c );
}

void dumpw( void )
{
    unsigned w;

    w = getw();
    printf( "%04x ", w );
}

void dumpl( void )
{
    unsigned long l;

    l = getl();
    printf( "%08lx ", l );
}

void dumpName( unsigned *len )
{
    int name_len;

    if( *len == 0 ) return;
    name_len = getb();
    --*len;
    putchar( '"' );
    while( name_len ) {
        putchar( getb() );
        --*len;
        --name_len;
    }
    putchar( '"' );
}

void dumpi( unsigned *len )
{
    unsigned index;

    index = geti( len );
    printf( "%u ", index );
}

void procCOMENT( unsigned len )
{
    int attrib;
    int comment_class;

    attrib = getb();
    comment_class = getb();
    switch( comment_class ) {
    case CMT_EASY_OMF:
    case CMT_MS_OMF:
        obj_32 = 1;
        break;
    }
    flushRec( len - 2 );
}

void procLNAMES( unsigned len )
{
    int name_len;
    char *p;

    while( len != 1 ) {
        name_len = getb();
        len -= name_len + 1;
        lnames[ curr_lname ] = memalloc( name_len + 1 );
        p = lnames[ curr_lname ];
        while( name_len ) {
            *p = getb();
            ++p;
            --name_len;
        }
        *p = '\0';
        ++curr_lname;
    }
    getb();     /* checksum */
}

void procSEGDEF( unsigned len )
{
    int acbp;
    unsigned seg_idx;

    acbp = getb();
    if(( acbp & 0xe0 ) == ( ALIGN_ABS << 5 )) {
        flushRec( len - 1 );
        return;
    }
    --len;
    if( obj_32 ) {
        getl();
        len -= 4;
    } else {
        getw();
        len -= 2;
    }
    seg_idx = geti( &len );
    segdefs[ curr_segdef ] = lnames[ seg_idx ];
    if( strcmp( lnames[ seg_idx ], "$$TYPES" ) == 0 ) {
        type_seg = curr_segdef;
    } else if( strcmp( lnames[ seg_idx ], "$$SYMBOLS" ) == 0 ) {
        sym_seg = curr_segdef;
    }
    ++curr_segdef;
    flushRec( len );
}

void dumpSpecial( unsigned t )
{
    if(( t & MS_RT_I_FIELD ) == 0 ) {
        printf( "special %02x ", t );
        return;
    }
    switch( t & MS_RT_TYP_FIELD ) {
    case MS_RT_TYP_SIGNED:
        printf( "int" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_00:
            printf( "_8 " );
            break;
        case MS_RT_SZ_01:
            printf( "_16 " );
            break;
        case MS_RT_SZ_10:
            printf( "_32 " );
            break;
        }
        break;
    case MS_RT_TYP_UNSIGNED:
        printf( "uint" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_00:
            printf( "_8 " );
            break;
        case MS_RT_SZ_01:
            printf( "_16 " );
            break;
        case MS_RT_SZ_10:
            printf( "_32 " );
            break;
        }
        break;
    case MS_RT_TYP_REAL:
        printf( "real" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_00:
            printf( "*4 " );
            break;
        case MS_RT_SZ_01:
            printf( "*8 " );
            break;
        case MS_RT_SZ_10:
            printf( "*10 " );
            break;
        }
        break;
    case MS_RT_TYP_COMPLEX:
        printf( "complex" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_00:
            printf( "*8 " );
            break;
        case MS_RT_SZ_01:
            printf( "*16 " );
            break;
        case MS_RT_SZ_10:
            printf( "*20 " );
            break;
        }
        break;
    case MS_RT_TYP_BOOLEAN:
        printf( "bool" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_00:
            printf( "_8 " );
            break;
        case MS_RT_SZ_01:
            printf( "_16 " );
            break;
        case MS_RT_SZ_10:
            printf( "_32 " );
            break;
        }
        break;
    case MS_RT_TYP_ASCII:
        printf( "ascii" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_00:
            printf( "_8 " );
            break;
        case MS_RT_SZ_01:
            printf( "_16 " );
            break;
        case MS_RT_SZ_10:
            printf( "_32 " );
            break;
        }
        break;
    case MS_RT_TYP_CURRENCY:
        printf( "currency" );
        switch( t & MS_RT_SZ_FIELD ) {
        case MS_RT_SZ_01:
            printf( "_8 " );
            break;
        }
        break;
    }
    switch( t & MS_RT_MD_FIELD ) {
    case MS_RT_MD_NEAR_PTR:
        printf( "near * " );
        break;
    case MS_RT_MD_FAR_PTR:
        printf( "far * " );
        break;
    case MS_RT_MD_HUGE_PTR:
        printf( "huge * " );
        break;
    case MS_RT_MD_DIRECT:
        break;
    }
}

void dumpLeaf( unsigned *len )
{
    uint_16 u2;
    uint_32 u4;
    int_8 s1;
    int_16 s2;
    int_32 s4;
    int leaf;
    unsigned slen;
    unsigned idx;

    leaf = getb();
    --*len;
    if( leaf < 0x80 ) {
        printf( "%u ", leaf );
        return;
    }
    switch( leaf ) {
    case MS_BCL_INT_8:
        s1 = getb();
        --*len;
        printf( "%d ", s1 );
        break;
    case MS_BCL_INT_16:
        s2 = getw();
        *len -= 2;
        printf( "%d ", s2 );
        break;
    case MS_BCL_INT_32:
        s4 = getl();
        *len -= 4;
        printf( "%ld ", s4 );
        break;
    case MS_BCL_UINT_16:
        u2 = getw();
        *len -= 2;
        printf( "%u ", u2 );
        break;
    case MS_BCL_UINT_32:
        u4 = getl();
        *len -= 4;
        printf( "%lu ", u4 );
        break;
    case MS_BCL_INDEX:
        if( obj_metaware ) {
            idx = getb();
            --*len;
            printf( "@%x ", idx + ( FIRST_DEFINED_TYPE - 1 ) );
            break;
        }
        idx = getw();
        *len -= 2;
        if( idx < FIRST_DEFINED_TYPE ) {
            dumpSpecial( idx );
            break;
        }
        printf( "@%x ", idx );
        break;
    case MS_BCL_STRING:
        slen = getb();
        --*len;
        putchar( '"' );
        while( slen ) {
            putchar( getb() );
            --*len;
            --slen;
        }
        putchar( '"' );
        putchar( ' ' );
        break;
    default:
        dumpCode( leaf, translateType );
    }
}

void dumpHexRec( void )
{
    char *p;

    if( dump_hex == 0 ) {
        return;
    }
    p = record;
    while( p != in_record ) {
        printf( "%02x ", *p );
        ++p;
    }
    putchar( '\n' );
    putchar( '\n' );
}

void dumpTypes( unsigned len )
{
    int linkage;
    unsigned tlen;
    int c;
    int lo, hi;
    int packed;
    int calling;

    for(;;) {
        if( len == 1 ) break;
        in_record = record;
        linkage = getb();
        lo = getb();
        if( linkage == 0 && lo == 0 ) {
            tlen = len - 3;
            len -= 2;
        } else {
            hi = getb();
            tlen = ( hi << 8 ) | lo;
            len -= 3;
        }
        printf( "%3x: %s (%u) ", type_index, linkage ? "T" : "F", tlen );
        ++type_index;
        len -= tlen;
        c = getb();
        --tlen;
        dumpCode( c, translateType );
        switch( c ) {
        case MS_SL_POINTER:
            dumpCode( getb(), translateType );
            --tlen;
            while( tlen ) {
                dumpLeaf( &tlen );
            }
            break;
        case MS_SL_SCALAR:
            dumpLeaf( &tlen );
            dumpSpecial( getb() );
            --tlen;
            while( tlen ) {
                dumpLeaf( &tlen );
            }
            break;
        case MS_SL_STRUCTURE:
            dumpLeaf( &tlen );
            dumpLeaf( &tlen );
            dumpLeaf( &tlen );
            dumpLeaf( &tlen );
            dumpLeaf( &tlen );
            packed = getb();
            --tlen;
            dumpCode( packed, translateType );
            flushRec( tlen );
            break;
        case MS_SL_PROCEDURE:
            dumpLeaf( &tlen );
            dumpLeaf( &tlen );
            calling = getb();
            --tlen;
            dumpCode( calling, translateType );
            dumpi( &tlen );
            dumpLeaf( &tlen );
            flushRec( tlen );
            break;
        case MS_SL_BITFIELD:
            putchar( ':' );
            dumpi( &tlen );
            dumpCode( getb(), translateType );
            dumpb();
            break;
        case MS_SL_ARRAY:
            while( tlen ) {
                dumpLeaf( &tlen );
            }
            break;
        case MS_SL_LIST:
            if( tlen ) {
                dumpLeaf( &tlen );
            }
            while( tlen ) {
                putchar( ',' );
                putchar( ' ' );
                dumpLeaf( &tlen );
            }
            break;
        default:
            flushRec( tlen );
        }
        putchar( '\n' );
        dumpHexRec();
    }
    getb();     /* checksum */
}

void dumpSymbols( unsigned len )
{
    unsigned slen;
    int c;

    for(;;) {
        if( len == 1 ) break;
        in_record = record;
        slen = getb();
        --len;
        printf( "(%u) ", slen );
        len -= slen;
        c = getb();
        --slen;
        if( c & MS_SYM_386_FLAG ) {
            obj_32 = 1;
        }
        c &= ~MS_SYM_386_FLAG;
        dumpCode( c, translateSymbol );
        switch( c ) {
        case MS_SYM_BP_RELATIVE:
            if( obj_32 ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            putchar( '@' );
            dumpw();
            slen -= 2;
            dumpName( &slen );
            break;
        case MS_SYM_LOCAL_SYM:
            if( obj_32 ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            dumpw();
            slen -= 2;
            putchar( '@' );
            dumpw();
            slen -= 2;
            dumpName( &slen );
            break;
        case MS_SYM_PROCEDURE_START:
            if( obj_32 ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            putchar( '@' );
            dumpw();
            slen -= 2;
            if( obj_metaware ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            dumpw();
            slen -= 2;
            if( obj_metaware ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            dumpw();
            slen -= 2;
            dumpb();
            dumpName( &slen );
            break;
        case MS_SYM_BLOCK_START:
            if( obj_32 ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            if( obj_metaware ) {
                dumpl();
                slen -= 4;
            } else {
                dumpw();
                slen -= 2;
            }
            dumpName( &slen );
            break;
        case MS_SYM_TYPEDEF_SYM:
            putchar( '@' );
            dumpw();
            slen -= 2;
            dumpName( &slen );
            break;
        case MS_SYM_CHANGE_DEF_SEG:
            dumpw();
            slen -= 2;
            dumpw();
            slen -= 2;
            break;
        default:
            flushRec( slen );
        }
        putchar( '\n' );
        dumpHexRec();
    }
    getb();     /* checksum */
}

void procLEDATA( unsigned len )
{
    unsigned seg_idx;

    seg_idx = geti( &len );
    if( seg_idx != type_seg && seg_idx != sym_seg ) {
        flushRec( len );
        return;
    }
    if( obj_32 ) {
        getl();
        len -= 4;
    } else {
        getw();
        len -= 2;
    }
    if( seg_idx == type_seg ) {
        dumpTypes( len );
    } else if( seg_idx == sym_seg ) {
        dumpSymbols( len );
    } else {
        flushRec( len );
    }
}

void main( int argc, char **argv )
{
    unsigned len;
    int rec;
    char *p;

    if( argc != 2 && argc != 3 ) {
        puts( "usage: MSDUMP <file> [-xm]" );
        puts( "-x     dump hex values of type/symbol records" );
        puts( "-m     use MetaWare CodeView interpretation" );
        puts( "options may be combined (i.e., -xm)" );
        exit( EXIT_FAILURE );
    }
    fp = fopen( argv[1], "rb" );
    if( fp == NULL ) {
        fatal( "could not open input file" );
    }
    dump_hex = 0;
    obj_32 = 0;
    obj_metaware = 0;
    if( argc == 3 ) {
        if( argv[2][0] == '-' || argv[2][0] == '/' ) {
            p = &argv[2][1];
            while( *p ) {
                switch( *p ) {
                case 'x':
                    dump_hex = 1;
                    break;
                case 'm':
                    obj_metaware = 1;
                    break;
                }
                ++p;
            }
        }
    }
    record_len = 256;
    record = memalloc( record_len );
    in_record = record;
    curr_lname = 1;
    curr_segdef = 1;
    for(;;) {
        rec = fgetc( fp );
        if( rec == EOF ) break;
        in_record = record;
        len = getw();
        if( len > record_len ) {
            record_len = (( len + 1 ) + 0x0f ) & ~0x0f;
            record = memrealloc( record, record_len );
            in_record = record;
        }
        switch( rec ) {
        case CMD_COMENT:
            procCOMENT( len );
            break;
        case CMD_LNAMES:
            procLNAMES( len );
            break;
        case CMD_SEGDEF:
        case CMD_SEGD32:
            procSEGDEF( len );
            break;
        case CMD_LEDATA:
        case CMD_LEDA32:
            procLEDATA( len );
            break;
        case CMD_TYPDEF:
            obj_metaware = 1;
            dumpTypes( len );
            break;
        default:
            flushRec( len );
        }
    }
    fclose( fp );
    exit( EXIT_SUCCESS );
}
