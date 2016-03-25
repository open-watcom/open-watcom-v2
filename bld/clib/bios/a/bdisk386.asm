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
;* Description:  BIOS disk access, 32-bit version.
;*
;*****************************************************************************


;
;
include mdef.inc
include struct.inc

        modstart b_disk

        defp    __ibm_bios_disk
        xdefp  "C",__ibm_bios_disk
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
ifdef __STACK__
        mov     EAX,4[ESP]      ; get service
        mov     EDX,8[ESP]      ; get pointer to structure
endif
        push    ES              ; save ES
        push    EBX             ; save BX
        push    ECX             ; save CX
        push    EDX             ; save DX
        mov     EBX,EDX         ; get pointer to diskinfo
        mov     AH,AL           ; get service
        mov     DL,[EBX]        ; get drive number
        cmp     AH,2            ; if function uses a buffer
        _if     ae              ; then
          mov   DH,4[EBX]       ; - get head
          mov   CX,8[EBX]       ; - track
          xchg  CH,CL           ; - shift into position
          ror   CL,2            ; - ...
          and   CL,0c0h         ; - ...
          or    CL,12[EBX]      ; - get sector number
          mov   AL,16[EBX]      ; - get number of sectors
          cmp   AH,4            ; - if function is not _DISK_VERIFY
          _if   ne              ; - then
            les EBX,20[EBX]     ; - - get buffer address
          _endif                ; endif
        _endif                  ; endif
        int     13h             ; issue disk request
        pop     EDX             ; restore BX
        pop     ECX             ; restore BX
        pop     EBX             ; restore BX
        pop     ES              ; restore ES
        ret                     ; return to caller
        endproc __ibm_bios_disk

        endmod
        end
