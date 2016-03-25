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
* Description:  Instruction decoding for Java Virtual Machine.
*
****************************************************************************/


extern const dis_range          JVMRangeTable[];
extern const int                JVMRangeTablePos[];
extern const unsigned char      JVMMaxInsName;

extern dis_handler_return JVMSByte( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMSShort( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMUByte( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMUShort( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMNull( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMIInc( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMWIndex( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMWide( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMMultiANewArray( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMBrShort( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMBrInt( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMTableSwitch( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMLookupSwitch( dis_handle *h, void *d, dis_dec_ins *ins );
extern dis_handler_return JVMInterface( dis_handle *h, void *d, dis_dec_ins *ins );
