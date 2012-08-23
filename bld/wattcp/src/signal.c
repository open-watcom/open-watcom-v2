/*
 *
 *   BSD sockets functionality for Waterloo TCP/IP
 *
 *   Version
 *
 *   0.5 : Dec 18, 1997 : G. Vanem - created
 */

#include <stdio.h>
#include <setjmp.h>

#include "socket.h"

#if defined(USE_BSD_FUNC)

#if defined(__HIGHC__)  /* Metaware's HighC have SIGBREAK == SIGQUIT */
#undef SIGBREAK   
#endif

/*
 *  Signal handling for most loops where we can be interrupted
 *  by user pressing ^C/^Break (generating SIGINT/SIGBREAK).
 *  Some targets may also raise SIGQUIT which we handle the same way.
 *  SIGALRM need special attention because we need to block it.
 */
#define TRAP_SIGALRM 0   /* 1: trap SIGALRM also */

static jmp_buf sig_jmp;

#if defined(SIGALRM) && TRAP_SIGALRM
static void (*old_sigalrm)(int);
#endif

#if defined(SIGBREAK)
static void (*old_sigbrk)(int);
#endif                           

#if defined(SIGQUIT)
static void (*old_sigquit)(int);
#endif
              
#if defined(SIGPIPE)
static void (*old_sigpipe)(int);
#endif                           

static void (*old_sigint)(int);

static int wat_brkmode;
static int signal_depth   = 0;
static int sigalrm_caught = 0;
static int sigbrk_caught  = 0;
static int sigquit_caught = 0;
static int sigint_caught  = 0;
static int sigpipe_caught = 0;

/*
 * We need to prevent SIGALRM and hence an alarm() handler from
 * messing up our sockets. Not finished!
 */
#if defined(__DJGPP__) && TRAP_SIGALRM

static sigset_t new_mask, old_mask;
static int      block_depth = 0;

static __inline void block_sigalrm (void)
{
  if (++block_depth == 1)
  {
    sigemptyset (&new_mask);
    sigaddset (&new_mask, SIGALRM);
    sigprocmask (SIG_BLOCK, &new_mask, &old_mask);
  }
}

static __inline void unblock_sigalrm (void)
{
  if (block_depth)
  {
    block_depth--;
    if (block_depth == 0)
       sigprocmask (SIG_SETMASK, &old_mask, NULL);
  }
}
#endif

/*
 * Our signal catcher. Increment proper counter and longjmp()
 */
static void sig_catch (int sig)
{     
  switch (sig)
  {
#if defined(SIGALRM)
    case SIGALRM:
         sigalrm_caught++;  
         break;
#endif
#if defined(SIGPIPE)
    case SIGPIPE:
         sigpipe_caught++;
         break;
#endif
#if defined(SIGBREAK)
    case SIGBREAK:
         sigbrk_caught++;
         wathndlcbrk = wat_brkmode;  /* restore break-mode ASAP */
         break;
#endif
#if defined(SIGQUIT)
    case SIGQUIT:
         sigquit_caught++;
         wathndlcbrk = wat_brkmode;
         break;
#endif
    case SIGINT:
         sigint_caught++;
         wathndlcbrk = wat_brkmode;
         break;
  }
  longjmp (sig_jmp, sig);     /* setjmp() returns `sig' */
}


#if defined(USE_DEBUG)
/*
 * Return name for signal we're trapping
 */
static __inline const char *sig_name (int sig)
{
#if defined(SIGALRM)
  if (sig == SIGALRM)
     return ("SIGALRM");
#endif

#if defined(SIGPIPE)
  if (sig == SIGPIPE)
     return ("SIGPIPE");
#endif

#if defined(SIGBREAK)
  if (sig == SIGBREAK)
     return ("SIGBREAK");
#endif

#if defined(SIGQUIT)
  if (sig == SIGQUIT)
     return ("SIGQUIT");
#endif

  if (sig == SIGINT)
     return ("SIGINT");
  return ("??");
}
#endif


