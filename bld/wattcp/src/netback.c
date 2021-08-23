#include <stdio.h>
#include <stdlib.h>
#include <dos.h>

#include "wattcp.h"
#include "language.h"
#include "strings.h"
#include "pctcp.h"
#include "netback.h"
#include "wdpmi.h"

#define TIMER_INTR 8
#define STK_SIZE   1024

#if (DOSX)
void backgroundon (void)
{
    outsnl (_LANG("Use wintr_init() / wintr_enable() instead"));
    exit (3);
}

#else

static void (*userRoutine)(void) = NULL;
static int inside = 0;

static void (__interrupt *oldinterrupt)(void);

static void __interrupt newinterrupt(void)
{
    (*oldinterrupt)();
    DISABLE();
    if (inside) {
        static UINT tempstack [STK_SIZE];
        stackset (&tempstack[STK_SIZE-1]);
        ENABLE();

        if (userRoutine != NULL)
            (*userRoutine)();
        tcp_tick (NULL);

        DISABLE();

        stackrestore();
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

void backgroundfn (void (*fn)(void))
{
    userRoutine = fn;
}
#endif  /* !DOSX */
