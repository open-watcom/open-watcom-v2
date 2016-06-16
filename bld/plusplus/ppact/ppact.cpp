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


////////////////////////////////////////////////////////////////////////////
// PPACT -- pretty-print code-generation actions file
//
// 94/08/09 -- J.W.Welch        -- initial version
// 94/08/19 -- J.W.Welch        -- support CGBackName, CALLBACK
// 94/10/02 -- J.W.Welch        -- generalize scanning
// 94/10/04 -- J.W.Welch        -- support new CGInitCall format
// 95/02/08 -- J.W.Welch        -- support CGSelect
// 95/03/13 -- J.W.Welch        -- ALPHA: support CgVarargsBasePtr
// 95/03/23 -- J.W.Welch        -- ALPHA: support CGStackAlloc
// 95/03/28 -- J.W.Welch        -- ALPHA: remove CGStackAlloc
//
// 95/10/03 -- J.W.Welch        -- use TRPRTDLL.DLL
// 96/03/29 -- J.W.Welch        -- handle operator (), etc. as a name
//
////////////////////////////////////////////////////////////////////////////

// #pragma inline_depth 0
#pragma warning 549 9

#include <ctype.h>
#include <malloc.h>
#include <process.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "setimpl.h"
#include "set.h"

#ifdef TRPRTDLL
    #include <treesupp.h>
    #pragma library ( "..\test\trpr\trprtdll.lib" );
#else
    #pragma library ( "ppact.lib" );
#endif

typedef unsigned CgId;          // id for a node

const char BOX_LINE_VERT = 179;
const char BOX_LINE_HORI = 196;
const char BOX_CORNER_UP = 218;
const char BOX_CORNER_DN = 192;
const char BOX_OPCODE_UP = 217;
const char BOX_OPCODE_DN = 191;
const char BOX_OPCODE_UD = 180;
const char BOX_TEE_UP    = 193;
const char BOX_TEE_DOWN  = 194;
const char BOX_TEE_RIGHT = 195;

#ifdef TRPRTDLL
const char OPCODE_ARG[]    = "ARG";
const char OPCODE_TRASH[]  = "TRASH";
const char OPCODE_DONE[]   = "DONE";
#else
const char OPCODE_ARG[]    = "Arg";
const char OPCODE_TRASH[]  = { 237,  27, 0 };
const char OPCODE_DONE[]   = {  17,   0    };
#endif


////////////////////////////////////////////////////////////////////////////
//
// Support
//
////////////////////////////////////////////////////////////////////////////

char* stxpcpy                    // STRING COPY
    ( char* tgt                 // - target
    , char const* src )         // - source
{
    for( ; ; ) {
        *tgt = *src;
        if( '\0' == *src ) break;
        ++ src;
        ++ tgt;
    }
    return tgt;
}


////////////////////////////////////////////////////////////////////////////
//
// Options
//
////////////////////////////////////////////////////////////////////////////

struct Options
{
    unsigned verbose :1;        // verbose printing
    unsigned actions :1;        // echo actions file
#ifndef NDEBUG
    unsigned pstack  :1;        // dump pstack
#endif

    Options()                   // CONSTRUCTOR
        : verbose( 0 )
        , actions( 0 )
#ifndef NDEBUG
        , pstack(0)
#endif
        {
        }
};

static Options options;         // program options


////////////////////////////////////////////////////////////////////////////
//
// MemAlloc -- template for allocations
//
////////////////////////////////////////////////////////////////////////////

template< class CLS >
struct MemAlloc {

    static Set<CLS> _free;

    void* operator new( size_t );
    void operator delete( void* );

};


template< class CLS >
void* MemAlloc<CLS>::operator new( size_t ) // OPERATOR NEW
    {
        CLS* retn;
        if( _free.empty() ) {
            retn = (CLS*)malloc( sizeof( CLS ) );
        } else {
            retn = _free.first();
            _free.erase( retn );
        }
        return (void*)retn;
    }


template< class CLS >
void MemAlloc<CLS>::operator delete( void* item ) // OPERATOR DELETE
    {
        if( 0 != item ) {
            _free.insert( (CLS*)item );
        }
    }


template< class CLS >
static Set<CLS> MemAlloc<CLS>::_free;


////////////////////////////////////////////////////////////////////////////
//
// Varray -- variable-sized array template
//
////////////////////////////////////////////////////////////////////////////


template< class CLS >
struct Varray {

    CLS* _array;            // array
    unsigned _bound;        // bound

    Varray();                               // CONSTRUCTOR

    ~Varray();                              // DESTRUCTOR

    CLS& operator [] ( unsigned );          // SUBSCRIPT OPERATOR

    CLS const & operator [] ( unsigned ) const; // SUBSCRIPT OPERATOR
};


template< class CLS >
Varray<CLS>::Varray                         // CONSTRUCTOR
    ( void )
    : _array( 0 )
    , _bound( 0 )
{
}


template< class CLS >
Varray<CLS>::~Varray                        // DESTRUCTOR
    ( void )
{
    delete[] _array;
}


template< class CLS >
CLS& Varray<CLS>::operator []               // SUBSCRIPT OPERATOR
    ( unsigned index )
{
    if( index >= _bound ) {
        CLS* newarr = new CLS[ index + 10 ];
        if( 0 != _array ) {
            ::memcpy( newarr, _array, _bound * sizeof( CLS ) );
            delete[] _array;
        }
        _array = newarr;
        _bound = index + 10;
    }
    return _array[ index ];
}


template< class CLS >
CLS const & Varray<CLS>::operator []        // SUBSCRIPT OPERATOR
    ( unsigned index )
    const
{
    return _array[ index ];
}


////////////////////////////////////////////////////////////////////////////
//
// Stk -- very simple stack template
//
////////////////////////////////////////////////////////////////////////////


template< class CLS >
struct Stk
{
    Varray<CLS> _stack;                     // stack data
    unsigned _sp;                           // stack index

    Stk()                                   // CONSTRUCTOR
        : _sp( 0 )
        {
        }

    CLS& operator[]                         // SUBSCRIPT
        ( unsigned index )
        {
            return _stack[ index ];
        }

    CLS const & operator[]                  // SUBSCRIPT
        ( unsigned index )
        const
        {
            return _stack[ index ];
        }

    int empty() const;                      // TEST IF EMPTY

    CLS& pop();                             // PUSH THE STACK

    CLS& push();                            // POP THE STACK

    CLS& top();                             // ACCESS TOP ELEMENT
};


template< class CLS >
int Stk<CLS>::empty                         // PUSH THE STACK
    ( void )
    const
{
    return _sp == 0;
}


template< class CLS >
CLS& Stk<CLS>::push                         // PUSH THE STACK
    ( void )
{
    ++ _sp;
    return _stack[ _sp - 1 ];
}


template< class CLS >
CLS& Stk<CLS>::pop                          // POP THE STACK
    ( void )
{
    -- _sp;
    return _stack[ _sp ];
}


template< class CLS >
CLS& Stk<CLS>::top                          // ACCESS TOP ELEMENT
    ( void )
{
    return _stack[ _sp - 1 ];
}




////////////////////////////////////////////////////////////////////////////
//
// Str -- very simple string class
//
////////////////////////////////////////////////////////////////////////////


struct Str
{
    char* _str;                             // allocated string

    Str( const char*, unsigned );           // CONSTRUCTOR

    Str()                                   // CONSTRUCTOR
        : _str( 0 )
        {
        }

    Str( const char * src )                 // CONSTRUCTOR
        : _str( 0 )
        {
            if( 0 != src ) {
                *this = Str( src, ::strlen( src ) );
            }
        }

    Str( Str const & src )                  // CONSTRUCTOR
        : _str( 0 )
        {
            *this = src;
        }

    ~Str()                                  // DESTRUCTOR
        {
            delete[] _str;
        }

    char& operator [] ( unsigned index )    // operator []
        {
            return _str[ index ];
        }

    operator char const * () const          // conversion to const char*
        {
            return _str;
        }

    operator char* ()                       // conversion to char*
        {
            return _str;
        }

    Str& operator = ( Str const & );        // assignment

};


