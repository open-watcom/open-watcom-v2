#include <stdio.h>
#include <stdlib.h>
#include <dos.h>
#include <setjmp.h>
#include <signal.h>

#include "copyrigh.h"
#include "wattcp.h"
#include "sock_ini.h"
#include "wdpmi.h"
#include "language.h"
#include "strings.h"
#include "pc_cbrk.h"
#include "iregs.h"


/* Turn off stack-checking here because stack may be out of bounds.
 * longjmp() hopefully fixes that.
 */

#pragma off(check_stack)

#ifdef TEST_PROG     /* for djgpp only */
  #include <dos.h>
  #include <conio.h>
  #include <unistd.h>
  #define BEEP()  do { sound(1000); usleep(3000); nosound(); } while(0)
#else
  #define BEEP()  ((void)0)
#endif

WORD         wathndlcbrk = 0;  /* changes operation of the break stuff */
volatile int watcbroke   = 0;  /* increment on SIGINT catch */

/*
 * tcp_cbreak(mode)
 *   - `cbrkmode' is composed of the following flags
 *     0x01 - disallow breakouts
 *     0x10 - display a message upon ^C/^BREAK
 */

static const char *msgs[] = {
    __LANG ("\7\r\nTerminating program"),
    __LANG ("\7\r\nCtrl-Breaks ignored")
};

static int     cbrkmode;
static jmp_buf sig_jmp;

static void sig_handler (int sig)
{
    BEEP();
    signal (sig, sig_handler);

    if (wathndlcbrk) { /* inside _arp_resolve(), lookup_domain() etc. */
        watcbroke++;
        if (cbrkmode & 0x10)
            outsnl ("\r\nInterrupting");
        return;
    }

    if (cbrkmode)
        outsnl (msgs[cbrkmode & 1]);

    if (!(cbrkmode & 1)) {
        longjmp (sig_jmp, 1);
    }
}

/*
 * want_brk = 0: no ^Break checking
 * want_brk = 1: normal ^Break checks
 * want_brk = 2: extended ^Break checking. What's the difference?
 */
int set_cbreak (int want_brk)
{
#if (DOSX == 0)
    union REGS reg;

    reg.h.ah = 0x33;
    reg.h.al = 0x00;
    reg.h.dl = want_brk;
    int86 (0x21, &reg, &reg);
    return (reg.h.dl);
#else
    IREGS regs;

    regs.r_ax = 0x3300;
    regs.r_dx = want_brk;
    GEN_RM_INTERRUPT (0x21, &regs);
    return loBYTE (regs.r_dx);
#endif
}

int tcp_cbreak (int mode)
{
    volatile int rc;

    cbrkmode = mode;

    signal (SIGINT, sig_handler);

    if (mode & 1) {
        rc = set_cbreak (0);
    } else {
        rc = set_cbreak (1);
    }

    /*
     * Some vendors calls signal-handlers with a very limited stack.
     * This would cause a stack-fault in e.g. pcdbug.c when tracing
     * packets sent in sock_exit()
     */
    if (setjmp(sig_jmp)) {
        watcbroke++;
        sock_exit();
        exit (0);
    }
    return (rc);
}

#if defined(TEST_PROG)

void usage (char *argv0)
{
    printf ("Usage: %s normal | nobrk | graceful\n", argv0);
    exit (-1);
}

int old_brk = -1;

int main (int argc, char **argv)
{
    int mode;

    if (argc != 2)
        usage (argv[0]);

    if (!stricmp(argv[1],"normal")) {
        mode = 0x10;
        wathndlcbrk = 0;
    } else if (!stricmp(argv[1],"nobrk")) {
        mode = 0x01;
        wathndlcbrk = 1;
        cputs ("Press <^BREAK><SPACE> three times to exit\r\n");
    } else if (!stricmp(argv[1],"graceful")) {
        mode = 0;
        wathndlcbrk = 1;
        cputs ("Press <^C> or <^BREAK><SPACE> three times to exit\r\n");
    } else {
        usage (argv[0]);
    }

    old_brk = tcp_cbreak (mode);
    watcbroke = 0;

    while (watcbroke < 3) {
        usleep (200000);
        cputs (".");
        if (kbhit()) {
            getche();
        }
    }
    cputs ("`watcbroke' set\r\n");
    set_cbreak (old_brk);
    return (1);
}

void sock_exit (void)
{
    cputs ("sock_exit() called\r\n");
    if (old_brk >= 0) {
        set_cbreak (old_brk);
    }
}
#endif /* TEST_PROG */
