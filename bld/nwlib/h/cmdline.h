/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2024      The Open Watcom Contributors. All Rights Reserved.
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
* Description:  Library manager command line processing.
*
****************************************************************************/


typedef enum {
    SCTRL_NORMAL = 0x00,
    SCTRL_SINGLE = 0x01,
    SCTRL_IMPORT = 0x02,
    SCTRL_EQUAL  = 0x04,
} scan_ctrl;

extern void         InitCmdLine( void );
extern void         ProcessCmdLine( char *argv[] );
extern void         FiniCmdLine( void );
extern char         *GetString( const char **c, scan_ctrl sctrl );
extern char         *GetFilenameExt( const char **c, scan_ctrl sctrl, const char *ext );
extern void         AddCommand( operation ops, const char **c, scan_ctrl sctrl );

extern const char   *SkipEqual( const char *c );
extern const char   *SkipWhite( const char *c );

extern const char   *CmdSetPos( const char *new_cmd );
extern const char   *CmdGetPos( void );
extern void         CmdSkipWhite( void );
extern void         CmdSkipEqual( void );
extern int          CmdPeekChar( void );
extern int          CmdPeekLowerChar( void );
extern int          CmdGetChar( void );
extern int          CmdGetLowerChar( void );
extern bool         CmdRecogChar( int );
extern bool         CmdRecogLowerChar( int );
extern void         CmdUngetChar( void );

extern lib_cmd      *CmdList;
extern options_def  Options;
