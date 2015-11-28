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
* Description:  Support for debugging overlays.
*
****************************************************************************/


extern bool InitOvlState( void );
extern void FiniOvlState( void );
extern void InvalidateTblCache( void );
extern bool SectIsLoaded( unsigned sect_id, int sect_map_id );
extern void SectLoad( unsigned sect_id );
extern void SectTblRead( machine_state *state );
extern void SetStateOvlSect( machine_state *state, unsigned sect_id );
extern bool TransOvlRetAddr( address *addr, unsigned ovl_level );
extern void RemapSection( unsigned section, unsigned new_seg );
extern void AddrSection( address *addr, unsigned ovl_map_id );
extern void AddrFix( address *addr );
extern void AddrFloat( address *addr );
