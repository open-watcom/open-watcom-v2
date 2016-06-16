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

#include "memmgr.h"
#include "cgfront.h"
#include "carve.h"
#include "ring.h"
#include "dbg.h"
#include "vbuf.h"
#include "fmttype.h"
#include "codegen.h"

static carve_t carveNode;       // memory: nodes
static carve_t carveLine;       // memory: lines
static carve_t carveSubtree;    // memory: subtrees
static carve_t carveDecoration; // memory: decoration
static bool printTypes;         // true ==> print types with nodes

typedef struct node NODE;
typedef struct line LINE;
typedef struct subtree SUBTREE;
typedef struct prline PRLINE;
typedef uint_16 COL;
typedef uint_16 DECOR;


struct node                     // NODE TO BE PRINTED
{   NODE *next;                 // - next in ring
    PTREE pnode;                // - parse-tree node
    DECOR numb;                 // - decoration #
    COL centre;                 // - centre of node
    COL left;                   // - left operand
    COL right;                  // - right operand
};

struct line                     // LINE OF NODES TO BE PRINTED
{   LINE *next;                 // - next in ring
    NODE *nodes;                // - nodes on line
    COL width;                  // - line width, so far
    COL bound;                  // - bound for next centre
};

struct subtree                  // SUBTREE TO BE PRINTED
{   SUBTREE *next;              // - next in ring
    LINE *lines;                // - lines to be printed
    PTREE root;                 // - root of tree
    COL centre;                 // - centre of node
};

struct prline                   // PRINT LINE BUFFERING
{   char *buffer;               // - buffer
    COL width;                  // - buffer size
};

typedef struct _decorated {     // DECORATED NODES
    struct _decorated * next;   // - next in ring
    NODE* node;                 // - decorated node
} DECORATED;

#define PTREE_ERROR_NODE "**error**"

#define LEFT_FROM   '\xD9'
#define LEFT_TO     '\xDA'
#define RIGHT_FROM  '\xC0'
#define RIGHT_TO    '\xBF'
#define VERT_LINE   '\xB3'
#define HORI_LINE   '\xC4'
#define BOTH_LSR    '\xC1'
#define BOTH_LB     '\xB4'
#define BOTH_BR     '\xC3'
#define BOTH_LRS    '\xC2'
#define BOTH_SLR    '\xC2'

static SUBTREE *subtrees;       // subtrees to be printed
static PRLINE node_line;        // buffering for node line
static PRLINE conn_line;        // buffering for connectors line
static DECORATED* decoration;   // decorated nodes
static DECOR decor_numb;        // decoration counter

static COL buildNode( PTREE expr, SUBTREE *subtree, LINE *pred );

static char *stxcpy(            // CONCATENATE HEXADECIMAL NUMBER
    char *tgt,                  // - target location
    unsigned value )            // - value to be concatenated
{
    char buffer[16];

    sprintf( buffer, "%x", value );
    return stxpcpy( tgt, buffer );
}


static DECOR addDecorated(      // ADD A DECORATED NODE
    NODE* node )                // - node
{
    DECORATED* dec;             // - decoration node (new or old for node)

    dec = RingCarveAlloc( carveDecoration, &decoration );
    dec->node = node;
    return ++ decor_numb;
}


static void prlineFree(         // FREE A PRINT LINE
    PRLINE *prl )               // - line control
{
    CMemFreePtr( &prl->buffer );
    prl->width = 0;
}


static void prlineExtend(       // EXTEND PRINT LINE IF REQD
    PRLINE *prl,                // - line control
    COL width )                 // - width to extend
{
    char *old;                  // - old buffer
    char *buf;                  // - new buffer

    if( width > prl->width ) {
        old = prl->buffer;
        buf = CMemAlloc( width + 1 );
        memcpy( buf, old, prl->width );
        memset( buf + prl->width, ' ', width - prl->width );
        prl->width = width;
        prl->buffer = buf;
        CMemFree( old );
    }
}


static void prlineInit(         // START BUFFERING ON A PRINT LINE
    PRLINE *prl )               // - line control
{
    memset( prl->buffer, ' ', prl->width );
}


static void prlineEmit(         // EMIT A PRINT LINE
    PRLINE *prl )               // - line control
{
    char *p;                    // - used to scan

    for( p = &prl->buffer[ prl->width ]; ; ) {
        -- p;
        if( p == prl->buffer ) break;
        if( *p != ' ' ) break;
    }
    *(p+1) = '\0';
    puts( prl->buffer );
}


static void begLine(            // START PRINTING LINE OF NODES
    void )
{
    prlineExtend( &node_line, 32 );
    prlineExtend( &conn_line, 32 );
    prlineInit( &node_line );
    prlineInit( &conn_line );
}


