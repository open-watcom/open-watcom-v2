#ifndef __PROFILE_H
#define __PROFILE_H

#if defined(__386__)
  extern void CheckCpuType (void);

  extern int x86_capability;

  extern void profile_start (const char *str);
  extern void profile_stop (void);
  extern void profile_init (void);

  #define START_PROFILE(func) profile_start (func)
  #define STOP_PROFILE()      profile_stop()

#else
  #define START_PROFILE(func) ((void)0)
  #define STOP_PROFILE()      ((void)0)
#endif

#if defined(__386__)

/*
 * use RDTSC instruction for precise timing and profiling
 */
extern void get_cpuid(unsigned val, unsigned *eax, unsigned *ebx, unsigned *ecx, unsigned *edx);
#pragma aux get_cpuid =     \
        ".586"              \
        "push eax"          \
        "push ebx"          \
        "push ecx"          \
        "push edx"          \
        "mov eax, esi"      \
        "cpuid"             \
        "mov esi, [esp]"    \
        "mov [esi], edx"    \
        "mov esi, [esp+4]"  \
        "mov [esi], ecx"    \
        "mov esi, [esp+8]"  \
        "mov [esi], ebx"    \
        "mov esi, [esp+12]" \
        "mov [esi], eax"    \
        "add esp, 16"       \
    __parm [__esi] [__eax] [__ebx] [__ecx] [__edx] \
    __modify [__esi __eax __ebx __ecx __edx];

extern uint64 get_rdtsc (void);
#pragma aux get_rdtsc = \
        ".586"          \
        "db 0Fh, 31h"   \
        "cld"           \
        "nop"           \
        "nop"           \
        "nop"           \
        "nop"           \
    __value [__edx __eax]

//EFLAGS_ALIGN_CHECK = 040000h
//EFLAGS_ID          = 200000h

extern int has_cpuid( void );
#pragma aux has_cpuid = \
        ".586" \
        "pushfd"                /* save EFLAGS */ \
        "pushfd"                /* Get EFLAGS in EAX */ \
        "pop  eax" \
        "mov  ebx, eax"         /* save original EFLAGS in EBX */ \
        "xor  eax, 040000h"     /* flip AC bit in EAX */ \
        "push eax"              /* set EAX as EFLAGS */ \
        "popfd"                 /* .. */ \
        "pushfd"                /* Get new EFLAGS in EAX */ \
        "pop  eax"              /* .. */ \
        "xor  ecx, ecx"         /* set no CPUID flag */ \
        "xor  eax, ebx"         /* check if AC bit changed */ \
        "and  eax, 040000h"     /* .. */ \
        "je short ret_ecx"      /* If not : we have a 386 */ \
        "mov  eax, ebx"         /* Restore orig EFLAGS in EAX */ \
        "xor  eax, 200000h"     /* flip ID flag */ \
        "push eax"              /* set EAX as EFLAGS */ \
        "popfd"                 /* .. */ \
        "pushfd"                /* Get new EFLAGS in EAX */ \
        "pop  eax"              /* .. */ \
        "xor  eax, ebx"         /* check if ID bit changed */ \
        "and  eax, 200000h"     /* .. */ \
        "je short is486x"       /* If not : we have 486? */ \
        "inc ecx "              /* set has CPUID flag */ \
"ret_ecx:" \
        "mov eax,ecx" \
        "popfd"                 /* restore original EFLAGS */ \
        "ret" \
"is486x: xor ax, ax" \
        "sahf" \
        "mov ax, 5" \
        "mov bx, 2" \
        "div bl" \
        "lahf" \
        "cmp ah, 2" \
        "jne short ret_ecx"     /* old CPU no CPUID */ \
        "cli"                   /* disable interrupt */ \
        "mov ax, 0C3h"          /* get CCR3 */ \
        "out 22h, ax"           /* .. */ \
        "in  ax, 23h"           /* .. */ \
        "push eax"              /* save original CCR3 on stack */ \
        "mov ebx, eax"          /* Enable access to all config registers */ \
        "and ebx, 0Fh"          /* by setting bit 4 */ \
        "or  ebx, 10h"          /* .. */ \
        "mov eax, 0C3h"         /* set CCR3 */ \
        "out 22h, ax"           /* .. */ \
        "mov eax, ebx"          /* .. */ \
        "out 23h, ax"           /* .. */ \
        "mov eax, 0E8h"         /* get CCR4 */ \
        "out 22h, ax"           /* .. */ \
        "in  ax, 23h"           /* .. */ \
        "or  eax, 80h"          /* and set bit 7 (CPUIDEN) */ \
        "mov ebx, eax"          /* to enable CPUID execution */ \
        "mov eax, 0E8h"         /* set CCR4 */ \
        "out 22h, ax"           /* .. */ \
        "mov eax, ebx"          /* .. */ \
        "out 23h, ax"           /* .. */ \
        "mov eax, 0FEh"         /* DIR0 : let's check this is a 6x86(L) */ \
        "out 22h, ax"           /* .. */ \
        "in  ax, 23h"           /* .. */ \
        "and eax, 0F0h"         /* should be 3xh */ \
        "cmp eax, 30h"          /* .. */ \
        "jne short no_cpuid"    /* If not : jump to no_cpuid */ \
        "mov eax, 0E9h"         /* get CCR5 : we reset the SLOP bit */ \
        "out 22h, ax"           /* .. */ \
        "in  ax, 23h"           /* .. */ \
        "and eax, 0FDh"         /* so that udelay calculation */ \
        "mov ebx, eax"          /* is correct on 6x86(L) CPUs */ \
        "mov eax, 0E9h"         /* set CCR5 */ \
        "out 22h, ax"           /* .. */ \
        "mov eax, ebx"          /* .. */ \
        "out 23h, ax"           /* .. */ \
        "inc ecx"               /* set has CPUID flag */ \
"no_cpuid:" \
        "mov ax, 0C3h"          /* Restore old CCR3 */ \
        "out 22h, ax"           /* .. */ \
        "pop eax"               /* get old CCR3 value from stack */ \
        "out 23h, ax"           /* .. */ \
        "sti"                   /* enable interrupt */ \
        "jmp short ret_ecx" \
    __value [__eax] \
    __modify [__ebx __ecx]

#endif

#endif /* __PROFILE_H */
