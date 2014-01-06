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


#ifndef __SCOPERES_H__
#define __SCOPERES_H__

// SCOPERES -- scopes resolution
//
// 94/11/25 -- J.W.Welch        -- defined


// PROTOTYPES:

void CgResCall                  // ADD: CALL TO RESOLVE
    ( CALLNODE* caller          // - caller
    , SYMBOL fun )              // - function
;
void CgResFini                  // COMPLETION
    ( void )
;
void CgResInit                  // INITIALIZATION
    ( void )
;
void CgResolve                  // RESOLVE ANY PENDING ACTIONS
    ( void )
;
bool CgResolveNonThrow          // RESOLVE A FUNCTION AS NON-THROW
    ( CALLGRAPH* ctl            // - call graph
    , CALLNODE* node )          // - possible non-throw
;
void CgResScBlkScanBegin        // START SCANNING OF A BLOCK SCOPE
    ( SCOPE scope               // - scope
    , CALLNODE* caller          // - caller
    , DT_METHOD dtm )           // - dtor method for function
;
void CgResScopeCall             // RESOLVING SCOPE-CALL
    ( CALLNODE* owner           // - function being scanned
    , SYMBOL ctor               // - NULL or ctoring function
    , SYMBOL dtor_component     // - NULL or dtor: component
    , SYMBOL dtor_temporary     // - NULL or dtor: temporary
    , SYMBOL dtor_scope )       // - NULL or dtor: scope
;
void CgResScopeGen              // FORCE GENERATION OF CURRENT SCOPE
    ( CALLNODE* owner )         // - function being scanned
;
void CgResScopeThrow            // SCOPE CONTAINS THROW
    ( CALLNODE* owner )
;
SCOPE CgResScScanEnd            // COMPLETE SCANNING OF A SCOPE
    ( void )
;
void CgResScStmtScanBegin       // START SCANNING OF A STATEMENT SCOPE
    ( SCOPE scope               // - scope
    , CALLNODE* caller          // - caller
    , DT_METHOD dtm )           // - dtor method for function
;

#endif
