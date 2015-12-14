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
* Description:  MAD (Machine Architecture Description) interface.
*
****************************************************************************/


#ifndef MAD_H_INCLUDED
#define MAD_H_INCLUDED

#include "madtypes.h"

/*
 *      Control Routines
 */

mad_status      MADInit( void );
mad_status      MADRegister( mad_handle, const char *file, const char *desc );
mad_status      MADLoad( mad_handle );
void            MADUnload( mad_handle );
mad_status      MADLoaded( mad_handle );
mad_handle      MADActiveSet( mad_handle );
mad_state_data  *MADStateCreate( void );
mad_state_data  *MADStateSet( mad_state_data * );
void            MADStateCopy( const mad_state_data *src, mad_state_data *dst );
void            MADStateDestroy( mad_state_data * );
void            MADFini( void );

typedef         walk_result (MAD_WALKER)( mad_handle, void * );
walk_result     MADWalk( MAD_WALKER *, void * );

size_t          MADNameFile( mad_handle, char *buff, size_t buff_size );
size_t          MADNameDescription( mad_handle, char *buff, size_t buff_size );

/*
 *      Address Arithmetic
 */

void            MADAddrAdd( address *, long, mad_address_format );
int             MADAddrComp( const address *, const address *, mad_address_format );
long            MADAddrDiff( const address *, const address *, mad_address_format );
mad_status      MADAddrMap( addr_ptr *, const addr_ptr *, const addr_ptr *, const mad_registers * );
mad_status      MADAddrFlat( const mad_registers * );
mad_status      MADAddrInterrupt( const addr_ptr *, unsigned, const mad_registers * );


/*
 *      Machine Types
 */

typedef         walk_result (MAD_TYPE_WALKER)( mad_type_handle, void * );
walk_result     MADTypeWalk( mad_type_kind, MAD_TYPE_WALKER *, void * );

mad_string      MADTypeName( mad_type_handle );
unsigned        MADTypePreferredRadix( mad_type_handle );
mad_type_handle MADTypeForDIPType( const dip_type_info * );
void            MADTypeInfo( mad_type_handle, mad_type_info * );
mad_status      MADTypeInfoForHost( mad_type_kind, int size, mad_type_info * );
mad_type_handle MADTypeDefault( mad_type_kind, mad_address_format, const mad_registers *, const address * );

mad_status      MADTypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg );
mad_status      MADTypeToString( unsigned radix, const mad_type_info *, const void *data, char *buff, size_t *buff_size_p );
mad_status      MADTypeHandleToString( unsigned radix, mad_type_handle, const void *data, char *buff, size_t *buff_size_p );


/*
 *      Machine Registers
 */

unsigned        MADRegistersSize( void );
mad_status      MADRegistersHost( mad_registers * );
mad_status      MADRegistersTarget( mad_registers * );

typedef         walk_result (MAD_REG_SET_WALKER)( const mad_reg_set_data *, void * );
walk_result     MADRegSetWalk( mad_type_kind, MAD_REG_SET_WALKER *, void * );

mad_string      MADRegSetName( const mad_reg_set_data * );
size_t          MADRegSetLevel( const mad_reg_set_data *, char *buff, size_t buff_size );

unsigned        MADRegSetDisplayGrouping( const mad_reg_set_data * );
mad_status      MADRegSetDisplayGetPiece( const mad_reg_set_data *, const mad_registers *mr, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value );
mad_status      MADRegSetDisplayModify( const mad_reg_set_data *, const mad_reg_info *reg, const mad_modify_list **possible, int *num_possible );
const mad_toggle_strings *MADRegSetDisplayToggleList( const mad_reg_set_data * );
unsigned        MADRegSetDisplayToggle( const mad_reg_set_data *, unsigned on, unsigned off );
mad_status      MADRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *curr );
mad_status      MADRegInspectAddr( const mad_reg_info *, const mad_registers *, address * );

typedef         walk_result (MAD_REG_WALKER)( const mad_reg_info *, int has_sublist, void * );
walk_result     MADRegWalk( const mad_reg_set_data *, const mad_reg_info *, MAD_REG_WALKER *, void * );

size_t          MADRegFullName( const mad_reg_info *ri, const char *op, char *buff, size_t buff_size );

void            MADRegSpecialGet( mad_special_reg, const mad_registers *block, addr_ptr * );
void            MADRegSpecialSet( mad_special_reg, mad_registers *block, const addr_ptr * );
size_t          MADRegSpecialName( mad_special_reg, const mad_registers *block, mad_address_format, char *buff, size_t buff_size );

const mad_reg_info *MADRegFromContextItem( context_item );

void            MADRegUpdateStart( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size );
void            MADRegUpdateEnd( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size );



/*
 *      Calling Conventions
 */

mad_status              MADCallStackGrowsUp( void );
const mad_string        *MADCallTypeList( void );
mad_status              MADCallBuildFrame( mad_string, address ret, address rtn, const mad_registers *in, mad_registers *out );
const mad_reg_info      *MADCallReturnReg( mad_string, address );
const mad_reg_info      **MADCallParmRegList( mad_string, address );

unsigned                MADCallUpStackSize( void );
mad_status              MADCallUpStackInit( mad_call_up_data *mcud, const mad_registers *mr );
mad_status              MADCallUpStackLevel( mad_call_up_data *mcud, const address *start, unsigned rtn_characteristics, long return_disp, const mad_registers *in, address *execution, address *frame, address *stack, mad_registers **out );



/*
 *      Instruction Disassembly
 */

unsigned                MADDisasmDataSize( void );
unsigned                MADDisasmNameMax( void );
mad_status              MADDisasm( mad_disasm_data *, address *, int adj );
size_t                  MADDisasmFormat( mad_disasm_data *, mad_disasm_piece, unsigned radix, char *buff, size_t buff_size );
unsigned                MADDisasmInsSize( mad_disasm_data * );
mad_status              MADDisasmInsUndoable( mad_disasm_data * );
mad_disasm_control      MADDisasmControl( mad_disasm_data *, const mad_registers * );
mad_status              MADDisasmInspectAddr( const char *start, unsigned len, unsigned radix, const mad_registers *, address * );
mad_status              MADDisasmInsNext( mad_disasm_data *, const mad_registers *, address * );

typedef                 walk_result (MAD_MEMREF_WALKER)( address, mad_type_handle, mad_memref_kind, void * );
walk_result             MADDisasmMemRefWalk( mad_disasm_data *, MAD_MEMREF_WALKER *, const mad_registers *, void * );

const mad_toggle_strings        *MADDisasmToggleList( void );
unsigned                MADDisasmToggle( unsigned on, unsigned off );


/*
 *      Instruction Tracing
 */

unsigned        MADTraceSize( void );
void            MADTraceInit( mad_trace_data *, const mad_registers * );
mad_trace_how   MADTraceOne( mad_trace_data *, mad_disasm_data *, mad_trace_kind, const mad_registers *, address * );
mad_status      MADTraceHaveRecursed( address watch, const mad_registers * );
mad_status      MADTraceSimulate( mad_trace_data *, mad_disasm_data *, const mad_registers *in, mad_registers *out );
void            MADTraceFini( mad_trace_data * );

mad_status      MADUnexpectedBreak( mad_registers *, char *buff, size_t *buff_size_p );

#endif
