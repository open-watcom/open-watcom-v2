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
* Description:  User program step and trace support.
*
****************************************************************************/


extern void             ResizeTraceData( void );
extern bool             TraceStart( bool tracing );
extern mad_trace_how    TraceHow( bool force_into );
extern bool             TraceSimulate( void );
extern bool             TraceModifications( MAD_MEMREF_WALKER *wk, void *d );
extern void             TraceStop( bool tracing );
extern void             TraceKill( void );
extern bool             SourceStep( void );
extern bool             CheckForDLLThunk( void );
extern unsigned         TraceCheck( unsigned conditions );
extern void             PerformTrace( void );
extern bool             HasLineInfo( address addr );
extern void             ExecTrace( trace_cmd_type type, debug_level level );
extern void             ProcTrace( void );
extern void             FiniTrace( void );
extern void             LevelSet( void );
extern void             LevelConf( void );