Str::Str                                    // CONSTRUCTOR
    ( const char* text
    , unsigned size )
    : _str( 0 )
{
    if( text != 0 && size != 0 ) {
        _str = new char[ size + 1 ];
        _str[ size ] = '\0';
        ::memcpy( _str, text, size );
    }
}


Str& Str::operator =                        // ASSIGNMENT
    ( Str const & src )
{
    if( this != &src ) {
        delete[] _str;
        if( src._str == 0 ) {
            _str = 0;
        } else {
            unsigned size = ::strlen( src._str );
            if( size == 0 ) {
                _str = 0;
            } else {
                _str = new char[ size + 1 ];
                _str[ size ] = '\0';
                ::memcpy( _str, src._str, size );
            }
        }
    }
    return *this;
}


////////////////////////////////////////////////////////////////////////////
//
// Edge information for printing
//
////////////////////////////////////////////////////////////////////////////


enum EdgeType                   // types of edges
{   edge_none                   // - no edge
,   edge_right_prt              // - right edge (printing)
,   edge_right_blank            // - right edge (blank)
,   edge_left_prt               // - left edge (printing)
,   edge_left_blank             // - left edge (blank)
,   edge_arg                    // - argument edge
,   edge_call                   // - call edge
};



////////////////////////////////////////////////////////////////////////////
//
// ExpOp -- abstract expression operand
//
////////////////////////////////////////////////////////////////////////////


struct ExpOp {

    enum PrtType                        // types of printing
    {   PRT_NONE            = 0x00      // - nothing
    ,   PRT_BEFORE_RIGHT    = 0x01      // - before: print right
    ,   PRT_ACTUAL_SELF     = 0x02      // - actual: print self
    ,   PRT_ACTUAL_RIGHT    = 0x04      // - actual: print right
    ,   PRT_ACTUAL_LEFT     = 0x08      // - actual: print left
    ,   PRT_AFTER_LEFT      = 0x10      // - after: print left
//  ,   PRT_UNDER_LEFT      = 0x20      // - after: print left, but under
    ,   PRT_CALL            = 0x20      // - call
    ,   PRT_ARG             = 0x40      // - argument
    ,   PRT_CALL_OR_ARG = PRT_CALL | PRT_ARG
    };

    Str _type;                          // type for operand
    CgId _id;                           // id

    static Set<ExpOp> _live;            // top-level live expressions

    ExpOp( Str const & type             // CONSTRUCTOR
         , CgId id );

    virtual ~ExpOp()                    // DESTRUCTOR
        {
        }

    static void addLive( ExpOp* item )  // ADD LIVE EXPRESSION
        {
            _live.insert( item );
        }

    static ExpOp* find( CgId id );      // FIND LIVE EXPRESSION

    virtual
    ExpOp* nodeLeft() const;            // GET 0 OR LEFT NODE

    virtual
    char * nodeName( char * ) const = 0;// GET NAME OF NODE

    virtual
    ExpOp* nodeRight() const;           // GET 0 OR RIGHT NODE

    static ExpOp* popLive( CgId id );   // FIND AND REMOVE LIVE EXPRESSION

    void print( FILE* );                // PRINT AN ENTRY

    virtual
    void printAfter( FILE* );           // PRINT AN ENTRY (AFTER)

    static void printAfter              // PRINT INDENTED ENTRY (AFTER)
        ( FILE* out
        , ExpOp* operand );

    virtual
    void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

    virtual
    void printBefore( FILE* );          // PRINT AN ENTRY (BEFORE)

    static
    void print( FILE*, ExpOp* );        // PRINT INDENTED ENTRY

    static
    void printIndentation( FILE* );     // PRINT INDENTATION CHARACTERS

    virtual
    PrtType printType() const;          // DETERMINE PRINTING TYPE

    static
    void removeLive( ExpOp* op )        // REMOVE LIVE EXPRESSION
        {
            _live.erase( op );
        }

    static void verifyEmpty( FILE* );   // VERIFY ALL LIVE EXPR'S DELETED

    enum NodeState                      // node states
    {   NS_RIGHT_BEFORE                 // - processing for right before
    ,   NS_RIGHT_ACTUAL                 // - processing for right actual
    ,   NS_ACTUAL                       // - processing for actual
    ,   NS_LEFT_ACTUAL                  // - processing for left actual
    ,   NS_LEFT_AFTER                   // - processing for left afters
    ,   NS_DONE                         // - completed processing
    };

protected:

    struct PrintStack {                 // PRINT STACK

        ExpOp* _node;                   // - node to be printed
        NodeState _state;               // - processing state
        EdgeType _edge;                 // - current edge
        unsigned inlined : 1;           // - TRUE ==> will be inlined

    };

    static Stk<PrintStack> _pstk;       // print stack

    static int pstackEmpty();           // test if stack empty

    static PrintStack& pstackPop();     // pop stack

    static PrintStack& pstackPush       // push stack
        ( ExpOp* node                   // - node
        , NodeState state               // - state
        , int inlined );                // - TRUE ==> is inline node

    static PrintStack& pstackTop();     // access top of stack

#ifndef NDEBUG
    static pstackDump()
    {
        static char* state[] =
        {   "NS_RIGHT_BEFORE"
        ,   "NS_RIGHT_ACTUAL"
        ,   "NS_ACTUAL      "
        ,   "NS_LEFT_ACTUAL "
        ,   "NS_LEFT_AFTER  "
        ,   "NS_DONE        "
        };
        static char* edge[] =
        {   "edge_none       "
        ,   "edge_right_prt  "
        ,   "edge_right_blank"
        ,   "edge_left_prt   "
        ,   "edge_left_blank "
        ,   "edge_arg        "
        ,   "edge_call       "
        };
        if( ! pstackEmpty() ) {
            printf( "PSTACK:\n" );
            unsigned index;
            for( index = 0; index < _pstk._sp; ++ index ) {
                printf( "%d %p %s %s %d "
                      , index
                      , _pstk._stack._array[index]._node
                      , state[ _pstk._stack._array[index]._state ]
                      , edge[ _pstk._stack._array[index]._edge ]
                      , _pstk._stack._array[index].inlined
                      );
                _pstk._stack._array[index]._node->printActual( stdout );
                printf( "\n" );
            }
        }
    }
#endif

};

ExpOp::ExpOp( Str const & type             // CONSTRUCTOR
    , CgId id )
    : _type( type )
    , _id( id )
{
}


static Set<ExpOp> ExpOp::_live;                 // live list
static Stk<ExpOp::PrintStack> ExpOp::_pstk;  // print stack


static ExpOp* ExpOp::find( CgId id )    // FIND LIVE EXPRESSION
{
    SetIter<ExpOp> iter( _live );
    ExpOp* expr;
    for( ; ; ++iter ) {
        expr = *iter;
        if( expr == 0 ) break;
        if( id == expr->_id ) break;
    }
    return expr;
}


ExpOp* ExpOp::nodeLeft                  // GET LEFT NODE
    ( void )
    const
{
    return 0;
}


ExpOp* ExpOp::nodeRight                 // GET RIGHT NODE
    ( void )
    const
{
    return 0;
}


static ExpOp* ExpOp::popLive( CgId id ) // FIND AND REMOVE LIVE EXPRESSION
{
    ExpOp* expr = find( id );
    if( expr != 0 ) {
        removeLive( expr );
    }
    return expr;
}


static
ExpOp::PrintStack& ExpOp::pstackPop     // POP PRINT STACK
    ( void )
{
    return _pstk.pop();
}


static
ExpOp::PrintStack& ExpOp::pstackTop     // ACCESS TOP OF PRINT STACK
    ( void )
{
    return _pstk.top();
}


static
ExpOp::PrintStack& ExpOp::pstackPush    // PUSH ON PRINT STACK
    ( ExpOp* node                       // - node
    , NodeState state                   // - state
    , int inlined )                     // - TRUE ==> is inline node
{
    PrintStack& stk = _pstk.push();
    stk._node = node;
    stk._state = state;
    stk._edge = edge_none;
    stk.inlined = inlined;
    return stk;
}


static int ExpOp::pstackEmpty           // TEST IF EMPTY
    ( void )
{
    return _pstk.empty();
}


