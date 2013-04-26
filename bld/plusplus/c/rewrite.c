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


#include "plusplus.h"

#include <stddef.h>

#include "errdefns.h"
#include "memmgr.h"
#include "preproc.h"
#include "carve.h"
#include "ptree.h"
#include "yydriver.h"
#include "rewrite.h"
#include "initdefs.h"
#include "pcheader.h"
#include "dbg.h"
#include "context.h"

/*
    The "rewrite file" consists of a string of tokens, each of which is
    preceded by one or more modifiers.

    The modifiers are decoded as extra token (T_REWRITE_MODS) and aditional data.

        byte:   b'00000000' -- next bytes are SRCFILE for source file

                b'10000000' -- next 4 bytes are fetched as signed value
                            -- if value > 0
                                    value is column #, fetch token
                               else
                                    (-value) is line #, column <-- 0

                b'1xxxxxxx' -- add 'xxxxxx' to line no., column <-- 0

                b'0xxxxxxx' -- add 'xxxxxx' to column, fetch token

    The sequence for a token is sumarized as:
        (a) optional file modifier
        (b) optional line modifier
        (c) column modifier
        (d) token

    Most tokens can be represented with their token number.  Some need
    more information to be saved like:

    T_STRING, T_LSTRING, T_ID, T_BAD_CHAR
        followed by bytes in token buffer ('\0' terminator)
    T_CONSTANT (integral)
        followed by <ConstType> <8-byte value> (least significant first)
    T_CONSTANT (floating point)
        followed by <ConstType> string of flt-pt number ('\0' terminator)
    T_BAD_TOKEN
        followed by contents of BadTokenInfo
*/

#define T_REWRITE_MODS         T_LAST_TOKEN

#define CODE_FILE 0x00              // code for SRCFILE
#define CODE_ABS  0x80              // code for line#, col#
#define MASK_ABS_LINE 0x80000000    // mask for absolute line
#define MASK_DELTA_LINE 0x80        // mask for delta line

#define REWRITE_EOT             (-1 << 8)

static REWRITE *currRewrite;
static TOKEN_LOCN *currLocn;

#define BLOCK_REWRITE           8
#define BLOCK_REWRITE_TOKENS    8
#define BLOCK_SRCFILE_HANDLE    8

#define _TokenPosGetIndex(s)    ((uint_8 *)(size_t)(s->stream - (uint_8 *)s->curr))
#define _TokenPosMapIndex(s)    ((uint_8 *)((uint_8 *)s->curr + (size_t)s->stream))

static carve_t carveREWRITE;
static carve_t carveREWRITE_TOKENS;
static carve_t carveSRCFILE_HANDLE;

static void rewriteInit( INITFINI* defn )
{
    defn = defn;
    currRewrite = NULL;
    carveREWRITE = CarveCreate( sizeof( REWRITE ), BLOCK_REWRITE );
    carveREWRITE_TOKENS = CarveCreate( sizeof( REWRITE_TOKENS ), BLOCK_REWRITE_TOKENS );
    carveSRCFILE_HANDLE = CarveCreate( sizeof( SRCFILE_HANDLE ), BLOCK_SRCFILE_HANDLE );
}

static void rewriteFini( INITFINI* defn )
{
    defn = defn;
    RewriteFree( currRewrite );
    currRewrite = NULL;
#ifndef NDEBUG
    CarveVerifyAllGone( carveREWRITE, "REWRITE" );
    CarveVerifyAllGone( carveREWRITE_TOKENS, "REWRITE_TOKENS" );
    CarveVerifyAllGone( carveSRCFILE_HANDLE, "SRCFILE_HANDLE" );
#endif
    CarveDestroy( carveREWRITE );
    CarveDestroy( carveREWRITE_TOKENS );
    CarveDestroy( carveSRCFILE_HANDLE );
}

INITDEFN( rewriting, rewriteInit, rewriteFini )

void RewriteFree( REWRITE *r )
/****************************/
{
    REWRITE_TOKENS *free_rt;
    REWRITE_TOKENS *rt;
    SRCFILE_HANDLE *h;
    SRCFILE_HANDLE *next_h;

    if( r == NULL ) {
        return;
    }
    if( ! r->alternate ) {
        for( rt = r->list; rt != NULL; ) {
            free_rt = rt;
            rt = rt->next;
            CarveFree( carveREWRITE_TOKENS, free_rt );
        }
        for( h = r->srcfiles_refd; h != NULL; h = next_h ) {
            next_h = h->next;
            CarveFree( carveSRCFILE_HANDLE, h );
        }
    }
    CarveFree( carveREWRITE, r );
}

