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


#ifndef _FNBODY_H
#define _FNBODY_H

#include "label.h"

enum {
    CS_FUNCTION         = 0x01,
    CS_BLOCK            = 0x02,
    CS_IF               = 0x03,
    CS_ELSE             = 0x04,
    CS_WHILE            = 0x05,
    CS_DO               = 0x06,
    CS_FOR              = 0x07,
    CS_SWITCH           = 0x08,
    CS_TRY              = 0x09,
    CS_CATCH            = 0x0a,
    CS_NULL             = 0x00
};

#define BLOCK_CSTACK    (16)
#define BLOCK_SWCASE    (32)
#define BLOCK_FNLABEL   (8)
#define BLOCK_FNCATCH   (8)

enum special_func {
    FUNC_NO_STACK_CHECK         = 0x01, /* do not generate stack checking code */
    FUNC_NULL                   = 0x00
};

typedef uint_16         depth_t;
typedef uint_16         blk_count_t;

typedef struct switch_case SWCASE;
struct switch_case {                    // CASE IN SWITCH
    SWCASE              *next;          // - next in ring for switch
    uint_32             value;          // - value
    TOKEN_LOCN defined;                 // - where defined
};

typedef struct blk_label BLK_LABEL;
struct blk_label                        // LABEL IN BLOCK STRUCTURE
{   CGLABEL id;                         // - id for code generation
    LAB_DEF *defn;                      // - NULL, first ref, or label def'n
};

typedef struct fncatch FNCATCH;
struct fncatch                          // CATCH INFORMATION
{   FNCATCH *next;                      // - next in ring for try
    TYPE type;                          // - type of catch expression
    TOKEN_LOCN defined;                 // - where defined
};

typedef struct control_stack CSTACK;    // CONTROL STACK
struct control_stack {
    CSTACK              *next;          // - enclosing structure
    BLK_LABEL           outside;        // - outside label
    SYMBOL              try_var;        // - try variable for block
    TOKEN_LOCN          defined;        // - where block defined
    union {
        struct {                        // CS_IF
            BLK_LABEL   else_part;
        } i;
        struct {                        // CS_WHILE, CS_DO, CS_FOR
            CGLABEL     top_loop;
            BLK_LABEL   continue_loop;
        } l;
        struct {                        // CS_TRY
            FNCATCH     *catches;       // - ring of catches
            SCOPE       defn_scope;     // - scope containing try
            uint_32     catch_no;       // - # for catch
            SYMBOL      try_var;        // - try variable for try
            TOKEN_LOCN  try_locn;       // - location of try stmt.
            unsigned    catch_err : 1;  // - catch error detected
        } t;
        struct {                        // CS_SWITCH
            SWCASE      *cases;         // - ring of cases
            SCOPE       defn_scope;     // - scope containing switch
            SCOPE       imm_block;      // - immediate block for switch
            uint_32     lo;             // - low value
            uint_32     hi;             // - high value
            TYPE        type;           // - type of switch expression
            TOKEN_LOCN  switch_locn;    // - location of switch stmt.
            TOKEN_LOCN  default_locn;   // - location of default stmt.
            unsigned    is_signed : 1;  // - switch expression is signed
            unsigned    default_dropped : 1; // - default has occurred
            unsigned    block_after :1; // - block { .. } follows switch
        } s;
        struct {                        // CS_BLOCK
            blk_count_t block;          // - counter
            unsigned    block_switch :1;// - block { .. } following switch
        } b;
    } u;
    uint_8              id;             // - type of block
    unsigned            expr_true   : 1;// bracketted expr. was TRUE
    unsigned            expr_false  : 1;// bracketted expr. was FALSE
};

typedef struct fn_label FNLABEL;
struct fn_label {
    FNLABEL             *next;
    BLK_LABEL           destination;
    char                *name;
    blk_count_t         block;
    unsigned            defined : 1;
    unsigned            referenced : 1;
    unsigned            dangerous : 1;
};

typedef struct init_var INIT_VAR;
struct init_var                         // STATIC VARIABLE ONCE ONLY CONTROL
{   SYMBOL              var;            // symbol for bit mask
    unsigned            mask;           // mask for current bit
};

#include "fnbodydt.h"

extern void FunctionBody( DECL_INFO * );
extern boolean FunctionBodyCtor( void );
extern void FunctionBodyDeadCode( void );
extern INIT_VAR *FunctionBodyGetInit( FUNCTION_DATA * );
extern void FunctionBodyShutdown( SYMBOL, FUNCTION_DATA * );
extern void FunctionBodyStartup( SYMBOL, FUNCTION_DATA *, enum special_func );
extern void FunctionBodyDefargShutdown( FUNCTION_DATA * );
extern void FunctionBodyDefargStartup( SYMBOL, FUNCTION_DATA * );
extern PTREE FunctionCalled( PTREE, SYMBOL );
extern PTREE FunctionCouldThrow( PTREE );
extern void FunctionHasCtorTest( void );
extern type_flag FunctionThisQualifier( void );
extern void FunctionHasRegistration( void );
extern unsigned FunctionRegistrationFlag( void );
extern void FunctionMightThrow( void );
extern ACCESS_ERR** FunctionBodyAccessErrors();

void AccessErrClear(            // CLEAR FUNCTION OF ACCESS ERRORS
    void )
;
void AccessErrMsg(              // ISSUE PROTECTION MESSAGE
    int msg,                    // - message
    SYMBOL fun,                 // - function
    TOKEN_LOCN* a_locn )        // - addr[ error location ]
;
boolean FnRetnOpt(              // TEST IF RETURN OPTIMIZATION ACTIVE
    void )
;
boolean FnRetnOptimizable(      // TEST IF SYMBOL COULD BE OPTIMIZED AWAY
    SYMBOL sym )                // - candidate symbol
;
void FnRetnOptOff(              // TURN OFF RETURN OPTIMIZATION
    void )
;
void FnRetnOptSetSym(           // SET SYMBOL FOR RETURN OPTIMIZATION
    SYMBOL sym )                // - the symbol
;
SYMBOL FnRetnOptSym(            // GET SYMBOL FOR RETURN OPTIMIZATION
    void )
;
#endif
