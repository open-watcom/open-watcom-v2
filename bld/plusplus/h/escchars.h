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
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


//  escape codes to be used when recognized by scanner

// from ISO C 2.2.2
#define ESCAPE_a        0x07    // alert
#define ESCAPE_b        0x08    // backspace
#define ESCAPE_f        0x0C    // form feed
#define ESCAPE_n        0x0A    // new line
#define ESCAPE_r        0x0D    // carriage return
#define ESCAPE_t        0x09    // horizontal tab
#define ESCAPE_v        0x0B    // vertical tab

//  escape classifications

#define ESCAPE_NONE     0   // - no escaping
#define ESCAPE_OCTAL    1   // - escape octal found
#define ESCAPE_HEX      2   // - \x found
//                          // - otherwise, classified as \char (ESCAPE_?)