static REWRITE_TOKENS *tokenAlloc( void )
{
    REWRITE_TOKENS *rt;

    rt = CarveAlloc( carveREWRITE_TOKENS );
    rt->next = NULL;
    rt->count = 0;
    rt->free = FALSE;
    return( rt );
}

static void putByte( REWRITE *r, uint_8 v )
{
    REWRITE_TOKENS *rt;

    rt = r->curr;
    if( rt->count == TOKEN_BLOCK_SIZE ) {
        REWRITE_TOKENS *new_rt;

        new_rt = tokenAlloc();
        rt->next = new_rt;
        r->curr = new_rt;
        rt = new_rt;
    }
    rt->stream[ rt->count++ ] = v;
}

static void putString( REWRITE *r, char *p )
{
    for( ; *p != '\0'; ++p ) {
        putByte( r, *p );
    }
    putByte( r, '\0' );
}

static void putBinary( REWRITE *r, uint_8 *bin, unsigned size )
{
    for( ; size > 0; --size, ++bin ) {
        putByte( r, *bin );
    }
}

static void putToken( REWRITE *r, TOKEN token )
{
    putBinary( r, (uint_8 *)&token, sizeof( token ) );
}

static uint_32 newSrcFileHandle( REWRITE *r, SRCFILE src_file )
{
    uint_32 index;
    SRCFILE_HANDLE *p;
    SRCFILE_HANDLE **head;
    SRCFILE_HANDLE *e;

    index = 0;
    head = &(r->srcfiles_refd);
    for( p = *head; p != NULL; p = *head ) {
        if( p->srcfile == src_file ) {
            return( index );
        }
        ++index;
        head = &(p->next);
    }
    e = CarveAlloc( carveSRCFILE_HANDLE );
    e->next = NULL;
    e->srcfile = src_file;
    *head = e;
    return( index );
}

static SRCFILE accessSrcFile( REWRITE *r, uint_32 srcfile_index )
{
    SRCFILE_HANDLE *p;

    for( p = r->srcfiles_refd; p != NULL; p = p->next ) {
        if( srcfile_index == 0 ) {
            return( p->srcfile );
        }
        --srcfile_index;
    }
    return( NULL );
}

static void putSrcFile( REWRITE *r, TOKEN_LOCN *locn )
{
    uint_32 srcfile_index;

    srcfile_index = newSrcFileHandle( r, locn->src_file );
    putToken( r, T_REWRITE_MODS );
    putByte( r, CODE_FILE );
    putBinary( r, (uint_8*)&srcfile_index, sizeof( srcfile_index ) );
    locn->line = 0;
    locn->column = 0;
}

static void putSrcLocn( REWRITE *r, TOKEN_LOCN *locn )
{
    SRCFILE currfile;
    uint_32 absolute;
    uint_8 code_byte;

    currfile = SrcFileCurrent();
    if( currfile != locn->src_file ) {
        locn->src_file = currfile;
        putSrcFile( r, locn );
    }
    putToken( r, T_REWRITE_MODS );
    if( ( TokenLine - locn->line ) > 127 ) {
        putByte( r, CODE_ABS );
        absolute = TokenLine | MASK_ABS_LINE;
        putBinary( r, (uint_8*)&absolute, sizeof( absolute ) );
        locn->line = TokenLine;
    } else if( ( TokenLine - locn->line ) > 0 ) {
        code_byte = ( TokenLine - locn->line ) | MASK_DELTA_LINE;
        putByte( r, code_byte );
        locn->line = TokenLine;
    }
    if( ( TokenColumn - locn->column ) > 127 ) {
        putByte( r, CODE_ABS );
        absolute = TokenColumn;
        putBinary( r, (uint_8*)&absolute, sizeof( absolute ) );
        locn->column = TokenColumn;
    } else if( ( TokenColumn - locn->column ) > 0 ) {
        code_byte = TokenColumn - locn->column;
        putByte( r, code_byte );
        locn->column = TokenColumn;
    } else {                        // this guards against token at same col.
        putByte( r, CODE_ABS );
        absolute = TokenColumn;
        putBinary( r, (uint_8*)&absolute, sizeof( absolute ) );
        locn->column = TokenColumn;
    }
}

