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
* Description:  MAD import routines.
*
****************************************************************************/


#ifndef MADIMP_H_INCLUDED
#define MADIMP_H_INCLUDED

#include "madtypes.h"
#include "digpck.h"

typedef struct imp_mad_state_data       imp_mad_state_data;

typedef         walk_result (DIGCLIENT MI_TYPE_WALKER)( mad_type_handle, void * );
typedef         walk_result (DIGCLIENT MI_REG_SET_WALKER)( const mad_reg_set_data *, void * );
typedef         walk_result (DIGCLIENT MI_REG_WALKER)( const mad_reg_info *, int has_sublist, void * );
typedef         walk_result (DIGCLIENT MI_MEMREF_WALKER)( address, mad_type_handle, mad_memref_kind write, void * );

#define MAD_MAJOR       1
#define MAD_MINOR_OLD   1
#define MAD_MINOR       2

typedef struct mad_imp_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         sizeof_struct;

    mad_status          (DIGENTRY *MIInit)(void);
    void                (DIGENTRY *MIFini)(void);
    unsigned            (DIGENTRY *MIStateSize)( void );
    void                (DIGENTRY *MIStateInit)( imp_mad_state_data * );
    void                (DIGENTRY *MIStateSet)( imp_mad_state_data * );
    void                (DIGENTRY *MIStateCopy)( const imp_mad_state_data *src, imp_mad_state_data *dst );

    void                (DIGENTRY *MIAddrAdd)( address *, long, mad_address_format );
    int                 (DIGENTRY *MIAddrComp)( const address *, const address *, mad_address_format );
    long                (DIGENTRY *MIAddrDiff)( const address *, const address *, mad_address_format );
    mad_status          (DIGENTRY *MIAddrMap)( addr_ptr *, const addr_ptr *, const addr_ptr *, const mad_registers * );
    mad_status          (DIGENTRY *MIAddrFlat)( const mad_registers * );
    mad_status          (DIGENTRY *MIAddrInterrupt)( const addr_ptr *, unsigned, const mad_registers * );

    walk_result         (DIGENTRY *MITypeWalk)( mad_type_kind, MI_TYPE_WALKER *, void * );
    mad_string          (DIGENTRY *MITypeName)( mad_type_handle );
    unsigned            (DIGENTRY *MITypePreferredRadix)( mad_type_handle );
    mad_type_handle     (DIGENTRY *MITypeForDIPType)( const dip_type_info * );
    void                (DIGENTRY *MITypeInfo)( mad_type_handle, mad_type_info * );
    mad_type_handle     (DIGENTRY *MITypeDefault)( mad_type_kind, mad_address_format, const mad_registers *, const address * );
    mad_status          (DIGENTRY *MITypeConvert)( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg );
    mad_status          (DIGENTRY *MITypeToString)( unsigned radix, const mad_type_info *, const void *, char *buff, size_t *buff_size_p );

    unsigned            (DIGENTRY *MIRegistersSize)( void );
    mad_status          (DIGENTRY *MIRegistersHost)( mad_registers * );
    mad_status          (DIGENTRY *MIRegistersTarget)( mad_registers * );
    walk_result         (DIGENTRY *MIRegSetWalk)( mad_type_kind, MI_REG_SET_WALKER *, void * );
    mad_string          (DIGENTRY *MIRegSetName)( const mad_reg_set_data * );
    size_t              (DIGENTRY *MIRegSetLevel)( const mad_reg_set_data *, char *buff, size_t buff_size );
    unsigned            (DIGENTRY *MIRegSetDisplayGrouping)( const mad_reg_set_data * );
    mad_status          (DIGENTRY *MIRegSetDisplayGetPiece)( const mad_reg_set_data *, mad_registers const *, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value );
    mad_status          (DIGENTRY *MIRegSetDisplayModify)( const mad_reg_set_data *, const mad_reg_info *reg, const mad_modify_list **possible, int *num_possible );
    const mad_toggle_strings    *(DIGENTRY *MIRegSetDisplayToggleList)( const mad_reg_set_data * );
    unsigned            (DIGENTRY *MIRegSetDisplayToggle)( const mad_reg_set_data *, unsigned on, unsigned off );
    mad_status          (DIGENTRY *MIRegModified)( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *curr );
    mad_status          (DIGENTRY *MIRegInspectAddr)( const mad_reg_info *, mad_registers const *, address * );
    walk_result         (DIGENTRY *MIRegWalk)( const mad_reg_set_data *, const mad_reg_info *, MI_REG_WALKER *, void * );
    void                (DIGENTRY *MIRegSpecialGet)( mad_special_reg, const mad_registers *block, addr_ptr * );
    void                (DIGENTRY *MIRegSpecialSet)( mad_special_reg, mad_registers *block, addr_ptr const * );
    size_t              (DIGENTRY *MIRegSpecialName)( mad_special_reg, mad_registers const *block, mad_address_format, char *buff, size_t buff_size );
    const mad_reg_info *(DIGENTRY *MIRegFromContextItem)( context_item );
    void                (DIGENTRY *MIRegUpdateStart)( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size );
    void                (DIGENTRY *MIRegUpdateEnd)( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size );

    mad_status          (DIGENTRY *MICallStackGrowsUp)( void );
    const mad_string    *(DIGENTRY *MICallTypeList)( void );
    mad_status          (DIGENTRY *MICallBuildFrame)( mad_string, address ret, address rtn, mad_registers const *in, mad_registers *out );
    const mad_reg_info  *(DIGENTRY *MICallReturnReg)( mad_string, address );
    const mad_reg_info  **(DIGENTRY *MICallParmRegList)( mad_string, address );
    mad_status          (DIGENTRY *old_MICallUpStackLevel)( address const *start, unsigned rtn_characteristics, long return_disp, mad_registers const *in, address *execution, address *frame, address *stack, mad_registers **out );

    unsigned            (DIGENTRY *MIDisasmDataSize)( void );
    unsigned            (DIGENTRY *MIDisasmNameMax)( void );
    mad_status          (DIGENTRY *MIDisasm)( mad_disasm_data *, address *, int );
    size_t              (DIGENTRY *MIDisasmFormat)( mad_disasm_data *, mad_disasm_piece, unsigned radix, char *buff, size_t buff_size );
    unsigned            (DIGENTRY *MIDisasmInsSize)( mad_disasm_data * );
    mad_status          (DIGENTRY *MIDisasmInsUndoable)( mad_disasm_data * );
    mad_disasm_control  (DIGENTRY *MIDisasmControl)( mad_disasm_data *, mad_registers const * );
    mad_status          (DIGENTRY *MIDisasmInspectAddr)( const char *start, unsigned len, unsigned radix, mad_registers const *, address * );
    walk_result         (DIGENTRY *MIDisasmMemRefWalk)( mad_disasm_data *, MI_MEMREF_WALKER *, mad_registers const *, void * );
    const mad_toggle_strings    *(DIGENTRY *MIDisasmToggleList)( void );
    unsigned            (DIGENTRY *MIDisasmToggle)( unsigned on, unsigned off );

    unsigned            (DIGENTRY *MITraceSize)( void );
    void                (DIGENTRY *MITraceInit)( mad_trace_data *, const mad_registers * );
    mad_trace_how       (DIGENTRY *MITraceOne)( mad_trace_data *, mad_disasm_data *, mad_trace_kind, mad_registers const *, address * );
    mad_status          (DIGENTRY *MITraceHaveRecursed)( address, mad_registers const * );
    mad_status          (DIGENTRY *MITraceSimulate)( mad_trace_data *, mad_disasm_data *, mad_registers const *in, mad_registers *out );
    void                (DIGENTRY *MITraceFini)( mad_trace_data * );

    mad_status          (DIGENTRY *MIUnexpectedBreak)( mad_registers *, char *buff, size_t *buff_size_p );

    mad_status          (DIGENTRY *MIDisasmInsNext)( mad_disasm_data *, const mad_registers *, address * );

    unsigned            (DIGENTRY *MICallUpStackSize)( void );
    mad_status          (DIGENTRY *MICallUpStackInit)( mad_call_up_data *, const mad_registers * );
    mad_status          (DIGENTRY *MICallUpStackLevel)( mad_call_up_data *, const address *start, unsigned rtn_characteristics, long return_disp, mad_registers const *in, address *execution, address *frame, address *stack, mad_registers **out );
} mad_imp_routines;

