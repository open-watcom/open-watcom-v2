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
* Description:  Definitions for #pragma aux processing.
*
****************************************************************************/


#ifndef __PRAGDEFN_H_
#define __PRAGDEFN_H_

#define BY_CPP_FRONT_END
#include "cgaux.h"

typedef struct aux_entry AUX_ENTRY;
typedef struct aux_info AUX_INFO;

#include <pushpck1.h>
struct aux_entry {
    AUX_ENTRY       *next;
    AUX_INFO        *info;
    char            name[1];
};
#include <poppck.h>

typedef char aux_flags;
#define AUX_FLAG_FAR16  0x01

#include "memmgr.h"
#include "callinfo.h"

struct inline_funcs {
    char            *name;      /* func name */
    byte_seq        *code;      /* sequence of bytes */
    hw_reg_set      *parms;     /* parameter information */
    hw_reg_set      returns;    /* function return information */
    hw_reg_set      save;       /* registers not modified */
};

global AUX_ENTRY        *AuxList;
global AUX_ENTRY        *CurrEntry;
global AUX_INFO         *CurrAlias;
global AUX_INFO         *CurrInfo;
global AUX_INFO         *DftCallConv;

struct pragma_dbg_toggles  {
#define toggle_pick( id )       unsigned id : 1;
#include "dbgtogg.h"
};
global struct pragma_dbg_toggles  PragDbgToggle;


#define MAX_POSSIBLE_REG        8

#define MAXIMUM_PARMSETS        32

// PROTOTYPES FOR PRAGMA PROCESSING

hw_reg_set *AuxParmDup(         // DUPLICATE AUX PARMS
    hw_reg_set *parms )
;
char *AuxObjnameDup(            // DUPLICATE AUX OBJNAME
    char *objname )
;
void freeAuxInfo(               // FREE ALL AUX INFO MEM
    AUX_INFO *i )
;
struct aux_entry *AuxLookup(    // LOOK UP AUX ENTRY
    char *name )
;
char *AuxRetrieve(              // RETRIEVE NAME OF AUX ENTRY
    AUX_INFO *pragma )
;
void CgInfoAddPragmaExtref(     // ADD EXTREF FOR PRAGMA'D NAME
    SYMBOL sym )
;
void CreateAux(                 // CREATE AUX ID
    char *id )                  // - id
;
void DumpObjectModelClass(      // DUMP OBJECT MODEL: CLASS
    TYPE type )                 // - structure type
;
void DumpObjectModelEnum(       // DUMP OBJECT MODEL: ENUM
    TYPE type )                 // - enum type
;
boolean IsPragmaAborts(         // TEST IF FUNCTION NEVER RETURNS
    SYMBOL sym )                // - function symbol
;
struct textsegment *LkSegName(  // LOOKUP SEGMENT NAME
    char *name )                // - segment name
;
void PragAux(                   // #PRAGMA AUX ...
    void )
;
void PragCurrAlias(             // LOCATE ALIAS FOR PRAGMA
    void )
;
void PragEnding(                // PROCESS END OF PRAGMA
    boolean set_sym )           // - TRUE ==> set SYMBOL's aux_info
;
void PragmaExtrefsInject        // INJECT EXTREFS FOR PRAGMAS
    ( void )
;
void PragmaExtrefsValidate      // VALIDATE EXTREFS FOR PRAGMAS
    ( void )
;
void PragInit(
    void )
;
void PragInitDefaultInfo(
    void )
;
void PragManyRegSets(           // GET PRAGMA REGISTER SETS
    void )
;
boolean PragmaChangeConsistent( // TEST IF PRAGMA CHANGE IS CONSISTENT
    AUX_INFO *oldp,             // - pragma, old
    AUX_INFO *newp )            // - pragma, new
;
boolean PragmasTypeEquivalent(  // TEST IF TWO PRAGMAS ARE TYPE-EQUIVALENT
    AUX_INFO *inf1,             // - pragma [1]
    AUX_INFO *inf2 )            // - pragma [2]
;
boolean PragmaOKForVariables(   // TEST IF PRAGMA IS SUITABLE FOR A VARIABLE
    AUX_INFO *datap )           // - pragma
;
boolean PragmaOKForInlines(     // TEST IF PRAGMA IS SUITABLE FOR INLINED FN
    AUX_INFO *fnp )             // - pragma
;
void PragmaSetToggle(           // SET TOGGLE
    boolean set_flag )          // - TRUE ==> set flag
;
void PragObjNameInfo(           // RECOGNIZE OBJECT NAME INFORMATION
    void )
;
boolean PragRecog(              // RECOGNIZE PRAGMA ID
    char *what )                // - id
;
boolean PragIdCurToken(         // IS CURTOKEN AN ID?
    void )
;
hw_reg_set PragRegList(         // GET PRAGMA REGISTER SET
    void )
;
hw_reg_set PragRegName(         // GET REGISTER NAME
    char *str )                 // - register
;
int PragSet(                    // GET ENDING PRAGMA DELIMITER
    void )
;
boolean ReverseParms(           // ASK IF PRAGMA REQUIRES REVERSED PARMS
    AUX_INFO * pragma )         // - pragma
;
void SetCurrInfo(               // SET CURRENT INFO. STRUCTURE
    void )
;

void AsmSysPCHWriteCode(        // write code sequence to PCH
    AUX_INFO *info )            // - code sequence
;
void AsmSysPCHReadCode(         // read code sequence from PCH
    AUX_INFO *info )            // - code sequence
;

#endif
