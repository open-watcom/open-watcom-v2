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


#include <stdarg.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "standard.h"
#include "coderep.h"
#include "cgdefs.h"
#include "typedef.h"
#include "spawn.h"
#include "tree.h"
#include "seldef.h"
#include "echoapi.h"
#include "model.h"
#include "sysmacro.h"
#include "useinfo.h"
#include "dump.h"
#include "feprotos.h"

extern  void            FatalError(char *);
extern  type_def        *TypeAddress(cg_type);
extern  bool            GetEnvVar(char*,char*,int);
extern  int             BGInlineDepth( void );

#define hdlSetUsed( handle, value ) ( ((use_info *)handle)->used = value )
#define hdlSetup( hdltype, handle ) ( handleSetup( hdltype, (use_info *)handle) )
#define hdlAddReuse( hdltype, handle ) ( handleAddReuse( hdltype, (use_info *)handle) )


// ERROR MESSAGE FUNCTIONS

static  FILE    *EchoAPIFile = NULL;
static  int     EchoAPIFlush = 0;

static void     EchoAPIRedirect() {
/******************************/
    char tmpfile[PATH_MAX];

    if( GetEnvVar("echoapiflush", tmpfile, 11 ) ) {
        EchoAPIFlush = 1;
    }
    if( GetEnvVar("echoapifile", tmpfile, 11 ) ) {
        EchoAPIFile = fopen( tmpfile, "wt" );
    }
}

static  void    EchoAPIUnredirect() {
/********************************/

    if( EchoAPIFile == NULL ) return;
    fclose( EchoAPIFile );
    EchoAPIFile = NULL;
}

static  void    EchoAPIChar( char c ) {
/**********************************/

    /* if the codegen is crashing we want flushing to occur
     */
    if( EchoAPIFile != NULL ) {
        fputc( c, EchoAPIFile );
        if( EchoAPIFlush ) {
            fflush( EchoAPIFile );
        }
    } else {
        fputc( c, stdout );
        if( EchoAPIFlush ) {
            fflush( stdout );
        }
    }
}

static  void    EchoAPIString( char const *s ) {
/*************************************/

    while( *s ) {
        EchoAPIChar( *s );
        s++;
    }
}

static void errMsg              // PRINT ERROR MESSAGE
    ( char const *msg )         // - message
{
    DumpString( "\n**** CgDbg Failure: " );
    DumpString( msg );
    DumpChar( '\n' );
}

static void handleFailure()
{
    DumpString( "Aborted after CgDbg Failure" );
    EchoAPIUnredirect();
    FatalError( "CGDbg Failure\n" );
}

static void printName( handle_type hdltype )
{
    switch( hdltype ) {
    case NO_HANDLE :
        DumpString("uninitialized");
        break;
    case CG_NAMES :
        DumpString("cg_name");
        break;
    case LABEL_HANDLE :
        DumpString("label_handle");
        break;
    case PATCH_HANDLE :
        DumpString("patch_handle");
        break;
    case SEL_HANDLE :
        DumpString("sel_handle");
        break;
    default :
        DumpString("default");
        break;
    }
}

static void handleMsg             // PRINT ERROR FOR RING
    ( handle_type hdltype,
      char const* msg )         // - message
{
    errMsg( msg );
    DumpString( "\n**** CgDbg Failure: for " );
    printName( hdltype );
    DumpChar('\n');
    handleFailure();
}

static void ErrExpectFound( handle_type expect, handle_type found )
{
    DumpString( "\n**** CgDbg Failure: " );
    DumpString( "\n\texpecting type ");
    printName(expect);
    DumpChar( '\n' );

    DumpString( "\n\tfound type ");
    printName(found);
    DumpChar( '\n' );
    handleFailure();
}

// TYPE CHECKING FUNCTIONS

void verifyNotUserType   // VERIFY NOT A USER-DEFINED TYPE
    ( cg_type type )            // - that type
{
    type_def    *alias;

    // Check for aliased types
    alias = TypeAddress( type );
    if( alias ) {
        type = alias->refno;
    }
    if( type >= T_FIRST_FREE ) {
        errMsg( "cannot use user-defined type" );
        handleFailure();
    }
}