static void endLine(            // COMPLETE PRINTING LINE OF NODES
    void )
{
    prlineEmit( &node_line );
    prlineEmit( &conn_line );
}


static void centreText(         // ADD CENTRED TEXT TO NODE LINE
    char *text,                 // - text
    COL posn )                  // - centre position
{
    COL size;                   // - text size
    COL offset;                 // - offset back from centre

    size = strlen( text );
    offset = size / 2;
    if( offset >= posn ) {
        posn = 0;
    } else {
        posn -=  offset + 1;
    }
    prlineExtend( &node_line, posn + size );
    memcpy( &node_line.buffer[ posn ], text, size );
}


static void connect(            // ADD A CONNECTION
    COL beg,                    // - start of connection
    COL end,                    // - end of connection
    char beg_sym,               // - symbol for start
    char end_sym )              // - symbol for end
{
    COL width;                  // - width of connector line

    prlineExtend( &conn_line, end );
    width = end - beg + 1;
    if( width == 1 ) {
        conn_line.buffer[ beg - 1 ] = VERT_LINE;
    } else {
        conn_line.buffer[ beg - 1 ] = beg_sym;
        conn_line.buffer[ end - 1 ] = end_sym;
        if( width > 2 ) {
            memset( &conn_line.buffer[ beg - 1 + 1 ], HORI_LINE, width - 2 );
        }
    }
}


static void connectLeft(        // ADD A LEFT CONNECTION
    COL src,                    // - start of connection
    COL tgt )                   // - end of connection
{
    if( tgt <= src ) {
        connect( tgt, src, LEFT_TO, LEFT_FROM );
    } else {
        connect( src, tgt, RIGHT_FROM, RIGHT_TO );
    }
}


static void connectRight(       // ADD A RIGHT CONNECTION
    COL src,                    // - start of connection
    COL tgt )                   // - end of connection
{
    if( src <= tgt ) {
        connect( src, tgt, RIGHT_FROM, RIGHT_TO );
    } else {
        connect( tgt, src, LEFT_TO, LEFT_FROM );
    }
}


static void connectBoth(        // ADD A TEE CONNECTOR(S)
    COL src,                    // - position of tee
    COL left,                   // - position of left
    COL right )                 // - position of right
{
    if( src == left ) {
        conn_line.buffer[ src - 1 ] = BOTH_BR;          // L=S,R
    } else if( src == right ) {
        conn_line.buffer[ src - 1 ] = BOTH_LB;          // L,S=R
    } else if( left < src ) {
        if( src < right ) {
            conn_line.buffer[ src - 1 ] = BOTH_LSR;     // L,S,R
        } else {
            conn_line.buffer[ right - 1 ] = BOTH_LRS;   // L,R,S
        }
    } else {
        conn_line.buffer[ left - 1 ] = BOTH_SLR;        // S,L,R
    }
}


static LINE *addLine(           // ADD A LINE ENTRY TO SUBTREE
    SUBTREE *subtree )          // - subtree
{
    LINE *line;                 // - new line

    line = RingCarveAlloc( carveLine, &subtree->lines );
    line->nodes = NULL;
    line->width = 0;
    line->bound = 0;
    return line;
}


static void textType(           // GET TEXT FOR A TYPE
    char *text,                 // - text location
    TYPE type,                  // - the type
    const char *id )            // - expression for id
{
    VBUF prefix, suffix;        // - used to print types

    FormatType( type, &prefix, &suffix );
    *text++ = ' ';
    text = stxpcpy( text, VbufString( &prefix ) );
    text = stxpcpy( text, id );
    text = stxpcpy( text, VbufString( &suffix ) );
    VbufFree( &prefix );
    VbufFree( &suffix );
}


