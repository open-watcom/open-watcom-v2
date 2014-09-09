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
* Description:  C++ debugging definitions.
*
****************************************************************************/


#ifndef NDEBUG

#include "dbgzap.h"
#include "iosupp.h"

// PROTOTYPES:

void* DbgCallGraph
    ( void )
;
PTREE DbgCommaInsertion         // COMMA INSERTION ROUTINE
    ( PTREE expr )              // - expression
;
void DbgDumpStateEntry(         // DUMP STATE ENTRY
    void* arg )                 // - state entry
;
void DbgDumpStateTable(         // DUMP STATE TABLE
    void* arg )                 // - control info
;
void DbgDumpStateTableDefn(     // DUMP STATE TABLE DEFINITION
    void* arg )                 // - definition
;
void DbgDumpTokenLocn           // DUMP A TOKEN_LOCN
    ( void const* locn )        // - the location
;
void DbgForgetScope(            // FORGET A SCOPE
    SCOPE scope )               // - scope
;
void DbgGenned(                 // INDICATE SYMBOL GENERATED
    SYMBOL sym )                // - the symbol
;
void DbgHeapFini                // COMPLETION
    ( void )
;
void DbgHeapInit                // INITIALIZATION
    ( void )
;
const char *DbgIcOpcode(        // GET IC OPCODE
    CGINTEROP opcode )          // - opcode
;
void DbgLogBeg                  // START LOGGING
    ( void )
;
void DbgLogBegFile              // START LOGGING TO A FILE
    ( char const * fname )      // - file name
;
int DbgLogEnd                   // END LOGGING
    ( void )
;
const char *DbgOperator(        // GET CGOP NAME
    CGOP number )               // - index for name
;
void DbgPrintPTREE(             // PRINT A PARSE-TREE BEAUTIFULLY
    PTREE root )                // - root of tree to be printed
;
void DbgRedirectBeg             // START REDIRECTION
    ( void )
;
int DbgRedirectEnd              // COMPLETE REDIRECTION
    ( void )
;
void DbgRememberScope(          // REMEMBER A NEWLY-DEFINED SCOPE
    SCOPE new_scope )           // - scope
;
char const * DbgSeName          // DUMP DTC_... name
    ( DTC_KIND se_type )        // - type of state entry
;
void DbgSetState(               // PRINT STATE VALUE SET, IF REQ'D
    const char* msg,            // - message text
    void* )                     // - state entry
;
void DumpCgFront(               // DUMP GENERATED CODE
    const char *prefix,         // - name added to print line
    DISK_ADDR disk_blk,         // - disk block
    DISK_OFFSET offset,         // - disk offset
    void *instruction )         // - intermediate code
;
void DumpClassInfo(             // DUMP CLASSINFO
    CLASSINFO *ci )             // - class information
;
void DumpCommentary(            // DUMP COMMENTARY LINE
    const char *comment )       // - comment
;
void DumpFullType(              // DUMP TYPE AND ALL TYPES IN LIST
    TYPE tp )                   // - type pointer
;
void DumpInitInfo(              // DUMP DATA INIT INFORMATION
    void *info )                // - stack entry to dump
;
void DumpMacPush(               // DUMP PUSH OF MACRO
    const void *p_mac,          // - macro being pushed
    const void **p_args )       // - arguments
;
void DumpMacToken(              // DUMP A MACRO TOKEN
    void )
;
void DumpMDefn(                 // DUMP MACRO DEFINITION
    char *p )                   // - definition
;
void DumpMemberPtrInfo(         // DUMP MEMBER_PTR_CAST STRUCTURE
    MEMBER_PTR_CAST *info )     // - the information
;
void DumpPTree(                 // DUMP A PARSE TREE
    PTREE node )                // - node in parse tree
;
void DumpScope(                 // DUMP SCOPE INFO FOR SYMBOL
    SCOPE scope )               // - scope
;
void DumpScopeInfo(             // DUMP INFORMATION FOR SCOPE
    SCOPE scope )               // - starting scope
;
void DumpScopes(                // DUMP ALL SCOPES
    void )
;
void DumpHashStats(             // DUMP HASH TABLE STATISTICS
    void )
;
void FtabDump()                 // DUMP FUNCTION STATE TABLE
;
void DumpSymbol(                // DUMP SYMBOL ENTRY
    void *_sym )                // - symbol
;
void DumpSymbolName(            // DUMP SYMBOL_NAME ENTRY
    SYMBOL_NAME sn )            // - symbol name entry
;
void DumpSymInfo(               // DUMP COMPLETE INFO FOR SYMBOL
    SYMBOL sym )                // - symbol
;
char *DbgSymNameFull(           // GET FULL SYMBOL NAME
    SYMBOL sym,                 // - symbol
    VBUF *vbuf )                // - variable length buffer
;
char *DbgSymNameShort(          // GET Short SYMBOL NAME
    SYMBOL sym )                // - symbol
;
void DumpToken(                 // DUMP TOKEN
    void )
;
void DumpType(                  // DUMP TYPE ENTRY
    TYPE tp )                   // - type pointer
;
void PrintType(                 // PRINT TYPE ENTRY
    TYPE tp )                   // - type pointer
;
void PrintFullType(             // PRINT FULL TYPE INFORMATION
    TYPE tp )                   // - type pointer
;
void DumpTemplateInfo(          // DUMP A TEMPLATE INFO
    TEMPLATE_INFO *tinfo )      // - template info
;
void DumpTemplateSpecialization(// DUMP A TEMPLATE SPECIALIZATION
    TEMPLATE_SPECIALIZATION *tspec )// - template specialization
;

#else
//
// Nullify debugging calls
//
#define DbgHeapFini()
#define DbgHeapInit()
#define DumpMacPush( a, b )
#define DumpMacToken()
#define DbgSetState( a, b )

#endif