// PRINTING SUPPORT

#define case_str( a ) case a : EchoAPIString( #a ); break;
#define case_prt( a, b ) case a : EchoAPIString( #b ); break;

static void EchoAPIHandle        // EchoAPI A HANDLE
    ( int handle                // - handle
    , char const *lab )         // - label for handle
{
    char buffer[16];
    EchoAPIString( lab );
    EchoAPIChar( '[' );
    itoa( handle, buffer, 16 );
    EchoAPIString( buffer );
    EchoAPIChar( ']' );
}

static void dumpLineHeader( void ) // let us know how deep inline we are
{

    int depth;

    depth = BGInlineDepth();
    while( depth-- != 0 ) {
        EchoAPIChar( '\t' );
    }
}

// debug output
void EchoAPI              // EchoAPI ROUTINE
    ( const char* text          // - text
    , ... )                     // - operands
{
    if( _IsModel( ECHO_API_CALLS ) ) {
        char buffer[256];
        va_list args;
        va_start( args, text );
        dumpLineHeader();
        for( ; *text != 0; ++text ) {
            if( *text == '%' ) {
                ++text;
                switch( *text ) {
                  case 'B' : {
                    EchoAPIHandle( (int)va_arg( args, back_handle ), "bh" );
                    break;
                  }
                  case 'c' : {
                    char const * text = va_arg( args, char const * );
                    EchoAPIString( text );
                    break;
                  }
                  case 'C' : {
                    EchoAPIHandle( (int)va_arg( args, call_handle ), "ch" );
                    break;
                  }
                  case 'i' : {
                    int val = va_arg( args, int );
                    itoa( val, buffer, 10 );
                    EchoAPIString( buffer );
                    break;
                  }
                  case 'L' : {
                    EchoAPIHandle( (int)va_arg( args, label_handle ), "lh" );
                    break;
                  }
                  case 'o' : {
                    cg_op op = va_arg( args, cg_op );
                    switch( op ) {
#if 0
                      // new code gen defn's 95/02/06
                      case_str( O_NOP )
                      case_prt( O_PLUS, + )
                      case_str( O_INTERNAL_01 )
                      case_prt( O_MINUS, - )
                      case_str( O_INTERNAL_02 )
                      case_prt( O_TIMES, * )
                      case_str( O_INTERNAL_03 )
                      case_prt( O_DIV, / )
                      case_str( O_MOD )
                      case_prt( O_AND, & )
                      case_prt( O_OR, | )
                      case_prt( O_XOR, ^ )
                      case_prt( O_RSHIFT, >> )
                      case_prt( O_LSHIFT, << )
                      case_str( O_POW )
                      case_str( O_ATAN2 )
                      case_str( O_FMOD )
                      case_str( O_UMINUS )
                      case_str( O_COMPLEMENT )
                      case_str( O_LOG )
                      case_str( O_COS )
                      case_str( O_SIN )
                      case_str( O_TAN )
                      case_str( O_SQRT )
                      case_str( O_FABS )
                      case_str( O_ACOS )
                      case_str( O_ASIN )
                      case_str( O_ATAN )
                      case_str( O_COSH )
                      case_str( O_EXP )
                      case_str( O_LOG10 )
                      case_str( O_SINH )
                      case_str( O_TANH )
                      case_str( O_PTR_TO_NATIVE )
                      case_str( O_PTR_TO_FORIEGN )
                      case_str( O_PARENTHESIS )
                      case_prt( O_CONVERT, <c= )
                      case_str( O_ROUND )
                      case_str( O_GETS )
                      case_str( O_INTERNAL_05 )
                      case_str( O_INTERNAL_06 )
                      case_str( O_INTERNAL_07 )
                      case_str( O_INTERNAL_08 )
                      case_str( O_INTERNAL_09 )
                      case_str( O_INTERNAL_10 )
                      case_str( O_INTERNAL_11 )
                      case_str( O_INTERNAL_14 )
                      case_str( O_INTERNAL_15 )
                      case_prt( O_EQ, == )
                      case_prt( O_NE, != )
                      case_prt( O_GT, > )
                      case_prt( O_LE, <= )
                      case_prt( O_LT, < )
                      case_prt( O_GE, >= )
                      case_str( O_INTERNAL_12 )
                      case_str( O_SLACK_20 )
                      case_str( O_SLACK_21 )
                      case_str( O_SLACK_22 )
                      case_str( O_SLACK_23 )
                      case_str( O_SLACK_24 )
                      case_str( O_SLACK_25 )
                      case_str( O_SLACK_26 )
                      case_str( O_SLACK_27 )
                      case_str( O_SLACK_28 )
                      case_str( O_SLACK_29 )
                      case_str( O_SLACK_30 )
                      case_str( O_SLACK_31 )
                      case_str( O_SLACK_32 )
                      case_str( O_SLACK_33 )
                      case_str( O_SLACK_34 )
                      case_str( O_SLACK_35 )
                      case_str( O_SLACK_36 )
                      case_str( O_SLACK_37 )
                      case_str( O_SLACK_38 )
                      case_str( O_SLACK_39 )
                      case_str( O_INTERNAL_13 )
                      case_prt( O_FLOW_AND, && )
                      case_prt( O_FLOW_OR, || )
                      case_str( O_FLOW_OUT )
                      case_prt( O_FLOW_NOT, ! )
                      case_prt( O_POINTS, @ )
                      case_str( O_GOTO )
                      case_str( O_BIG_GOTO )
                      case_str( O_IF_TRUE )
                      case_str( O_IF_FALSE )
                      case_str( O_INVOKE_LABEL )
                      case_str( O_LABEL )
                      case_str( O_BIG_LABEL )
                      case_str( O_LABEL_RETURN )
                      case_str( O_PROC )
                      case_str( O_PARM_DEF )
                      case_str( O_AUTO_DEF )
                      case_str( O_COMMA )
                      case_str( O_PASS_PROC_PARM )
                      case_str( O_DEFN_PROC_PARM )
                      case_str( O_CALL_PROC_PARM )
                      case_str( O_PRE_GETS )
                      case_str( O_POST_GETS )
                      case_str( O_SIDE_EFFECT )
#else
                      case_str( O_NOP )
                      case_prt( O_PLUS, + )
                      case_str( O_INTERNAL_01 )
                      case_prt( O_MINUS, - )
                      case_str( O_INTERNAL_02 )
                      case_prt( O_TIMES, * )
                      case_str( O_INTERNAL_03 )
                      case_prt( O_DIV, / )
                      case_str( O_MOD )
                      case_prt( O_AND, & )
                      case_prt( O_OR, | )
                      case_prt( O_XOR, ^ )
                      case_prt( O_RSHIFT, >> )
                      case_prt( O_LSHIFT, << )
                      case_str( O_POW )
                      case_str( O_INTERNAL_16 )
                      case_str( O_ATAN2 )
                      case_str( O_FMOD )
                      case_str( O_UMINUS )
                      case_str( O_COMPLEMENT )
                      case_str( O_LOG )
                      case_str( O_COS )
                      case_str( O_SIN )
                      case_str( O_TAN )
                      case_str( O_SQRT )
                      case_str( O_FABS )
                      case_str( O_ACOS )
                      case_str( O_ASIN )
                      case_str( O_ATAN )
                      case_str( O_COSH )
                      case_str( O_EXP )
                      case_str( O_LOG10 )
                      case_str( O_SINH )
                      case_str( O_TANH )
                      case_str( O_PTR_TO_NATIVE )
                      case_str( O_PTR_TO_FORIEGN )
                      case_str( O_PARENTHESIS )
                      case_prt( O_CONVERT, <c= )
                      case_str( O_ROUND )
                      case_str( O_GETS )
                      case_str( O_INTERNAL_05 )
                      case_str( O_INTERNAL_06 )
                      case_str( O_INTERNAL_07 )
                      case_str( O_INTERNAL_08 )
                      case_str( O_INTERNAL_09 )
                      case_str( O_INTERNAL_10 )
                      case_str( O_INTERNAL_11 )
                      case_str( O_INTERNAL_14 )
                      case_str( O_INTERNAL_15 )
                      case_prt( O_EQ, == )
                      case_prt( O_NE, != )
                      case_prt( O_GT, > )
                      case_prt( O_LE, <= )
                      case_prt( O_LT, < )
                      case_prt( O_GE, >= )
                      case_str( O_INTERNAL_12 )
                      case_str( O_INTERNAL_17 )
                      case_str( O_INTERNAL_18 )
                      case_str( O_INTERNAL_19 )
                      case_str( O_INTERNAL_20 )
                      case_str( O_INTERNAL_21 )
                      case_str( O_INTERNAL_22 )
                      case_str( O_INTERNAL_23 )
                      case_str( O_INTERNAL_24 )
                      case_str( O_INTERNAL_25 )
                      case_str( O_INTERNAL_26 )
                      case_str( O_INTERNAL_27 )
                      case_str( O_INTERNAL_28 )
                      case_str( O_INTERNAL_29 )
                      case_str( O_INTERNAL_30 )
                      case_str( O_INTERNAL_31 )
                      case_str( O_INTERNAL_32 )
                      case_str( O_INTERNAL_33 )
                      case_str( O_INTERNAL_34 )
                      case_str( O_STACK_ALLOC )
                      case_str( O_VA_START )
                      case_str( O_SLACK_31 )
                      case_str( O_SLACK_32 )
                      case_str( O_SLACK_33 )
                      case_str( O_SLACK_34 )
                      case_str( O_SLACK_35 )
                      case_str( O_SLACK_36 )
                      case_str( O_SLACK_37 )
                      case_str( O_SLACK_38 )
                      case_str( O_SLACK_39 )
                      case_str( O_INTERNAL_13 )
                      case_prt( O_FLOW_AND, && )
                      case_prt( O_FLOW_OR, || )
                      case_str( O_FLOW_OUT )
                      case_prt( O_FLOW_NOT, ! )
                      case_prt( O_POINTS, @ )
                      case_str( O_GOTO )
                      case_str( O_BIG_GOTO )
                      case_str( O_IF_TRUE )
                      case_str( O_IF_FALSE )
                      case_str( O_INVOKE_LABEL )
                      case_str( O_LABEL )
                      case_str( O_BIG_LABEL )
                      case_str( O_LABEL_RETURN )
                      case_str( O_PROC )
                      case_str( O_PARM_DEF )
                      case_str( O_AUTO_DEF )
                      case_str( O_COMMA )
                      case_str( O_PASS_PROC_PARM )
                      case_str( O_DEFN_PROC_PARM )
                      case_str( O_CALL_PROC_PARM )
                      case_str( O_PRE_GETS )
                      case_str( O_POST_GETS )
                      case_str( O_SIDE_EFFECT )
#endif
                      default :
                        EchoAPIString( "O_0" );
                        itoa( op, buffer, 10 );
                        EchoAPIString( buffer );
                        break;
                    }
                    break;
                  }
                  case 'n' : {
                    EchoAPIHandle( (int)va_arg( args, cg_name ), "cg" );
                    break;
                  }
                  case 'P' : {
                    EchoAPIHandle( (int)va_arg( args, patch_handle ), "ph" );
                    break;
                  }
                  case 's' : {
                    cg_sym_handle handle = va_arg( args, cg_sym_handle );
                    EchoAPIString( FEName( handle ) );
                    EchoAPIChar( '[' );
                    itoa( (int)handle, buffer, 16 );
                    EchoAPIString( buffer );
                    EchoAPIChar( ']' );
                    break;
                  }
                  case 'S' : {
                    EchoAPIHandle( (int)va_arg( args, sel_handle ), "sh" );
                    break;
                  }
                  case 'T' : {
                    EchoAPIHandle( (int)va_arg( args, temp_handle ), "th" );
                    break;
                  }
                  case 't' : {
                    cg_type type = va_arg( args, cg_type );
                    switch( type ) {
                      case_str( T_UINT_1 )
                      case_str( T_INT_1 )
                      case_str( T_UINT_2 )
                      case_str( T_INT_2 )
                      case_str( T_UINT_4 )
                      case_str( T_INT_4 )
                      case_str( T_UINT_8 )
                      case_str( T_INT_8 )
                      case_str( T_LONG_POINTER )
                      case_str( T_HUGE_POINTER )
                      case_str( T_NEAR_POINTER )
                      case_str( T_LONG_CODE_PTR )
                      case_str( T_NEAR_CODE_PTR )
                      case_str( T_SINGLE )
                #ifdef BY_C_FRONT_END
                      case_str( TY_DOUBLE )
                #else
                      case_str( T_DOUBLE )
                #endif
                      case_str( T_LONG_DOUBLE )
                      case_str( T_UNKNOWN )
                #ifdef BY_C_FRONT_END
                      case_str( TY_DEFAULT )
                #else
                      case_str( T_DEFAULT )
                #endif
                      case_str( T_INTEGER )
                #ifdef BY_C_FRONT_END
                      case_str( TY_UNSIGNED )
                #else
                      case_str( T_UNSIGNED )
                #endif
                      case_str( T_POINTER )
                      case_str( T_CODE_PTR )
                      case_str( T_BOOLEAN )
                      case_str( T_PROC_PARM )
                      default :
                        EchoAPIString( "T_0" );
                        itoa( type, buffer, 10 );
                        EchoAPIString( buffer );
                        break;
                    }
                    break;
                  }
                  case 'x' : {
                    int val = va_arg( args, int );
                    EchoAPIString( "0x" );
                    itoa( val, buffer, 16 );
                    EchoAPIString( buffer );
                    break;
                  }
                  default : {
                    EchoAPIString( "*** BAD %" );
                    EchoAPIChar( *text );
                    EchoAPIString( " ***" );
                    break;
                  }
                }
            } else {
                EchoAPIChar( *text );
            }
        }
    }
}

