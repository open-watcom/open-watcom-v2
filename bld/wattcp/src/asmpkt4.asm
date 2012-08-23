ifdef DOSX and DOS4GW

PAGE 60,132

; PKT_DEBUG = 1       ; uncomment to enable debug

.386
ASMPKT4_TEXT segment dword public use16 'CODE'
assume cs:ASMPKT4_TEXT, ds:nothing, es:nothing

;
; Packet-driver receiver upcall handler.  This module is only
; used for Watcom 32-bit targets using DOS4GW/CauseWay etc., but
; not WDOSX which luckily supports callbacks.
;
; Because most Watcom DOS-extenders doesn't support "Real Mode Callback"
; (rmcb), this module must handle the buffer+enqueue operations here.
; Other targets call pkt_enqueue() in pcpkt.c.
;
; This module cannot contain relocatable variables because this
; module will be copied to allocated DOS-memory at run-time.
; This code is tiny-model with run-time offsets starting at 0.
;
; The `asmpkt_inf' variable is initialised from pcpkt.c before this
; module is copied to DOS-memory.
;
; The `*_save' variables are for saving stuff between 1st (AL = 0)
; and 2nd (AL != 0) packet-driver upcall.
;

PUBLIC pkt_receiver4_start, pkt_receiver4_start_
PUBLIC pkt_receiver4_rm,    pkt_receiver4_rm_
PUBLIC pkt_receiver4_end,   pkt_receiver4_end_
PUBLIC asmpkt_inf,         _asmpkt_inf
PUBLIC asmpkt_size_chk,    _asmpkt_size_chk

;
; These structures must match 'struct pkt_ringbuf' in pcqueue.h
; and 'struct pkt_info' in pcpkt.h
;
PKT_RINGBUF struc
         in_index   dd  ?      ; queue index head
         out_index  dd  ?      ; queue index tail
         buf_size   dd  ?      ; size of each buffer
         num_buf    dd  ?      ; number of buffers
         num_drop   dd  ?      ; number of dropped pkts
         buf_start  dd  ?      ; start of buffer pool (not used)
         dos_ofs    dw  ?      ; DOS offset of buffer pool
PKT_RINGBUF ends

PKT_INFO struc
         ip_handle         dw  ?
         arp_handle        dw  ?
         rarp_handle       dw  ?
         pppoe_disc_handle dw  ?
         pppoe_sess_handle dw  ?
         is_serial         dw  ?
         max_pkt_size      dw  ?
         ip_queue          PKT_RINGBUF <>
         arp_queue         PKT_RINGBUF <>
         ip_buf            db (20*1524) dup (?)   ; IP_BUFS * IP_SIZE
         arp_buf           db  (4*72)   dup (?)   ; ARP_BUFS * ARP_SIZE
PKT_INFO ends

ETH_MAX = 1514           ; max Ethernet size


pkt_receiver4_start:
pkt_receiver4_start_:
 _asmpkt_inf  label dword
  asmpkt_inf  dd 0       ; CS:0
  head_save   dd 0       ; CS:4
  queue_save  dd 0       ; CS:8
  index_save  dw 0       ; CS:12

ifdef PKT_DEBUG
  xy_pos dw 160*13
  temp   dw (14) dup (0)
endif

;
; Location of variables at run-time.
;
PKT_INF = cs:[0]         ; asmpkt_inf
HEAD    = cs:[4]         ; head_save
QUEUE   = cs:[8]         ; queue_save
INDEX   = cs:[12]        ; index_save

ifdef PKT_DEBUG
  XY_POS  = cs:[14]
  VAR_1   = cs:[16]
  VAR_2   = cs:[18]
  VAR_3   = cs:[20]
  VAR_4   = cs:[22]
  VAR_5   = cs:[24]
  VAR_6   = cs:[26]
  VAR_7   = cs:[28]
  VAR_8   = cs:[30]
  VAR_9   = cs:[32]
  VAR_10  = cs:[34]
  VAR_11  = cs:[36]
  VAR_12  = cs:[38]
  VAR_13  = cs:[40]
  VAR_14  = cs:[42]
endif

;-----------------------------------------------------
;
PUTCH  MACRO char
ifdef PKT_DEBUG
       push es
       push di
       push ax
       mov  ax, 0B800h
       mov  es, ax
       mov  di, XY_POS
       mov  al, char
       mov  ah, 15+16           ; white on blue
       stosw
       add  word ptr XY_POS, 2  ; increment X-pos
       pop  ax
       pop  di
       pop  es
