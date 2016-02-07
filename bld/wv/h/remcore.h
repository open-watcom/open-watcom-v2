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
* Description:  Remote access core - trap file interface.
*
****************************************************************************/


extern void             FiniCache( void );
extern void             InitCache( address addr, size_t size );
extern bool             HaveCache( void );
extern size_t           ProgPeek( address addr, void *data, size_t len );
extern size_t           ProgPoke( address addr, const void *data, size_t len );
extern unsigned long    ProgChkSum( address addr, trap_elen len );
extern trap_retval      PortPeek( unsigned port, void *data, trap_elen size );
extern trap_retval      PortPoke( unsigned port, const void *data, trap_elen size );
extern void             ReadDbgRegs( void );
extern void             WriteDbgRegs( void );
extern trap_elen        ArgsLen( const char *args );
extern void             ClearMachineDataCache( void );
extern error_idx        DoLoad( const char *args, unsigned long *phandle );
extern bool             KillProgOvlay( void );
extern unsigned         MakeProgRun( bool single );
extern bool             Redirect( bool input, char *hndlname );
extern void             RemoteMapAddr( addr_ptr *addr, addr_off *lo_bound, addr_off *hi_bound, unsigned long handle );
extern void             RemoteSetUserScreen( void );
extern void             RemoteSetDebugScreen( void );
extern unsigned         RemoteReadUserKey( unsigned wait );
extern unsigned long    RemoteGetLibName( unsigned long lib_hdl, char *buff, trap_elen buff_len );
extern unsigned         RemoteGetMsgText( char *buff, trap_elen buff_len );
extern unsigned         RemoteMachineData( address addr, unsigned info_type, dig_elen in_size, const void *inp, dig_elen out_size, void *outp );
extern dword            RemoteSetBreak( address addr );
extern void             RemoteRestoreBreak( address addr, dword value );
extern bool             RemoteSetWatch( address addr, unsigned size, unsigned long *mult );
extern void             RemoteRestoreWatch( address addr, unsigned size );
extern void             RemoteSplitCmd( char *cmd, char **end, char **parm );
extern void             CheckSegAlias( void );
extern void             GetSysConfig( void );
extern bool             InitCoreSupp( void );
extern void             FiniCoreSupp( void );
extern char             *TrapClientString( unsigned tc );