cg_name EchoAPICgnameReturn // EchoAPI cg_name RETURN VALUE
    ( cg_name retn )            // - cg_name value
{
    EchoAPI( " -> %n\n", retn );
    return retn;
}

call_handle EchoAPICallHandleReturn // EchoAPI call_handle RETURN VALUE
    ( call_handle retn )            // - call_handle value
{
    EchoAPI( " -> %C\n", retn );
    return retn;
}

cg_type EchoAPICgtypeReturn // EchoAPI cg_type RETURN VALUE
    ( cg_type retn )            // - cg_type value
{
    EchoAPI( " -> %t\n", retn );
    return retn;
}


int EchoAPIHexReturn      // EchoAPI HEXADECIMAL RETURN VALUE
    ( int retn )                // - value
{
    EchoAPI( " -> %x\n", retn );
    return retn;
}


int EchoAPIIntReturn      // EchoAPI DECIMAL RETURN VALUE
    ( int retn )                // - value
{
    EchoAPI( " -> %i\n", retn );
    return retn;
}


select_node *EchoAPISelHandleReturn // EchoAPI sel_handle RETURN VALUE
    ( select_node *retn )         // - sel_handle value
{
    EchoAPI( " -> %S\n", retn );
    return retn;
}


call_handle EchoAPITempHandleReturn // EchoAPI temp_handle RETURN VALUE
    ( temp_handle retn )            // - temp_handle value
{
    EchoAPI( " -> %T\n", retn );
    return retn;
}


