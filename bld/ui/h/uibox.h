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
* Description:  Box drawing character definitions.
*
****************************************************************************/


#ifndef _UIBOX_H_
#define _UIBOX_H_
/*
 *    These are the definitions for characters that are used to
 *    make up the various boxes and lines that are used. Each
 *    symbolic name has 4 letters, one for each possible piece
 *    of the character (clockwise starting at the top). B stands
 *    for blank, S for single, and D for double. For example,
 *    BSSB is the top left corner of a single line box.
 *
 *               |
 *             -- --  thus BSSB ->    --
 *               |                   |
 */

#if 0

    #define     B_SBSB                  (char) 0xb3
    #define     B_SBSS                  (char) 0xb4
    #define     B_SBSD                  (char) 0xb5
    #define     B_DBDS                  (char) 0xb6
    #define     B_BBDS                  (char) 0xb7
    #define     B_BBSD                  (char) 0xb8
    #define     B_DBDD                  (char) 0xb9
    #define     B_DBDB                  (char) 0xba
    #define     B_BBDD                  (char) 0xbb
    #define     B_DBBD                  (char) 0xbc
    #define     B_DBBS                  (char) 0xbd
    #define     B_SBBD                  (char) 0xbe
    #define     B_BBSS                  (char) 0xbf
    #define     B_SSBB                  (char) 0xc0
    #define     B_SSBS                  (char) 0xc1
    #define     B_BSSS                  (char) 0xc2
    #define     B_SSSB                  (char) 0xc3
    #define     B_BSBS                  (char) 0xc4
    #define     B_SSSS                  (char) 0xc5
    #define     B_SDSB                  (char) 0xc6
    #define     B_DSDB                  (char) 0xc7
    #define     B_DDBB                  (char) 0xc8
    #define     B_BDDB                  (char) 0xc9
    #define     B_DDBD                  (char) 0xca
    #define     B_BDDD                  (char) 0xcb
    #define     B_DDDB                  (char) 0xcc
    #define     B_BDBD                  (char) 0xcd
    #define     B_DDDD                  (char) 0xce
    #define     B_SDBD                  (char) 0xcf
    #define     B_DSBS                  (char) 0xd0
    #define     B_BDSD                  (char) 0xd1
    #define     B_BSDS                  (char) 0xd2
    #define     B_DSBB                  (char) 0xd3
    #define     B_SDBB                  (char) 0xd4
    #define     B_BDSB                  (char) 0xd5
    #define     B_BSDB                  (char) 0xd6
    #define     B_DSDS                  (char) 0xd7
    #define     B_SDSD                  (char) 0xd8
    #define     B_SBBS                  (char) 0xd9
    #define     B_BSSB                  (char) 0xda

    #define     B_FBLK                  (char) 0xdb
    #define     B_DBLK                  (char) 0xdc
    #define     B_LBLK                  (char) 0xdd
    #define     B_RBLK                  (char) 0xde
    #define     B_UBLK                  (char) 0xdf

#endif
#endif