endif
ENDM

;-----------------------------------------------------
;
STORE_VARS MACRO
ifdef PKT_DEBUG 
       push ds
       push si
       push ax
       lds  si, PKT_INF
       mov  ax, [si].ip_handle
       mov  VAR_1, ax
       mov  ax, [si].arp_handle
       mov  VAR_2, ax
       mov  ax, [si].rarp_handle
       mov  VAR_3, ax
       mov  ax, [si].is_serial
       mov  VAR_4, ax

       add  si, offset ip_queue
       mov  ax, word ptr [si].in_index
       mov  VAR_5, ax
       mov  ax, word ptr [si].out_index
       mov  VAR_6, ax
       mov  ax, word ptr [si].buf_size
       mov  VAR_7, ax
       mov  ax, word ptr [si].num_buf
       mov  VAR_8, ax
       mov  ax, word ptr [si].dos_ofs
       mov  VAR_9, ax

       add  si, offset arp_queue - offset ip_queue
       mov  ax, word ptr [si].in_index
       mov  VAR_10, ax
       mov  ax, word ptr [si].out_index
       mov  VAR_11, ax
       mov  ax, word ptr [si].buf_size
       mov  VAR_12, ax
       mov  ax, word ptr [si].num_buf
       mov  VAR_13, ax
       mov  ax, word ptr [si].dos_ofs
       mov  VAR_14, ax
       pop  ax
       pop  si
       pop  ds
endif
ENDM

;
; The packet-driver calls this routine to ask for a receive buffer (AL=0)
; And to tell when the buffer has been copied (AL != 0).
;
; Entry AL = 0; driver requests a buffer. We return ES:DI of next
;               available ring-buffer (or ES:DI = 0:0 if no room)
;       CX = length of packet
;       BX = handle (IP or ARP/RARP)
; Entry AL != 0; driver has put the data in buffer, we then update
;                the ring-buffer head index (INDEX).
;       BX = handle (IP or ARP/RARP)
;

pkt_receiver4_rm:
pkt_receiver4_rm_:
        pushf
        cli                    ; no interruptions now
        or   al, al
        jnz  @enque            ; AL != 0, branch and enqueue packet

        PUTCH ' '
        PUTCH '0'

        ;
        ; AL = 0: buffer request
        ;
        call pkt_get_buffer    ; get a buffer, return ES:DI (may be 0:0)
        popf
        retf

        ;
        ; AL != 0: enqueue packet
        ;
@enque:
        PUTCH '1'

        or   si, si            ; NULL-ptr is not valid
        jz   @no_enque
        call pkt_enqueue       ; copy packet at DS:SI to queue-head

        STORE_VARS

@no_enque:
        popf
        retf


