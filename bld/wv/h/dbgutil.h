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
* Description:  Debugger utility routines.
*
****************************************************************************/


extern unsigned     DefaultSize( default_kind dk );
extern char         *CnvULongHex( unsigned long value, char *buff, size_t buff_len );
extern char         *CnvLongDec( long value, char *buff, size_t buff_len );
extern char         *CnvULongDec( unsigned long value, char *buff, size_t buff_len );
extern char         *CnvLong( long value, char *buff, size_t buff_len );
extern char         *CnvULong( unsigned long value, char *buff, size_t buff_len );
extern char         *AddrToIOString( address *a, char *buff, size_t buff_len );
extern size_t       QualifiedSymName( sym_handle *sh, char *name, size_t max, bool uniq );
extern char         *CnvAddr( address addr, cnvaddr_option cao, bool uniq, char *p, size_t max );
extern char         *CnvNearestAddr( address addr, char *buff, size_t buff_len );
extern char         *StrAddr( address *addr, char *buff, size_t buff_len );
extern char         *UniqStrAddr( address *addr, char *buff, size_t buff_len );
extern char         *LineAddr( address  *addr, char *buff, size_t buff_len );
extern void         RingBell( void );
extern void         Warn( char *p );
extern cmd_list     *AllocCmdList( const char *start, size_t len );
extern void         FreeCmdList( cmd_list *cmds );
extern void         LockCmdList( cmd_list *cmds );
extern void         TypeInpStack( input_type set );
extern void         ClearInpStack( input_type clear );
extern input_type   SetInpStack( input_type new );
extern void         PopInpStack( void );
extern void         PushInpStack( inp_data_handle handle, inp_rtn_func *rtn, bool save_lang );
extern void         CopyInpFlags( void );
extern void         PushCmdList( cmd_list *cmds );
#ifdef DEADCODE
extern void         PushCmdText( char *cmds );
#endif
extern bool         PurgeInpStack( void );
extern void         FreezeInpStack( void );
extern void         UnAsm( address addr, char *buff, size_t buff_len );
extern const char   *ModImageName( mod_handle handle );
