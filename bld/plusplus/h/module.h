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


#ifndef __MODULE_H__
#define __MODULE_H__

// MODULE.H -- functions for module processing
//
// 92/03/10 -- J.W.Welch        -- defined

unsigned LocalDtorCount(        // GET # OF LOCAL SYMBOLS TO DTOR
    void )
;
SYMBOL LocalDtorSymbol(         // GET NEXT DTOR FOR LOCAL INITIALIZATION
    SYMBOL *sym )               // - symbol to be DTOR'ed
;
unsigned LocalStaticDtor(       // RECORD DTORABLE LOCAL STATIC
    SYMBOL sym )                // - symbol to be DTOR'ed
;
unsigned ModuleDtorCount(       // GET # OF MODULE SYMBOLS TO DTOR
    void )
;
SYMBOL ModuleDtorSymbol(        // GET NEXT DTOR FOR MODULE
    SYMBOL *sym )               // - symbol to be DTOR'ed
;
void ModuleInitConnect(         // CODE GENERATION FOR MODULE-INIT. CONNECTION
    void )
;
void ModuleInitFini(            // COMPLETE MODULE-INITIALIZATION FUNCTION
    void )
;
SYMBOL ModuleInitFuncSym(       // GET SYMBOL FOR MODULE-INITIALIZATION FUNC.
    void )
;
void ModuleInitUsed(            // FLAG MODULE-INITIALIZATION FUNCTION AS USED
    void )
;
SCOPE ModuleFnScope(            // SCOPE MOD-INIT FN IS DEF'D IN
    void )
;
void ModuleInitInit(            // START MODULE-INITIALIZATION FUNCTION
    void )
;
SCOPE ModuleInitResume(         // RESUME MODULE-INITIALIZATION FUNCTION
    void )
;
void ModuleInitResumeScoped(    // RESUME SCOPED MODULE-INITIALIZATION FUNC.
    SCOPE scope )               // - scope before function
;
void ModuleInitRestore(         // UNDO MAKING MODULE-INIT FUNCTION A MEMBER FUNCTION
    SCOPE restore_scope )       // - value returned by ModuleInitResume
;
SCOPE ModuleInitScope(          // GET BLOCK SCOPE FOR MODULE INITIALIZATION
    void )
;

bool ModuleIsZap1( CGFILE_INS *p );
void ModuleAdjustZap1( CGFILE * );
void *ModuleIsZap2( CGFILE_INS *p );
void ModuleAdjustZap2( CGFILE *, void *h );

#endif