// USE HANDLE EXACTLY ONCE

void handleUseOnce( handle_type hdltype, use_info *useinfo )
{
    handleExists( hdltype, useinfo );
    if( useinfo->used ) {
        handleMsg( hdltype, "handle already used" );
    } else {
        useinfo->used = TRUE;
    }
}

static void handleSetup( handle_type hdltype, use_info *useinfo )
{
    useinfo->hdltype = hdltype;
    useinfo->used = FALSE;
}

void handleAddReuse( handle_type hdltype, use_info *useinfo )
{
    if( useinfo->hdltype == hdltype ) {
        if( useinfo->used ) {
            handleSetup( hdltype, useinfo );
        } else {
            handleMsg( hdltype, "Handle already exists" );
        }
    } else {
        handleSetup( hdltype, useinfo );
    }
}

void handleAdd( handle_type hdltype, use_info *useinfo )
{
    if( useinfo->hdltype != NO_HANDLE || useinfo->used ) {
        handleMsg( hdltype, "Handle already exists" );
    } else {
        handleSetup( hdltype, useinfo );
    }
}

void hdlAddUnary         // ADD A HANDLE AFTER A UNARY OPERATION
    ( handle_type hdltype       // - the handle type
    , tn handle                 // - the handle
    , tn old )                  // - handle operated upon by unary oper'n
{
    old = old;
    hdlAddReuse( hdltype, handle );
}