/*
 *      Control Routines
 */

mad_status      DIGENTRY MIInit(void);
void            DIGENTRY MIFini(void);
unsigned        DIGENTRY MIStateSize( void );
void            DIGENTRY MIStateInit( imp_mad_state_data * );
void            DIGENTRY MIStateSet( imp_mad_state_data * );
void            DIGENTRY MIStateCopy( const imp_mad_state_data *src, imp_mad_state_data *dst );

/*
 *      Address Arithmetic
 */

void            DIGENTRY MIAddrAdd( address *, long, mad_address_format );
int             DIGENTRY MIAddrComp( const address *, const address *, mad_address_format );
long            DIGENTRY MIAddrDiff( const address *, const address *, mad_address_format );
mad_status      DIGENTRY MIAddrMap( addr_ptr *, const addr_ptr *, const addr_ptr *, const mad_registers * );
mad_status      DIGENTRY MIAddrFlat( const mad_registers * );
mad_status      DIGENTRY MIAddrInterrupt( const addr_ptr *, unsigned, const mad_registers * );

/*
 *      Machine Types
 */

walk_result     DIGENTRY MITypeWalk( mad_type_kind, MI_TYPE_WALKER *, void * );

mad_string      DIGENTRY MITypeName( mad_type_handle );
unsigned        DIGENTRY MITypePreferredRadix( mad_type_handle );
mad_type_handle DIGENTRY MITypeForDIPType( const dip_type_info * );
void            DIGENTRY MITypeInfo( mad_type_handle, mad_type_info * );
mad_type_handle DIGENTRY MITypeDefault( mad_type_kind, mad_address_format, const mad_registers *, const address * );

