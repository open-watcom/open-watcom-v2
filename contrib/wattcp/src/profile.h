#ifndef __PROFILE_H
#define __PROFILE_H

#if defined(__DJGPP__) || defined(__WATCOM386__)
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

/*
 * use RDTSC instruction for precise timing and profiling
 */
#if defined(__DJGPP__)
  /*@unused@*/ static __inline__ void get_cpuid (DWORD val, DWORD *eax, DWORD *ebx, DWORD *ecx, DWORD *edx)
  {
    __asm__ __volatile__ (
           ".byte 0x0F,0xA2;"   /* cpuid opcode */
           : "=a" (*eax), "=b" (*ebx), "=c" (*ecx), "=d" (*edx)
           : "0" (val));
    ARGSUSED (val);  /* for lint */
    ARGSUSED (eax);
    ARGSUSED (ebx);
    ARGSUSED (ecx);
    ARGSUSED (edx);
  }

  /*
   * Generate code for the initial timestamp read - the cld
   * and nops are included for repeatable pairing and to eliminate
   * shadowing effects from previous instructions.
   *
   * This code is originally by
   *   Tom Burgess <Tom_Burgess@bc.sympatico.ca> and
   *   Douglas Eleveld <deleveld@dds.nl>
   */
  /*@unused@*/ static __inline__ uint64 get_rdtsc (void)
  {
    uint64 tsc;
    __asm__ __volatile__ (
            ".byte 0x0F, 0x31;"   /* rdtsc opcode */
            "movl %%eax, (%%edi);"
            "movl %%edx, 4(%%edi);"
            "cld;"
            "nop; nop; nop;"
            "nop; nop; nop;"
            "nop; nop;"
            :: "D" (&tsc) : "eax", "edx");
    /*@-usedef*/
    return (tsc);
  }
  /*@unused@*/ static __inline__ void get_rdtsc2 (/*@-usedef*/ uint64 *tsc)
  {
    __asm__ __volatile__ (
            ".byte 0x0F, 0x31;"
            : "=a" (tsc[0]), "=d" (tsc[1]) : );
    ARGSUSED (tsc);  /* for lint */
  }

#elif defined(__WATCOM386__)
  extern void get_cpuid (DWORD val, DWORD *eax, DWORD *ebx, DWORD *ecx, DWORD *edx);
  #pragma aux get_cpuid =   \
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
        parm [esi] [eax] [ebx] [ecx] [edx] \
        modify [esi eax ebx ecx edx];

  extern uint64 get_rdtsc (void);
  #pragma aux get_rdtsc = \
          ".586"          \
          "db 0Fh, 31h"   \
          "cld"           \
          "nop"           \
          "nop"           \
          "nop"           \
          "nop"           \
          modify [eax edx];
#endif

#endif /* __PROFILE_H */
