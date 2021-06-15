/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
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
#include "cgstd.h"
#include "cgaux.h"
#include "memmgr.h"
#include "callinfo.h"


#define MAX_POSSIBLE_REG        8

#define MAXIMUM_PARMSETS        32

typedef struct aux_entry AUX_ENTRY;
typedef struct aux_info AUX_INFO;

typedef struct aux_entry {
    AUX_ENTRY       *next;
    AUX_INFO        *info;
    char            name[1];
} aux_entry;

typedef struct inline_funcs {
    char            *name;      /* func name */
    byte_seq        *code;      /* sequence of bytes */
    hw_reg_set      *parms;     /* parameter information */
    hw_reg_set      returns;    /* function return information */
    hw_reg_set      save;       /* registers not modified */
} inline_funcs;

typedef struct alt_inline_funcs {
    byte_seq        *byteseq;
    inline_funcs    alt_ifunc;
} alt_inline_funcs;

global AUX_ENTRY        *AuxList;
global AUX_ENTRY        *CurrEntry;
global AUX_INFO         *CurrAlias;
global AUX_INFO         *CurrInfo;

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
AUX_ENTRY *AuxLookup(           // LOOK UP AUX ENTRY
    const char *name )
;
const char *AuxRetrieve(        // RETRIEVE NAME OF AUX ENTRY
    AUX_INFO *pragma )
;
void CgInfoAddPragmaExtrefS(    // ADD EXTREF FOR PRAGMA'D NAME
    SYMBOL sym )
;
void CgInfoAddPragmaExtrefN(    // ADD EXTREF FOR PRAGMA'D NAME
    char *name )
;
void CreateAux(                 // CREATE AUX ID
    const char *id )            // - id
;
void DumpObjectModelClass(      // DUMP OBJECT MODEL: CLASS
    TYPE type )                 // - structure type
;
void DumpObjectModelEnum(       // DUMP OBJECT MODEL: ENUM
    TYPE type )                 // - enum type
;
bool IsPragmaAborts(            // TEST IF FUNCTION NEVER RETURNS
    SYMBOL sym )                // - function symbol
;
bool IsFuncAborts(              // TEST IF FUNCTION NEVER RETURNS
    SYMBOL func )               // - function symbol
;
struct textsegment *LkSegName(  // LOOKUP SEGMENT NAME
    char *name )                // - segment name
;
void PragAux(                   // #PRAGMA AUX ...
    void )
;
AUX_INFO *PragmaAuxAlias(       // LOCATE ALIAS FOR PRAGMA
    const char *name )
;
void PragmaAuxEnding(           // PROCESS END OF PRAGMA
    bool set_sym )              // - true ==> set SYMBOL's aux_info
;
void PragmaExtrefsInject        // INJECT EXTREFS FOR PRAGMAS
    ( void )
;
void PragmaExtrefsValidate      // VALIDATE EXTREFS FOR PRAGMAS
    ( void )
;
void PragmaAuxInit(
    void )
;
void PragManyRegSets(           // GET PRAGMA REGISTER SETS
    void )
;
bool PragmaChangeConsistent(    // TEST IF PRAGMA CHANGE IS CONSISTENT
    AUX_INFO *oldp,             // - pragma, old
    AUX_INFO *newp )            // - pragma, new
;
bool PragmasTypeEquivalent(     // TEST IF TWO PRAGMAS ARE TYPE-EQUIVALENT
    AUX_INFO *inf1,             // - pragma [1]
    AUX_INFO *inf2 )            // - pragma [2]
;
bool PragmaOKForVariables(      // TEST IF PRAGMA IS SUITABLE FOR A VARIABLE
    AUX_INFO *datap )           // - pragma
;
bool PragmaOKForInlines(        // TEST IF PRAGMA IS SUITABLE FOR INLINED FN
    AUX_INFO *fnp )             // - pragma
;
void PragmaSetToggle(           // SET TOGGLE
    const char *name,           // - toggle name
    int func,                   // - -1/0/1 ==> func pop/off/on
    bool push )                 // - true ==> push current value on stack
;
void PragObjNameInfo(           // RECOGNIZE OBJECT NAME INFORMATION
    void )
;
bool PragRecogId(               // RECOGNIZE PRAGMA ID
    const char *what )          // - id
;
bool PragRecogName(             // RECOGNIZE PRAGMA NAME
    const char *what )          // - name
;
int PragRegIndex(               // GET PRAGMA REGISTER INDEX
    const char *registers,
    const char *name,
    size_t len,
    bool ignorecase )
;
int PragRegNumIndex(            // GET PRAGMA REGISTER NUMBER INDEX (RISC CPU)
    const char *str,
    size_t len,
    int max_reg )
;
void PragRegNameErr(
    const char *regname,
    size_t regnamelen )
;
hw_reg_set PragRegList(         // GET PRAGMA REGISTER SET
    void )
;
hw_reg_set PragRegName(         // GET REGISTER NAME
    const char *regname,        // - register name
    size_t regnamelen )         // - register name len
;
bool ReverseParms(              // ASK IF PRAGMA REQUIRES REVERSED PARMS
    AUX_INFO * pragma )         // - pragma
;
void SetCurrInfo(               // SET CURRENT INFO. STRUCTURE
    const char *name )
;

void AsmSysPCHWriteCode(        // write code sequence to PCH
    AUX_INFO *info )            // - code sequence
;
void AsmSysPCHReadCode(         // read code sequence from PCH
    AUX_INFO *info )            // - code sequence
;
const char *SkipUnderscorePrefix(
    const char *str,
    size_t *len,
    bool iso_compliant_names )
;
bool GetPragmaAuxAliasInfo(
    void )
;
void GetPragmaAuxAlias(
    void )
;
#endif