void ExpOp::print                       // PRINT A TREE
    ( FILE* out )
{
    pstackPush( this, NS_RIGHT_BEFORE, 0 );
    for( ; ; ) {
#ifndef NDEBUG
        if( options.pstack ) {
            pstackDump();
        }
#endif
        PrintStack & top = pstackTop();
        PrtType type = top._node->printType();
        switch( top._state ) {
          case NS_RIGHT_BEFORE :
            if( type & PRT_BEFORE_RIGHT ) {
                pstackPush( top._node->nodeRight(), NS_RIGHT_BEFORE, 0 );
                top._edge = edge_right_blank;
                top._state = NS_RIGHT_ACTUAL;
                continue;
            }
          case NS_RIGHT_ACTUAL :
            if( type & PRT_ACTUAL_RIGHT ) {
                pstackPush( top._node->nodeRight(), NS_RIGHT_BEFORE, 1 );
                if( top.inlined ) {
                    top._state = NS_DONE;
                } else {
                    top._state = NS_ACTUAL;
                }
                continue;
            } else if( type & PRT_ACTUAL_LEFT ) {
                pstackPush( top._node->nodeLeft(), NS_RIGHT_BEFORE, 1 );
                if( top.inlined ) {
                    top._state = NS_DONE;
                } else {
                    top._state = NS_ACTUAL;
                }
                continue;
            }
          case NS_ACTUAL :
            if( top.inlined ) {
                _pstk.pop();
                continue;
            } else {
                printIndentation( out );
                ExpOp* node;
                PrtType ntype;
                for( node = top._node, ntype = type; ; ) {
                    node->printActual( out );
                    if( ntype & PRT_ACTUAL_RIGHT ) {
                        node = node->nodeRight();
                    } else if( ntype & PRT_ACTUAL_LEFT ) {
                        node = node->nodeLeft();
                    } else {
                        break;
                    }
                    ntype = node->printType();
                }
                fputc( '\n', out );
            }
          case NS_LEFT_ACTUAL :
            if( type & PRT_ACTUAL_RIGHT ) {
                pstackPush( top._node->nodeRight(), NS_LEFT_ACTUAL, 1 );
                if( type & PRT_AFTER_LEFT ) {
                    if( type & PRT_CALL ) {
                        top._edge = edge_call;
                    } else if( type & PRT_ARG ) {
                        top._edge = edge_none;
                        if( ! type & PRT_AFTER_LEFT ) {
                            ((&top)-1)->_edge = edge_left_prt;
                        }
                    } else {
                        top._edge = edge_left_prt;
                    }
                    top._state = NS_LEFT_AFTER;
                } else {
                    top._state = NS_DONE;
                }
                continue;
            } else if( type & PRT_ACTUAL_LEFT ) {
                pstackPush( top._node->nodeLeft(), NS_LEFT_ACTUAL, 1 );
                top._state = NS_DONE;
                continue;
            }
          case NS_LEFT_AFTER :
            if( type & PRT_AFTER_LEFT ) {
                if( type & PRT_ARG ) {
                    top._node = top._node->nodeLeft();
                    top._edge = edge_none;
                    top._state = NS_RIGHT_BEFORE;
                    top.inlined = 0;
                    if( ! ( top._node->printType() & PRT_AFTER_LEFT ) ) {
                        ((&top)-1)->_edge = edge_left_prt;
                    }
                } else {
                    ExpOp* left = top._node->nodeLeft();
                    if( type & PRT_CALL ) {
                        if( left->printType() & PRT_AFTER_LEFT ) {
                            top._edge = edge_call;
                        } else {
                            top._edge = edge_left_prt;
                        }
                    } else {
                        top._edge = edge_left_prt;
                    }
                    pstackPush( left, NS_RIGHT_BEFORE, 0 );
                    top._state = NS_DONE;
                }
                continue;
            }
          case NS_DONE :
            _pstk.pop();
            if( _pstk.empty() ) break;
            continue;
        }
        break;
    }
}


void ExpOp::printActual                 // DEFAULT PRINT (ACTUAL
    ( FILE* out )
{
#if 0
    if( options.verbose ) {
        fprintf( out
               , " %s id=%d"
               , (const char*)_type
               , _id );
    }
#else
    char buffer[128];
    fputs( nodeName( buffer ), out );
#endif
}


char* ExpOp::nodeName                   // DEFAULT NODE NAME FOR ExpOp
    ( char * buffer )                   // - default buffer
    const
{
    buffer[0] = '\0';
    if( options.verbose ) {
        char* p = buffer;
        char buf[32];
        if( 0 != _type ) {
            p = stxpcpy( p, " " );
            p = stxpcpy( p, (char const *)_type );
        }
        if( 0 != _id ) {
            p = stxpcpy( p, " id=" );
            p = stxpcpy( p, utoa( _id, buf, 16 ) );
        }
    }
    return buffer;
}


void ExpOp::printAfter                  // DEFAULT PRINT (AFTER)
    ( FILE* )
{
}


static void ExpOp::printAfter           // PRINT INDENTED ENTRY (AFTER)
    ( FILE* out
    , ExpOp* operand )
{
    print( out, operand );
}


void ExpOp::printBefore                 // DEFAULT PRINT (BEFORE)
    ( FILE* )
{
}


static void ExpOp::printIndentation     // PRINT INDENTATION
    ( FILE* out )
{
    unsigned bound = _pstk._sp - 1;
    unsigned curr = bound - 1;
    int index;
    int skipped_args = 0;
    char seg[5] = "    ";
    char last;
    for( index = 0; index < bound; ++ index ) {
        PrintStack & ps = _pstk[ index ];
        EdgeType& edge = ps._edge;
        if( index == curr ) {
            if( edge == edge_right_blank ) {
                last = BOX_CORNER_UP;
                edge = edge_right_prt;
            } else if( edge == edge_call ) {
                last = BOX_TEE_RIGHT;
            } else {
                if( skipped_args == 0 ) {
                    last = BOX_CORNER_DN;
                    edge = edge_left_blank;
                } else {
                    last = BOX_TEE_RIGHT;
                }
            }
        } else {
#if 0
            if( edge == edge_arg ) {
                ++ skipped_args;
                continue;
            } else if( edge == edge_call ) {
                ++ skipped_args;
            } else {
                skipped_args = 0;
            }
#endif
            if( edge == edge_left_prt
#if 1
             || edge == edge_call
#endif
             || edge == edge_right_prt ) {
                last = BOX_LINE_VERT;
            } else {
                last = ' ';
            }
        }
        seg[3] = last;
        fputs( seg, out );
    }
}


ExpOp::PrtType ExpOp::printType         // DEFAULT PRINT-TYPE (ONLY SELF)
    ( void )
    const
{
    return PRT_ACTUAL_SELF;
}


static void ExpOp::print                // PRINT INDENTED ENTRY
    ( FILE* out
    , ExpOp* operand )
{
    if( 0 != operand ) {
        operand->print( out );
    }
}


#ifdef TRPRTDLL
////////////////////////////////////////////////////////////////////////////
//
// Use tree-printing DLL
//
////////////////////////////////////////////////////////////////////////////


TreePtr treeSuppLeft                    // GET LEFT OPERAND
    ( TreePtr node )                    // - node
{
    return TreePtr( ((ExpOp*)node)->nodeLeft() );
}


TreePtr treeSuppRight                   // GET RIGHT OPERAND
    ( TreePtr node )                    // - node
{
    return TreePtr( ((ExpOp*)node)->nodeRight() );
}


char const * treeSuppName               // GET NAME
    ( TreePtr node )                    // - node
{
    static char name_buffer[120];
    return ((ExpOp*)node)->nodeName( name_buffer );
}


unsigned treeSuppWidth                  // GET WIDTH
    ( TreePtr node )                    // - node
{
    return strlen( treeSuppName( node ) );
}


static FILE* output_file;


void treeSuppPrint                      // PRINT A LINE
    ( char const *line )                // - the line
{
    fputs( line, output_file );
    fputc( '\n', output_file );
}


static TreeSupport support              // TREE-PRINTING SUPPORT STRUCTURE
(   &treeSuppLeft
,   &treeSuppRight
,   &treeSuppWidth
,   &treeSuppName
,   &treeSuppPrint
,   0
,   0
);