mad_status      DIGENTRY MITypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg );
mad_status      DIGENTRY MITypeToString( unsigned radix, const mad_type_info *, const void *, char *buff, size_t *buff_size_p );


/*
 *      Machine Registers
 */

unsigned        DIGENTRY MIRegistersSize( void );
mad_status      DIGENTRY MIRegistersHost( mad_registers * );
mad_status      DIGENTRY MIRegistersTarget( mad_registers * );

walk_result     DIGENTRY MIRegSetWalk( mad_type_kind, MI_REG_SET_WALKER *, void * );

mad_string      DIGENTRY MIRegSetName( const mad_reg_set_data * );
size_t          DIGENTRY MIRegSetLevel( const mad_reg_set_data *, char *buff, size_t buff_size );

unsigned        DIGENTRY MIRegSetDisplayGrouping( const mad_reg_set_data * );
mad_status      DIGENTRY MIRegSetDisplayGetPiece( const mad_reg_set_data *, const mad_registers *, unsigned piece, const char **descript_p, size_t *max_descript_p, const mad_reg_info **reg, mad_type_handle *disp_type, size_t *max_value );
mad_status      DIGENTRY MIRegSetDisplayModify( const mad_reg_set_data *, const mad_reg_info *reg, const mad_modify_list **possible, int *num_possible );
const mad_toggle_strings *DIGENTRY MIRegSetDisplayToggleList( const mad_reg_set_data * );
unsigned        DIGENTRY MIRegSetDisplayToggle( const mad_reg_set_data *, unsigned on, unsigned off );
mad_status      DIGENTRY MIRegModified( const mad_reg_set_data *rsd, const mad_reg_info *ri, const mad_registers *old, const mad_registers *curr );
mad_status      DIGENTRY MIRegInspectAddr( const mad_reg_info *, const mad_registers *, address * );

