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


    name font8x8

ifdef __386__
    _DATA segment word public 'DATA'
else
    Font8x8_DATA segment para 'FONT_DATA'
endif

    public __8x8Font
    public __8x8BitMap


__8x8BitMap label byte
    DB      000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    DB      07eh, 081h, 0a5h, 081h, 0bdh, 099h, 081h, 07eh
    DB      07eh, 0ffh, 0dbh, 0ffh, 0c3h, 0e7h, 0ffh, 07eh
    DB      06ch, 0feh, 0feh, 0feh, 07ch, 038h, 010h, 000h
    DB      010h, 038h, 07ch, 0feh, 07ch, 038h, 010h, 000h
    DB      038h, 07ch, 038h, 0feh, 0feh, 07ch, 038h, 07ch
    DB      010h, 010h, 038h, 07ch, 0feh, 07ch, 038h, 07ch
    DB      000h, 000h, 018h, 03ch, 03ch, 018h, 000h, 000h
    DB      0ffh, 0ffh, 0e7h, 0c3h, 0c3h, 0e7h, 0ffh, 0ffh
    DB      000h, 03ch, 066h, 042h, 042h, 066h, 03ch, 000h
    DB      0ffh, 0c3h, 099h, 0bdh, 0bdh, 099h, 0c3h, 0ffh
    DB      00fh, 007h, 00fh, 07dh, 0cch, 0cch, 0cch, 078h
    DB      03ch, 066h, 066h, 066h, 03ch, 018h, 07eh, 018h
    DB      03fh, 033h, 03fh, 030h, 030h, 070h, 0f0h, 0e0h
    DB      07fh, 063h, 07fh, 063h, 063h, 067h, 0e6h, 0c0h
    DB      099h, 05ah, 03ch, 0e7h, 0e7h, 03ch, 05ah, 099h
    DB      080h, 0e0h, 0f8h, 0feh, 0f8h, 0e0h, 080h, 000h
    DB      002h, 00eh, 03eh, 0feh, 03eh, 00eh, 002h, 000h
    DB      018h, 03ch, 07eh, 018h, 018h, 07eh, 03ch, 018h
    DB      066h, 066h, 066h, 066h, 066h, 000h, 066h, 000h
    DB      07fh, 0dbh, 0dbh, 07bh, 01bh, 01bh, 01bh, 000h
    DB      03eh, 063h, 038h, 06ch, 06ch, 038h, 0cch, 078h
    DB      000h, 000h, 000h, 000h, 07eh, 07eh, 07eh, 000h
    DB      018h, 03ch, 07eh, 018h, 07eh, 03ch, 018h, 0ffh
    DB      018h, 03ch, 07eh, 018h, 018h, 018h, 018h, 000h
    DB      018h, 018h, 018h, 018h, 07eh, 03ch, 018h, 000h
    DB      000h, 018h, 00ch, 0feh, 00ch, 018h, 000h, 000h
    DB      000h, 030h, 060h, 0feh, 060h, 030h, 000h, 000h
    DB      000h, 000h, 0c0h, 0c0h, 0c0h, 0feh, 000h, 000h
    DB      000h, 024h, 066h, 0ffh, 066h, 024h, 000h, 000h
    DB      000h, 018h, 03ch, 07eh, 0ffh, 0ffh, 000h, 000h
    DB      000h, 0ffh, 0ffh, 07eh, 03ch, 018h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h
    DB      030h, 078h, 078h, 030h, 030h, 000h, 030h, 000h
    DB      06ch, 06ch, 06ch, 000h, 000h, 000h, 000h, 000h
    DB      06ch, 06ch, 0feh, 06ch, 0feh, 06ch, 06ch, 000h
    DB      030h, 07ch, 0c0h, 078h, 00ch, 0f8h, 030h, 000h
    DB      000h, 0c6h, 0cch, 018h, 030h, 066h, 0c6h, 000h
    DB      038h, 06ch, 038h, 076h, 0dch, 0cch, 076h, 000h
    DB      060h, 060h, 0c0h, 000h, 000h, 000h, 000h, 000h
    DB      018h, 030h, 060h, 060h, 060h, 030h, 018h, 000h
    DB      060h, 030h, 018h, 018h, 018h, 030h, 060h, 000h
    DB      000h, 066h, 03ch, 0ffh, 03ch, 066h, 000h, 000h
    DB      000h, 030h, 030h, 0fch, 030h, 030h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 000h, 030h, 030h, 060h
    DB      000h, 000h, 000h, 0fch, 000h, 000h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 000h, 030h, 030h, 000h
    DB      006h, 00ch, 018h, 030h, 060h, 0c0h, 080h, 000h
    DB      07ch, 0c6h, 0ceh, 0deh, 0f6h, 0e6h, 07ch, 000h
    DB      030h, 070h, 030h, 030h, 030h, 030h, 0fch, 000h
    DB      078h, 0cch, 00ch, 038h, 060h, 0cch, 0fch, 000h
    DB      078h, 0cch, 00ch, 038h, 00ch, 0cch, 078h, 000h
    DB      01ch, 03ch, 06ch, 0cch, 0feh, 00ch, 01eh, 000h
    DB      0fch, 0c0h, 0f8h, 00ch, 00ch, 0cch, 078h, 000h
    DB      038h, 060h, 0c0h, 0f8h, 0cch, 0cch, 078h, 000h
    DB      0fch, 0cch, 00ch, 018h, 030h, 030h, 030h, 000h
    DB      078h, 0cch, 0cch, 078h, 0cch, 0cch, 078h, 000h
    DB      078h, 0cch, 0cch, 07ch, 00ch, 018h, 070h, 000h
    DB      000h, 030h, 030h, 000h, 000h, 030h, 030h, 000h
    DB      000h, 030h, 030h, 000h, 000h, 030h, 030h, 060h
    DB      018h, 030h, 060h, 0c0h, 060h, 030h, 018h, 000h
    DB      000h, 000h, 0fch, 000h, 000h, 0fch, 000h, 000h
    DB      060h, 030h, 018h, 00ch, 018h, 030h, 060h, 000h
    DB      078h, 0cch, 00ch, 018h, 030h, 000h, 030h, 000h
    DB      07ch, 0c6h, 0deh, 0deh, 0deh, 0c0h, 078h, 000h
    DB      030h, 078h, 0cch, 0cch, 0fch, 0cch, 0cch, 000h
    DB      0fch, 066h, 066h, 07ch, 066h, 066h, 0fch, 000h
    DB      03ch, 066h, 0c0h, 0c0h, 0c0h, 066h, 03ch, 000h
    DB      0f8h, 06ch, 066h, 066h, 066h, 06ch, 0f8h, 000h
    DB      0feh, 062h, 068h, 078h, 068h, 062h, 0feh, 000h
    DB      0feh, 062h, 068h, 078h, 068h, 060h, 0f0h, 000h
    DB      03ch, 066h, 0c0h, 0c0h, 0ceh, 066h, 03eh, 000h
    DB      0cch, 0cch, 0cch, 0fch, 0cch, 0cch, 0cch, 000h
    DB      078h, 030h, 030h, 030h, 030h, 030h, 078h, 000h
    DB      01eh, 00ch, 00ch, 00ch, 0cch, 0cch, 078h, 000h
    DB      0e6h, 066h, 06ch, 078h, 06ch, 066h, 0e6h, 000h
    DB      0f0h, 060h, 060h, 060h, 062h, 066h, 0feh, 000h
    DB      0c6h, 0eeh, 0feh, 0feh, 0d6h, 0c6h, 0c6h, 000h
    DB      0c6h, 0e6h, 0f6h, 0deh, 0ceh, 0c6h, 0c6h, 000h
    DB      038h, 06ch, 0c6h, 0c6h, 0c6h, 06ch, 038h, 000h
    DB      0fch, 066h, 066h, 07ch, 060h, 060h, 0f0h, 000h
    DB      078h, 0cch, 0cch, 0cch, 0dch, 078h, 01ch, 000h
    DB      0fch, 066h, 066h, 07ch, 06ch, 066h, 0e6h, 000h
    DB      078h, 0cch, 0e0h, 070h, 01ch, 0cch, 078h, 000h
    DB      0fch, 0b4h, 030h, 030h, 030h, 030h, 078h, 000h
    DB      0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 0fch, 000h
    DB      0cch, 0cch, 0cch, 0cch, 0cch, 078h, 030h, 000h
    DB      0c6h, 0c6h, 0c6h, 0d6h, 0feh, 0eeh, 0c6h, 000h
    DB      0c6h, 0c6h, 06ch, 038h, 038h, 06ch, 0c6h, 000h
    DB      0cch, 0cch, 0cch, 078h, 030h, 030h, 078h, 000h
    DB      0feh, 0c6h, 08ch, 018h, 032h, 066h, 0feh, 000h
    DB      078h, 060h, 060h, 060h, 060h, 060h, 078h, 000h
    DB      0c0h, 060h, 030h, 018h, 00ch, 006h, 002h, 000h
    DB      078h, 018h, 018h, 018h, 018h, 018h, 078h, 000h
    DB      010h, 038h, 06ch, 0c6h, 000h, 000h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 000h, 000h, 000h, 0ffh
    DB      030h, 030h, 018h, 000h, 000h, 000h, 000h, 000h
    DB      000h, 000h, 078h, 00ch, 07ch, 0cch, 076h, 000h
    DB      0e0h, 060h, 060h, 07ch, 066h, 066h, 0dch, 000h
    DB      000h, 000h, 078h, 0cch, 0c0h, 0cch, 078h, 000h
    DB      01ch, 00ch, 00ch, 07ch, 0cch, 0cch, 076h, 000h
    DB      000h, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    DB      038h, 06ch, 060h, 0f0h, 060h, 060h, 0f0h, 000h
    DB      000h, 000h, 076h, 0cch, 0cch, 07ch, 00ch, 0f8h
    DB      0e0h, 060h, 06ch, 076h, 066h, 066h, 0e6h, 000h
    DB      030h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    DB      00ch, 000h, 00ch, 00ch, 00ch, 0cch, 0cch, 078h
    DB      0e0h, 060h, 066h, 06ch, 078h, 06ch, 0e6h, 000h
    DB      070h, 030h, 030h, 030h, 030h, 030h, 078h, 000h
    DB      000h, 000h, 0cch, 0feh, 0feh, 0d6h, 0c6h, 000h
    DB      000h, 000h, 0f8h, 0cch, 0cch, 0cch, 0cch, 000h
    DB      000h, 000h, 078h, 0cch, 0cch, 0cch, 078h, 000h
    DB      000h, 000h, 0dch, 066h, 066h, 07ch, 060h, 0f0h
    DB      000h, 000h, 076h, 0cch, 0cch, 07ch, 00ch, 01eh
    DB      000h, 000h, 0dch, 076h, 066h, 060h, 0f0h, 000h
    DB      000h, 000h, 07ch, 0c0h, 078h, 00ch, 0f8h, 000h
    DB      010h, 030h, 07ch, 030h, 030h, 034h, 018h, 000h
    DB      000h, 000h, 0cch, 0cch, 0cch, 0cch, 076h, 000h
    DB      000h, 000h, 0cch, 0cch, 0cch, 078h, 030h, 000h
    DB      000h, 000h, 0c6h, 0d6h, 0feh, 0feh, 06ch, 000h
    DB      000h, 000h, 0c6h, 06ch, 038h, 06ch, 0c6h, 000h
    DB      000h, 000h, 0cch, 0cch, 0cch, 07ch, 00ch, 0f8h
    DB      000h, 000h, 0fch, 098h, 030h, 064h, 0fch, 000h
    DB      01ch, 030h, 030h, 0e0h, 030h, 030h, 01ch, 000h
    DB      018h, 018h, 018h, 000h, 018h, 018h, 018h, 000h
    DB      0e0h, 030h, 030h, 01ch, 030h, 030h, 0e0h, 000h
    DB      076h, 0dch, 000h, 000h, 000h, 000h, 000h, 000h
    DB      000h, 010h, 038h, 06ch, 0c6h, 0c6h, 0feh, 000h
    DB      078h, 0cch, 0c0h, 0cch, 078h, 018h, 00ch, 078h
    DB      000h, 0cch, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    DB      01ch, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    DB      07eh, 0c3h, 03ch, 006h, 03eh, 066h, 03fh, 000h
    DB      0cch, 000h, 078h, 00ch, 07ch, 0cch, 07eh, 000h
    DB      0e0h, 000h, 078h, 00ch, 07ch, 0cch, 07eh, 000h
    DB      030h, 030h, 078h, 00ch, 07ch, 0cch, 07eh, 000h
    DB      000h, 000h, 078h, 0c0h, 0c0h, 078h, 00ch, 038h
    DB      07eh, 0c3h, 03ch, 066h, 07eh, 060h, 03ch, 000h
    DB      0cch, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    DB      0e0h, 000h, 078h, 0cch, 0fch, 0c0h, 078h, 000h
    DB      0cch, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    DB      07ch, 0c6h, 038h, 018h, 018h, 018h, 03ch, 000h
    DB      0e0h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    DB      0c6h, 038h, 06ch, 0c6h, 0feh, 0c6h, 0c6h, 000h
    DB      030h, 030h, 000h, 078h, 0cch, 0fch, 0cch, 000h
    DB      01ch, 000h, 0fch, 060h, 078h, 060h, 0fch, 000h
    DB      000h, 000h, 07fh, 00ch, 07fh, 0cch, 07fh, 000h
    DB      03eh, 06ch, 0cch, 0feh, 0cch, 0cch, 0ceh, 000h
    DB      078h, 0cch, 000h, 078h, 0cch, 0cch, 078h, 000h
    DB      000h, 0cch, 000h, 078h, 0cch, 0cch, 078h, 000h
    DB      000h, 0e0h, 000h, 078h, 0cch, 0cch, 078h, 000h
    DB      078h, 0cch, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    DB      000h, 0e0h, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    DB      000h, 0cch, 000h, 0cch, 0cch, 07ch, 00ch, 0f8h
    DB      0c3h, 018h, 03ch, 066h, 066h, 03ch, 018h, 000h
    DB      0cch, 000h, 0cch, 0cch, 0cch, 0cch, 078h, 000h
    DB      018h, 018h, 07eh, 0c0h, 0c0h, 07eh, 018h, 018h
    DB      038h, 06ch, 064h, 0f0h, 060h, 0e6h, 0fch, 000h
    DB      0cch, 0cch, 078h, 0fch, 030h, 0fch, 030h, 030h
    DB      0f8h, 0cch, 0cch, 0fah, 0c6h, 0cfh, 0c6h, 0c7h
    DB      00eh, 01bh, 018h, 03ch, 018h, 018h, 0d8h, 070h
    DB      01ch, 000h, 078h, 00ch, 07ch, 0cch, 07eh, 000h
    DB      038h, 000h, 070h, 030h, 030h, 030h, 078h, 000h
    DB      000h, 01ch, 000h, 078h, 0cch, 0cch, 078h, 000h
    DB      000h, 01ch, 000h, 0cch, 0cch, 0cch, 07eh, 000h
    DB      000h, 0f8h, 000h, 0f8h, 0cch, 0cch, 0cch, 000h
    DB      0fch, 000h, 0cch, 0ech, 0fch, 0dch, 0cch, 000h
    DB      03ch, 06ch, 06ch, 03eh, 000h, 07eh, 000h, 000h
    DB      038h, 06ch, 06ch, 038h, 000h, 07ch, 000h, 000h
    DB      030h, 000h, 030h, 060h, 0c0h, 0cch, 078h, 000h
    DB      000h, 000h, 000h, 0fch, 0c0h, 0c0h, 000h, 000h
    DB      000h, 000h, 000h, 0fch, 00ch, 00ch, 000h, 000h
    DB      0c3h, 0c6h, 0cch, 0deh, 033h, 066h, 0cch, 00fh
    DB      0c3h, 0c6h, 0cch, 0dbh, 037h, 06fh, 0cfh, 003h
    DB      018h, 018h, 000h, 018h, 018h, 018h, 018h, 000h
    DB      000h, 033h, 066h, 0cch, 066h, 033h, 000h, 000h
    DB      000h, 0cch, 066h, 033h, 066h, 0cch, 000h, 000h
    DB      022h, 088h, 022h, 088h, 022h, 088h, 022h, 088h
    DB      055h, 0aah, 055h, 0aah, 055h, 0aah, 055h, 0aah
    DB      0dbh, 077h, 0dbh, 0eeh, 0dbh, 077h, 0dbh, 0eeh
    DB      018h, 018h, 018h, 018h, 018h, 018h, 018h, 018h
    DB      018h, 018h, 018h, 018h, 0f8h, 018h, 018h, 018h
    DB      018h, 018h, 0f8h, 018h, 0f8h, 018h, 018h, 018h
    DB      036h, 036h, 036h, 036h, 0f6h, 036h, 036h, 036h
    DB      000h, 000h, 000h, 000h, 0feh, 036h, 036h, 036h
    DB      000h, 000h, 0f8h, 018h, 0f8h, 018h, 018h, 018h
    DB      036h, 036h, 0f6h, 006h, 0f6h, 036h, 036h, 036h
    DB      036h, 036h, 036h, 036h, 036h, 036h, 036h, 036h
    DB      000h, 000h, 0feh, 006h, 0f6h, 036h, 036h, 036h
    DB      036h, 036h, 0f6h, 006h, 0feh, 000h, 000h, 000h
    DB      036h, 036h, 036h, 036h, 0feh, 000h, 000h, 000h
    DB      018h, 018h, 0f8h, 018h, 0f8h, 000h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 0f8h, 018h, 018h, 018h
    DB      018h, 018h, 018h, 018h, 01fh, 000h, 000h, 000h
    DB      018h, 018h, 018h, 018h, 0ffh, 000h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 0ffh, 018h, 018h, 018h
    DB      018h, 018h, 018h, 018h, 01fh, 018h, 018h, 018h
    DB      000h, 000h, 000h, 000h, 0ffh, 000h, 000h, 000h
    DB      018h, 018h, 018h, 018h, 0ffh, 018h, 018h, 018h
    DB      018h, 018h, 01fh, 018h, 01fh, 018h, 018h, 018h
    DB      036h, 036h, 036h, 036h, 037h, 036h, 036h, 036h
    DB      036h, 036h, 037h, 030h, 03fh, 000h, 000h, 000h
    DB      000h, 000h, 03fh, 030h, 037h, 036h, 036h, 036h
    DB      036h, 036h, 0f7h, 000h, 0ffh, 000h, 000h, 000h
    DB      000h, 000h, 0ffh, 000h, 0f7h, 036h, 036h, 036h
    DB      036h, 036h, 037h, 030h, 037h, 036h, 036h, 036h
    DB      000h, 000h, 0ffh, 000h, 0ffh, 000h, 000h, 000h
    DB      036h, 036h, 0f7h, 000h, 0f7h, 036h, 036h, 036h
    DB      018h, 018h, 0ffh, 000h, 0ffh, 000h, 000h, 000h
    DB      036h, 036h, 036h, 036h, 0ffh, 000h, 000h, 000h
    DB      000h, 000h, 0ffh, 000h, 0ffh, 018h, 018h, 018h
    DB      000h, 000h, 000h, 000h, 0ffh, 036h, 036h, 036h
    DB      036h, 036h, 036h, 036h, 03fh, 000h, 000h, 000h
    DB      018h, 018h, 01fh, 018h, 01fh, 000h, 000h, 000h
    DB      000h, 000h, 01fh, 018h, 01fh, 018h, 018h, 018h
    DB      000h, 000h, 000h, 000h, 03fh, 036h, 036h, 036h
    DB      036h, 036h, 036h, 036h, 0ffh, 036h, 036h, 036h
    DB      018h, 018h, 0ffh, 018h, 0ffh, 018h, 018h, 018h
    DB      018h, 018h, 018h, 018h, 0f8h, 000h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 01fh, 018h, 018h, 018h
    DB      0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh, 0ffh
    DB      000h, 000h, 000h, 000h, 0ffh, 0ffh, 0ffh, 0ffh
    DB      0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h, 0f0h
    DB      00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh, 00fh
    DB      0ffh, 0ffh, 0ffh, 0ffh, 000h, 000h, 000h, 000h
    DB      000h, 000h, 076h, 0dch, 0c8h, 0dch, 076h, 000h
    DB      000h, 078h, 0cch, 0f8h, 0cch, 0f8h, 0c0h, 0c0h
    DB      000h, 0fch, 0cch, 0c0h, 0c0h, 0c0h, 0c0h, 000h
    DB      000h, 0feh, 06ch, 06ch, 06ch, 06ch, 06ch, 000h
    DB      0fch, 0cch, 060h, 030h, 060h, 0cch, 0fch, 000h
    DB      000h, 000h, 07eh, 0d8h, 0d8h, 0d8h, 070h, 000h
    DB      000h, 066h, 066h, 066h, 066h, 07ch, 060h, 0c0h
    DB      000h, 076h, 0dch, 018h, 018h, 018h, 018h, 000h
    DB      0fch, 030h, 078h, 0cch, 0cch, 078h, 030h, 0fch
    DB      038h, 06ch, 0c6h, 0feh, 0c6h, 06ch, 038h, 000h
    DB      038h, 06ch, 0c6h, 0c6h, 06ch, 06ch, 0eeh, 000h
    DB      01ch, 030h, 018h, 07ch, 0cch, 0cch, 078h, 000h
    DB      000h, 000h, 07eh, 0dbh, 0dbh, 07eh, 000h, 000h
    DB      006h, 00ch, 07eh, 0dbh, 0dbh, 07eh, 060h, 0c0h
    DB      038h, 060h, 0c0h, 0f8h, 0c0h, 060h, 038h, 000h
    DB      078h, 0cch, 0cch, 0cch, 0cch, 0cch, 0cch, 000h
    DB      000h, 0fch, 000h, 0fch, 000h, 0fch, 000h, 000h
    DB      030h, 030h, 0fch, 030h, 030h, 000h, 0fch, 000h
    DB      060h, 030h, 018h, 030h, 060h, 000h, 0fch, 000h
    DB      018h, 030h, 060h, 030h, 018h, 000h, 0fch, 000h
    DB      00eh, 01bh, 01bh, 018h, 018h, 018h, 018h, 018h
    DB      018h, 018h, 018h, 018h, 018h, 0d8h, 0d8h, 070h
    DB      030h, 030h, 000h, 0fch, 000h, 030h, 030h, 000h
    DB      000h, 076h, 0dch, 000h, 076h, 0dch, 000h, 000h
    DB      038h, 06ch, 06ch, 038h, 000h, 000h, 000h, 000h
    DB      000h, 000h, 000h, 018h, 018h, 000h, 000h, 000h
    DB      000h, 000h, 000h, 000h, 018h, 000h, 000h, 000h
    DB      00fh, 00ch, 00ch, 00ch, 0ech, 06ch, 03ch, 01ch
    DB      078h, 06ch, 06ch, 06ch, 06ch, 000h, 000h, 000h
    DB      070h, 018h, 030h, 060h, 078h, 000h, 000h, 000h
    DB      000h, 000h, 03ch, 03ch, 03ch, 03ch, 000h, 000h
    DB      000h, 000h, 000h, 000h, 000h, 000h, 000h, 000h

