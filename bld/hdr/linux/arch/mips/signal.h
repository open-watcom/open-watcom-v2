#define SIG_ERR ((__sighandler_t)-1)    /* error return from signal */
#define SIG_DFL ((__sighandler_t)0)     /* default signal handling */
#define SIG_IGN ((__sighandler_t)1)     /* ignore signal */

#define SIGHUP       1  /* Hangup (POSIX).  */
#define SIGINT       2  /* Interrupt (ANSI).  */
#define SIGQUIT      3  /* Quit (POSIX).  */
#define SIGILL       4  /* Illegal instruction (ANSI).  */
#define SIGTRAP      5  /* Trace trap (POSIX).  */
#define SIGIOT       6  /* IOT trap (4.2 BSD).  */
#define SIGABRT      SIGIOT /* Abort (ANSI).  */
#define SIGEMT       7
#define SIGFPE       8  /* Floating-point exception (ANSI).  */
#define SIGKILL      9  /* Kill, unblockable (POSIX).  */
#define SIGBUS      10  /* BUS error (4.2 BSD).  */
#define SIGSEGV     11  /* Segmentation violation (ANSI).  */
#define SIGSYS      12
#define SIGPIPE     13  /* Broken pipe (POSIX).  */
#define SIGALRM     14  /* Alarm clock (POSIX).  */
#define SIGTERM     15  /* Termination (ANSI).  */
#define SIGUSR1     16  /* User-defined signal 1 (POSIX).  */
#define SIGUSR2     17  /* User-defined signal 2 (POSIX).  */
#define SIGCHLD     18  /* Child status has changed (POSIX).  */
#define SIGCLD      SIGCHLD /* Same as SIGCHLD (System V).  */
#define SIGPWR      19  /* Power failure restart (System V).  */
#define SIGWINCH    20  /* Window size change (4.3 BSD, Sun).  */
#define SIGURG      21  /* Urgent condition on socket (4.2 BSD).  */
#define SIGIO       22  /* I/O now possible (4.2 BSD).  */
#define SIGPOLL     SIGIO   /* Pollable event occurred (System V).  */
#define SIGSTOP     23  /* Stop, unblockable (POSIX).  */
#define SIGTSTP     24  /* Keyboard stop (POSIX).  */
#define SIGCONT     25  /* Continue (POSIX).  */
#define SIGTTIN     26  /* Background read from tty (POSIX).  */
#define SIGTTOU     27  /* Background write to tty (POSIX).  */
#define SIGVTALRM   28  /* Virtual alarm clock (4.2 BSD).  */
#define SIGPROF     29  /* Profiling alarm clock (4.2 BSD).  */
#define SIGXCPU     30  /* CPU limit exceeded (4.2 BSD).  */
#define SIGXFSZ     31  /* File size limit exceeded (4.2 BSD).  */
#define _SIGMAX     32

#define SIGRTMIN    32
#define SIGRTMAX    _NSIG

/*
 * SA_FLAGS values:
 *
 * SA_ONSTACK indicates that a registered stack_t will be used.
 * SA_INTERRUPT is a no-op, but left due to historical reasons. Use the
 * SA_RESTART flag to get restarting signals (which were the default long ago)
 * SA_NOCLDSTOP flag to turn off SIGCHLD when children stop.
 * SA_RESETHAND clears the handler when the signal is delivered.
 * SA_NOCLDWAIT flag on SIGCHLD to inhibit zombies.
 * SA_NODEFER prevents the current signal from being masked in the handler.
 *
 * SA_ONESHOT and SA_NOMASK are the historical Linux names for the Single
 * Unix names RESETHAND and NODEFER respectively.
 */
#define SA_ONSTACK      0x08000000
#define SA_RESETHAND    0x80000000
#define SA_RESTART      0x10000000
#define SA_SIGINFO      0x00000008
#define SA_NODEFER      0x40000000
#define SA_NOCLDWAIT    0x00010000
#define SA_NOCLDSTOP    0x00000001

#define SA_NOMASK       SA_NODEFER
#define SA_ONESHOT      SA_RESETHAND
#define SA_INTERRUPT    0x20000000  /* dummy -- ignored */

#define SA_RESTORER     0x04000000  /* Only for o32 */

/*
 * sigaltstack controls
 */
#define SS_ONSTACK     1
#define SS_DISABLE     2

#define MINSIGSTKSZ    2048
#define SIGSTKSZ       8192

#define SIG_BLOCK       1   /* for blocking signals */
#define SIG_UNBLOCK     2   /* for unblocking signals */
#define SIG_SETMASK     3   /* for setting the signal mask */
#define SIG_SETMASK32   256 /* Goodie from SGI for BSD compatibility: set only the low 32 bit of the sigset. */
