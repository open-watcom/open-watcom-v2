/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2002-2023 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Import library management routines.
*
****************************************************************************/


#define COFFBASEDESCRIPTORSIZE 573
#define COFFBASENULLDESCRIPTORSIZE 127
#define COFFBASENULLTHUNKSIZE 148
#define COFFBASEAXPIMPORTSIZE 385
#define COFFBASEPPCIMPORTSIZE 656
#define COFFBASEAXPNAMEDSIZE 484
#define COFFBASEPPCNAMEDSIZE 755

extern char CoffDescrip1[];
extern char CoffDescrip2[];
extern char CoffNullDescriptor[];
extern char CoffNullThunk[];
extern char CoffAXPOrdinal[];
extern char CoffPPCOrdinal[];
extern char CoffAXPNamed1[];
extern char CoffAXPNamed2[];
extern char CoffPPCNamed1[];
extern char CoffPPCNamed2[];

#define CoffDescrip1_SIZE       0x174
#define CoffDescrip2_SIZE       0x7e
#define CoffNullDescriptor_SIZE 0x7d
#define CoffNullThunk_SIZE      0x7c
#define CoffAXPOrdinal_SIZE     0x15e
#define CoffPPCOrdinal_SIZE     0x26a
#define CoffAXPNamed1_SIZE      0xfa
#define CoffAXPNamed2_SIZE      0xc6
#define CoffPPCNamed1_SIZE      0x19a
#define CoffPPCNamed2_SIZE      0x132