#endif


static void ExpOp::verifyEmpty          // VERIFY LIVE SET IS EMPTY
    ( FILE* out )
{
    SetIter<ExpOp> iter( _live );
    for( ; ; ) {
        ExpOp* expr = *iter;
        if( expr == 0 ) break;
        fputs( "\nFollowing expression was not connected\n", out );
#ifdef TRPRTDLL
        support.print( TreePtr( expr ) );
#else
        expr->print( out );
#endif
        iter.erase();
        delete expr;
    }
}


////////////////////////////////////////////////////////////////////////////
//
// ExpBitMask -- expression bit mask
//
////////////////////////////////////////////////////////////////////////////


struct ExpBitMask
    : public ExpOp
{
    Str _start;
    Str _size;
    ExpOp* _operand;

    ExpBitMask( Str const & type        // CONSTRUCTOR
              , CgId id
              , CgId operand
              , Str const & start
              , Str const & size )
        : ExpOp( type, id )
        , _start( start )
        , _size( size )
        , _operand( ExpOp::popLive( operand ) )
        {
        }

    ExpOp* nodeLeft() const;            // GET 0 OR LEFT NODE

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

    PrtType printType() const;          // DETERMINE PRINTING TYPE

};


ExpOp* ExpBitMask::nodeLeft             // GET LEFT NODE
    ( void )
    const
{
    return _operand;
}


char* ExpBitMask::nodeName              // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, "BitMask(" );
    p = stxpcpy( p, _start );
    p = stxpcpy( p, "," );
    p = stxpcpy( p, _size );
    p = stxpcpy( p, ")" );
    ExpOp::nodeName( p );
    return buffer;
}


#if 0
void ExpBitMask::printActual            // PRINT ACTUAL
    ( FILE* out )
{
    fprintf( out
           , "BitMask(%s,%s)"
           , _start
           , _size );
    ExpOp::printActual( out );
}
#endif


ExpOp::PrtType ExpBitMask::printType    // PRINT-TYPE
    ( void )
    const
{
    return PrtType( PRT_ACTUAL_SELF | PRT_AFTER_LEFT );
}


////////////////////////////////////////////////////////////////////////////
//
// ExpCallBack -- CGCallback
//
////////////////////////////////////////////////////////////////////////////


struct ExpCallBack
    : public ExpOp
    , public MemAlloc<ExpCallBack>
{
    Str _rtn;                           // name of symbol
    Str _data;                          // data reference

    ExpCallBack( Str const & rtn        // CONSTRUCTOR
               , Str const & data
               , CgId id )
        : ExpOp( "", id )
        , _rtn( rtn )
        , _data( data )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


char* ExpCallBack::nodeName             // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, "CallBack " );
    p = stxpcpy( p, _rtn );
    p = stxpcpy( p, " " );
    p = stxpcpy( p, _data );
    return buffer;
}


#if 0
void ExpCallBack::printActual           // PRINT ACTUAL
    ( FILE* out )
{
    fputs( " CallBack ", out );
    fputs( _rtn, out );
    fputc( ' ', out );
    fputs( _data, out );
}
#endif


////////////////////////////////////////////////////////////////////////////
//
// ExpInteger -- expression integer
//
////////////////////////////////////////////////////////////////////////////


struct ExpInteger
    : public ExpOp
    , public MemAlloc<ExpInteger>
{
    Str _value;

    ExpInteger( Str const & type        // CONSTRUCTOR
              , Str const & value
              , CgId id )
        : ExpOp( type, id )
        , _value( value )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


char* ExpInteger::nodeName              // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, _value );
    if( options.verbose ) {
        p = stxpcpy( p, " Integer" );
        ExpOp::nodeName( p );
    }
    return buffer;
}


#if 0
void ExpInteger::printActual            // PRINT ACTUAL
    ( FILE* out )
{
    fputc( ' ', out );
    fputs( _value, out );
    if( options.verbose ) {
        fputs( " Integer", out );
        ExpOp::printActual( out );
    }
}
#endif


////////////////////////////////////////////////////////////////////////////
//
// ExpPatchNode -- patch node
//
////////////////////////////////////////////////////////////////////////////


struct ExpPatchNode
    : public ExpOp
    , public MemAlloc<ExpPatchNode>
{
    Str _handle;

    ExpPatchNode( Str const & type      // CONSTRUCTOR
                , Str const & handle
                , CgId id )
        : ExpOp( type, id )
        , _handle( handle )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


char* ExpPatchNode::nodeName            // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, "PatchNode[ " );
    p = stxpcpy( p, _handle );
    p = stxpcpy( p, " ]" );
    if( options.verbose ) {
        ExpOp::nodeName( p );
    }
    return buffer;
}


#if 0
void ExpPatchNode::printActual          // PRINT ACTUAL
    ( FILE* out )
{
    fputs( " PatchNode[ ", out );
    fputs( _handle, out );
    fputs( " ]", out );
    if( options.verbose ) {
        ExpOp::printActual( out );
    }
}
#endif


////////////////////////////////////////////////////////////////////////////
//
// ExpSym -- expression symbol
//
////////////////////////////////////////////////////////////////////////////


struct ExpSym
    : public ExpOp
    , public MemAlloc<ExpSym>
{
    Str _name;                          // name of symbol

    ExpSym( Str const & type            // CONSTRUCTOR
          , Str const & name
          , CgId id )
        : ExpOp( type, id )
        , _name( name )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


char* ExpSym::nodeName                  // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, _name );
    if( options.verbose ) {
        p = stxpcpy( p, " Symbol" );
        ExpOp::nodeName( p );
    }
    return buffer;
}


#if 0
void ExpSym::printActual                // PRINT ACTUAL
    ( FILE* out )
{
#if 0
    fputc( ' ', out );
    fputs( _name, out );
    if( options.verbose ) {
        fputs( " Symbol", out );
        ExpOp::printActual( out );
    }
#else
    char buffer[128];
    fputs( nodeName( buffer ), out );
#endif
}
#endif


////////////////////////////////////////////////////////////////////////////
//
// ExpTemp -- expression temporary
//
////////////////////////////////////////////////////////////////////////////


struct ExpTemp
    : public ExpOp
    , public MemAlloc<ExpTemp>
{
    Str _number;                        // temporary number

    ExpTemp( Str const & type            // CONSTRUCTOR
           , Str const & numb
           , CgId id )
        : ExpOp( type, id )
        , _number( numb )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


char* ExpTemp::nodeName                 // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, "Temp[" );
    p = stxpcpy( p, _number );
    p = stxpcpy( p, "]" );
    if( options.verbose ) {
        ExpOp::nodeName( p );
    }
    return buffer;
}


#if 0
void ExpTemp::printActual               // PRINT ACTUAL
    ( FILE* out )
{
    fputs( " Temp[", out );
    fputs( _number, out );
    fputc( ']', out );
    if( options.verbose ) {
        ExpOp::printActual( out );
    }
}
#endif


////////////////////////////////////////////////////////////////////////////
//
// ExpOpCode -- expression opcode
//
////////////////////////////////////////////////////////////////////////////


struct ExpOpCode
    : public ExpOp
{
    Str _opcode;                        // opcode

    ExpOpCode( Str const & type         // CONSTRUCTOR
             , Str const & opcode
             , CgId id )
        : ExpOp( type, id )
        , _opcode( opcode )
        {
        }

    int inlineable() const;             // TEST IF OPCODE INLINEABLE

    char* nodeName( char * ) const;     // GET NODE NAME

    void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


int ExpOpCode::inlineable               // TEST IF INLINEABLE OPCODE
    ( void )
    const
{
    return ::strlen( _opcode ) < 4;
}


char* ExpOpCode::nodeName               // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, _opcode );
    if( options.verbose ) {
        ExpOp::nodeName( p );
    }
    return buffer;
}