;
; Check handle, check correct ring-buffer (ip_queue or arp_queue) in
; '_pkt_inf'. If room in buffer, return next head buffer, else NULL.
;
; static struct _far pkt_ringbuf *QUEUE;
; static char   _far             *HEAD;
; static int                      INDEX;
;
; char * pkt_get_buffer (int handle, int rx_len)
; {
;   if (!PKT_INF)     /* should never happen */
;      return (NULL);
;
;   if (handle == PKT_INF->arp_handle && !PKT_INF->is_serial)
;        QUEUE = &PKT_INF->arp_queue;
;   else QUEUE = &PKT_INF->ip_queue;
;
;   if (rx_len > QUEUE->buf_size - 4)
;   {
;     QUEUE->num_drop++;
;     return (NULL);
;   }
;
;   INDEX = QUEUE->in_index + 1;
;   if (INDEX >= QUEUE->num_buf)
;       INDEX = 0;
;
;   if (INDEX == QUEUE->out_index)
;   {
;     QUEUE->num_drop++;
;     return (NULL);
;   }
;   HEAD = MK_FP (_ES, QUEUE->dos_ofs + (QUEUE->buf_size * QUEUE->in_index);
;   return (HEAD);
; }
;
; BX = IP or ARP/RARP handle
; CX = buffer size
;
; return ES:DI, AX,BX,DX changed

pkt_get_buffer:
        les  di, PKT_INF
        mov  ax, es
        cmp  ax, 0                        ; seg(_pkt_inf) == 0 ?
        je   @ret_null                    ; yes, discard it

        push ds                           ; save DS
        mov  ds, ax                       ; DS = ES
        mov  ax, offset ip_queue          ; assume it's IP
        cmp  bx, [di].arp_handle
        jne  @is_ip

        test [di].is_serial, 0FFFFh       ; using serial driver
        jnz  @is_ip                       ; yes, cannot be ARP/RARP/PPPoE
        mov  ax, offset arp_queue

        PUTCH 'a'

@is_ip:
        add  di, ax                       ; ES:DI = ip_queue or arp_queue
        mov  ax, word ptr [di].buf_size
        sub  ax, 4
        cmp  cx, ax                       ; if (rx_len > QUEUE->buf_size - 4)
        ja   @too_big                     ;   drop it

        mov  QUEUE[0], di
        mov  QUEUE[2], es

        mov  ax, word ptr [di].in_index
        inc  ax
        cmp  ax, word ptr [di].num_buf    ; (index >= queue->num_buf)
        jb   @no_wrap                     ; no, don't wrap
        xor  ax, ax                       ; yes, index wrapped
        PUTCH 'w'

@no_wrap:
        mov  INDEX, ax                    ; store INDEX
        cmp  ax, word ptr [di].out_index
        je   @drop                        ; if (INDEX == QUEUE->out_index)
                                          ;   drop packet (queue full)
        mov  ax, word ptr [di].buf_size
        mul  word ptr [di].in_index       ; AX = (QUEUE->buf_size * QUEUE->in_index)
        add  ax, word ptr [di].dos_ofs    ; AX += QUEUE->dos_ofs
        mov  di, ax                       ;
        mov  ax, es

        mov  HEAD[0], di
        mov  HEAD[2], ax                  ; save HEAD = ES:DI
        pop  ds
        PUTCH '!'
        ret                               ; return ES:DI

@too_big:
        PUTCH 't'
@drop:
        PUTCH 'd'
        inc  word ptr [di].num_drop
        adc  word ptr [di+2].num_drop, 0
        pop  ds

@ret_null:
        PUTCH 'D'
        xor  di, di
        mov  es, di                       ; return (NULL)
        ret

;
; Enqueue a packet to the head of queue.
; No need to copy data, only zero-pad buffer.
;
; void pkt_enqueue (int rx_len)
; {
;   int pad_len = QUEUE->buf_size - 4 - rx_len; /* 4 = size of PKTQ_MARKER */
;
;   memset (HEAD + rx_len, 0, pad_len); /* zero-fill rest of buffer */
;   QUEUE->in_index = INDEX;
; }
;
; DS:SI = receiver buffer to enqueue (=HEAD)
; CX = rx_len
;
pkt_enqueue:
        les  di, QUEUE
        mov  bx, word ptr es:[di].buf_size
        sub  bx, 4
        sub  bx, cx

        cld
        add  si, cx
        xchg di, si            ; DI = SI+rx_len, ES = DS already
        xchg cx, bx
        shr  cx, 2             ; CX = pad_len/4
        rep  stosd             ; fill rest of buffer

        PUTCH '*'

        mov  si, word ptr QUEUE[0]
        mov  ax, INDEX
        mov  word ptr [si].in_index, ax
        ret

        nop
        nop          ; incase ptr-arithmetic is wrong !

;
; Label to calculate the size to copy to DOS-memory
;

pkt_receiver4_end:   ; TX-buffer and scratch-area (PKT_TMP) starts here
pkt_receiver4_end_:

;
; PKT_TMP db (30)      dup (?)  ; these gets overlayed buffer below
; TX_BUF db  (IP_SIZE) dup (?)
;
; Use 'asmpkt_size_chk' to compared with 'sizeof(*_pkt_inf)' in pcpkt.c
; WASM's SIZE operator doesn't seem to handle structure defs (only data).
;
ifdef ??version    ; Turbo Assembler
 _asmpkt_size_chk label word
  asmpkt_size_chk dw SIZE PKT_INFO
else
  dummy_pkt_start = $
  dummy_pkt_info  PKT_INFO <>            ; won't be used or copied to DOS-mem
 _asmpkt_size_chk label word             ; doesn't use space in .exe-image ?
  asmpkt_size_chk dw $ - dummy_pkt_start
endif

ASMPKT4_TEXT ends

endif   ; ifdef DOSX and DOS4GW

END

