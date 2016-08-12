/****************************************************************************
*
*                            Open Watcom Project
*
* Copyright (c) 2016-2016 The Open Watcom Contributors. All Rights Reserved.
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
* Description:  DIG client callback function prototypes.
*
****************************************************************************/


pick( void *,           Alloc,          ( size_t ) )
pick( void *,           Realloc,        ( void *, size_t ) )
pick( void,             Free,           ( void * ) )
pick( dig_fhandle,      Open,           ( const char *, dig_open ) )
pick( unsigned long,    Seek,           ( dig_fhandle, unsigned long, dig_seek ) )
pick( size_t,           Read,           ( dig_fhandle, void *, size_t ) )
pick( size_t,           Write,          ( dig_fhandle, const void *, size_t ) )
pick( void,             Close,          ( dig_fhandle ) )
pick( void,             Remove,         ( const char *, dig_open ) )
pick( unsigned,         MachineData,    ( address, dig_info_type, dig_elen, const void *, dig_elen, void * ) )