void ExpOpCode::printActual             // PRINT ACTUAL
    ( FILE* out )
{
    char* opcd = _opcode;
    size_t size =  ::strlen( opcd );
    char extended[6];
    if( 4 > size ) {
        extended[4] = '\0';
        extended[0] = BOX_LINE_HORI;
        extended[1] = BOX_LINE_HORI;
        extended[2] = BOX_LINE_HORI;
        ::memcpy( extended, opcd, size );
        char last;
        PrtType pt = PrtType( printType() & ~ PRT_CALL );
        switch( pt ) {
          case PRT_ACTUAL_SELF | PRT_BEFORE_RIGHT :
            last = BOX_OPCODE_UP;
            break;
          case PRT_ACTUAL_SELF | PRT_BEFORE_RIGHT | PRT_ACTUAL_LEFT :
            last = BOX_TEE_UP;
            break;
          case PRT_ACTUAL_SELF | PRT_ACTUAL_RIGHT | PRT_AFTER_LEFT :
            last = BOX_TEE_DOWN;
            break;
          case PRT_ACTUAL_SELF | PRT_AFTER_LEFT :
            last = BOX_OPCODE_DN;
            break;
          case PRT_ACTUAL_SELF | PRT_BEFORE_RIGHT | PRT_AFTER_LEFT :
            last = BOX_OPCODE_UD;
            break;
          default :
            last = BOX_LINE_HORI;
            break;
        }
        extended[3] = last;
        opcd = extended;
    }
    fputs( opcd, out );
    if( options.verbose ) {
        char buffer[ 32 ];
        fputs( ExpOp::nodeName( buffer ), out );
    }
}


////////////////////////////////////////////////////////////////////////////
//
// ExpUnOp -- expression unary operand
//
////////////////////////////////////////////////////////////////////////////


struct ExpUnOp
    : public ExpOpCode
    , public MemAlloc<ExpUnOp>
{

    ExpOp* _operand;                    // operand

    ExpUnOp( Str const & type           // CONSTRUCTOR
           , Str const & opcode
           , CgId id
           , CgId operand )
        : ExpOpCode( type, opcode, id )
        , _operand( ExpOp::popLive( operand ) )
        {
        }

    virtual
    ~ExpUnOp()                          // DESTRUCTOR
        {
            delete _operand;
        }

    ExpOp* nodeLeft() const;            // GET 0 OR LEFT NODE

    char* nodeName( char * ) const;     // GET NODE NAME

    PrtType printType() const;          // DETERMINE PRINTING TYPE

};


ExpOp* ExpUnOp::nodeLeft                // GET LEFT NODE
    ( void )
    const
{
    return _operand;
}


char* ExpUnOp::nodeName                 // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    return ExpOpCode::nodeName( buffer );
}


ExpOp::PrtType ExpUnOp::printType       // PRINT-TYPE
    ( void )
    const
{
    PrtType retn;
    if( 0 == _operand ) {
        retn = PRT_ACTUAL_SELF;
    } else if( options.verbose || ! inlineable() ) {
        retn = PrtType( PRT_ACTUAL_SELF | PRT_AFTER_LEFT );
    } else {
        retn = PrtType( PRT_ACTUAL_SELF | PRT_ACTUAL_LEFT );
    }
    return retn;
}


////////////////////////////////////////////////////////////////////////////
//
// ExpBinOp -- expression binary operand
//
////////////////////////////////////////////////////////////////////////////


struct ExpBnOp
    : public ExpOpCode
{

    ExpOp* _left;                       // left operand
    ExpOp* _right;                      // right operand

    ExpBnOp( Str const & type          // CONSTRUCTOR
           , Str const & opcode
           , CgId id
           , CgId left
           , CgId right )
        : ExpOpCode( type, opcode, id )
        , _left( ExpOp::popLive( left ) )
        , _right( ExpOp::popLive( right ) )
        {
        }

    ~ExpBnOp()                          // DESTRUCTOR
        {
            delete _left;
            delete _right;
        }

    ExpOp* nodeLeft() const;            // GET 0 OR LEFT NODE

    char* nodeName( char * ) const;     // GET NODE NAME

    ExpOp* nodeRight() const;           // GET 0 OR RIGHT NODE

    PrtType printType() const;          // DETERMINE PRINTING TYPE

};


struct ExpBinOp
    : public ExpBnOp
    , public MemAlloc<ExpBinOp>
{
    ExpBinOp( Str const & type          // CONSTRUCTOR
            , Str const & opcode
            , CgId id
            , CgId left
            , CgId right )
        : ExpBnOp( type, opcode, id, left, right )
        {
        }
};


ExpOp* ExpBnOp::nodeLeft                // GET LEFT NODE
    ( void )
    const
{
    return _left;
}


char* ExpBnOp::nodeName                 // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    return ExpOpCode::nodeName( buffer );
}


ExpOp* ExpBnOp::nodeRight               // GET RIGHT NODE
    ( void )
    const
{
    return _right;
}


ExpOp::PrtType ExpBnOp::printType       // PRINT-TYPE
    ( void )
    const
{
    PrtType retn;
    if( 0 == _right ) {
        if( 0 == _left ) {
            retn = PRT_ACTUAL_SELF;
        } else {
            retn = PrtType( PRT_ACTUAL_SELF | PRT_AFTER_LEFT );
        }
    } else if( 0 == _left ) {
        if( 0 == ::strcmp( _opcode, OPCODE_ARG ) ) {
            retn = PrtType( PRT_ACTUAL_SELF | PRT_ACTUAL_RIGHT );
        } else {
            retn = PrtType( PRT_ACTUAL_SELF | PRT_BEFORE_RIGHT );
        }
    } else {
        if( options.verbose || ! inlineable() ) {
            retn = PrtType( PRT_ACTUAL_SELF
                          | PRT_BEFORE_RIGHT
                          | PRT_AFTER_LEFT );
        } else {
            retn = PrtType( PRT_ACTUAL_SELF
                          | PRT_ACTUAL_RIGHT
                          | PRT_AFTER_LEFT );
        }
    }
    return retn;
}


////////////////////////////////////////////////////////////////////////////
//
// ExpCall -- call expression
//
// right: function
// left: first argument
//
////////////////////////////////////////////////////////////////////////////


struct ExpCall
    : public ExpBnOp
    , public MemAlloc<ExpCall>
{

    Str _handle;                        // call handle

    ExpCall( Str const & type           // CONSTRUCTOR
           , CgId id
#ifdef TRPRTDLL
           , CgId operand
#endif
           )
#ifdef TRPRTDLL
        : ExpBnOp( type, "CALL", id, operand, 0 )
#else
        : ExpBnOp( type, "()", id, 0, id )
#endif
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

    PrtType printType() const;          // DETERMINE PRINTING TYPE

};


char* ExpCall::nodeName                 // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p;
#ifdef TRPRTDLL
    p = ExpBnOp::nodeName( buffer );
#else
    p = ExpOp::nodeName( buffer );
    p += strlen( p );
#endif
    char const *handle = _handle;
    if( 0 != handle ) {
        p = stxpcpy( p, handle );
        p = stxpcpy( p, " " );
    }
    return buffer;
}


#if 0
void ExpCall::printActual               // PRINT ENTRY
    ( FILE* out )
{
    ExpOpCode::printActual( out );
    char const *handle = _handle;
    if( 0 != handle ) {
        fputs( handle, out );
        fputc( ' ', out );
    }
}
#endif


ExpOp::PrtType ExpCall::printType() const // GET PRINT TYPE
{
    PrtType retn;
    if( 0 == _left ) {
        retn = PrtType( PRT_ACTUAL_SELF
                      | PRT_ACTUAL_RIGHT
                      | PRT_CALL );
    } else {
        retn = PrtType( PRT_ACTUAL_SELF
                      | PRT_AFTER_LEFT
                      | PRT_ACTUAL_RIGHT
                      | PRT_CALL );
    }
    return retn;
}


////////////////////////////////////////////////////////////////////////////
//
// ExpArg -- call-argument expression
//
////////////////////////////////////////////////////////////////////////////


struct ExpArg
    : public ExpBnOp
    , public MemAlloc<ExpArg>
{

    ExpArg( Str const & type           // CONSTRUCTOR
          , CgId id
          , CgId parm )
        : ExpBnOp( type, OPCODE_ARG, 0, 0, parm )
        {
            ExpCall* caller = (ExpCall*)find( id );
#ifdef TRPRTDLL
            _left = caller->_right;
            caller->_right = this;
#else
            _left = caller->_left;
            caller->_left = this;
#endif
        }

    char* nodeName( char * ) const;     // GET NODE NAME

    PrtType printType() const;          // DETERMINE PRINTING TYPE

};


