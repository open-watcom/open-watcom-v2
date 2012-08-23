/*
 * Copyright (c) 1985-1998 Microsoft Corporation
 * This file is part of the Microsoft Research IPv6 Network Protocol Stack.
 * You should have received a copy of the Microsoft End-User License Agreement
 * for this software along with this release; see the file "license.txt".
 * If not, please see http://www.research.microsoft.com/msripv6/license.htm,
 * or write to Microsoft Research, One Microsoft Way, Redmond, WA 98052-6399.
 *
 * Abstract:
 *
 * This module implements a function to compute the internet checksum
 * of a buffer.
 *
 * Rewritten for gcc's AT&T asm by G. Vanem 2000 for
 * the Watt-32 tcp/ip stack.
 *
 * On an AMD K6-II, inchksum_fast() is around 4.5 faster than inchksum() in
 * chksum.c. On an Intel Pentium the gain is around 10 times !!
 */

#include <machine/asm.h>

.text

#define LOOP_UNROLLING_BITS  5
#define LOOP_UNROLLING       (1 << LOOP_UNROLLING_BITS)

#define buf_arg  12     /* stack offset of buffer address */
#define len_arg  16     /* stack offset of length */

to_cksum_last_word:
  jmp cksum_last_word

to_cksum_done:
  jmp cksum_done

to_cksum_dword_loop_done:
  jmp cksum_dword_loop_done

/* 
 * WORD cdecl inchksum_fast (const void *buf, int len);
 */
ENTRY (_w32_inchksum_fast);

  pushl %ebx                       /* save nonvolatile register */
  pushl %esi                       /* save nonvolatile register */

  movl  len_arg(%esp), %ecx        /* get length in bytes */
  sub   %eax, %eax                 /* clear computed checksum */
  test  %ecx, %ecx                 /* any bytes to checksum at all? */
  jz    to_cksum_done              /* no bytes to checksum */

/*
 * if the checksum buffer is not word aligned, then add the first byte of
 * the buffer to the input checksum.
 */
  movl  buf_arg(%esp), %esi        /* get source address */
  subl  %edx, %edx                 /* set up to load word into EDX below */
  testl $1, %esi                   /* check if buffer word aligned */
  jz    cksum_word_aligned         /* if zf, buffer word aligned */
  movb  (%esi), %ah                /* get first byte (we know we'll have
                                    * to swap at the end) */
  incl  %esi                       /* increment buffer address */
  decl  %ecx                       /* decrement number of bytes */
  jz    to_cksum_done              /* if zf set, no more bytes */

/*
 * If the buffer is not an even number of of bytes, then initialize
 * the computed checksum with the last byte of the buffer.
 */

cksum_word_aligned:
  shrl $1, %ecx                    /* convert to word count */
  jnc  cksum_start                 /* if nc, even number of bytes */
  movb (%esi, %ecx, 2), %al        /* initialize the computed checksum */
  jz   to_cksum_done               /* if zf set, no more bytes */

/*
 * Compute checksum in large blocks of dwords, with one partial word up front
 * if necessary to get dword alignment, and another partial word at the end
 * if needed.
 */

/*
 * Compute checksum on the leading word, if that's necessary to get dword
 * alignment.
 */

cksum_start:
  testl $2, %esi                   /* check if source dword aligned */
  jz    cksum_dword_aligned        /* source is already dword aligned */
  movw  (%esi), %dx                /* get first word to checksum */
  addl  $2, %esi                   /* update source address */
  addl  %edx, %eax                 /* update partial checksum
                                    *  (no carry is possible, because EAX
                                    *  and EDX are both 16-bit values) */
  decl  %ecx                       /* count off this word (zero case gets
                                    *  picked up below) */

/*
 * Checksum as many words as possible by processing a dword at a time.
 */

