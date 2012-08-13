                .286p
code            segment 'CODE'
assume          cs:code, ds:code

;
; QNX Boot loader. This is on block one of the QNX partition. The PC/AT
; BIOS reads this block into 7000h and transfers control to it.
;
; N.B. Actually, it's loaded at 0:7C00 [bac]
;
BOOT_SEG =      7000h   ; The segment where the following offsets apply
BOOT_OFF =      0FE00h  ; Where we move the loader

BIOS_TIMER =    46ch    ; Where the BIOS keeps the tick count (18/sec).

FBLK_SIZE =     512     ; Number bytes in a disk block

stk             =       0fa00h  ; stack (grows down)
block2          =       0fa00h  ; Where we read QNX partition block 2
xtnt_1st_blk    =       20      ; structure offsets of directory entry on disk.
xtnt_1st_nblks  =       24
xtnt_blk        =       28
xtnt_cnt        =       48

xtnttab         =       0fc00h  ; Where we read the extent table

                org     0fff0h
load_seg        dw      ?       ; 0BAh, 0BEh --> "BABE"
bootsrc         db      ?       ; Boot source 'F'/'H'=floppy/hard
sctr_base       db      ?       ; Sector base
boffset         dw      ?, ?    ; Block offset of QNX partition (4 byte long)
nheads          dw      ?       ; Number of heads
nsctr_trk       db      ?       ; Sectors per track
ncyl            dw      ?       ; Number of cylinders
nsctr_cyl       dw      ?
;hdsignature    dw      ?       ; 055h, AAEh  (For hard disk bootable partition)

;
; Copy boot program up to top 512 bytes of a 64K segment
; then transfer control to boot
;
        org     100h

start:  call restart    ; The call will push our return addr
restart:cli
ip:     pop si          ; Recover return address
        sub si,3        ; Size of the call instruction
        push cs
        pop ds
        mov ax,BOOT_SEG
        mov es,ax
        mov di,BOOT_OFF ; Where we want to go
        mov cx,FBLK_SIZE/2
        rep movsw
        db 0eah         ; jmp seg:off
        dw offset boot-start+BOOT_OFF
        dw BOOT_SEG
;       push es
;       mov ax,offset boot-start+BOOT_OFF
;       push ax
;       retf

boot:
;
; Trap timer interrupts while booting
;
;       xor ax,ax
;       mov ds,ax
;       mov word ptr ds:70h,offset dummy-start+BOOT_OFF
;       mov word ptr ds:72h,cs
        mov ax,es
        mov ds,ax
        mov ss,ax
        mov sp,stk

        sti

        mov ax,nheads
        mul nsctr_trk
        mov nsctr_cyl,ax

        mov ax,2                ; We want to read the second block
        cwd
        mov di,1
        mov bx,block2
        call read_blk           ; Read block 2 (starts at 1) of QNX parition

;
; The .boot    entry is the 3rd group of 64.
; The .altboot entry is the 4th group of 64.
;
        mov si,2*64             ; Offset of dir entry for .altboot (4th entry)
        cmp word ptr xtnt_cnt[bx+si+64],0
        jz xtnt                 ; If no data use .boot instead

;
; Tell 'em .altboot is available and
; give 'em some time to choose it
;
        push si
        mov si,offset msg-start+BOOT_OFF
prt:    lodsb
        call putc
        cmp al,'.'              ; cmp al == '.'
        jne prt
        pop si

        mov cx,2*18             ; 2 seconds.
waitkey:
;       push cx
        mov ah,1
        int 16h
;       pop cx
        jnz iskey
        push es
        xor dx,dx
        mov es,dx
        mov dx,word ptr es:BIOS_TIMER
bloop:  cmp dx,word ptr es:BIOS_TIMER
        je bloop
        pop es
        loop waitkey
        jmp short xtnt
iskey:  mov ah,0
        int 16h
        cmp al,1bh
        jne xtnt

alt:    or bootsrc,' '          ; .altboot requested
        add si,64