char* ExpArg::nodeName                  // GET NODE NAME
    ( char * )                          // - buffer
    const
{
    return "ARG";
}


ExpOp::PrtType ExpArg::printType() const // GET PRINT TYPE
{
    PrtType retn;
    if( 0 == _left ) {
        retn = PrtType( PRT_ACTUAL_SELF
                      | PRT_ACTUAL_RIGHT
                      | PRT_ARG );
    } else {
        retn = PrtType( PRT_ACTUAL_SELF
                      | PRT_AFTER_LEFT
                      | PRT_ACTUAL_RIGHT
                      | PRT_ARG );
    }
    return retn;
}


////////////////////////////////////////////////////////////////////////////
//
// ExpVarArgBase -- expression for CGVarargsBasePtr
//
////////////////////////////////////////////////////////////////////////////


struct ExpVarArgBase
    : public ExpOp
    , public MemAlloc<ExpVarArgBase>
{
    Str _type;                          // type for CGVarargsBasePtr

    ExpVarArgBase( Str const & type_result // CONSTRUCTOR
                 , Str const & type_used
                 , CgId id )
        : ExpOp( type_result, id )
        , _type( type_used )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME

//  void printActual( FILE* );          // PRINT AN ENTRY (ACTUAL)

};


char* ExpVarArgBase::nodeName           // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, "CgVarargsBasePtr( " );
    p = stxpcpy( p, _type );
    p = stxpcpy( p, " )" );
    if( options.verbose ) {
        ExpOp::nodeName( buffer );
    }
    return buffer;
}


#ifdef TRPRTDLL
////////////////////////////////////////////////////////////////////////////
//
// ExpType -- type node
//
////////////////////////////////////////////////////////////////////////////


struct ExpType
    : public ExpOp
    , public MemAlloc<ExpType>
{
    Str _type;                          // type for CGVarargsBasePtr

    ExpType( Str const & type_result )  // CONSTRUCTOR
        : ExpOp( 0, 0 )
        , _type( type_result )
        {
        }

    char* nodeName( char * ) const;     // GET NODE NAME
};


char* ExpType::nodeName                 // GET NODE NAME
    ( char * buffer )                   // - buffer
    const
{
    char* p = buffer;
    p = stxpcpy( p, _type );
    return buffer;
}
#endif



////////////////////////////////////////////////////////////////////////////
//
// Main line
//
////////////////////////////////////////////////////////////////////////////


static void err( const char* seg, ... )     // ERR MESSAGE
{
    va_list args;
    va_start( args, seg );
    const char* text;
    printf( "*** PPACT ERROR:" );
    for( text = seg; text != 0; text = va_arg( args, char* ) ) {
        printf( " %s", text );
    }
    printf( "\n" );
    va_end( args );
    exit( 1 );
}


static int isDelim( char chr )              // TEST IF DELIMITER
{
    if( chr == '('
     || chr == ','
     || chr == ')'
     || chr == ','
      ) {
        return 1;
    } else {
        return 0;
    }
}


static char const * scanPastGarbage         // SCAN PAST DELIMITERS, BLANKS
    ( char const *rptr )
{
    while( isDelim( *rptr ) || isspace( *rptr ) ) ++rptr;
    return rptr;
}


static char const nameOperator[] = "operator";
int const sizeOperator = sizeof( nameOperator ) - 1;


static char const * scanName                // SCAN A NAME
    ( char const *rptr
    , char const * & name
    , size_t& size )
{
    rptr = scanPastGarbage( rptr );
    if( *rptr == '\0' ) {
        size = 0;
        name = 0;
    } else {
        bool have_operator_name = false;
        bool have_operator_code = false;
        name = rptr;
        for( ; ; ++rptr ) {
            bool is_delim = isDelim( *rptr );
            if( *rptr == '\0' || is_delim ) {
                size = rptr - name;
                if( size > sizeOperator
                 && 0 == memcmp( nameOperator, name, sizeOperator ) ) {
                    // handle operator()
                    char const* op = name + sizeOperator;
                    for( ; isspace(*op); ++op );
                    if( *op == '(' && op == rptr ) continue;
                    ++op;
                    if( *op == ')' && op == rptr ) continue;
                }
                for( ; size > 0; --size ) {
                    if( ! isspace( name[size-1] ) ) break;
                }
                if( size == 0 ) {
                    name = 0;
                }
                break;
            }
        }
    }
    return rptr;
}


static char const * scanCgh                 // SCAN cg(#)
    ( const char* rptr
    , CgId &id )
{
    char const *cg;
    size_t size;
    rptr = scanName( rptr, cg, size );
    if( 2 == size ) {
        rptr = scanName( rptr, cg, size );
    } else {
        cg += 3;
        size -= 4;
    }
    id = 0;
    if( size == 4 && cg[0] == 'N' ) {
        // NULL
    } else {
        for( ; size > 0; -- size ) {
            char val = *cg++;
            if( '0' <= val && val <= '9' ) {
                val -= '0';
            } else if( 'a' <= val && val <= 'f' ) {
                val -= 'a' - 10;
            } else {
                val -= 'A' - 10;
            }
            id = id * 16 + val;
        }
    }
    return rptr;
}


static char const * scanCghResult           // SCAN ) -> cg(#)
    ( const char* rptr
    , CgId &id )
{
    rptr = scanPastGarbage( rptr );
    if( *rptr == ')' ) {
        ++rptr;
        rptr = scanPastGarbage( rptr );
    }
    if( *rptr == '-' ) {
        ++rptr;
        if( *rptr == '>' ) {
            ++rptr;
        }
    }
    return scanCgh( rptr, id );
}


static char const * scanString              // SCAN A STRING
    ( char const * rptr
    , Str& str )
{
    char const * src;
    size_t size;
    rptr = scanName( rptr, src, size );
    str = Str( src, size );
    return rptr;
}


inline
static char const * scanInteger             // SCAN INTEGER
    ( char const * rptr
    , Str & str )
{
    return scanString( rptr, str );
}


static char const * scanBackHandle          // SCAN BACK-HANDLE
    ( char const * rptr
    , Str & str )
{
    char const * src;
    size_t size;
    rptr = scanName( rptr, src, size );
    str = Str( src, size );
    return rptr;
}


inline
static char const * scanLabel               // SCAN LABEL
    ( char const * rptr
    , Str & str )
{
    return scanString( rptr, str );
}


inline
static char const * scanOpCode              // SCAN OPCODE
    ( char const * rptr
    , Str & str )
{
    return scanString( rptr, str );
}


static char const * scanSymbol              // SCAN SYMBOL
    ( char const * rptr
    , Str & str )
{
    char const * src;
    size_t size;
    rptr = scanName( rptr, src, size );
    if( options.actions && ! options.verbose && 0 != src ) {
        // symbol-table address is visible
        char const *end;
        size_t left;
        for( end = src, left = size; left > 0; ++ end, -- left ) {
            if( *end == '[' ) {
                size = end - src;
            }
        }
    }
    str = Str( src, size );
    return rptr;
}


static char const * scanType                // SCAN TYPE
    ( char const * rptr
    , Str & str )
{
    return scanString( rptr, str );
}


static void printLiveExpr                   // PRINT LIVE EXPRESSION
    ( FILE* out
    , ExpOp* expr )
{
    if( expr != 0 ) {
        fputc( '\n', out );
#ifdef TRPRTDLL
        support.print( TreePtr( expr ) );
#else
        expr->print( out );
#endif
        delete expr;
        ExpOp::verifyEmpty( out );
    }
}


static void cgaddparm( char const * rptr )  // CGADDPARM
{
    Str type;
    CgId operand;
    CgId parm;
    rptr = scanCgh( rptr, operand );
    rptr = scanCgh( rptr, parm );
    rptr = scanType( rptr, type );
    new ExpArg( type, operand, parm );
}