cksum_dword_aligned:
  pushl %ecx                       /* so we can tell if there's a trailing
                                    *  word later */
  shrl  $1, %ecx                   /* # of dwords to checksum */
  jz    to_cksum_last_word         /* no dwords to checksum */

  movl  (%esi), %edx               /* preload the first dword */
  addl  $4, %esi                   /* point to the next dword */
  decl  %ecx                       /* count off the dword we just loaded */
  jz    to_cksum_dword_loop_done
                                   /* skip the loop if that was the only dword */
  movl  %ecx, %ebx                 /* EBX = # of dwords left to checksum */
  addl  $(LOOP_UNROLLING-1), %ecx     /* round up loop count */
  shrl  $(LOOP_UNROLLING_BITS), %ecx  /* convert from word count to unrolled
                                       *  loop count */
  andl  $(LOOP_UNROLLING-1), %ebx     /* # of partial dwords to do in first
                                       *  loop
                                       */

  jz    cksum_dword_loop           /* special-case when no partial loop,
                                    *  because fixup below doesn't work
                                    *  in that case (carry flag is
                                    *  cleared at this point, as required
                                    *  at loop entry)
                                    */

  leal -4*LOOP_UNROLLING(%esi, %ebx, 4), %esi
                                   /* adjust buffer pointer back to
                                    *  compensate for hardwired displacement
                                    *  at loop entry point
                                    * ***doesn't change carry flag***
                                    */

  shll $2, %ebx         /* because "jmp loop_entries(%ebx,4)" doesn't work */

  jmp  *loop_entries (%ebx)         /* enter the loop to do the first,
                                    * partial iteration, after which we can
                                    * just do 64-word blocks
                                    * ***doesn't change carry flag***
                                    */

cksum_dword_loop:

#define loop_entry(x)   \
        loop_entry_##x: \
        adc %edx, %eax; \
        mov x(%esi), %edx

  loop_entry (0)
  loop_entry (4)
  loop_entry (8)
  loop_entry (12)
  loop_entry (16)
  loop_entry (20)
  loop_entry (24)
  loop_entry (28)
  loop_entry (32)
  loop_entry (36)
  loop_entry (40)
  loop_entry (44)
  loop_entry (48)
  loop_entry (52)
  loop_entry (56)
  loop_entry (60)
  loop_entry (64)
  loop_entry (68)
  loop_entry (72)
  loop_entry (76)
  loop_entry (80)
  loop_entry (84)
  loop_entry (88)
  loop_entry (92)
  loop_entry (96)
  loop_entry (100)
  loop_entry (104)
  loop_entry (108)
  loop_entry (112)
  loop_entry (116)
  loop_entry (120)
  loop_entry (124)   /* 4*(LOOP_UNROLLING-1) */


cksum_dword_loop_end:
  leal 4*LOOP_UNROLLING(%esi), %esi  /* update source address */
  decl %ecx                        /* count off unrolled loop iteration */
  jnz  cksum_dword_loop            /* do more blocks */

cksum_dword_loop_done:
  adcl  %edx, %eax                 /* finish dword checksum */
  movl  $0, %edx                   /* prepare to load trailing word */
  adcl  %edx, %eax

/*
 * Compute checksum on the trailing word, if there is one.
 * High word of EDX = 0 at this point
 * Carry flag set iff there's a trailing word to do at this point
 */

cksum_last_word:
  popl  %ecx                       /* get back word count */
  testl $1, %ecx                   /* is there a trailing word? */
  jz    cksum_done                 /* no trailing word */
  addw  (%esi), %ax                /* add in the trailing word */
  adcl  $0, %eax

cksum_done:
  movl  %eax, %ecx                 /* fold the checksum to 16 bits */
  rorl  $16, %ecx
  addl  %ecx, %eax
  movl  buf_arg(%esp), %ebx
  shrl  $16, %eax
  testl $1, %ebx                   /* check if buffer word aligned */
  jz    cksum_combine              /* if zf set, buffer word aligned */
  rorw  $8, %ax                    /* byte aligned--swap bytes back */

cksum_combine:
  popl %esi
  adcl $0, %eax
  popl %ebx
  ret


.data
.align 2

loop_entries:
  .long  0
  .long  loop_entry_124
  .long  loop_entry_120
  .long  loop_entry_116
  .long  loop_entry_112
  .long  loop_entry_108
  .long  loop_entry_104
  .long  loop_entry_100
  .long  loop_entry_96
  .long  loop_entry_92
  .long  loop_entry_88
  .long  loop_entry_84
  .long  loop_entry_80
  .long  loop_entry_76
  .long  loop_entry_72
  .long  loop_entry_68
  .long  loop_entry_64
  .long  loop_entry_60
  .long  loop_entry_56
  .long  loop_entry_52
  .long  loop_entry_48
  .long  loop_entry_44
  .long  loop_entry_40
  .long  loop_entry_36
  .long  loop_entry_32
  .long  loop_entry_28
  .long  loop_entry_24
  .long  loop_entry_20
  .long  loop_entry_16
  .long  loop_entry_12
  .long  loop_entry_8
  .long  loop_entry_4

