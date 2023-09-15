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
* Description:  MAD import routines.
*
****************************************************************************/


#ifndef MADIMP_H_INCLUDED
#define MADIMP_H_INCLUDED

#include "madcli.h"

#define MAD_VERSION_MAJOR   2
#define MAD_VERSION_MINOR   0

#define MADImp(n)           MADImp ## n
#define _MADImp(n)          _MADImp ## n n

#define MADIMPENTRY(n)      DIGENTRY MADImp( n )

typedef struct imp_mad_state_data       imp_mad_state_data;

typedef         walk_result (DIGCLIENT MI_TYPE_WALKER)( mad_type_handle, void * );
typedef         walk_result (DIGCLIENT MI_REG_SET_WALKER)( const mad_reg_set_data *, void * );
typedef         walk_result (DIGCLIENT MI_REG_WALKER)( const mad_reg_info *, int has_sublist, void * );
typedef         walk_result (DIGCLIENT MI_MEMREF_WALKER)( address, mad_type_handle, mad_memref_kind write, void * );

#define pick(r,n,p) typedef r (DIGENTRY *_MADImp ## n) p;
#include "_madimp.h"
#undef pick

#define pick(r,n,p) extern r MADIMPENTRY( n ) p;
#include "_madimp.h"
#undef pick

#include "digpck.h"

typedef struct mad_imp_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         sizeof_struct;

    _MADImp( Init );
    _MADImp( Fini );
    _MADImp( StateSize );
    _MADImp( StateInit );
    _MADImp( StateSet );
    _MADImp( StateCopy );

    _MADImp( AddrAdd );
    _MADImp( AddrComp );
    _MADImp( AddrDiff );
    _MADImp( AddrMap );
    _MADImp( AddrFlat );
    _MADImp( AddrInterrupt );

    _MADImp( TypeWalk );
    _MADImp( TypeName );
    _MADImp( TypePreferredRadix );
    _MADImp( TypeForDIPType );
    _MADImp( TypeInfo );
    _MADImp( TypeDefault );
    _MADImp( TypeConvert );
    _MADImp( TypeToString );

    _MADImp( RegistersSize );
    _MADImp( RegistersHost );
    _MADImp( RegistersTarget );
    _MADImp( RegSetWalk );
    _MADImp( RegSetName );
    _MADImp( RegSetLevel );
    _MADImp( RegSetDisplayGrouping );
    _MADImp( RegSetDisplayGetPiece );
    _MADImp( RegSetDisplayModify );
    _MADImp( RegSetDisplayToggleList );
    _MADImp( RegSetDisplayToggle );
    _MADImp( RegModified );
    _MADImp( RegInspectAddr );
    _MADImp( RegWalk );
    _MADImp( RegSpecialGet );
    _MADImp( RegSpecialSet );
    _MADImp( RegSpecialName );
    _MADImp( RegFromContextItem );
    _MADImp( RegUpdateStart );
    _MADImp( RegUpdateEnd );

    _MADImp( CallStackGrowsUp );
    _MADImp( CallTypeList );
    _MADImp( CallBuildFrame );
    _MADImp( CallReturnReg );
    _MADImp( CallParmRegList );

    _MADImp( DisasmDataSize );
    _MADImp( DisasmNameMax );
    _MADImp( Disasm );
    _MADImp( DisasmFormat );
    _MADImp( DisasmInsSize );
    _MADImp( DisasmInsUndoable );
    _MADImp( DisasmControl );
    _MADImp( DisasmInspectAddr );
    _MADImp( DisasmMemRefWalk );
    _MADImp( DisasmToggleList );
    _MADImp( DisasmToggle );

    _MADImp( TraceSize );
    _MADImp( TraceInit );
    _MADImp( TraceOne );
    _MADImp( TraceHaveRecursed );
    _MADImp( TraceSimulate );
    _MADImp( TraceFini );

    _MADImp( UnexpectedBreak );

    _MADImp( DisasmInsNext );

    _MADImp( CallUpStackSize );
    _MADImp( CallUpStackInit );
    _MADImp( CallUpStackLevel );
} mad_imp_routines;

typedef struct mad_client_routines {
    unsigned_8          major;
    unsigned_8          minor;
    unsigned_16         sizeof_struct;

    _DIGCli( Alloc );
    _DIGCli( Realloc );
    _DIGCli( Free );

    _DIGCli( Open );
    _DIGCli( Read );
    _DIGCli( Close );

    _MADCli( ReadMem );
    _MADCli( WriteMem );

    _MADCli( String );
    _MADCli( AddString );
    _MADCli( RadixPrefix );

    _MADCli( Notify );

    _MADCli( MachineData );

    _MADCli( AddrToString );
    _MADCli( MemExpr );

    _MADCli( AddrSection );
    _MADCli( AddrOvlReturn );

    _MADCli( SystemConfig );

    _MADCli( TypeInfoForHost );
    _MADCli( TypeConvert );
    _MADCli( TypeToString );
} mad_client_routines;

#include "digunpck.h"

typedef mad_imp_routines * DIGENTRY mad_init_func( mad_status *status, mad_client_routines *client );
#ifdef __WINDOWS__
typedef void DIGENTRY mad_fini_func( void );

typedef struct mad_link_block {
    mad_init_func   *load;
    mad_fini_func   *unload;
} mad_link_block;
#endif

DIG_DLLEXPORT extern mad_init_func MADLOAD;
#ifdef __WINDOWS__
extern mad_fini_func MADUNLOAD;
#endif

#define MC(n)       MC ## n

#define pick(r,n,p) extern r MC( n ) p;
#include "_digcli.h"
#include "_madcli.h"
#undef pick

extern void         MC( Status )( mad_status );

#endif
