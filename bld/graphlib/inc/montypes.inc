;*****************************************************************************
;*
;*                            Open Watcom Project
;*
;*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
;*
;*  ========================================================================
;*
;*    This file contains Original Code and/or Modifications of Original
;*    Code as defined in and that are subject to the Sybase Open Watcom
;*    Public License version 1.0 (the 'License'). You may not use this file
;*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
;*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
;*    provided with the Original Code and Modifications, and is also
;*    available at www.sybase.com/developer/opensource.
;*
;*    The Original Code and all software distributed under the License are
;*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
;*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
;*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
;*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
;*    NON-INFRINGEMENT. Please see the License for the specific language
;*    governing rights and limitations under the License.
;*
;*  ========================================================================
;*
;* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
;*               DESCRIBE IT HERE!
;*
;*****************************************************************************


; monitor types returned by SysMonType_
;       AL = active display
;       AH = alternate display

MT_NODISPLAY    =       00h     ; no display in system
MT_MDPA         =       01h     ; IBM Mono Display/Printer Adapter
MT_CGA_COLOUR   =       02h     ; IBM Colour/Graphics Display Adapter
MT_JRCGA        =       03h     ; IBM PCjr Colour Graphics Adapter
MT_EGA_COLOUR   =       04h     ; IBM Enhanced Graphics Adapter (colour)
MT_EGA_MONO     =       05h     ; IBM Enhanced Graphics Adapter (mono)
MT_PGC          =       06h     ; IBM Professional Graphics Controller
MT_VGA_MONO     =       07h     ; IBM Video Graphics Array (mono)
MT_VGA_COLOUR   =       08h     ; IBM Video Graphics Array (colour)
MT_UNUSED       =       09h     ; reserved for future use
MT_MCGA_DIGITAL =       0Ah     ; IBM Multi-Colour Graphics (Digital Colour)
MT_MCGA_MONO    =       0Bh     ; IBM Multi-Colour Graphics (Analog Monochrome)
MT_MCGA_COLOUR  =       0Ch     ; IBM Multi-Colour Graphics (Analog Colour)
MT_HERC         =       0Dh     ; HERCULES Monochrome Graphics Adapter
MT_HERCPLUS     =       0Eh     ; HERCULES Graphics Adapter Plus
MT_HERCINCL     =       0Fh     ; HERCULES InColor Graphics Adapter