GlyphTable label byte
    DW      8,    0, 8,    8, 8,   16, 8,   24
    DW      8,   32, 8,   40, 8,   48, 8,   56
    DW      8,   64, 8,   72, 8,   80, 8,   88
    DW      8,   96, 8,  104, 8,  112, 8,  120
    DW      8,  128, 8,  136, 8,  144, 8,  152
    DW      8,  160, 8,  168, 8,  176, 8,  184
    DW      8,  192, 8,  200, 8,  208, 8,  216
    DW      8,  224, 8,  232, 8,  240, 8,  248
    DW      8,  256, 8,  264, 8,  272, 8,  280
    DW      8,  288, 8,  296, 8,  304, 8,  312
    DW      8,  320, 8,  328, 8,  336, 8,  344
    DW      8,  352, 8,  360, 8,  368, 8,  376
    DW      8,  384, 8,  392, 8,  400, 8,  408
    DW      8,  416, 8,  424, 8,  432, 8,  440
    DW      8,  448, 8,  456, 8,  464, 8,  472
    DW      8,  480, 8,  488, 8,  496, 8,  504
    DW      8,  512, 8,  520, 8,  528, 8,  536
    DW      8,  544, 8,  552, 8,  560, 8,  568
    DW      8,  576, 8,  584, 8,  592, 8,  600
    DW      8,  608, 8,  616, 8,  624, 8,  632
    DW      8,  640, 8,  648, 8,  656, 8,  664
    DW      8,  672, 8,  680, 8,  688, 8,  696
    DW      8,  704, 8,  712, 8,  720, 8,  728
    DW      8,  736, 8,  744, 8,  752, 8,  760
    DW      8,  768, 8,  776, 8,  784, 8,  792
    DW      8,  800, 8,  808, 8,  816, 8,  824
    DW      8,  832, 8,  840, 8,  848, 8,  856
    DW      8,  864, 8,  872, 8,  880, 8,  888
    DW      8,  896, 8,  904, 8,  912, 8,  920
    DW      8,  928, 8,  936, 8,  944, 8,  952
    DW      8,  960, 8,  968, 8,  976, 8,  984
    DW      8,  992, 8, 1000, 8, 1008, 8, 1016
    DW      8, 1024, 8, 1032, 8, 1040, 8, 1048
    DW      8, 1056, 8, 1064, 8, 1072, 8, 1080
    DW      8, 1088, 8, 1096, 8, 1104, 8, 1112
    DW      8, 1120, 8, 1128, 8, 1136, 8, 1144
    DW      8, 1152, 8, 1160, 8, 1168, 8, 1176
    DW      8, 1184, 8, 1192, 8, 1200, 8, 1208
    DW      8, 1216, 8, 1224, 8, 1232, 8, 1240
    DW      8, 1248, 8, 1256, 8, 1264, 8, 1272
    DW      8, 1280, 8, 1288, 8, 1296, 8, 1304
    DW      8, 1312, 8, 1320, 8, 1328, 8, 1336
    DW      8, 1344, 8, 1352, 8, 1360, 8, 1368
    DW      8, 1376, 8, 1384, 8, 1392, 8, 1400
    DW      8, 1408, 8, 1416, 8, 1424, 8, 1432
    DW      8, 1440, 8, 1448, 8, 1456, 8, 1464
    DW      8, 1472, 8, 1480, 8, 1488, 8, 1496
    DW      8, 1504, 8, 1512, 8, 1520, 8, 1528
    DW      8, 1536, 8, 1544, 8, 1552, 8, 1560
    DW      8, 1568, 8, 1576, 8, 1584, 8, 1592
    DW      8, 1600, 8, 1608, 8, 1616, 8, 1624
    DW      8, 1632, 8, 1640, 8, 1648, 8, 1656
    DW      8, 1664, 8, 1672, 8, 1680, 8, 1688
    DW      8, 1696, 8, 1704, 8, 1712, 8, 1720
    DW      8, 1728, 8, 1736, 8, 1744, 8, 1752
    DW      8, 1760, 8, 1768, 8, 1776, 8, 1784
    DW      8, 1792, 8, 1800, 8, 1808, 8, 1816
    DW      8, 1824, 8, 1832, 8, 1840, 8, 1848
    DW      8, 1856, 8, 1864, 8, 1872, 8, 1880
    DW      8, 1888, 8, 1896, 8, 1904, 8, 1912
    DW      8, 1920, 8, 1928, 8, 1936, 8, 1944
    DW      8, 1952, 8, 1960, 8, 1968, 8, 1976
    DW      8, 1984, 8, 1992, 8, 2000, 8, 2008
    DW      8, 2016, 8, 2024, 8, 2032, 8, 2040

__8x8Font label byte        ; FONT_ENTRY structure
    DW      0               ; short       type;
    DW      7               ; short       ascent;
    DW      8               ; short       width;
    DW      8               ; short       height;
    DW      8               ; short       avgwidth;
    DW      0               ; short       firstchar;
    DW      255             ; short       lastchar;
    DB      81 dup( 0 )     ; char        filename[ 81 ];
    DB      32 dup( 0 )     ; char        facename[ 32 ];
    DB      0               ; char        filler;
    DW      200h            ; short       version;
    DD      GlyphTable      ; char _FAR   *glyph_table;
    DD      __8x8BitMap     ; char _FAR   *bitmap_table;
    DD      0               ; long        start_offset;
    DD      0               ; long        glyph_offset;
    DD      0               ; long        bitmap_offset;
    DW      0               ; short       bitmap_size;

ifdef __386__
    _DATA ends
else
    Font8x8_DATA ends
endif

    end