static void saveToken( REWRITE *r, TOKEN_LOCN *locn )
{
    putSrcLocn( r, locn );
    switch( CurToken ) {
    case T_STRING:
    case T_LSTRING:
    case T_ID:
    case T_BAD_CHAR:
        putToken( r, CurToken );
        putString( r, Buffer );
        break;
    case T_BAD_TOKEN:
        putToken( r, CurToken );
        putBinary( r, (uint_8*)&BadTokenInfo, sizeof( BadTokenInfo ) );
        break;
    case T_SAVED_ID:
        putToken( r, T_ID );
        putString( r, NameStr( SavedId ) );
        break;
    case T_CONSTANT:
        putToken( r, CurToken );
        putByte( r, ConstType );
        switch( ConstType ) {
        case TYP_LONG_DOUBLE:
        case TYP_DOUBLE:
        case TYP_FLOAT:
            putString( r, Buffer );
            break;
        default:
            putBinary( r, (uint_8*)&Constant64, sizeof( Constant64 ) );
            break;
        }
        break;
    default:
        putToken( r, CurToken );
        break;
    }
}

static REWRITE *newREWRITE( TOKEN end_token, TOKEN_LOCN *locn )
{
    REWRITE *r;
    REWRITE_TOKENS *rt;

    r = CarveAlloc( carveREWRITE );
    rt = tokenAlloc();
    r->list = rt;
    r->curr = rt;
    r->srcfiles_refd = NULL;
    r->stream = rt->stream;
    r->last_token = end_token;
    r->busy = FALSE;
    r->free = FALSE;
    r->alternate = FALSE;
    locn->src_file = SrcFileCurrent();
    putSrcFile( r, locn );
    return( r );
}

static REWRITE *dupREWRITE( REWRITE *old_r )
{
    REWRITE *r;

    r = CarveAlloc( carveREWRITE );
    r->list = old_r->list;
    r->curr = old_r->curr;
    r->srcfiles_refd = old_r->srcfiles_refd;
    r->stream = old_r->stream;
    r->last_token = old_r->last_token;
    r->busy = FALSE;
    r->alternate = TRUE;
    r->free = FALSE;
    return( r );
}

static PTREE recoverToken( PTREE tree )
{
    switch( tree->op ) {
    case PT_INT_CONSTANT:
    case PT_FLOATING_CONSTANT:
        /* these cases are not possible in correct C++ programs */
        /* Buffer and ConstType should still be set */
        CurToken = T_CONSTANT;
        break;
    case PT_ID:
        CurToken = T_SAVED_ID;
        SavedId = tree->u.id.name;
        break;
    case PT_UNARY:
    case PT_BINARY:
        switch( tree->cgop ) {
        case CO_INDIRECT:
            CurToken = T_TIMES;
            break;
        case CO_COLON_COLON:
            CurToken = T_COLON_COLON;
            break;
        case CO_OPERATOR:
            CurToken = T_OPERATOR;
            break;
        case CO_TILDE:
            CurToken = T_TILDE;
            break;
        case CO_NEW:
            CurToken = T_NEW;
            break;
        case CO_DELETE:
            CurToken = T_DELETE;
            break;
        case CO_STORAGE:
            return( tree );
        default:
#ifndef NDEBUG
            CFatal( "recoverToken: unknown tree cgop" );
#endif
            return( tree );
        }
        break;
    default:
#ifndef NDEBUG
        CFatal( "recoverToken: unknown tree cgop" );
#endif
        return( tree );
    }
    saveToken( currRewrite, currLocn );
    return( tree );
}

static void captureMulti( REWRITE *r, PTREE multi, TOKEN_LOCN *locn )
{
    TOKEN save_CurToken;
    NAME save_SavedId;
    REWRITE *save_rewrite;
    TOKEN_LOCN *save_locn;

    if( multi == NULL ) {
        return;
    }
    save_CurToken = CurToken;
    save_SavedId = SavedId;
    save_rewrite = currRewrite;
    save_locn = currLocn;
    currLocn = locn;
    currRewrite = r;
    multi = PTreeTraversePostfix( multi, recoverToken );
    PTreeFreeSubtrees( multi );
    currRewrite = save_rewrite;
    CurToken = save_CurToken;
    currLocn = save_locn;
    SavedId = save_SavedId;
    NextToken();
}

