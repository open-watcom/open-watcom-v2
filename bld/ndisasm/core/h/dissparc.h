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
* Description:  Instruction decoding for Sun SPARC architecture.
*
****************************************************************************/


extern const dis_range          SPARCRangeTable[];
extern const int                SPARCRangeTablePos[];
extern const unsigned char      SPARCMaxInsName;

extern dis_handler_return SPARCSetHi( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCBranch( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCCall( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCOp3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCMem( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCFPop2( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCFPop3( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCMemF( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return SPARCMemC( dis_handle *h, void *d, dis_dec_ins *ins );