void hdlAddBinary        // ADD A HANDLE AFTER A BINARY OPERATION
    ( handle_type hdltype
    , tn handle                // - the handle
    , tn old_l                 // - handle operated upon by unary oper'n
    , tn old_r )               // - handle operated upon by unary oper'n
{
    old_l = old_l;
    old_r = old_r;
    hdlAddReuse( hdltype, handle );
}

void hdlAddTernary       // ADD A HANDLE AFTER A TERNARY OPERATION
    ( handle_type hdltype
    , tn handle                // - the handle
    , tn old_t                 // - handle operated upon by unary oper'n
    , tn old_l                 // - handle operated upon by unary oper'n
    , tn old_r )               // - handle operated upon by unary oper'n
{
    old_t = old_t;
    old_l = old_l;
    old_r = old_r;
    hdlAddReuse( hdltype, handle );
}

void handleExists           // VERIFY EXISTING HANDLE
    ( handle_type hdltype
    , use_info *useinfo )              // - the handle
{
    if( useinfo == NULL ) {
        handleMsg( hdltype, "handle does not exist" );
    }
    if( useinfo->hdltype != hdltype ) {
        ErrExpectFound( hdltype, useinfo->hdltype );
    }
}

void hdlAllUsed          // VERIFY ALL HANDLES IN RING HAVE BEEN USED
    ( handle_type hdltype )
{
    // check to ensure that all nodes of type hdltype are used LMW
    // create linked list of node when creating, and check all are empty
    // after tree is burned
    hdltype = hdltype;
}

