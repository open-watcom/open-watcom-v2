                .286
code            segment
assume          cs:code, ds:code

;
; QNX Boot loader.  This is on block one of the QNX partition.
; The PC/AT BIOS reads this block into 7000h and transfers control to it
;
; N.B. Actually, it's loaded at 0:7C00 [bac]
;

BIOS_TIMER      =       046ch   ; Where the BIOS keeps the tick count (18/sec)

BLK_SIZE        =       512     ; Number bytes in a disk block

stk             =       0800h   ; stack (grows down)
xtnttab         =       0400h   ; Where we read the extent table
block2          =       0200h   ; Where we read QNX partition block 2

xtnt            struc           ; structure offsets of directory entry on disk
                db 20   dup(?)
xtnt_1st_blk    dw      ?, ?    ; 20
xtnt_1st_nblks  dw      ?, ?    ; 24
xtnt_blk        dw      ?, ?    ; 28
                db 16   dup(?)
xtnt_cnt        dw      ?, ?    ; 48
xtnt            ends

                org     01f0h
load_seg        dw      ?       ; "BABE"
;bootsrc                db      'F'     ; Boot source 'F'/'H'=floppy/hard
bootsrc         db      0       ; Boot source 'F'/'H'=floppy/hard
sctr_base       db      ?       ; Sector base
boffset         dw      ?, ?    ; Block offset of QNX partition (4 byte long)
nheads          dw      ?       ; Number of heads
nsctr_trk       db      ?, ?    ; Sectors per track
ncyl            dw      ?       ; Number of cylinders
nsctr_cyl       dw      ?       ; 055h, AAh  (For hard disk bootable partition)

;
; Copy boot program up to top of available memory
; then transfer control to boot
;
                org     0

start:;         jmp short begin ; BIOS scan wants jmp instruction
begin:          call restart    ; The call will push our return addr
restart:        int 12h         ; memory size in kilobytes
                sub ax,4        ; reserve top 4k
                shl ax,6
                push cs         ; paranoia
                pop ds
                pop si          ; Recover return address
                sub si,3        ; Size of jmp/call instructions
                mov es,ax
                xor di,di       ; Where we want to go
                mov cx,BLK_SIZE/2
                cli
                rep movsw
                sti
                push es
                push offset boot
                retf

boot:           push cs
                pop ds
                push cs
                pop ss
                mov sp,stk
;
; Tell 'em .altboot is available and
; give 'em some time to choose it
;
                mov si,offset msg
prt:            lodsb
                or al,al
                je root
                call putc
                jmp prt
;
; calculate sectors/cylinder (for converting block numbers)
; and then read the first block of the root directory
;
root:           mov ax,nheads
                mul nsctr_trk
                mov nsctr_cyl,ax

                mov ax,2                ; We want to read the second block
                cwd
                mov di,1
                mov bx,block2
                call read_blk           ; Read block 2 (base 1) of partition

                mov ds,cx
                mov cx,3*9              ; 27 ticks (1.5 seconds)
kbpoll:         mov ax,0100h            ; check if key pressed
                int 16h                 ; (returns scan code/character)
                jnz chkalt
                mov dx,word ptr ds:BIOS_TIMER
roll:           cmp dx,word ptr ds:BIOS_TIMER
                je roll                 ; spin till timer increments
                loop kbpoll

chkalt:         push cs
                pop ds
                mov bp,block2+2*64      ; Assume .boot (3rd dir entry)
                cmp al,1bh
                jne load
                or bootsrc,' '          ; .altboot or altsysinit requested
                cmp xtnt_cnt[bp+64],0   ; Check for empty .altboot (4th entry)
                je load
                add bp,64

load:           mov cx,xtnt_cnt[bp]
                cmp cx,1
                je l0                   ; Skip if only one extent
;
; read extent table for finding subsequent extents
;
                push bx
                mov ax,xtnt_blk[bp]
                mov dx,xtnt_blk[bp+2]
                mov di,1
                mov bx,xtnttab
                call read_blk
                pop bx

l0:             lea si,xtnt_1st_blk[bp]
                mov di,xtnttab+8
                mov ax,[si]
                movsw                   ; Move xtnt_1st_blk
                mov dx,[si]
                movsw
                movsw                   ; Move xtnt_1st_cnt
                movsw

                mov di,1
                call read_blk           ; Read block 2 (base 1) of QNX parition
                mov bp,60h              ; Default is segment 60
                cmp [bx+4],'DD'         ; Signature present?
                jne lp0
                mov bp,[bx+10]          ; Get load segment (signature present)
