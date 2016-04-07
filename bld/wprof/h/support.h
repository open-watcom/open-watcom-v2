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
* Description:  Profiler support functions dealing with executable formats.
*
****************************************************************************/


extern void         AsmSize( void );
extern void         AsmFini( void );
extern file_handle  ExeOpen( char * name );
extern void         ExeClose( file_handle fh );
extern void         SetExeOffset( address a );
extern void         SetExeImage( image_info *image );
extern bool         SetExeFile( file_handle fh, bool overlay );
extern image_info   *AddrImage( address *addr );
extern void         MapAddressToActual( image_info *curr_image, addr_ptr *addr );
extern void         MapAddressToMap( addr_ptr *addr );
extern void         MapAddressIntoSection( address *addr );
extern bool         LoadImageOverlays( void );
extern void         SetNumBytes( uint_16 num );
extern void         CodeAdvance( address *addr );
extern size_t       FormatAddr( address a, char *buffer, size_t max );
extern void         GetFullInstruct( address a, char * buffer, size_t max );
extern bool         CnvAddr( address addr, char *buff, size_t buff_len );
extern bool         IsX86BigAddr( address a );
extern bool         IsX86RealAddr( address a );
extern int_16       GetDataByte( void );
extern bool         EndOfSegment( void );
