/*  cpuModel.S
 *
 *  This file contains all assembly code for the Intel Cpu identification.
 *  It is based on linux cpu detection code.
 *
 *  Intel also provides public similar code in the book
 *  called :
 *  
 *  Pentium Processor Family
 *      Developer Family
 *  Volume  3 : Architecture and Programming Manual
 *
 * At the following place :
 *  
 *  Chapter 5 : Feature determination
 *  Chapter 25: CPUID instruction   
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id: cpuModel.S,v 1.1 1998/08/05 15:15:46 joel Exp $
 */

#include <machine/asm.h>
#define ASM
#include "register.h"

#define SYM(x)        _ ## x
#define DATA(x,type) .align 2; .globl SYM(x); SYM(x): type

.text

/* 
 * check Processor type: 386, 486, 6x86(L) or CPUID capable processor 
 */
ENTRY (CheckCpuType);

    /*  Assume 386 for now
     */
    movl $3, SYM (x86_type)

    /* Start using the EFLAGS AC bit determination method. If this
     * bit can be set we have a 486 or above.
     */
    pushfl                          /* save EFLAGS */
    pushfl                          /* Get EFLAGS in EAX */
    popl %eax
    
    movl  %eax, %ecx                /* save original EFLAGS in ECX */
    xorl  $EFLAGS_ALIGN_CHECK, %eax /* flip AC bit in EAX */
    pushl %eax                      /* set EAX as EFLAGS */
    popfl           
    pushfl                          /* Get new EFLAGS in EAX */
    popl  %eax
    
    xorl  %ecx, %eax                /* check if AC bit changed */
    andl  $EFLAGS_ALIGN_CHECK,%eax
    je is386                        /* If not : we have a 386 */

    /* Assume 486 for now
     */
    movl  $4, SYM (x86_type)
    movl  %ecx, %eax                /* Restore orig EFLAGS in EAX */
    xorl  $EFLAGS_ID, %eax          /* flip  ID flag */
    pushl %eax                      /* set EAX as EFLAGS */
    popfl               
    pushfl                          /* Get new EFLAGS in EAX */
    popl  %eax
            
    xorl  %ecx, %eax                /* check if ID bit changed */
    andl  $EFLAGS_ID, %eax

    /* 
     * if we are on a straight 486DX, SX, or 487SX we can't
     * change it. OTOH 6x86MXs and MIIs check OK.
     * Also if we are on a Cyrix 6x86(L)
     */
    je is486x

isnew:  
    /* restore original EFLAGS
     */
    popfl
    incl SYM (have_cpuid)     /* we have CPUID instruction */

    /* use CPUID to get :
     *  processor type,
     *  processor model,
     *  processor mask,
     * by using it with EAX = 1
     */
    movl $1, %eax
    cpuid           

    movb %al, %cl             /* save reg for future use */
    
    andb $0x0F, %ah           /* mask processor family */
    movb %ah, SYM (x86_type)  /* put result in x86_type */
    
    andb $0xF0, %al           /* get model */
    shrb $4, %al
    movb %al, SYM (x86_model) /* store it in x86_model */
    
    andb $0x0F, %cl           /* get mask revision */
    movb %cl, SYM (x86_mask)  /* store it in x86_mask */
    
    movl %edx, SYM (x86_capability) /* store feature flags */
    
    /* get vendor info by using CPUID with EAX = 0
     */
    xorl %eax, %eax
    cpuid

    /* store results contained in ebx, edx, ecx in x86_vendor_id string.
     */
    movl %ebx, SYM (x86_vendor_id)
    movl %edx, SYM (x86_vendor_id)+4
    movl %ecx, SYM (x86_vendor_id)+8
    call check_x87
    ret

/*
 * Now we test if we have a Cyrix 6x86(L). We didn't test before to avoid
 * clobbering the new BX chipset used with the Pentium II, which has a
 * register at the same addresses as those used to access the Cyrix special
 * configuration registers (CCRs).
 */
    /*
     * A Cyrix/IBM 6x86(L) preserves flags after dividing 5 by 2
     * (and it _must_ be 5 divided by 2) while other CPUs change
     * them in undefined ways. We need to know this since we may
     * need to enable the CPUID instruction at least.
     * We couldn't use this test before since the PPro and PII behave
     * like Cyrix chips in this respect.
     */
is486x:
    xor  %ax, %ax
    sahf
    movw $5, %ax
    movw $2, %bx
    div  %bl
    lahf
    cmpb $2, %ah
    jne  ncyrix

    /*
     * N.B. The pattern of accesses to 0x22 and 0x23 is *essential*
     *      so do not try to "optimize" it! For the same reason we
     *      do all this with interrupts off.
     */
#define setCx86(reg, val) \
        movw reg, %ax;    \
        outw %ax, $0x22;  \
        movw val, %ax;    \
        outw %ax, $0x23

#define getCx86(reg)      \
        movw reg, %ax;    \
        outw %ax, $0x22;  \
        inw  $0x23, %ax

    cli
    getCx86 ($0xC3)         /*  get CCR3 */
    movw %ax, %cx           /* Save old value */
    movw %ax, %bx
    andw $0x0F, %bx         /* Enable access to all config registers */
    orw $0x10, %bx          /* by setting bit 4 */
    setCx86 ($0xC3, %bx)

    getCx86 ($0xE8)         /* now we can get CCR4 */
    orw $0x80, %ax          /* and set bit 7 (CPUIDEN) */
    movw %ax, %bx           /* to enable CPUID execution */
    setCx86 ($0xE8, %bx)

    getCx86 ($0xFE)         /* DIR0 : let's check this is a 6x86(L) */
    andw $0xF0, %ax         /* should be 3xh */
    cmpw $0x30, %ax
    jne n6x86
    getCx86 ($0xE9)         /* CCR5 : we reset the SLOP bit */
    andw $0xFD, %ax         /* so that udelay calculation */
    movw %ax, %bx           /* is correct on 6x86(L) CPUs */
    setCx86 ($0xE9, %bx)

    setCx86 ($0xC3, %cx)    /* Restore old CCR3 */
    sti
    jmp isnew               /* We enabled CPUID now */

n6x86:
    setCx86 ($0xC3, %cx)    /* Restore old CCR3 */
    sti

ncyrix:
    popfl                   /* restore original EFLAGS */
    call check_x87
    ret

is386:
    popfl                   /* restore original EFLAGS */
    call check_x87
    ret
    

/*
 * This checks for 287/387.
 */
check_x87:
    movb  $0, SYM (hard_math)  /* assume no coprocessor */
    fninit
    fstsw %ax
    cmpb  $0, %al
    je    1f
    ret

1:  movb $1, SYM (hard_math)
    ret

.data

DATA (x86_type,       .byte 0)
DATA (have_cpuid,     .long 0)
DATA (x86_model,      .byte 0)
DATA (x86_mask,       .byte 0)
DATA (x86_capability, .long 0)
DATA (x86_vendor_id,  .zero 13)
DATA (hard_math,      .byte 0)