int _sock_sig_setup (void)
{
  volatile int sig;

  if (signal_depth > 0)
     return (0);

  if (++signal_depth > 1)
     return (0);

  _sock_start_timer();
  wat_brkmode = wathndlcbrk;
  wathndlcbrk = 0;
  watcbroke   = 0;

  sigalrm_caught = sigbrk_caught  = 0;
  sigint_caught  = sigquit_caught = 0;

  old_sigint = signal (SIGINT, sig_catch);

#if defined(SIGQUIT)
  old_sigquit = signal (SIGQUIT, sig_catch);
#endif

#if defined(SIGBREAK)
  old_sigbrk = signal (SIGBREAK, sig_catch);
#endif

#if defined(SIGPIPE)
  old_sigpipe = signal (SIGPIPE, sig_catch);
#endif

#if defined(SIGALRM) && TRAP_SIGALRM
  old_sigalrm = signal (SIGALRM, sig_catch);
  block_sigalrm();
#endif

  sig = setjmp (sig_jmp);
  if (sig == 0)
     return (0);


  /* We'll get here only when sig_catch() calls longjmp()
   */
  SOCK_DEBUGF ((NULL, ", interrupted by %s\n", sig_name(sig)));
  _sock_sig_restore();
  return (-1);
}

/*
 * Unhook signal-handlers and optionally chain to previous handlers
 * if we caught signals.
 */
void _sock_sig_restore (void)
{
  if (signal_depth == 0)
     return;

  if (--signal_depth > 0)
     return;

  _sock_stop_timer();
  watcbroke   = 0;
  wathndlcbrk = wat_brkmode;

#if defined(SIGALRM) && TRAP_SIGALRM
  signal (SIGALRM, old_sigalrm);
  unblock_sigalrm();

#if 0
  /* don't do this since a socket function might be called from an
   * alarm handler. This could cause serious recursion and stack fault.
   */
  if (sigalrm_caught && old_sigalrm != SIG_IGN && old_sigalrm != SIG_DFL)
  {
    sigalrm_caught = 0;
    (*old_sigalrm) (SIGALRM);
  }
#endif
#endif

#if defined(SIGBREAK)
  signal (SIGBREAK, old_sigbrk);
  if (sigbrk_caught && old_sigbrk != SIG_IGN && old_sigbrk != SIG_DFL)
  {
     sigbrk_caught = 0;
    (*old_sigbrk) (SIGBREAK);
  }
#endif

#if defined(SIGPIPE)
  signal (SIGPIPE, old_sigpipe);
  if (sigpipe_caught)
  {
    if (old_sigpipe != SIG_IGN && old_sigpipe != SIG_DFL)
    {
       sigpipe_caught = 0;
      (*old_sigpipe) (SIGPIPE);
    }
    else
    {
      outsnl (_LANG("Terminating on SIGPIPE"));
      exit (-1);
    }
  }
#endif

#if defined(SIGQUIT)
  signal (SIGQUIT, old_sigquit);
  if (sigquit_caught)
  {
    if (old_sigquit != SIG_IGN && old_sigquit != SIG_DFL)
    {
      sigquit_caught = 0;
      (*old_sigquit) (SIGQUIT);
    }
    else
    {
      SOCK_DEBUGF ((NULL, "\nExiting stuck program"));
      exit (-1);
    }
  }
#endif

  signal (SIGINT, old_sigint);
  if (sigint_caught && old_sigint != SIG_IGN && old_sigint != SIG_DFL)
  {
    sigint_caught = 0;
    (*old_sigint) (SIGINT);
  }

  sigalrm_caught = sigbrk_caught  = 0;
  sigint_caught  = sigquit_caught = 0;
  sigpipe_caught = 0;
} 

/*
 * Raise SIGPIPE if signal defined.
 * Return -1 with errno = EPIPE.
 */
int _sock_sig_epipe (void)
{
#if defined(SIGPIPE)
  raise (SIGPIPE);
#endif
  SOCK_ERR (EPIPE);
  return (-1);
}

#endif  /* USE_BSD_FUNC */
