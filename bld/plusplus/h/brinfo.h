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


#ifndef __BRINFO_H__
#define __BRINFO_H__

// BRINFO.H -- definitions for Optima-style browsing

#ifdef OPT_BR

#include "macro.h"

void BrinfCloseScope            // CLOSE A SCOPE
    ( SCOPE scope )             // - scope to be closed
;
void BrinfCloseSource           // CLOSE A SOURCE FILE
    ( SRCFILE srcfile )         // - file being closed
;
void BrinfCmdLine               // INDICATE CMD-LINE BEING PROCESSED
    ( void )
;
void BrinfDeclClass             // VALID CLASS HAS BEEN DECLARED
    ( TYPE cltype )             // - class type
;
MEPTR BrinfDeclMacro            // VALID MACRO HAS BEEN DECLARED
    ( MEPTR mac )               // - macro defined
;
void BrinfDeclNamespace         // VALID NAMESPACE HAS BEEN DECLARED
    ( SYMBOL ns )               // - namespace defined
;
void BrinfDeclSymbol            // VALID SYMBOL HAS BEEN DECLARED
    ( SYMBOL tc )               // - symbol defined
;
void BrinfDeclTemplateClass     // VALID CLASS TEMPLATE HAS BEEN DECLARED
    ( SYMBOL tc )               // - template defined
;
void BrinfDeclTypedef           // VALID TYPEDEF HAS BEEN DECLARED
    ( SYMBOL tc )               // - namespace defined
;
bool BrinfDependsMacroDefined   // DEPENDENCY: MACRO DEFINED OR NOT
    ( bool defed                // - whether defined or not
    , char const * name         // - characters in name
    , unsigned nlen )           // - name length
;
void BrinfIncludeSource         // INCLUDE A SOURCE
    ( char const * fname        // - file name (scanned)
    , TOKEN_LOCN * locn )       // - place
;
void BrinfInit                  // START OF PROCESSING FOR BROWSE INFO
    ( bool )                    // - true ==> primary initialization
;
void BrinfMacroRelease          // RELEASE BROWSE MACRO-ING WHEN NO BROWSING
    ( void )
;
void BrinfOpenScope             // OPEN A SCOPE
    ( SCOPE scope )             // - opened scope
;
void BrinfOpenSource            // OPEN A SOURCE
    ( SRCFILE srcfile )         // - source file
;
unsigned long BrinfPch          // WRITE BROWSING INTO PCH
    ( int handle )              // - file handle
;
void BrinfPchRead               // PCH BROWSING TO BE STARTED
    ( void )
;
MEPTR BrinfReferenceMacro       // MACRO REFERENCE
    ( MEPTR mac )               // - macro accessed
;
void BrinfReferenceSymbol       // SYMBOL REFERENCE
    ( TOKEN_LOCN* locn          // - location of access
    , SYMBOL sym )              // - symbol accessed
;
void BrinfUndefMacro            // UNDEFINE A MACRO
    ( MEPTR mac )               // - that macro
;
void BrinfWrite                 // WRITE OUT BROWSE INFORMATION
    ( void )
;
void MacroWriteBrinf            // WRITE MACROS TO BRINF
    ( void )
;
#else
// disable BrInfo module
#define BrinfCloseScope(s)
#define BrinfCloseSource(s)
#define BrinfDeclClass(i)
#define BrinfDeclMacro(m)
#define BrinfDeclNamespace(n)
#define BrinfDeclSymbol(s)
#define BrinfDeclTemplateClass(c)
#define BrinfDeclTypedef(t)
#define BrinfDependsMacroDefined(d,n,s)
#define BrinfIncludeSource(n,l)
#define BrinfInit( b )
#define BrinfOpenScope(s)
#define BrinfOpenSource(n)
#define BrinfPch( h ) 0
#define BrinfMacroDependsDefined(d,n,l)
#define BrinfMacroRelease()
#define BrinfReferenceMacro(m) m
#define BrinfReferenceSymbol(l,s)
#define BrinfUndefMacro(m)
#define BrinfWrite()
#endif


#endif
