#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "wattcp.h"
#include "language.h"
#include "strings.h"
#include "pctcp.h"
#include "wdpmi.h"
#include "netback.h"

#define TIMER_INTR 8
#define STK_SIZE   1024


#if (DOSX)
  void backgroundon (void)
  {
    outsnl (_LANG("Use wintr_init() / wintr_enable() instead"));
    exit (3);
  }

#elif !defined(NO_INLINE_ASM)  /* MSC <=6 unsupported */

  static void (*userRoutine)(void) = NULL;
  static int inside = 0;

  #ifdef __TURBOC__
    static void interrupt (*oldinterrupt)(void);
  #else
    static void (interrupt *oldinterrupt)();
  #endif

  static void interrupt newinterrupt(void)
  {
    (*oldinterrupt)();
    DISABLE();
    if (inside)
    {
      static UINT tempstack [STK_SIZE];
  #ifdef __WATCOMC__
      stackset (&tempstack[STK_SIZE-1]);
  #else
        static UINT old_SP;
        static WORD old_SS;
        asm  mov ax,ss
        asm  mov old_SS,ax
        asm  mov ax,sp
        asm  mov old_SP,ax
        asm  mov ax,ds
        asm  mov ss,ax
        asm  lea sp,tempstack[STK_SIZE-1]

  #endif
      ENABLE();

      if (userRoutine)
        (*userRoutine)();
      tcp_tick (NULL);

      DISABLE();

  #ifdef __WATCOMC__
      stackrestore();
  #else
      asm  mov ax,old_SS
      asm  mov ss,ax
      asm  mov ax,old_SP
      asm  mov sp,ax

  #endif
      inside = 0;
    }
    ENABLE();
  }

  void backgroundon (void)
  {
    oldinterrupt = getvect (TIMER_INTR);
    setvect (TIMER_INTR, newinterrupt);
  }

  void backgroundoff (void)
  {
    setvect (TIMER_INTR, oldinterrupt);
  }

  void backgroundfn (void (*fn)())
  {
    userRoutine = fn;
  }
#endif  /* DOSX */