static char *textPTREE(         // GET TEXT FOR A PARSE-TREE NODE
    NODE* node )                // - print node
{
    char *text;                 // - text pointer (returned)
    static char buffer[256];    // - buffer (valid until next call)
    bool type_add;              // - true ==> add type
    PTREE pnode;                // - parse-tree node

    pnode = node->pnode;
    switch( pnode->op ) {
      case PT_ERROR :
        stxpcpy( buffer, PTREE_ERROR_NODE );
        type_add = false;
        break;
      case PT_UNARY :
      case PT_BINARY :
        stxpcpy( buffer, DbgOperator( pnode->cgop ) );
        type_add = printTypes;
        break;
      case PT_INT_CONSTANT :
        switch( TypedefModifierRemoveOnly( pnode->type )->id ) {
          case TYP_SCHAR :
          case TYP_SSHORT :
          case TYP_SINT :
          case TYP_SLONG :
            stxicpy( buffer, pnode->u.int_constant );
            break;
          case TYP_SLONG64 :
          case TYP_ULONG64 :
            buffer[0] = '<';
            text = stxcpy( &buffer[1], pnode->u.int64_constant.u._32[0] );
            *text = '~';
            text = stxcpy( text + 1, pnode->u.int64_constant.u._32[1] );
            text[0] = '>';
            text[1] = '\0';
            break;
          default :
            stxdcpy( buffer, pnode->u.uint_constant );
            break;
        }
        type_add = printTypes;
        break;
      case PT_FLOATING_CONSTANT :
        BFCnvFS( pnode->u.floating_constant, buffer, 256 );
        type_add = printTypes;
        break;
      case PT_STRING_CONSTANT :
        stxvcpy( buffer, pnode->u.string->string, pnode->u.string->len );
        type_add = printTypes;
        break;
      case PT_TYPE :
        textType( buffer, pnode->type, "<> " );
        type_add = false;
        break;
      case PT_ID :
        stxpcpy( buffer, NameStr( pnode->u.id.name ) );
        type_add = false;
        break;
      case PT_SYMBOL :
        if( pnode->cgop == CO_NAME_THIS ) {
            stxpcpy( buffer, "this" );
        } else if( pnode->cgop == CO_NAME_CDTOR_EXTRA ) {
            stxpcpy( buffer, "cdtor_extra" );
        } else {
            SYMBOL sym;
            if( pnode->cgop == CO_NAME_PARM_REF ) {
                text = stxpcpy( buffer, "parm-ref:" );
            } else {
                text = buffer;
            }
            sym = pnode->u.symcg.symbol;
            if( sym == NULL ) {
                stxpcpy( text, "this" );
            } else if( sym->name == NULL ) {
                stxpcpy( text, "**NULL**" );
            } else if( sym->name->name == NULL ) {
                stxpcpy( text, "**NULL**" );
            } else {
                stxpcpy( text, NameStr( sym->name->name ) );
            }
        }
        type_add = printTypes;
        break;
      case PT_DUP_EXPR :
        text = stxpcpy( buffer, "dup[" );
        text = stxcpy( text, (unsigned)(pointer_int)pnode->u.subtree[0] );
        stxpcpy( text, "]" );
        type_add = printTypes;
        break;
      case PT_IC :
        text = stxpcpy( buffer, DbgIcOpcode( pnode->u.ic.opcode ) );
        text = stxpcpy( text, " " );
        text = stxcpy( text, pnode->u.ic.value.uvalue );
        type_add = printTypes;
        break;
    }
    if( type_add && ( pnode->type != NULL ) ) {
        textType( strend( buffer )
                , pnode->type
                , (pnode->flags & PTF_LVALUE ) ? "<LV> " : "<RV> " );
    }
    if( 0 != node->numb ) {
        text = stxpcpy( strend( buffer ), " {" );
        text = stxdcpy( text, node->numb );
        text = stxpcpy( text, "}" );
    }
    return( buffer );
}


static void buildSubtree(       // BUILD A SUBTREE
    PTREE root )                // - root of subtree
{
    SUBTREE *subtree;           // - info for subtree

    RingIterBeg( subtrees, subtree ) {
        if( subtree->root == root ) return;
    } RingIterEnd( subtree )
    subtree = RingCarveAlloc( carveSubtree, &subtrees );
    subtree->lines = NULL;
    subtree->root = root;
    subtree->centre = buildNode( root, subtree, NULL );
}


static COL buildNode(           // BUILD A NODE
    PTREE expr,                 // - parse-tree expression
    SUBTREE *subtree,           // - subtree under construction
    LINE *pred )                // - preceding line (or NULL)
{
    NODE *node;                 // - node for expression
    LINE *line;                 // - line entry for node
    COL width;                  // - node width
    COL centre;                 // - trial centre
    COL offset;                 // - centre between operands
    COL bound;                  // - bound for next centre on next line

    if( expr == NULL ) return 0;
    node = CarveAlloc( carveNode );
    node->pnode = expr;
    node->centre = 0;
    node->left = 0;
    node->right = 0;
    if( NULL == expr->decor ) {
        node->numb = 0;
    } else {
        node->numb = addDecorated( node );
    }
    if( ( pred == NULL ) || ( pred == subtree->lines ) ) {
        line = addLine( subtree );
    } else {
        line = pred->next;
    }
    RingAppend( &line->nodes, node );
    width = strlen( textPTREE( node ) );
    switch( expr->op ) {
      case PT_UNARY :
        node->left = buildNode( expr->u.subtree[0], subtree, line );
        break;
      case PT_BINARY :
        node->left = buildNode( expr->u.subtree[0], subtree, line );
        node->right = buildNode( expr->u.subtree[1], subtree, line );
        break;
      case PT_DUP_EXPR :
        buildSubtree( expr->u.subtree[0] );
        break;
    }
    if( node->left == 0 ) {
        centre = node->right;
        bound = centre;
    } else if( node->right == 0 ) {
        centre = node->left;
        bound = centre;
    } else {
        centre = ( node->left + node->right ) / 2;
        bound = node->right;
    }
    offset = line->width + width / 2 + 1;
    if( centre < offset ) {
        centre = offset;
    }
    if( centre < line->bound ) {
        centre = line->bound;
    }
    node->centre = centre;
    line->width = centre - ( width / 2 ) + width;
    line->bound = line->width;
    if( bound != 0 ) {
        if( bound < centre ) {
            bound = centre;
        }
        ++ bound;
        line = line->next;
        if( line->bound < bound ) {
            line->bound = bound;
        }
    }
    return centre;
}


