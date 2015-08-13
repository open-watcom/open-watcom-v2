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
* Description:  Debugger lexical scanner.
*
****************************************************************************/


extern bool         scan_string;
extern char         *StringStart;
extern unsigned     StringLength;
extern unsigned     ScanCCharNum;


extern void         InitScan( void );
extern void         FiniScan( void );
extern const char   *ScanPos( void );
extern unsigned     ScanLen( void );
extern int          ScanCmd( const char *cmd_table );
extern bool         ScanEOC( void );
extern bool         TokenName( unsigned token, const char **start, unsigned *len );
extern void         Recog( unsigned token );
extern bool         ScanQuote( const char **start, size_t *len );
extern bool         ScanItem( bool blank_delim, const char **start, size_t *len );
extern bool         ScanItemDelim( const char *delim, bool blank_delim, const char **start, size_t *len );
extern void         ReqEOC( void );
extern void         FlushEOC( void );
extern const char   *NamePos( void );
extern unsigned     NameLen( void );
extern const char   *ReScan( const char *point );
extern void         ScanExpr( token_table *tbl );
extern void         AddActualChar( char data );
extern void         AddChar( void );
extern void         AddCEscapeChar( void );
extern void         Scan( void );
extern void         RawScanInit( void );
extern char         RawScanChar( void );
extern void         RawScanAdvance( void );
extern void         RawScanFini( void );
extern unsigned     NewCurrRadix( unsigned radix );
extern unsigned     SetCurrRadix( unsigned radix );
extern void         RestoreRadix( void );
extern void         DefaultRadixSet( unsigned radix );
extern void         RadixSet( void );
extern void         RadixConf( void );
extern void         FindRadixSpec( unsigned char value, const char **start, unsigned *len );
extern char         *AddHexSpec( char *p );