// Callback stuff

typedef struct cb_name CB_NAME;
struct cb_name                  // CB_NAME -- call back name
{   CB_NAME* next;              // - next in ring
    char const* name;           // - name
    void (*rtn)( void* );       // - rtn address
};
static CB_NAME* callback_names; // call-back names

// CALL-BACK NAMES

void CgEchoAPICbName                // REGISTER A CALL-BACK NAME
    ( void (*rtn)( void* )      // - rtn address
    , char const* name )        // - rtn name
{
    CB_NAME* cbn;

    _Alloc( cbn, sizeof( CB_NAME ) ); // Free'd in DbgFini
    cbn->rtn = rtn;
    cbn->name = name;
    // insert
    cbn->next = callback_names;
    callback_names = cbn;
}

void EchoAPIInit()
{
    callback_names = NULL;
    EchoAPIRedirect();
}

// remove callback name from list

void EchoAPIFini()
{
    CB_NAME *cbn, *next;               // - call back name

    for( cbn = callback_names; cbn; cbn = next ) {
        next = cbn->next;
        _Free( cbn, sizeof( CB_NAME ) );
    }
    callback_names = NULL;
    EchoAPIUnredirect();
}

static char* callBackName       // MAKE CALL-BACK NAME FOR PRINTING
    ( void (*rtn)( void* )      // - rtn address
    , char * buffer )           // - buffer
{
    CB_NAME* cbn;               // - call back name
    char const* name = NULL;    // - name to be used

    for( cbn = callback_names; cbn; cbn = cbn->next ) {
        if( rtn == cbn->rtn ) {
            name = cbn->name;
            break;
        }
    }
    if( name == NULL ) {
        sprintf( buffer, "%x", rtn );
    } else {
        sprintf( buffer, "%x:%s", rtn, name );
    }
    return buffer;
}


void EchoAPICallBack(               // CALL-BACK SUPPORT
    tn node
  , cg_callback rtn
  , callback_handle param
  , char *start_end )            // - call-back entry
{
    char buffer[64];

    EchoAPI( "\n*** CALL BACK[%x] to %c(%x) "
          , node
          , callBackName( rtn, buffer )
          , param );
    EchoAPI(start_end);
}