xtnt:   mov cx,xtnt_cnt[bx+si]
        cmp cx,1
        je l0                   ; Skip if no extent block
        push bx
        mov ax,xtnt_blk[bx+si]
        mov dx,xtnt_blk[bx+si+2]
        mov di,1
        mov bx,xtnttab
        call read_blk
        pop bx

l0:     lea si,xtnt_1st_blk[bx+si]
        mov di,xtnttab+8
        mov ax,[si]
        movsw           ; Move xtnt_1st_blk
        mov dx,[si]
        movsw
        movsw           ; Move xtnt_1st_cnt
        movsw

        mov di,1
        call read_blk           ; Read block 2 (starts at 1) of QNX parition
        mov si,60h              ; Default is segment 60.
        cmp word ptr es:[bx+4],'DD'     ; Signature present?
        jne lp0
        mov si,es:[bx+10]       ; Get load segment (signature is present).
lp0:    mov load_seg,si
        mov es,si
        mov si,xtnttab+8

lp1:    mov ax,[si]
        mov dx,[si+2]
lp2:    mov di,[si+4]           ; [si+6] better be zero
        cmp di,128
        jbe lp3
        mov di,128
lp3:    xor bx,bx
        call read_blk           ; Read block
        add ax,di               ; Increment block number
        adc dx,0
        sub [si+4],di           ; Decrement number of blocks
        shl di,5                ; log2(512/16)
        mov bx,es
        add bx,di
        mov es,bx               ; Bump load segment
        cmp word ptr [si+4],0
        jnz lp2                 ; Continue while more blocks to read
        add si,8
        loop lp1

;
; Start up process manager
;
system_up:
        mov al,0ffh             ; Disallow any further hardware interrupts
        out 21h,al
        cli

        mov bl,bootsrc
        mov ax,load_seg
;       or ax,ax                ; This will be zero for an old image
;       je oldimage
        mov es,ax
        mov byte ptr es:0ch,bl  ; New images patch the cs:000c of proc
;       jmp short startos
;oldimage:
;       mov ax,60h              ; Old images patch the bootsrc at 60:00ff
;       mov es,ax
;       mov byte ptr es:0ffh,bl
;       add ax,20h              ; Old images start execution at 80:0000
startos:
        push ax
        xor ax,ax
        push ax
        retf                    ; Start the OS

;dummy: iret

;
; Read di blocks at dx,ax into es:bx
;
read_blk:
        push ax
        push bx
        push cx
        push dx
;       call spin
        sub ax,1
        sbb dx,0
        add ax,boffset
        adc dx,boffset+2
        div nsctr_cyl           ; ax = track, dx = remainder
        cmp ax,1024
        ja badcyl
        mov ch,al
        mov cl,ah
        ror cl,2                ; Top 2 bits of track in cl
        mov ax,dx
        cwd
        idiv nsctr_trk
        mov dh,al
        or cl,ah
        add cl,sctr_base
        xor dl,dl
        mov al,bootsrc
        or al,20h               ; Force lower case
        cmp al,'h'
        jne tryagain            ; Read from floppy disk 1
        mov dl,80h              ; Read from hard disk 1
tryagain:
        mov ax,di
        mov ah,2                ; Read some blocks
        int 13h
        jnc no_err
        xor ah,ah               ; Reset disk
        int 13h
        mov al,'?'              ; Inform user of bad block by printing '?'
        call putc
        jmp tryagain
badcyl: mov al,'C'              ; Inform user of cylinder>1023 by prnting 'C'
        call putc
cloop:  jmp cloop
no_err: pop dx
        pop cx
        pop bx
        pop ax
        ret

;
; We arrive with the block in DX,AX so we will use some bits in AX
; to flash the user that something is happening.
;
;spin:  push ax
;       push bx
;       push cx
;       and al,3
;       add al,18h
;       mov ah,10
;       xor bx,bx
;       mov cx,1
;       int 10h
;       pop cx
;       pop bx
;       pop ax
;       ret

putc:   push ax
        push bx
;       push cx
        push si
        mov bx,7
        mov ah,14
        int 10h
        pop si
;       pop cx
        pop bx
        pop ax
        ret

                org     02d0h
msg             db      "Press Esc to boot alternate OS.", 0

code            ends
                end     start
