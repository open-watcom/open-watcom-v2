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
* Description:  Instruction decoding for Alpha AXP architecture.
*
****************************************************************************/


extern const dis_range          AXPRangeTable[];
extern const int                AXPRangeTablePos[];
extern const unsigned char      AXPMaxInsName;

extern dis_handler_return AXPPal( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPMemory( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPFPMemory( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPMemoryLA( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPMemoryFC( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPJump( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPBranch( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPFPBranch( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPOperate( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPOperateV( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPFPOperate( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPFPConvert( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPIEEEOperate( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPIEEECompare( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPIEEEConvert( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPVAXOperate( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return AXPVAXConvert( dis_handle *h, void *d, dis_dec_ins *ins );