lp0:            mov load_seg,bp

                mov si,xtnttab+8
lp1:            mov ax,[si]
                mov dx,[si+2]
lp2:            mov di,[si+4]           ; [si+6] better be zero
                mov es,bp
                xor bx,bx
                call read_blk           ; Read block
                add ax,di               ; Increment block number
                adc dx,0
                sub [si+4],di           ; Decrement number of blocks
                shl di,5                ; log2(512/16) [paragraphs/block]
                add bp,di
                cmp word ptr [si+4],0
                jnz lp2                 ; Continue while more blocks to read
                add si,8
                loop lp1

;
; Start up process manager
;
system_up:      mov al,0ffh             ; Disallow further hardware interrupts
                out 21h,al
                cli

                mov bl,bootsrc
;               mov ax,load_seg
;               or ax,ax                ; This will be zero for an old image
;               je oldimage
;               mov es,ax
                mov es,load_seg
                mov es:0ch,bl           ; New images patch the cs:000c of proc
;               jmp short startos
;oldimage:      mov ax,60h              ; Old images patch the bootsrc at 60:ff
;               mov es,ax
;               mov es:0ffh,bl
;               add ax,20h              ; Old images start execution at 80:0
;startos:       push ax
                push es
                xor ax,ax
                push ax
                retf                    ; Start the OS

;dummy:         iret

;
; Read di blocks at dx:ax into es:bx
;
read_blk:       push ax
                push bx
                push cx
                push dx
                sub ax,1
                sbb dx,0
                add ax,boffset
                adc dx,boffset+2
                div nsctr_cyl           ; ax = track, dx = remainder
                cmp ax,1024
                jl cylok
;
; Award BIOS allows two more bits of cylinder information
; in bits 6 and 7 of [dh]
;
                mov al,'C'              ; Inform user of cylinder > 1023
                call putc
cloop:          jmp cloop
cylok:          mov ch,al
                mov cl,ah
                ror cl,2                ; Top 2 bits of track in cl
                mov ax,dx
                cwd
                div nsctr_trk
                mov dh,al
                or cl,ah
                add cl,sctr_base
;
; if [di] is larger than remaining sectors/track then just read remainder
; also should return number of sectors read [al] in [di]
;
                test bootsrc,8          ; ctrl('h')
                je floppy
hard:           mov dl,80h              ; Read from hard disk
                mov al,dl               ; read up to 64k
                jmp short minimize
floppy:         xor dl,dl               ; Read from floppy disk
                mov al,[nsctr_trk]      ; read up to end of track
                sub al,ah
minimize:       xor ah,ah               ; di = min(di, al)
                cmp ax,di
                jg tryagain
                mov di,ax
tryagain:       mov al,13
                call putc
                mov al,10
                call putc
                mov ax,di
                call putx
                mov ax,bx
                call putx
                mov ax,cx
                call putx
                mov ax,dx
                call putx
                mov ax,es
                call putx
                mov ax,di
                mov ah,2                ; Read some blocks
                int 13h
                jnc no_err
;               mov al,'?'              ; Inform user of bad block
                mov al,ah
                add al,'0'
                call putc
                xor ah,ah               ; Reset disk
                int 13h
shr di,1
                jmp tryagain
no_err:         or ax,ax                ; zero sectors read with no error?
                jz blip
                mov di,ax
;blip:          mov al,'.'              ; positive feedback
blip:           add al,'@'
                call putc
                pop dx
                pop cx
                pop bx
                pop ax
                ret

putx:           rol ax,4
                call putn
                rol ax,4
                call putn
                rol ax,4
                call putn
                rol ax,4
                call putn
                mov al,' '
                call putc
                ret

putn:           push ax
                and al,15
                add al,'0'
                cmp al,'9'
                jbe put
                add al,7        ; '@'-'9'
put:            call putc
                pop ax
                ret

putc:           push bx
                mov bx,7
                mov ah,14
                int 10h
                pop bx
                ret

msg             db              "Press Esc to boot alternate OS", 0

code            ends
                end     start

