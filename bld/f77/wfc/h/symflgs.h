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
* Description:  Definition of "flags" field of symbol table entry
*
****************************************************************************/


//
//+===================================================================+
//|15              |14              |13              |12              |
//|----------------|----------------|---------------------------------|
//|                |                |                                 |
//|                |                |              class              |
//|                |                |                                 |
//|     type       |    usage       |     common block        (11)    |
//|  established   |  establised    |     parameter constant  (10)    |
//|                |                |     subprogram          (01)    |
//|                |                |     variable            (00)    |
//|                |                |                                 |
//|===================================================================|
//|11              |10              | 9              | 8              |
//|----------------|----------------|----------------|----------------|
//|                |                |                |do-loop variable|
//|  primary       |  secondary     |  subprogram    |                |
//|  entry point   |  entry point   |  argument      |----------------|
//|                |                |                |  searched for  |
//|                |                |                |   in library   |
//|-------------------------------------------------------------------|
//|  common block  |  common block  |     blank      |  common block  |
//|     loaded     |  initialized   |  common block  |   allocated    |
//|===================================================================|
//| 7              | 6              | 5              | 4              |
//|----------------|----------------|----------------|----------------|
//|   in common    |  equivalenced  |  subscripted   |    shadowed    |
//|                |                |   variable     |     symbol     |
//|----------------|----------------|---------------------------------|
//|   intrinsic    |                | 1st bit          2nd bit        |
//|                |    external    |          subprogram type        |
//|----------------|                |---------------------------------|
//|  common block  |(weak external) |common block has|common block has|
//|  size warning  |                |relocation list |  equiv'd name  |
//|===================================================================|
//| 3              | 2              | 1              | 0              |
//|----------------|----------------|----------------|----------------|
//|  appeared in   |     DATA       |   referenced   |    SAVEd       |
//|dim. expression |  initialized   |     symbol     |                |
//|-------------------------------------------------------------------|
//|  3rd bit       |  remote block  |   referenced   |    unnamed     |
//|subprogtype     |    defined     |     symbol     |   subprogram   |
//|----------------|----------------|       or       |----------------|
//|                | common block   |    address     |    SAVEd       |
//|                | in block data  |    assigned    |                |
//+===================================================================+

#define SY_TYPE          0x8000U        // type established bit
#define SY_USAGE         0x4000U        // usage established
#define SY_CLASS         0x3000U        // symbol class
#define SY_COMMON        0x3000U        // - a common block          (11)
#define SY_PARAMETER     0x2000U        // - a parameter constant    (10)
#define SY_SUBPROGRAM    0x1000U        // - a subprogram            (01)
#define SY_VARIABLE      0x0000U        // - a variable              (00)

#define SY_SUB_PARM      0x0200U        // subprogram parameter
#define SY_REFERENCED    0x0002U        // symbol referenced
#define SY_SAVED         0x0001U        // appeared in SAVE statement
                                        // - for variables and common blocks

// VARIABLE
// ========

#define SY_DO_PARM       0x0100U        // DO parameter
#define SY_VALUE_PARM    0x0100U        // dummy argument is passed by value
#define SY_IN_EC         0x00C0U        // equivalenced and/or in common block
#define SY_IN_COMMON     0x0080U        // - occurs in COMMON block list
#define SY_IN_EQUIV      0x0040U        // - occurs in EQUIVALENCE list
#define SY_SUBSCRIPTED   0x0020U        // subscripted variable
#define SY_SPECIAL_PARM  0x0010U        // statement function dummy argument
                                        // implied DO variable
                                        // function return value
#define SY_IN_DIMEXPR    0x0008U        // used in dimension expression
#define SY_DATA_INIT     0x0004U        // initialized in DATA statement

// SUBPROGRAM
// ==========

#define SY_PS_ENTRY      0x0c00U        // primary or secondary entry point
#define SY_PENTRY        0x0800U        // - primary entry point
#define SY_SENTRY        0x0400U        // - secondary entry point
#define SY_RT_ROUTINE    0x0100U        // subprogram is a run-time routine
#define SY_INTRINSIC     0x0080U        // subprogram was INTRINSIC'ed
#define SY_EXTERNAL      0x0040U        // subprogram was EXTERNAL'ed
#define SY_SUBPROG_IDX   3              // shift SY_SUBPROG_TYPE bits by this
                                        // to make them index 0
#define SY_SUBPROG_TYPE  0x0038U        // kind of subprogram is ...
#define SY_PROGRAM       0x0030U        // - program                (110)
#define SY_BLOCK_DATA    0x0028U        // - block data             (101)
#define SY_FUNCTION      0x0020U        // - function               (100)
#define SY_REMOTE_BLOCK  0x0018U        // - remote block           (011)
#define SY_SUBROUTINE    0x0010U        // - subroutine             (010)
#define SY_STMT_FUNC     0x0008U        // - statement function     (001)
#define SY_FN_OR_SUB     0x0000U        // - function or subroutine (000)
#define SY_RB_DEFINED    0x0004U        // remote block defined
#define SY_RELAX_EXTERN  0x0004U        // don't force an external reference
#define SY_IF_ARGUMENT   0x0004U        // intrinsic function used as argument
#define SY_ADDR_ASSIGNED 0x0002U        // address assigned (for global symbols)
#define SY_UNNAMED       0x0001U        // unnamed program/block data

// PARAMETER
// =========

// COMMON
// ======

#define SY_COMMON_LOAD   0x0800U        // COMMON block from object file
#define SY_COMMON_INIT   0x0400U        // COMMON block initialized
#define SY_BLANK_COMMON  0x0200U        // blank COMMON block
#define SY_COMMON_ALLOC  0x0100U        // COMMON block allocated
#define SY_COMSIZE_WARN  0x0080U        // warning for named common block
                                        // having different sizes given
#define SY_EQUIVED_NAME  0x0010U        // common block has equivalenced name
#define SY_IN_BLOCK_DATA 0x0004U        // common block appeared in block data
                                        // subprogram

// Extra flags:
// ============

#define SY_DEFINED              0x01    // symbol has been defined
#define SY_FAKE_REFERENCE       0x02    // set when "noreference" option used
#define SY_VOLATILE             0x04    // set when variable specified as
                                        // target of IOSTAT or INQUIRE
                                        // specifiers
#define SY_ALLOCATABLE          0x08    // symbol is allocatable

