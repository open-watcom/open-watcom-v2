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
* Description:  User program registers access and management.
*
****************************************************************************/


extern void             DefAddr( memory_expr def_seg, address *addr );
extern address          GetRegIP( void );
extern void             SetRegIP( address addr );
extern void             RecordSetRegIP( address addr );
extern void             SetRegSP( address addr );
extern void             SetRegBP( address addr );
extern address          GetRegSP( void );
extern address          GetRegBP( void );
extern void             SetMemBefore( bool tracing );
extern void             SetMemAfter( bool tracing );
extern void             ResizeRegData( void );
extern void             ClearMachState( void );
extern void             InitMachState( void );
extern void             FiniMachState( void );
extern void             SetupMachState( void );
extern void             CopyMachState( machine_state *from, machine_state *to );
extern machine_state    *AllocMachState( void );
extern void             FreeMachState( machine_state *state );
extern void             CollapseMachState( void );
extern bool             CheckStackPos( void );
extern bool             AdvMachState( int action );
extern size_t           ChangeMem( address addr, const void *data, size_t size );
extern unsigned         UndoLevel( void );
#ifdef DEADCODE
extern bool             MachStateInfoRelease( void );
#endif
extern void             SetStackPos( location_context *lc, int pos );
extern void             MoveStackPos( int by );
extern int              GetStackPos( void );
extern void             PosMachState( int rel_pos );
extern void             LastMachState( void );
extern void             LastStackPos( void );
extern void             ProcRegister( void );
extern void             ProcUndo( void );
extern char             *GetActionString( int action );
extern char             *GetUndoString( void );
extern char             *GetRedoString( void );
extern void             ProcStackPos( void );
extern void             GoHome( void );
extern void             ParseRegSet( bool multiple, location_list *ll, dip_type_info *ti );
extern void             RegValue( item_mach *value, const mad_reg_info *reginfo, machine_state *mach );
extern void             RegNewValue( const mad_reg_info *reginfo, const item_mach *new_val, mad_type_handle type );