REWRITE *RewritePackageFunction( PTREE multi )
/********************************************/
{
    ppctl_t old_ppctl;
    boolean skip_first;
    REWRITE *r;
    unsigned depth;
    unsigned asm_depth;
    TOKEN_LOCN locn;
    TOKEN_LOCN *plocn;

    skip_first = FALSE;
    r = ParseGetRecordingInProgress( &plocn );
    if( r == NULL ) {
        plocn = &locn;
        r = newREWRITE( T_RIGHT_BRACE, &locn );
    } else {
        skip_first = TRUE;
    }
    captureMulti( r, multi, plocn );
    old_ppctl = PPControl;
    asm_depth = 0;
    depth = 1;          /* we've seen one '{' */
    while( CurToken != T_EOF ) {
        DbgAssert( depth != 0 );
        switch( CurToken ) {
        case T_NULL:
#ifndef NDEBUG
            DbgAssert( asm_depth != 0 );
#endif
            PPCTL_DISABLE_EOL();
            if( depth == asm_depth ) {
                PPCTL_DISABLE_ASM();
                asm_depth = 0;
            }
            break;
        case T___ASM:
            if( asm_depth == 0 ) {
                PPCTL_ENABLE_EOL();
                PPCTL_ENABLE_ASM();
                asm_depth = depth;
            }
            break;
        case T_SEMI_COLON:
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            ++depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            --depth;
            if( depth == asm_depth ) {
                PPCTL_DISABLE_EOL();
                PPCTL_DISABLE_ASM();
                asm_depth = 0;
            }
            break;
        default:
            break;
        }
        if( ! skip_first ) {
            saveToken( r, plocn );
        }
        skip_first = FALSE;
        if( depth == 0 )
            break;
        NextToken();
        if( PPControl & PPCTL_ASM ) {
            PPCTL_ENABLE_EOL();
        }
    }
    PPControl = old_ppctl;
    return( r );
}

static REWRITE *memInitError( REWRITE *r, TOKEN_LOCN *locn )
{
    CErr1( ERR_MEM_INIT_REWRITE_ERROR );
    CErr2p( INF_CTOR_INIT_STARTED_HERE, locn );
    RewriteFree( r );
    return( NULL );
}

REWRITE *RewritePackageMemInit( PTREE multi )
/*******************************************/
{
    REWRITE *r;
    unsigned paren_depth;
    unsigned brace_depth;
    TOKEN_LOCN locn;
    TOKEN_LOCN start_locn;

    SrcFileGetTokenLocn( &start_locn );
    r = newREWRITE( T_RIGHT_BRACE, &locn );
    captureMulti( r, multi, &locn );
    brace_depth = 0;
    paren_depth = 0;
    for(;;) {
        if( CurToken == T_EOF ) {
            memInitError( r, &start_locn );
            break;
        }
        saveToken( r, &locn );
        switch( CurToken ) {
        case T_LEFT_PAREN:
            ++paren_depth;
            break;
        case T_RIGHT_PAREN:
            if( paren_depth == 0 ) {
                return( memInitError( r, &start_locn ) );
            }
            --paren_depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                return( memInitError( r, &start_locn ) );
            }
            --brace_depth;
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            if( paren_depth == 0 ) {
                return( r );
            }
            ++brace_depth;
            break;
        default:
            break;
        }
        NextToken();
    }
    return( NULL );
}

static REWRITE *defArgError( REWRITE *r, TOKEN_LOCN *locn )
{
    CErr1( ERR_DEF_ARG_REWRITE_ERROR );
    CErr2p( INF_DEF_ARG_STARTED_HERE, locn );
    RewriteFree( r );
    return( NULL );
}

