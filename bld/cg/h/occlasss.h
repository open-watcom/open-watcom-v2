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
* Description:  Pick file for OC class items definition.
*
****************************************************************************/


pick_class( OC_DEAD )            // 0x00
pick_class( OC_INFO )            // 0x01
pick_class( OC_CODE )            // 0x02
pick_class( OC_DATA )            // 0x03
pick_class( OC_RCODE )           // 0x04
pick_class( OC_BDATA )           // 0x05
pick_class( OC_LABEL )           // 0x06
pick_class( OC_LREF )            // 0x07
pick_class( OC_CALL )            // 0x08
pick_class( OC_CALLI )           // 0x09
pick_class( OC_JCOND )           // 0x0A
pick_class( OC_JCONDI )          // 0x0B unused
pick_class( OC_JMP )             // 0x0C
pick_class( OC_JMPI )            // 0x0D
pick_class( OC_RET )             // 0x0E
pick_class( OC_IDATA )           // 0x0F unused
