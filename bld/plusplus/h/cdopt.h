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


#ifndef __CDOPT_H__
#define __CDOPT_H__

#define CDOPT
#define CDOPT_OPEQ

// CDOPT.H -- definitions for CTOR/DTOR optimizations
//
// 93/04/18 -- J.W.Welch        -- defined


typedef struct cd_descr CD_DESCR;
typedef struct cdopt_iter CDOPT_ITER;

#define TOB_DEFS                                           \
 TOB_DEF( TOB_VBASE )           /* - virtual base       */ \
,TOB_DEF( TOB_DBASE )           /* - direct base        */ \
,TOB_DEF( TOB_MEMB  )           /* - member: atomic     */ \
,TOB_DEF( TOB_ARRAY )           /* - member: array      */ \
,TOB_DEF( TOB_ORIG  )           /* - original object    */ \
,TOB_DEF( TOB_NONE  )           /* - nothing left       */

#define TITER_DEFS                                                     \
 TITER_DEF( TITER_NONE        ) /* - none left                      */ \
,TITER_DEF( TITER_ARRAY_EXACT ) /* - array (exact)                  */ \
,TITER_DEF( TITER_ARRAY_VBASE ) /* - array (in vbase)               */ \
,TITER_DEF( TITER_CLASS_EXACT ) /* - class (exact)                  */ \
,TITER_DEF( TITER_CLASS_VBASE ) /* - class (in vbase)               */ \
,TITER_DEF( TITER_CLASS_DBASE ) /* - class (in dbase)               */ \
,TITER_DEF( TITER_MEMB        ) /* - expanding thru member          */ \
,TITER_DEF( TITER_DBASE       ) /* - expanding thru direct base     */ \
,TITER_DEF( TITER_VBASE       ) /* - expanding thru virtual base    */ \
,TITER_DEF( TITER_NAKED_DTOR  ) /* - naked dtor                     */

typedef enum {                  // TOB -- type of object in original class
    #define TOB_DEF(a) a
    TOB_DEFS
    #undef TOB_DEF
,   MAX_TOB_DEF
} TOB;

typedef enum {                  // TITER -- type of iteration element
    #define TITER_DEF(a) a
    TITER_DEFS
    #undef TITER_DEF
,   MAX_TITER_DEF
} TITER;

// PROTOTYPES:


void CDoptBackEnd(              // START OF BACK-END PROCESSING
    void )
;
void CDoptChkAccFun(            // CHECK FUNCTIONS ACCESS FOR CLASS
    CD_DESCR* descr )           // - descriptor for a class
;
CD_DESCR* CDoptDefCtorBuild(    // BUILD LIST OF OBJECTS TO BE DEFAULT CTOR'ED
    TYPE cltype )               // - type for dtor
;
CD_DESCR* CDoptDtorBuild(       // BUILD LIST OF OBJECTS TO BE DTOR'D
    TYPE cltype )               // - type for dtor
;
boolean CDoptErrorOccurred(     // TEST IF ERROR OCCURRED
    CD_DESCR* info )
;
boolean CDoptInlinedMember(     // DETERMINE IF INLINED MEMBER
    CDOPT_ITER* iter )          // - iteration data
;
CDOPT_ITER* CDoptIterBeg(       // START OF ITERATION
    CD_DESCR* info )            // - information for class
;
boolean CDoptIterCannotDefine(  // CHECK FOR UNDEFINEABLE CTOR
    CDOPT_ITER* iter )          // - iteration data
;
void CDoptIterEnd(              // COMPLETION OF ITERATION
    CDOPT_ITER* iter )          // - iteration data
;
SYMBOL CDoptIterFunction(       // GET FUNCTION SYMBOL FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
;
TITER CDoptIterNextComp(        // GET NEXT COMPONENT
    CDOPT_ITER* iter )          // - iteration data
;
TITER CDoptIterNextElement(     // GET NEXT ELEMENT FOR COMPONENT
    CDOPT_ITER* iter )          // - iteration data
;
unsigned CDoptObjectKind        // DETERMINE DTC_... OBJECT KIND
    ( CDOPT_ITER* iter )        // - iteration data
;
target_size_t CDoptIterOffsetComp( // GET EXACT OFFSET OF CURRENT COMPONENT
    CDOPT_ITER* iter )          // - iteration data
;
target_size_t CDoptIterOffsetExact( // GET EXACT OFFSET OF CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
;
TYPE CDoptIterType(             // GET TYPE FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
;
boolean CDoptIterExact(         // GET cdarg "exact" FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
;
BASE_CLASS* CDoptIterVBase(     // GET VIRTUAL BASE CLASS FOR CURRENT ELEMENT
    CDOPT_ITER* iter )          // - iteration data
;
void CDoptNoAccFun(             // INDICATE FUNCTIONS ACCESSED FOR CLASS
    CD_DESCR* descr )           // - descriptor for a class
;
CD_DESCR* CDoptDefOpeqBuild(    // BUILD LIST OF OBJECTS TO BE DEFAULT OP='ED
    TYPE cltype )               // - type for dtor
;

#endif