static void cgautodecl                      // CGAUTODECL
    ( char const * rptr
    , FILE* out )
{
    Str symbol;
    Str type;
    rptr = scanSymbol( rptr, symbol );
    rptr = scanType( rptr, type );
    fprintf( out, "Automatic: %s %s\n", symbol, type );
}


static void cgassign( char const * rptr ) // CGASSIGN
{
    Str opcode;
    scanOpCode( "=R=", opcode );
    Str type;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpBinOp( type, opcode, result, left, right ) );
}


static void cgbackname( char const * rptr ) // CGBACKNAME
{
    Str opcode = "CGBackName ";
    Str type;
    Str operand;
    CgId result;
    rptr = scanBackHandle( rptr, operand );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpSym( type, operand, result ) );
    ExpOp::addLive( new ExpUnOp( type, opcode, result, result ) );
}


static void cgbinary( char const * rptr )   // CGBINARY
{
    Str opcode;
    Str type;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    if( 0 == ::strcmp( opcode, "O_COMMA" ) ) {
        opcode = ",";
    }
    ExpOp::addLive( new ExpBinOp( type, opcode, result, left, right ) );
}


static void cgbitmask( char const * rptr )  // CGBITMASK
{
    CgId operand;
    Str start;
    Str size;
    Str type;
    CgId result;
    rptr = scanCgh( rptr, operand );
    rptr = scanString( rptr, start );
    rptr = scanString( rptr, size );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpBitMask( type, result, operand, start, size ) );
}


static void cgcallback( char const * rptr ) // CGCALLBACK
{
    Str rtn;
    Str data;
    CgId result;
    rptr = scanString( rptr, rtn );
    rptr = scanString( rptr, data );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpCallBack( rtn, data, result ) );
}


static void callback                        // CALLBACK
    ( char const * rptr
    , FILE* out )
{
    Str rtn;
    Str data;
    rptr = scanString( rptr, rtn );
    rptr = scanString( rptr, data );
    fputs( "Called back to ", out );
    fputs( rtn, out );
    fputs( "( ", out );
    fputs( data, out );
    fputs( " )\n", out );
}


static void cgchoose( char const * rptr )   // CGCHOOSE
{
    Str opcode;
    Str type;
    CgId test;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanCgh( rptr, test );
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    scanOpCode( ":", opcode );
    ExpOp::addLive( new ExpBinOp( type, opcode, result, left, right ) );
    scanOpCode( "?", opcode );
    ExpOp::addLive( new ExpBinOp( type, opcode, result, test, result ) );
}


static void cgcompare( char const * rptr )  // CGCOMPARE
{
    Str opcode;
    Str type;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpBinOp( type, opcode, result, left, right ) );
}


static void cgcontrol                       // CGCONTROL
    ( char const * rptr
    , FILE* out )
{
    Str label;
    Str opcode;
    CgId id;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, id );
    rptr = scanLabel( rptr, label );
    if( 0 == ::strcmp( opcode, "O_LABEL" ) ) {
        fprintf( out, "\nLabel: %s\n", label );
    } else if( 0 == id ) {
        fprintf( out, "\nGoTo %s\n", label );
    } else {
        fprintf( out, "\nGoTo (%s) %s\n", opcode, label );
        printLiveExpr( out, ExpOp::popLive( id ) );
    }
}


static void cgselect                        // CGSELECT
    ( char const * rptr
    , FILE* out )
{
    Str handle;
    CgId id;
    rptr = scanString( rptr, handle );
    rptr = scanCgh( rptr, id );
    printLiveExpr( out, new ExpUnOp( "", "Select", 0, id ) );
}


static void cgdone                          // CGDONE
    ( char const * rptr
    , FILE* out )
{
    CgId id;
    scanCgh( rptr, id );
    printLiveExpr( out, new ExpUnOp( "", OPCODE_DONE, 0, id ) );
}


static void cgfename( char const * rptr )   // CGFENAME
{
    Str symbol;
    Str type;
    CgId id;
    rptr = scanSymbol( rptr, symbol );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, id );
    ExpOp::addLive( new ExpSym( type, symbol, id ) );
}


static void cgflow( char const * rptr )     // CGFLOW
{
    Str opcode;
    Str type = "Boolean";
    CgId left;
    CgId right;
    CgId result;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanCghResult( rptr, result );
    if( 0 == right ) {
        ExpOp::addLive( new ExpUnOp( type, opcode, result, left ) );
    } else {
        ExpOp::addLive( new ExpBinOp( type, opcode, result, left, right ) );
    }
}


static void cginitcall( char const * rptr )  // CGINITCALL
{
    Str type;
    Str symbol;
    CgId operand;
    CgId result;
    rptr = scanCgh( rptr, operand );
    rptr = scanType( rptr, type );
    rptr = scanSymbol( rptr, symbol );
    rptr = scanCghResult( rptr, result );
#ifdef TRPRTDLL
    ExpOp::addLive( new ExpCall( type, result, operand ) );
#else
    ExpOp::addLive( new ExpUnOp( type, "Fn:", result, operand ) );
    ExpOp::addLive( new ExpCall( type, result ) );
#endif
}


static void cgcall( char const * rptr )     // CGCALL
{
    CgId operand;
    Str handle;
    rptr = scanCgh( rptr, operand );
    rptr = scanString( rptr, handle );
    if( 0 != (const char*)handle ) {
        ExpCall* call = (ExpCall*)ExpOp::find( operand );
        if( call != 0 ) {
            call->_handle = handle;
        }
    }
}


static void cginteger( char const * rptr )  // CGINTEGER
{
    Str value;
    Str type;
    CgId id;
    rptr = scanInteger( rptr, value );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, id );
    ExpOp::addLive( new ExpInteger( type, value, id ) );
}


static void cglvassign( char const * rptr ) // CGLVASSIGN
{
    Str opcode;
    scanOpCode( "=", opcode );
    Str type;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpBinOp( type, opcode, result, left, right ) );
}


static void cglvpregets( char const * rptr ) // CGLVPREGETS
{
    Str opcode;
    Str type;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    char opnew[20];
    opnew[0] = opcode[0];
    opnew[1] = '=';
    ::strcpy( &opnew[2], " pre" );
    ExpOp::addLive( new ExpBinOp( type, opnew, result, left, right ) );
}


static void cgparmdecl                      // CGPARMDECL
    ( char const * rptr
    , FILE* out )
{
    Str symbol;
    Str type;
    rptr = scanSymbol( rptr, symbol );
    rptr = scanType( rptr, type );
    fprintf( out, "Parameter: %s %s\n", symbol, type );
}


static void cgpatchnode( char const * rptr )  // CGPATCHNODE
{
    Str handle;
    Str type;
    CgId id;
    rptr = scanString( rptr, handle );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, id );
    ExpOp::addLive( new ExpPatchNode( type, handle, id ) );
}


static void cgpostgets( char const * rptr ) // CGPOSTGETS
{
    Str opcode;
    Str type;
    CgId left;
    CgId right;
    CgId result;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, left );
    rptr = scanCgh( rptr, right );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    char opnew[20];
    opnew[0] = opcode[0];
    opnew[1] = opnew[1];
    ::strcpy( &opnew[2], " post" );
    ExpOp::addLive( new ExpBinOp( type, opnew, result, left, right ) );
}


static void cgprocdecl                      // CGPROCDECL
    ( char const * rptr
    , FILE* out )
{
    Str symbol;
    Str type;
    rptr = scanSymbol( rptr, symbol );
    rptr = scanType( rptr, type );
    fprintf( out, "\nProcedure: %s %s\n", symbol, type );
}


static void cgreturn                        // CGRETURN
    ( char const * rptr
    , FILE* out )
{
    CgId id;
    Str type;
    rptr = scanCgh( rptr, id );
    scanType( rptr, type );
    printLiveExpr( out, new ExpUnOp( "", "Return", 0, id ) );
}


static void cgtempname( char const * rptr ) // CGTEMPNAME
{
    Str number;
    Str type;
    CgId id;
    rptr = scanInteger( rptr, number );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, id );
    ExpOp::addLive( new ExpTemp( type, number, id ) );
}


static void cgtrash                         // CGTRASH
    ( char const * rptr
    , FILE* out )
{
    CgId id;
    scanCgh( rptr, id );
    printLiveExpr( out, new ExpUnOp( "", OPCODE_TRASH, 0, id ) );
}