static void printNode(          // PRINT A NODE
    NODE *node )                // - node to be printed
{
    char *text;                 // - text to be printed

    text = textPTREE( node );
    centreText( text, node->centre );
    if( node->left != 0 ) {
        connectLeft( node->centre, node->left );
    }
    if( node->right != 0 ) {
        connectRight( node->centre, node->right );
        if( node->left != 0 ) {
            connectBoth( node->centre, node->left, node->right );
        }
    }
}


static void printSubtree(       // PRINT A SUBTREE
    void *_subtree )            // - subtree to be printed
{
    char buffer[ 256 ];         // - buffer
    char *bptr;                 // - buffer ptr
    LINE *line;                 // - current line
    NODE *node;                 // - current node
    TOKEN_LOCN locn;            // - subtree location
    bool print_locn;            // - true ==> print location

    SUBTREE *subtree = _subtree;

    begLine();
    if( subtree->next == subtrees ) {
        print_locn = true;
        bptr = stxpcpy( buffer, "tree[" );
    } else {
        endLine();
        begLine();
        print_locn = false;
        bptr = stxpcpy( buffer, "dup[" );
    }
    bptr = stxcpy( bptr, (unsigned)(pointer_int)subtree->root );
    if( print_locn ) {
        PTreeExtractLocn( subtree->root, &locn );
        if( NULL != locn.src_file ) {
            bptr = stxpcpy( bptr, " " );
            bptr = stxpcpy( bptr, SrcFileName( locn.src_file ) );
            bptr = stxpcpy( bptr, " line[" );
            bptr = stxdcpy( bptr, locn.line );
            bptr = stxpcpy( bptr, "] column[" );
            bptr = stxdcpy( bptr, locn.column );
            bptr = stxpcpy( bptr, "]" );
        }
    }
    centreText( buffer, subtree->centre );
    connectLeft( subtree->centre, subtree->centre );
    endLine();
    RingIterBeg( subtree->lines, line ) {
        begLine();
        RingIterBeg( line->nodes, node ) {
            printNode( node );
        } RingIterEnd( node );
        endLine();
    } RingIterEnd( line )
}


static void printDecoration(    // PRINT DECORATION FOR A NODE
    void )
{
    DECORATED* dec;             // - decoration entry

    if( NULL != decoration ) {
        printf( "\n Decorated Nodes:\n\n" );
        RingIterBeg( decoration, dec ) {
            PtdPrint( dec->node->numb, dec->node->pnode );
        } RingIterEnd( dec );
    }
}


void DbgPrintPTREE(             // PRINT A PARSE-TREE BEAUTIFULLY
    PTREE root )                // - root of tree to be printed
{
    decor_numb = 0;
    carveNode = CarveCreate( sizeof( NODE ), 16 );
    carveLine = CarveCreate( sizeof( LINE ), 16 );
    carveSubtree = CarveCreate( sizeof( SUBTREE ), 16 );
    carveDecoration = CarveCreate( sizeof( DECORATED ), 16 );
    subtrees = NULL;
    decoration = NULL;
    buildSubtree( root );
    RingWalk( subtrees, &printSubtree );
    printDecoration();
    CarveDestroy( carveNode );
    CarveDestroy( carveLine );
    CarveDestroy( carveSubtree );
    CarveDestroy( carveDecoration );
    prlineFree( &node_line );
    prlineFree( &conn_line );
}


void DbgPrintPTREEtyped(        // PRINT BEAUTIFUL PTREE, WITH TYPES
    PTREE root )                // - root of tree
{
    printTypes = true;
    DbgPrintPTREE( root );
    printTypes = false;
}

