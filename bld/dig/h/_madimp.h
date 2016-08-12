/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  MAD import routines.
*
****************************************************************************/


/*
 *      Control Routines
 */
pick( mad_status,                   Init,               ( void ) )
pick( void,                         Fini,               ( void ) )
pick( unsigned,                     StateSize,          ( void ) )
pick( void,                         StateInit,          ( imp_mad_state_data * ) )
pick( void,                         StateSet,           ( imp_mad_state_data * ) )
pick( void,                         StateCopy,          ( const imp_mad_state_data *src, imp_mad_state_data *dst ) )
/*
 *      Address Arithmetic
 */
pick( void,                         AddrAdd,            ( address *, long, mad_address_format ) )
pick( int,                          AddrComp,           ( const address *, const address *, mad_address_format ) )
pick( long,                         AddrDiff,           ( const address *, const address *, mad_address_format ) )
pick( mad_status,                   AddrMap,            ( addr_ptr *, const addr_ptr *, const addr_ptr *, const mad_registers * ) )
pick( mad_status,                   AddrFlat,           ( const mad_registers * ) )
pick( mad_status,                   AddrInterrupt,      ( const addr_ptr *, unsigned, const mad_registers * ) )
/*
 *      Machine Types
 */
pick( walk_result,                  TypeWalk,           ( mad_type_kind, MI_TYPE_WALKER *, void * ) )
pick( mad_string,                   TypeName,           ( mad_type_handle ) )
pick( mad_radix,                    TypePreferredRadix, ( mad_type_handle ) )
pick( mad_type_handle,              TypeForDIPType,     ( const dip_type_info * ) )
pick( void,                         TypeInfo,           ( mad_type_handle, mad_type_info * ) )
pick( mad_type_handle,              TypeDefault,        ( mad_type_kind, mad_address_format, const mad_registers *, const address * ) )
pick( mad_status,                   TypeConvert,        ( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg ) )
pick( mad_status,                   TypeToString,       ( mad_radix radix, const mad_type_info *, const void *, char *buff, size_t *buff_size_p ) )
/*
 *      Machine Registers
 */
pick( unsigned,                     RegistersSize,      ( void ) )
pick( mad_status,                   RegistersHost,      ( mad_registers * ) )
pick( mad_status,                   RegistersTarget,    ( mad_registers * ) )
pick( walk_result,                  RegSetWalk,         ( mad_type_kind, MI_REG_SET_WALKER *, void * ) )
pick( mad_string,                   RegSetName,         ( const mad_reg_set_data * ) )
pick( size_t,                       RegSetLevel,        ( const mad_reg_set_data *, char *buff, size_t buff_size ) )
pick( unsigned,                     RegSetDisplayGrouping,  ( const mad_reg_set_data * ) )
pick( mad_status,                   RegSetDisplayGetPiece,  ( const mad_reg_set_data *, mad_registers const *, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value ) )
pick( mad_status,                   RegSetDisplayModify,    ( const mad_reg_set_data *, const mad_reg_info *reg, const mad_modify_list **possible, int *num_possible ) )
pick( const mad_toggle_strings *,   RegSetDisplayToggleList,( const mad_reg_set_data * ) )
pick( unsigned,                     RegSetDisplayToggle,    ( const mad_reg_set_data *, unsigned on, unsigned off ) )
pick( mad_status,                   RegModified,        ( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *curr ) )
pick( mad_status,                   RegInspectAddr,     ( const mad_reg_info *, mad_registers const *, address * ) )
pick( walk_result,                  RegWalk,            ( const mad_reg_set_data *, const mad_reg_info *, MI_REG_WALKER *, void * ) )
pick( void,                         RegSpecialGet,      ( mad_special_reg, const mad_registers *block, addr_ptr * ) )
pick( void,                         RegSpecialSet,      ( mad_special_reg, mad_registers *block, addr_ptr const * ) )
pick( size_t,                       RegSpecialName,     ( mad_special_reg, mad_registers const *block, mad_address_format, char *buff, size_t buff_size ) )
pick( const mad_reg_info *,         RegFromContextItem, ( context_item ) )
pick( void,                         RegUpdateStart,     ( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size ) )
pick( void,                         RegUpdateEnd,       ( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size ) )
/*
 *      Calling Conventions
 */
pick( mad_status,                   CallStackGrowsUp,   ( void ) )
pick( const mad_string *,           CallTypeList,       ( void ) )
pick( mad_status,                   CallBuildFrame,     ( mad_string, address ret, address rtn, mad_registers const *in, mad_registers *out ) )
pick( const mad_reg_info *,         CallReturnReg,      ( mad_string, address ) )
pick( const mad_reg_info **,        CallParmRegList,    ( mad_string, address ) )
pick( mad_status,                   OldCallUpStackLevel,    ( address const *start, unsigned rtn_characteristics, long return_disp, mad_registers const *in, address *execution, address *frame, address *stack, mad_registers **out ) )
pick( unsigned,                     CallUpStackSize,    ( void ) )
pick( mad_status,                   CallUpStackInit,    ( mad_call_up_data *, const mad_registers * ) )
pick( mad_status,                   CallUpStackLevel,   ( mad_call_up_data *, const address *start, unsigned rtn_characteristics, long return_disp, mad_registers const *in, address *execution, address *frame, address *stack, mad_registers **out ) )
/*
 *      Instruction Disassembly
 */
pick( unsigned,                     DisasmDataSize,     ( void ) )
pick( unsigned,                     DisasmNameMax,      ( void ) )
pick( mad_status,                   Disasm,             ( mad_disasm_data *, address *, int ) )
pick( size_t,                       DisasmFormat,       ( mad_disasm_data *, mad_disasm_piece, mad_radix radix, char *buff, size_t buff_size ) )
pick( unsigned,                     DisasmInsSize,      ( mad_disasm_data * ) )
pick( mad_status,                   DisasmInsUndoable,  ( mad_disasm_data * ) )
pick( mad_disasm_control,           DisasmControl,      ( mad_disasm_data *, mad_registers const * ) )
pick( mad_status,                   DisasmInspectAddr,  ( const char *start, unsigned len, mad_radix radix, mad_registers const *, address * ) )
pick( mad_status,                   DisasmInsNext,      ( mad_disasm_data *, const mad_registers *, address * ) )
pick( walk_result,                  DisasmMemRefWalk,   ( mad_disasm_data *, MI_MEMREF_WALKER *, mad_registers const *, void * ) )
pick( const mad_toggle_strings *,   DisasmToggleList,   ( void ) )
pick( unsigned,                     DisasmToggle,       ( unsigned on, unsigned off ) )
/*
 *      Instruction Tracing
 */
pick( unsigned,                     TraceSize,          ( void ) )
pick( void,                         TraceInit,          ( mad_trace_data *, const mad_registers * ) )
pick( mad_trace_how,                TraceOne,           ( mad_trace_data *, mad_disasm_data *, mad_trace_kind, mad_registers const *, address * ) )
pick( mad_status,                   TraceHaveRecursed,  ( address, mad_registers const * ) )
pick( mad_status,                   TraceSimulate,      ( mad_trace_data *, mad_disasm_data *, mad_registers const *in, mad_registers *out ) )
pick( void,                         TraceFini,          ( mad_trace_data * ) )

pick( mad_status,                   UnexpectedBreak,    ( mad_registers *, char *buff, size_t *buff_size_p ) )
