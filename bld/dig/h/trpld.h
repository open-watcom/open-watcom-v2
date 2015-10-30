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
* Description:  TRAP file loader global variable declaration
*
****************************************************************************/


#define TRAP_PARM_SEPARATOR ';'

extern trap_version     TrapVer;
extern trap_req_func    *ReqFunc;

#ifdef ENABLE_TRAP_LOGGING
extern char             *TrapTraceFileName;
extern bool             TrapTraceFileFlush;

extern int              OpenTrapTraceFile( void );
extern int              CloseTrapTraceFile( void );
#endif

/* Client interface routines */
extern char             *LoadDumbTrap( trap_version * );
extern char             *LoadTrap( const char *, char *, trap_version * );
extern void             TrapSetFailCallBack( void (*func)(void) );
extern unsigned         TrapAccess( unsigned, in_mx_entry_p, unsigned, mx_entry_p );
extern unsigned         TrapSimpAccess( unsigned, in_data_p, unsigned, out_data_p );
extern void             KillTrap(void);
extern const trap_requests *TrapLoad( const trap_callbacks *client );

/* Client support routines */
extern void             TrapSetFailCallBack( void (*func)(void) );
extern void             TrapSetAccessCallBack( void (*func)(void) );
extern void             TrapFailAllRequests( void );