walk_result     DIGENTRY MIRegWalk( const mad_reg_set_data *, const mad_reg_info *, MI_REG_WALKER *, void * );

void            DIGENTRY MIRegSpecialGet( mad_special_reg, const mad_registers *block, addr_ptr * );
void            DIGENTRY MIRegSpecialSet( mad_special_reg, mad_registers *block, const addr_ptr * );
size_t          DIGENTRY MIRegSpecialName( mad_special_reg, const mad_registers *block, mad_address_format, char *buff, size_t buff_size );

const mad_reg_info *DIGENTRY MIRegFromContextItem( context_item );

void            DIGENTRY MIRegUpdateStart( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size );
void            DIGENTRY MIRegUpdateEnd( mad_registers *, unsigned flags, unsigned bit_start, unsigned bit_size );



/*
 *      Calling Conventions
 */

mad_status              DIGENTRY MICallStackGrowsUp( void );
const mad_string        *DIGENTRY MICallTypeList( void );
mad_status              DIGENTRY MICallBuildFrame( mad_string, address ret, address rtn, const mad_registers *in, mad_registers *out );
const mad_reg_info      *DIGENTRY MICallReturnReg( mad_string, address );
const mad_reg_info      **DIGENTRY MICallParmRegList( mad_string, address );

unsigned                DIGENTRY MICallUpStackSize( void );
mad_status              DIGENTRY MICallUpStackInit( mad_call_up_data *, const mad_registers * );
mad_status              DIGENTRY MICallUpStackLevel( mad_call_up_data *, const address *start, unsigned rtn_characteristics, long return_disp, const mad_registers *in, address *execution, address *frame, address *stack, mad_registers **out );


/*
 *      Instruction Disassembly
 */

unsigned                DIGENTRY MIDisasmDataSize( void );
unsigned                DIGENTRY MIDisasmNameMax( void );
mad_status              DIGENTRY MIDisasm( mad_disasm_data *, address *, int );
size_t                  DIGENTRY MIDisasmFormat( mad_disasm_data *, mad_disasm_piece, unsigned radix, char *buff, size_t buff_size );
unsigned                DIGENTRY MIDisasmInsSize( mad_disasm_data * );
mad_status              DIGENTRY MIDisasmInsUndoable( mad_disasm_data * );
mad_disasm_control      DIGENTRY MIDisasmControl( mad_disasm_data *, const mad_registers * );
mad_status              DIGENTRY MIDisasmInspectAddr( const char *start, unsigned len, unsigned radix, const mad_registers *, address * );
mad_status              DIGENTRY MIDisasmInsNext( mad_disasm_data *, const mad_registers *, address * );

walk_result             DIGENTRY MIDisasmMemRefWalk( mad_disasm_data *, MI_MEMREF_WALKER *, const mad_registers *, void * );

const mad_toggle_strings        *DIGENTRY MIDisasmToggleList( void );
unsigned                DIGENTRY MIDisasmToggle( unsigned on, unsigned off );


/*
 *      Instruction Tracing
 */

unsigned                DIGENTRY MITraceSize( void );
void                    DIGENTRY MITraceInit( mad_trace_data *, const mad_registers * );
mad_trace_how           DIGENTRY MITraceOne( mad_trace_data *, mad_disasm_data *, mad_trace_kind, const mad_registers *, address * );
mad_status              DIGENTRY MITraceHaveRecursed( address, const mad_registers * );
mad_status              DIGENTRY MITraceSimulate( mad_trace_data *, mad_disasm_data *, const mad_registers *in, mad_registers *out );
void                    DIGENTRY MITraceFini( mad_trace_data * );

mad_status              DIGENTRY MIUnexpectedBreak( mad_registers *, char *buff, size_t *buff_size_p );


