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


#define AST_OFF         0x0000  // all bits off
#define AST_IO          0x0001  // i/o expression - array without list is OK
#define AST_SUB         0x0002  // subexpression - more proccessing needed
#define AST_CNA         0x0004  // call statement with no arguments
#define AST_CCR         0x0008  // condition codes need to be set
#define AST_ASF         0x0010  // arithmetic statement function
#define AST_EOK         0x0020  // equal sign OK in this expression
#define AST_MEQ         0x0040  // have already seen a valid "="
#define AST_MSG         0x0080  // multiple assignment extension issued
#define AST_CEX         0x0100  // constant expression required
#define AST_DIM         0x0200  // dimension expression required
#define AST_DIEXP       0x0400  // expression used to initialize in DATA stmt
#define AST_DEXP        0x0800  // expression to be initialized in DATA stmt
#define AST_ALT         0x1000  // alternate returns allowed on call
#define AST_ISIZEOF     0x2000  // set when ISIZEOF found during downscan
