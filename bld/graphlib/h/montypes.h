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


/* monitor types returned by SysMonType_
 *       AL = active display
 *       AH = alternate display
 */

enum {
    MT_NODISPLAY,       // no display in system
    MT_MDPA,            // IBM Mono Display/Printer Adapter
    MT_CGA_COLOUR,      // IBM Colour/Graphics Display Adapter
    MT_EGA_ENHANCED,    // IBM EGA (enhanced colour)
    MT_EGA_COLOUR,      // IBM EGA (colour)
    MT_EGA_MONO,        // IBM EGA (mono)
    MT_PGC,             // IBM Professional Graphics Controller
    MT_VGA_MONO,        // IBM Video Graphics Array (mono)
    MT_VGA_COLOUR,      // IBM Video Graphics Array (colour)
    MT_JRCGA,           // IBM PCjr Colour Graphics Adapter
    MT_MCGA_DIGITAL,    // IBM Multi-Colour Graphics (Digital Colour)
    MT_MCGA_MONO,       // IBM Multi-Colour Graphics (Analog Monochrome)
    MT_MCGA_COLOUR,     // IBM Multi-Colour Graphics (Analog Colour)
    MT_HERC,            // HERCULES Monochrome Graphics Adapter
    MT_HERCPLUS,        // HERCULES Graphics Adapter Plus
    MT_HERCINCL,        // HERCULES InColor Graphics Adapter
    MT_SVGA_MONO,       // SuperVGA (mono)
    MT_SVGA_COLOUR      // SuperVGA (colour)
};