typedef struct mad_client_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         sizeof_struct;

    void                *(DIGCLIENT *MADCliAlloc)( size_t );
    void                *(DIGCLIENT *MADCliRealloc)( void *, size_t );
    void                (DIGCLIENT *MADCliFree)( void * );

    dig_fhandle         (DIGCLIENT *MADCliOpen)( char const*, dig_open );
    unsigned long       (DIGCLIENT *MADCliSeek)( dig_fhandle, unsigned long, dig_seek );
    unsigned            (DIGCLIENT *MADCliRead)( dig_fhandle, void *, unsigned );
    void                (DIGCLIENT *MADCliClose)( dig_fhandle );

    size_t              (DIGCLIENT *MADCliReadMem)( address, size_t size, void *buff );
    size_t              (DIGCLIENT *MADCliWriteMem)( address, size_t size, void const *buff );

    size_t              (DIGCLIENT *MADCliString)( mad_string, char *buff, size_t buff_size );
    mad_status          (DIGCLIENT *MADCliAddString)( mad_string, const char * );
    size_t              (DIGCLIENT *MADCliRadixPrefix)( unsigned radix, char *buff, size_t buff_size );

    void                (DIGCLIENT *MADCliNotify)( mad_notify_type, void const * );

    unsigned            (DIGCLIENT *MADCliMachineData)( address, unsigned, unsigned, void const*, unsigned, void * );

    mad_status          (DIGCLIENT *MADCliAddrToString)( address, mad_type_handle, mad_label_kind, char *buff, size_t buff_size );
    mad_status          (DIGCLIENT *MADCliMemExpr)( char const *expr, unsigned radix, address * );

    void                (DIGCLIENT *MADCliAddrSection)( address * );
    mad_status          (DIGCLIENT *MADCliAddrOvlReturn)( address * );

    system_config       *(DIGCLIENT *MADCliSystemConfig)( void );

    mad_status          (DIGCLIENT *MADCliTypeInfoForHost)( mad_type_kind, int size, mad_type_info * );
    mad_status          (DIGCLIENT *MADCliTypeConvert)( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg );
    mad_status          (DIGCLIENT *MADCliTypeToString)( unsigned radix, const mad_type_info *, const void *data, char *buff, size_t *buff_size_p );
} mad_client_routines;

typedef mad_imp_routines * DIGENTRY mad_init_func( mad_status *status, mad_client_routines *client );
#ifdef __WINDOWS__
typedef void DIGENTRY mad_fini_func( void );
#endif

DIG_DLLEXPORT mad_init_func MADLOAD;
#ifdef __WINDOWS__
DIG_DLLEXPORT mad_fini_func MADUNLOAD;
#endif

void            *MCAlloc( size_t amount );
void            *MCRealloc( void *p, size_t amount );
void            MCFree( void *p );

dig_fhandle     MCOpen( const char *, dig_open );
unsigned long   MCSeek( dig_fhandle, unsigned long, dig_seek );
unsigned        MCRead( dig_fhandle, void *, unsigned );
void            MCClose( dig_fhandle );

size_t          MCReadMem( address, size_t size, void *buff );
size_t          MCWriteMem( address, size_t size, void *buff );

size_t          MCString( mad_string, char *buff, size_t buff_size );
mad_status      MCAddString( mad_string, const char * );
size_t          MCRadixPrefix( unsigned radix, char *buff, size_t buff_size );

void            MCNotify( mad_notify_type, void * );

unsigned        MCMachineData( address, unsigned, unsigned, void *, unsigned, void * );

mad_status      MCAddrToString( address, mad_type_handle, mad_label_kind, char *buff, size_t buff_size );
mad_status      MCMemExpr( const char *expr, unsigned radix, address * );

void            MCAddrSection( address * );
mad_status      MCAddrOvlReturn( address * );

system_config   *MCSystemConfig( void );

mad_status      MCTypeInfoForHost( mad_type_kind, int size, mad_type_info * );
mad_status      MCTypeConvert( const mad_type_info *in_t, const void *in_d, const mad_type_info *out_t, void *out_d, addr_seg );
mad_status      MCTypeToString( unsigned radix, const mad_type_info *, const void *data, char *buff, size_t *buff_size_p );

void            MCStatus( mad_status );

#include "digunpck.h"

#endif