REWRITE *RewritePackageDefArg( PTREE multi )
/******************************************/
{
    REWRITE *r;
    unsigned paren_depth;
    unsigned brace_depth;
    TOKEN_LOCN locn;
    TOKEN_LOCN start_locn;

    DbgAssert( CurToken == T_EQUAL );
    NextToken();
    SrcFileGetTokenLocn( &start_locn );
    r = newREWRITE( T_DEFARG_END, &locn );
    captureMulti( r, multi, &locn );
    brace_depth = 0;
    paren_depth = 0;
    for(;;) {
        if( CurToken == T_EOF ) {
            defArgError( r, &start_locn );
            break;
        }
        switch( CurToken ) {
        case T_LEFT_PAREN:
            ++paren_depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            --brace_depth;
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            if( paren_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            ++brace_depth;
            break;
        case T_RIGHT_PAREN :
            if( paren_depth == 0 ) {
                if( brace_depth != 0 ) {
                    return( defArgError( r, &start_locn ) );
                }
                UndoNextToken();
                return( r );
            }
            --paren_depth;
            break;
        case T_COMMA :
        case T_DOT_DOT_DOT:
            if( brace_depth == 0 && paren_depth == 0 ) {
                UndoNextToken();
                return( r );
            }
            break;
        default:
            break;
        }
        saveToken( r, &locn );
        NextToken();
    }
    return( NULL );
}

REWRITE *RewritePackageTemplateArgument( void )
/******************************************/
{
    REWRITE *r;
    unsigned angle_depth;
    unsigned brace_depth;
    unsigned bracket_depth;
    unsigned paren_depth;
    TOKEN_LOCN locn;
    TOKEN_LOCN start_locn;

    SrcFileGetTokenLocn( &start_locn );
    r = newREWRITE( T_DEFARG_END, &locn );
    angle_depth = brace_depth = bracket_depth = paren_depth = 0;
    for(;;) {
        if( CurToken == T_EOF ) {
            defArgError( r, &start_locn );
            break;
        }
        switch( CurToken ) {
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            ++brace_depth;
            break;
        case T_LEFT_BRACKET:
        case T_ALT_LEFT_BRACKET:
            ++bracket_depth;
            break;
        case T_LEFT_PAREN:
            ++paren_depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            --brace_depth;
            break;
        case T_RIGHT_BRACKET:
        case T_ALT_RIGHT_BRACKET:
            if( bracket_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            --bracket_depth;
            break;
        case T_RIGHT_PAREN:
            if( paren_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            --paren_depth;
            break;
        case T_LT:
            /* for non-type template arguments this should always be
             * parsed as less-than */
            if( ( brace_depth == 0 ) && ( bracket_depth == 0 )
             && ( paren_depth == 0 ) ) {
                ++angle_depth;
            }
            break;
        case T_COMMA:
        case T_GT:
            if( ( brace_depth == 0 ) && ( bracket_depth == 0 )
             && ( paren_depth == 0 ) ) {
                if( angle_depth == 0 ) {
                    return( r );
                }
                else if( CurToken == T_GT ) {
                    --angle_depth;
                }
            }
            break;
        case T_RSHIFT:
            // see Right Angle Brackets (N1757/05-0017)
            if( CompFlags.enable_std0x
             && ( brace_depth == 0 ) && ( bracket_depth == 0 )
             && ( paren_depth == 0 ) ) {
                if( angle_depth == 1 ) {
                    CurToken = T_GT;
                    saveToken( r, &locn );
                    return( r );
                } else if( angle_depth == 0 ) {
                    return( r );
                } else {
                    angle_depth -= 2;
                }
            }
            break;
        default:
            break;
        }
        saveToken( r, &locn );
        NextToken();
    }
    return( NULL );
}

REWRITE *RewritePackageTemplateDefArg( void )
/******************************************/
{
    REWRITE *r;

    if( CurToken != T_EQUAL ) {
        return( NULL );
    }
    NextToken();
    r = RewritePackageTemplateArgument();
    UndoNextToken();
    return( r );
}

REWRITE *RewritePackagePassThrough( REWRITE *r )
/****************************************/
{
    REWRITE *dummy;
    unsigned paren_depth;
    unsigned brace_depth;
    TOKEN_LOCN locn;
    TOKEN_LOCN start_locn;

    DbgAssert( CurToken == T_EQUAL );
    NextToken();
    SrcFileGetTokenLocn( &start_locn );
    dummy = newREWRITE( T_DEFARG_END, &locn );
    brace_depth = 0;
    paren_depth = 0;
    for(;;) {
        if( CurToken == T_EOF ) {
            defArgError( r, &start_locn );
            break;
        }
        switch( CurToken ) {
        case T_LEFT_PAREN:
            ++paren_depth;
            break;
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            --brace_depth;
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            if( paren_depth == 0 ) {
                return( defArgError( r, &start_locn ) );
            }
            ++brace_depth;
            break;
        case T_RIGHT_PAREN :
            if( paren_depth == 0 ) {
                if( brace_depth != 0 ) {
                    return( defArgError( r, &start_locn ) );
                }
                UndoNextToken();
                return( dummy );
            }
            --paren_depth;
            break;
        case T_COMMA :
        case T_DOT_DOT_DOT:
            if( brace_depth == 0 && paren_depth == 0 ) {
                UndoNextToken();
                return( dummy );
            }
            break;
        default:
            break;
        }
        saveToken( r, &locn );
        NextToken();
    }
    return( NULL );
}

static REWRITE *templateError( REWRITE *r, TOKEN_LOCN *locn )
{
    CErr1( ERR_CLASS_TEMPLATE_REWRITE_ERROR );
    CErr2p( INF_CLASS_TEMPLATE_STARTED_HERE, locn );
    RewriteFree( r );
    return( NULL );
}

REWRITE *RewritePackageClassTemplate( REWRITE *r, TOKEN_LOCN *locn )
/******************************************************************/
{
    unsigned brace_depth;
    boolean first_time;
    TOKEN_LOCN start_locn;

    SrcFileGetTokenLocn( &start_locn );
    brace_depth = 0;
    first_time = TRUE;
    for(;;) {
        if( CurToken == T_EOF ) {
            templateError( r, &start_locn );
            break;
        }
        if( ! first_time ) {
            /* already been saved by YYDRIVER */
            saveToken( r, locn );
        }
        first_time = FALSE;
        switch( CurToken ) {
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                return( templateError( r, &start_locn ) );
            }
            --brace_depth;
            if( brace_depth == 0 ) {
                NextToken();
                if( !ExpectingToken( T_SEMI_COLON ) ) {
                    return( templateError( r, &start_locn ) );
                }
                return( r );
            }
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            ++brace_depth;
            break;
        default:
            break;
        }
        NextToken();
    }
    return( NULL );
}

REWRITE *RewritePackageClassTemplateMember( REWRITE *r, TOKEN_LOCN *locn )
/************************************************************************/
{
    unsigned brace_depth;
    boolean first_time;
    TOKEN_LOCN start_locn;

    SrcFileGetTokenLocn( &start_locn );
    brace_depth = 0;
    first_time = TRUE;
    for(;;) {
        if( CurToken == T_EOF ) {
            templateError( r, &start_locn );
            break;
        }
        if( ! first_time ) {
            /* already been saved by YYDRIVER */
            saveToken( r, locn );
        }
        first_time = FALSE;
        switch( CurToken ) {
        case T_RIGHT_BRACE:
        case T_ALT_RIGHT_BRACE:
            if( brace_depth == 0 ) {
                return( templateError( r, &start_locn ) );
            }
            --brace_depth;
            if( brace_depth == 0 ) {
                NextToken();
                if( CurToken == T_SEMI_COLON ) {
                    saveToken( r, locn );
                    NextToken();
                }
                return( r );
            }
            break;
        case T_LEFT_BRACE:
        case T_ALT_LEFT_BRACE:
            ++brace_depth;
            break;
        case T_SEMI_COLON:
            if( brace_depth == 0 ) {
                NextToken();
                return( r );
            }
            break;
        default:
            break;
        }
        NextToken();
    }
    return( NULL );
}

REWRITE *RewritePackageToken( void )
/**********************************/
{
    REWRITE *r;
    TOKEN_LOCN locn;

    r = newREWRITE( T_EOF, &locn );
    if( CurToken != T_EOF ) {
        if( CurToken == T_SAVED_ID ) {
            saveToken( r, &locn );
            NextToken();
        }
        saveToken( r, &locn );
    }
    return( r );
}

void RewriteRestoreToken( REWRITE *r )
/************************************/
{
    REWRITE *last_rewrite;
    void (*last_source)( void );

    last_source = SetTokenSource( RewriteToken );
    last_rewrite = RewriteRewind( r );
    if( CurToken == T_ID ) {
        LookPastName();
        if( LAToken == T_EOF ) {
            CurToken = T_ID;
        }
    }
    RewriteClose( last_rewrite );
    ResetTokenSource( last_source );
    RewriteFree( r );
}

REWRITE *RewriteRewind( REWRITE *r )
/**********************************/
{
    REWRITE *last_rewrite;

    last_rewrite = currRewrite;
    if( r->busy ) {
        r = dupREWRITE( r );
    }
    currRewrite = r;
    r->busy = TRUE;
    r->curr = r->list;
    r->stream = r->curr->stream;
    RewriteToken();
    return( last_rewrite );
}

static int getByte( REWRITE *r, REWRITE_TOKENS **rt, uint_8 **stop )
{
    REWRITE_TOKENS *my_rt;

    if( r->stream == *stop ) {
        my_rt = (*rt)->next;
        if( my_rt == NULL ) {
            return( REWRITE_EOT );
        }
        *rt = my_rt;
        r->curr = my_rt;
        *stop = &(my_rt->stream[my_rt->count]);
        r->stream = my_rt->stream;
    }
    return( *(r->stream++) );
}

static unsigned getString( REWRITE*r, REWRITE_TOKENS**rt, uint_8**stop, char*dest)
{
    char c;
    unsigned len;

    len = 0;
    for(;;) {
        c = getByte( r, rt, stop );
        if( c == '\0' ) break;
        *dest = c;
        ++dest;
        ++len;
    }
    *dest = '\0';
    return( len );
}

static int getBinary( REWRITE*r, REWRITE_TOKENS**rt, uint_8**stop, uint_8 *bin, unsigned size )
{
    int  rc = 0;

    for( ; size > 0; --size, ++bin ) {
        *bin = rc = getByte( r, rt, stop );
    }
    return( rc == REWRITE_EOT );
}

static TOKEN getToken( REWRITE *r, REWRITE_TOKENS **rt, uint_8 **stop )
{
    TOKEN       token;

    if( getBinary( r, rt, stop, (uint_8 *)&token, sizeof( token ) ) )
        return( r->last_token );
    return( token );
}

void RewriteToken( void )
/***********************/
{
    REWRITE *r;
    REWRITE_TOKENS *rt;
    uint_8 *stop;
    uint_32 srcfile_index;
    SRCFILE srcfile;
    uint_32 absolute;
    int code_byte;
    unsigned len;

    r = currRewrite;
    rt = r->curr;
    stop = &(rt->stream[rt->count]);
    CurToken = getToken( r, &rt, &stop );
    if( CurToken == T_REWRITE_MODS ) {
        code_byte = getByte( r, &rt, &stop );
        if( code_byte == CODE_FILE ) {
            getBinary( r, &rt, &stop, (uint_8*)&srcfile_index, sizeof( srcfile_index ) );
            srcfile = accessSrcFile( r, srcfile_index );
            SrcFilePoint( srcfile );
            TokenLine = 0;
            TokenColumn = 0;
            CurToken = getToken( r, &rt, &stop );
            if( CurToken == T_REWRITE_MODS ) {
                code_byte = getByte( r, &rt, &stop );
            }
        }
    }
    if( CurToken == T_REWRITE_MODS ) {
        for( ;; ) {
            if( code_byte == CODE_ABS ) {
                getBinary( r, &rt, &stop, (uint_8*)&absolute, sizeof( absolute ) );
                if( MASK_ABS_LINE & absolute ) {
                    TokenLine = absolute & ~MASK_ABS_LINE;
                    TokenColumn = 0;
                } else {
                    TokenColumn = absolute;
                    break;
                }
            } else if( MASK_DELTA_LINE & code_byte ) {
                TokenLine += code_byte & ~MASK_DELTA_LINE;
            } else {
                TokenColumn += code_byte;
                break;
            }
            code_byte = getByte( r, &rt, &stop );
        }
        CurToken = getToken( r, &rt, &stop );
    }
    switch( CurToken ) {
    case T_ID:
    case T_BAD_CHAR:
        len = getString( r, &rt, &stop, Buffer );
        TokenLen = len;
        break;
    case T_BAD_TOKEN:
        getBinary( r, &rt, &stop, (uint_8*)&BadTokenInfo, sizeof( BadTokenInfo ) );
        break;
    case T_STRING:
    case T_LSTRING:
        len = getString( r, &rt, &stop, Buffer );
        TokenLen = len + 1;
        break;
    case T_CONSTANT:
        ConstType = getByte( r, &rt, &stop );
        switch( ConstType ) {
        case TYP_LONG_DOUBLE:
        case TYP_DOUBLE:
        case TYP_FLOAT:
            len = getString( r, &rt, &stop, Buffer );
            TokenLen = len;
            break;
        default:
            getBinary( r, &rt, &stop, (uint_8*)&Constant64, sizeof( Constant64 ) );
            break;
        }
        break;
    default:
        break;
    }
#ifndef NDEBUG
    CtxScanToken();
    DumpToken();
#endif
}

void RewriteClose( REWRITE *rewrite )
/***********************************/
{
    REWRITE *r;

    r = currRewrite;
    if( r != NULL && r->alternate ) {
        RewriteFree( r );
    }
    currRewrite = rewrite;
}

REWRITE *RewriteRecordInit( TOKEN_LOCN *locn )
/********************************************/
{
    REWRITE *r;

    r = newREWRITE( T_EOF, locn );
    return( r );
}

void RewriteRecordToken( REWRITE *r, TOKEN_LOCN *locn )
/*****************************************************/
{
    if( CurToken != T_EOF ) {
        saveToken( r, locn );
    }
}

REWRITE *RewriteGetIndex( REWRITE *e )
/************************************/
{
    return( CarveGetIndex( carveREWRITE, e ) );
}

REWRITE *RewriteMapIndex( REWRITE *i )
/************************************/
{
    return( CarveMapIndex( carveREWRITE, i ) );
}

static void markFreeRewrite( void *p )
{
    REWRITE *b = p;

    b->free = TRUE;
}

static void saveRewrite( void *e, carve_walk_base *d )
{
    REWRITE *s = e;
    REWRITE_TOKENS *save_list;
    REWRITE_TOKENS *save_curr;
    SRCFILE_HANDLE *h;
    uint_8 *save_stream;

    if( s->free ) {
        return;
    }
    save_stream = s->stream;
    save_list = s->list;
    save_curr = s->curr;
    s->stream = _TokenPosGetIndex( s );  // must be first
    s->list = CarveGetIndex( carveREWRITE_TOKENS, save_list );
    s->curr = CarveGetIndex( carveREWRITE_TOKENS, save_curr );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    for( h = s->srcfiles_refd; h != NULL; h = h->next ) {
        PCHWriteCVIndex( (cv_index)SrcFileGetIndex( h->srcfile ) );
    }
    PCHWriteCVIndexTerm();
    s->list = save_list;
    s->curr = save_curr;
    s->stream = save_stream;
}

static void markFreeRewriteTokens( void *p )
{
    REWRITE_TOKENS *b = p;

    b->free = TRUE;
}

static void saveRewriteTokens( void *e, carve_walk_base *d )
{
    REWRITE_TOKENS *s = e;
    REWRITE_TOKENS *save_next;

    if( s->free ) {
        return;
    }
    save_next = s->next;
    s->next = CarveGetIndex( carveREWRITE_TOKENS, save_next );
    PCHWriteCVIndex( d->index );
    PCHWriteVar( *s );
    s->next = save_next;
}

pch_status PCHWriteRewrites( void )
{
    carve_walk_base data;

    CarveWalkAllFree( carveREWRITE, markFreeRewrite );
    CarveWalkAll( carveREWRITE, saveRewrite, &data );
    PCHWriteCVIndexTerm();
    CarveWalkAllFree( carveREWRITE_TOKENS, markFreeRewriteTokens );
    CarveWalkAll( carveREWRITE_TOKENS, saveRewriteTokens, &data );
    PCHWriteCVIndexTerm();
    return( PCHCB_OK );
}

pch_status PCHReadRewrites( void )
{
    cv_index i;
    REWRITE *r;
    REWRITE_TOKENS *rt;
    cvinit_t data;

    CarveInitStart( carveREWRITE, &data );
    for( ; (r = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *r );
        r->list = CarveMapIndex( carveREWRITE_TOKENS, r->list );
        r->curr = CarveMapIndex( carveREWRITE_TOKENS, r->curr );
        r->stream = _TokenPosMapIndex( r );  // must be last
        r->srcfiles_refd = NULL;
        for( ; (i = PCHReadCVIndex()) != CARVE_NULL_INDEX; ) {
            newSrcFileHandle( r, SrcFileMapIndex( (SRCFILE)i ) );
        }
    }
    CarveInitStart( carveREWRITE_TOKENS, &data );
    for( ; (rt = PCHReadCVIndexElement( &data )) != NULL; ) {
        PCHReadVar( *rt );
        rt->next = CarveMapIndex( carveREWRITE_TOKENS, rt->next );
    }
    return( PCHCB_OK );
}

pch_status PCHInitRewrites( boolean writing )
{
    if( writing ) {
        PCHWriteCVIndex( CarveLastValidIndex( carveREWRITE ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveREWRITE_TOKENS ) );
        PCHWriteCVIndex( CarveLastValidIndex( carveSRCFILE_HANDLE ) );
    } else {
        carveREWRITE = CarveRestart( carveREWRITE );
        CarveMapOptimize( carveREWRITE, PCHReadCVIndex() );
        carveREWRITE_TOKENS = CarveRestart( carveREWRITE_TOKENS );
        CarveMapOptimize( carveREWRITE_TOKENS, PCHReadCVIndex() );
        carveSRCFILE_HANDLE = CarveRestart( carveSRCFILE_HANDLE );
        CarveMapOptimize( carveSRCFILE_HANDLE, PCHReadCVIndex() );
    }
    return( PCHCB_OK );
}

pch_status PCHFiniRewrites( boolean writing )
{
    if( ! writing ) {
        CarveMapUnoptimize( carveREWRITE );
        CarveMapUnoptimize( carveREWRITE_TOKENS );
        CarveMapUnoptimize( carveSRCFILE_HANDLE );
    }
    return( PCHCB_OK );
}