static void cgunary( char const * rptr )    // CGUNARY
{
    Str opcode;
    Str type;
    CgId operand;
    CgId result;
    rptr = scanOpCode( rptr, opcode );
    rptr = scanCgh( rptr, operand );
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
#ifdef TRPRTDLL
    if( 0 == ::strcmp( opcode, "<c=" ) ) {
        opcode = "CONVERT";
        ExpType* type_op = new ExpType( type );
        ExpBnOp* conv_op = new ExpBnOp( type, opcode, result, 0, operand );
        conv_op->_left = type_op;
        ExpOp::addLive( conv_op );
    } else {
        ExpOp::addLive( new ExpUnOp( type, opcode, result, operand ) );
    }
#else
    ExpOp::addLive( new ExpUnOp( type, opcode, result, operand ) );
#endif
}


static void cgvarargsbaseptr( char const * rptr ) // CGVARARGBASEPTR
{
    Str type;
    CgId result;
    rptr = scanType( rptr, type );
    rptr = scanCghResult( rptr, result );
    ExpOp::addLive( new ExpVarArgBase( "TY_INTEGER", type, result ) );
}


static void dbsrccue                        // DBSRCCUE
    ( char const * rptr
    , FILE* out )
{
    Str number;
    rptr = scanInteger( rptr, number );
    rptr = scanInteger( rptr, number );
    fprintf( out, "\nLine %s\n", number );
}


static void fegenproc                       // FEGENPROC
    ( char const * rptr
    , FILE* out )
{
    Str sym;
    Str han;
    rptr = scanString( rptr, sym );
    rptr = scanString( rptr, han );
    fprintf( out, "\nFEGenProc %s %s\n", sym, han );
}


static char buffer[1024];


static void process( FILE* out )            // PROCESSING
{
    Str cmd_code;
    const char* rec = scanString( buffer, cmd_code );
    const char* cmd = cmd_code;
    if( options.actions ) {
        fputs( buffer, out );
    }
    if( cmd != 0 ) {
    //
    // following are tree operands
    //
        if( 0 == ::stricmp( cmd, "cgdone" ) ) {
            cgdone( rec, out );
        } else if( 0 == ::stricmp( cmd, "cgtrash" ) ) {
            cgtrash( rec, out );
        } else if( 0 == ::stricmp( cmd, "cgunary" ) ) {
            cgunary( rec );
        } else if( 0 == ::stricmp( cmd, "cgbinary" ) ) {
            cgbinary( rec );
        } else if( 0 == ::stricmp( cmd, "cgfename" ) ) {
            cgfename( rec );
        } else if( 0 == ::stricmp( cmd, "cginteger" ) ) {
            cginteger( rec );
        } else if( 0 == ::stricmp( cmd, "cgaddparm" ) ) {
            cgaddparm( rec );
        } else if( 0 == ::stricmp( cmd, "cginitcall" ) ) {
            cginitcall( rec );
        } else if( 0 == ::stricmp( cmd, "cgcall" ) ) {
            cgcall( rec );
        } else if( 0 == ::stricmp( cmd, "cglvassign" ) ) {
            cglvassign( rec );
        } else if( 0 == ::stricmp( cmd, "cglvpregets" ) ) {
            cglvpregets( rec );
        } else if( 0 == ::stricmp( cmd, "cgreturn" ) ) {
            cgreturn( rec, out );
        //
        // above are ordered for speed
        // below are in alphabetical order
        //
        } else if( 0 == ::stricmp( cmd, "cgassign" ) ) {
            cgassign( rec );
        } else if( 0 == ::stricmp( cmd, "cgbackname" ) ) {
            cgbackname( rec );
        } else if( 0 == ::stricmp( cmd, "cgbitmask" ) ) {
            cgbitmask( rec );
        } else if( 0 == ::stricmp( cmd, "cgcallback" ) ) {
            cgcallback( rec );
        } else if( 0 == ::stricmp( cmd, "cgchoose" ) ) {
            cgchoose( rec );
        } else if( 0 == ::stricmp( cmd, "cgcontrol" ) ) {
            cgcontrol( rec, out );
        } else if( 0 == ::stricmp( cmd, "cgcompare" ) ) {
            cgcompare( rec );
        } else if( 0 == ::stricmp( cmd, "cgflow" ) ) {
            cgflow( rec );
        } else if( 0 == ::stricmp( cmd, "cgpatchnode" ) ) {
            cgpatchnode( rec );
        } else if( 0 == ::stricmp( cmd, "cgpostgets" ) ) {
            cgpostgets( rec );
        } else if( 0 == ::stricmp( cmd, "cgselect" ) ) {
            cgselect( rec, out );
        } else if( 0 == ::stricmp( cmd, "cgtempname" ) ) {
            cgtempname( rec );
        } else if( 0 == ::stricmp( cmd, "cgvarargsbaseptr" ) ) {
            cgvarargsbaseptr( rec );
    //
    // following are processed when not -a
    //
        } else if( ! options.actions ) {
            if( 0 == ::stricmp( cmd, "dbsrccue" ) ) {
                dbsrccue( rec, out );
            } else if( 0 == ::stricmp( cmd, "cgautodecl" ) ) {
                cgautodecl( rec, out );
            } else if( 0 == ::stricmp( cmd, "callback" ) ) {
                callback( rec, out );
            } else if( 0 == ::stricmp( cmd, "cgparmdecl" ) ) {
                cgparmdecl( rec, out );
            } else if( 0 == ::stricmp( cmd, "cgprocdecl" ) ) {
                cgprocdecl( rec, out );
            } else if( 0 == ::stricmp( cmd, "fegenproc" ) ) {
                fegenproc( rec, out );
            }
        }
    }
}


static void processOptions                  // PROCESS OPTIONS
    ( const char* opts )                    // - options
{
    for( ; *opts != '\0'; ) {
        char opt = *opts++;
        switch( opt ) {
          case 'a' :                // a = echo actions file
            options.actions = 1;
            break;
          case 'v' :                // v = verbose
            options.verbose = 1;
            break;
          case 'h' :                // h, ? = Help
          case '?' :
            puts( "ppact [ options ] [ input [ output ] ]"    );
            puts( ""                                          );
            puts( "options: t -- print type information"      );
            puts( "         a -- print actions file"          );
            puts( "         h -- display help information"    );
            exit( 0 );
            break;
#ifndef NDEBUG
          case 'p' :
            options.pstack = 1;
            break;
#endif
          default :
            err( "invalid option(s):", opts, 0 );
        }
    }

}


int main( int argc, char* argv[] )          // MAIN-LINE
{
    char* file_in = 0;
    char* file_out = 0;
    int arg;
    for( arg = 1; arg < argc; ++arg ) {
        char* arg_text = argv[ arg ];
        if( arg_text[0] == '/' || arg_text[0] == '-' ) {
            processOptions( &arg_text[1] );
        } else if( arg_text[0] == '?' ) {
            processOptions( arg_text );
        } else if( file_in == 0 ) {
            file_in = arg_text;
        } else if( file_out == 0 ) {
            file_out = arg_text;
        } else {
            err( "Invalid command option:", arg_text, 0 );
        }
    }
    if( file_in == 0 ) {
        file_in = "actions";
    }
    FILE* inp = fopen( file_in, "rt" );
    if( 0 == inp ) {
        err( "Cannot open:", file_in, 0 );
    }
    FILE* out;
    if( file_out == 0 ) {
        out = stdout;
    } else {
        out = fopen( file_out, "wt" );
        if( 0 == inp ) {
            err( "Cannot open:", file_out, 0 );
        }
    }
#ifdef TRPRTDLL
    output_file = out;
#endif
    for( ; ; ) {
        if( 0 == fgets( buffer, sizeof( buffer ), inp ) ) break;
        process( out );
    }
    ExpOp::verifyEmpty( out );
    printf( "\nPPACT completed\n" );
    fprintf( out, "\nPPACT completed\n" );
    if( file_out != 0 ) {
        fclose( out );
    }
    fclose( inp );
    return 0;
}

// #pragma on (dump_cg)
