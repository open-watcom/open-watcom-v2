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


;
;
include mdef.inc
include struct.inc

        modstart b_disk

        defp    __ibm_bios_disk
        if __WASM__ ge 100
         xdefp  "C",__ibm_bios_disk
        else
         xdefp  <"C",__ibm_bios_disk>
        endif
;
;       unsigned __ibm_bios_disk( unsigned service, struct diskinfo_t *diskinfo );
;struct  diskinfo_t {            /* disk parameters */
;        unsigned drive;         /* drive number    */
;        unsigned head;          /* head number     */
;        unsigned track;         /* track number    */
;        unsigned sector;        /* sector number   */
;        unsigned nsectors;      /* number of sectors to read/write/compare  */
;        void far *buffer;       /* buffer to read to,write from, or compare */
;};
;
        push    ES              ; save ES
        push    BX              ; save BX
        push    CX              ; save CX
        push    DX              ; save DX
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        push    DS              ; save DS
        mov     DS,CX           ; point to diskinfo struct
else
        mov     BX,DX           ; get pointer to diskinfo
endif
        mov     AH,AL           ; get service
        mov     DL,[BX]         ; get drive number
        cmp     AH,2            ; if function uses a buffer
        _if     ae              ; then
          mov   DH,2[BX]        ; - get head
          mov   CX,4[BX]        ; - track
          xchg  CH,CL           ; - shift into position
          ror   CL,1            ; - ...
          ror   CL,1            ; - ...
          and   CL,0c0h         ; - ...
          or    CL,6[BX]        ; - get sector number
          mov   AL,8[BX]        ; - get number of sectors
          cmp   AH,4            ; - if function is not _DISK_VERIFY
          _if   ne              ; - then
            les BX,10[BX]       ; - - get buffer address
          _endif                ; endif
        _endif                  ; endif
        int     13h             ; issue disk request
if _MODEL and (_BIG_DATA or _HUGE_DATA)
        pop     DS              ; restore DS
endif
        pop     DX              ; restore BX
        pop     CX              ; restore BX
        pop     BX              ; restore BX
        pop     ES              ; restore ES
        ret                     ; return to caller
        endproc __ibm_bios_disk

        endmod
        end
