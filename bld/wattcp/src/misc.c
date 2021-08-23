/*
 *  Module for various things:
 *   - host/network order (little/big endian) swapping of bytes.
 *   - initialise peek/poke macros.
 *   - allocate transfer buffer for DOSX targets.
 *   - address validation for DOSX targets.
 *   - simple range limited random routine.
 *   - ffs routine to find the first bit set.
 *   - stack checker exit routine for Watcom.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <dos.h>

#include "wattcp.h"
#include "wdpmi.h"
#include "strings.h"
#include "pcsed.h"
#include "misc.h"

/* These arrays are used several places (save some space)
 */
char hex_chars[] = "0123456789abcdef";
char hex_CHARS[] = "0123456789ABCDEF";

#if (DOSX == 0)
int cpu_type = 86;      /* Detected CPU type; 86,386, ... */
#else
int cpu_type = 386;
#endif

int __bss_count = 0;    /* detect program restarts (!!to-do) */

#ifdef HAS_FP
FARPTR _watt_dosFp = (FARPTR)0; /* Put in _DATA segment */
#endif

#if (DOSX)
static void setup_dos_xfer_buf (void);
#endif

/*
 * Turn off stack-checking to avoid destroying assumptions
 * made in bswap patch code and ffs() below. And also to make
 * this run a bit faster.
 */

#pragma off(check_stack)
void init_misc (void)
{
#if defined(HAS_FP) && (DOSX & PHARLAP)
  /*
   * For 32-bit compilers with 48-bit far-pointers.
   * `init_misc' MUST be called before `peekx()' functions are used.
   */
  FP_SET (_watt_dosFp, 0, SS_DOSMEM);

#elif defined(HAS_FP) /* MSVC + WDOSX */
  UNFINISHED();
#endif

  /* Check if we're running a 80486+ CPU.
   */
#if (DOSX & PHARLAP)
  {
    CONFIG_INF cnf;
    _dx_config_inf (&cnf, (UCHAR*)&cnf);
    if (cnf.c_processor >= 4)
       cpu_type = 486;
  }
#elif (DOSX & (DOS4GW|WDOSX))
  if (dpmi_cpu_type() >= 4)
     cpu_type = 486;
#endif

#if (DOSX)
  setup_dos_xfer_buf();
#endif

  srand (peekw(0,0x46C)); /* initialize rand using BIOS clock */
  init_timers();
}

/*
 * returns a random integer in range [a..b]
 */
int Random (unsigned a, unsigned b)
{
  if (a == b)
     return (a);

  if (a > b)
  {
    unsigned x = b;
    b = a;
    a = x;
  }
  return (a + (unsigned)(rand() % (b-a+1)));
}

/*
 * Wait for a random period in range [a..b] millisec
 */
void RandomWait (unsigned a, unsigned b)
{
  DWORD t = set_timeout (Random(a, b));

  while (!chk_timeout(t))
        ;
}

#if defined(USE_DEBUG) && defined(NOT_USED)
/*
 * dword_str() - return nicely formatted string " xx,xxx,xxx"
 * with thousand separators.
 */
const char *dword_str (DWORD val)
{
  static char buf[20];
  char   tmp[20];

  if (val < 1000UL)
  {
    sprintf (buf, "%lu", val);
    return (buf);
  }
  if (val < 1000000UL)       /* 1E6 */
  {
    sprintf (buf, "%lu.%03lu", val/1000UL, val % 1000UL);
    return (buf);
  }
  if (val < 1000000000UL)    /* 1E9 */
  {
    sprintf (tmp, "%9lu", val);
    sprintf (buf, "%.3s,%.3s,%.3s", tmp, tmp+3, tmp+6);
    return (buf);
  }
  sprintf (tmp, "%12lu", val);
  sprintf (buf, "%.3s,%.3s,%.3s,%.3s", tmp, tmp+3, tmp+6, tmp+9);
  return (buf);
}
#endif /* USE_DEBUG && NOT_USED */


#if (DOSX)
/*
 * Test for valid read/write data address.
 * We assume linear address 'addr' is both readable and writeable.
 */
BOOL valid_addr (DWORD addr, int len)
{
  if (addr < 0x1000 || (addr >= 0xFFFFFFFFL - len))
     return (FALSE);

  if (addr + len > _get_limit(My_DS()))
     return (FALSE);

  return (TRUE);
}
#endif  /* DOSX */


/*
 * Pharlap targets:      Determine location of DOS-transfer buffer.
 * DOS4GW/WDOSX targets: Allocate a small (1kB) DOS-transfer buffer.
 */
#if (DOSX & PHARLAP)
  REALPTR _watt_dosTbr;          /* rmode-address of transfer buffer */
  FARPTR  _watt_dosTbp;          /* pmode-address of transfer buffer */
  ULONG   _watt_dosTbSize = 0;   /* size of transfer buffer          */
  REALPTR _watt_r2p_addr  = 0;   /* address for real to pmode switch */

  static void setup_dos_xfer_buf (void)
  {
    _dx_rmlink_get (&_watt_r2p_addr,  &_watt_dosTbr,
                    &_watt_dosTbSize, &_watt_dosTbp);
  }

#elif (DOSX & (DOS4GW|WDOSX))
  unsigned short _watt_dosTbSeg = 0; /* paragraph address of xfer buffer */
  unsigned short _watt_dosTbSel = 0; /* selector for transfer buffer     */
  int _watt_dosTbSize = 0;           /* size of transfer buffer          */

  static void free_tb_sel (void)
  {
    if (_watt_dosTbSel)
       dpmi_real_free (_watt_dosTbSel);
    _watt_dosTbSel = 0;
  }
  static void setup_dos_xfer_buf (void)
  {
    _watt_dosTbSize = 1024;
    _watt_dosTbSeg  = dpmi_real_malloc (_watt_dosTbSize, &_watt_dosTbSel);
    if (_watt_dosTbSeg)
         _watt_dosTbSize = 0;
    else atexit (free_tb_sel);
  }
#endif

#if defined(USE_DEBUG)
  void unfinished (const char *file, unsigned line)
  {
    fprintf (stderr, "%s (%u):\7Help! Unfinished code.\n", file, line);
    _exit (-1);
  }
#endif

/*
 * Checks for bugs when compiling in large model C compiler
 *
 * Borland C uses a 4K stack by default. In all memory models the
 * stack grows down toward the heap.
 *
 * If you accidentally place tcp_Socket onto the stack, then you
 * will have already used up that whole 4K and then some!
 *
 * In large model, this will mess up the data space in a major way
 * because the stack starts at SS:_stklen, or SS:1000, so you will
 * wrap the SP pointer back around to FFFE and start writing over
 * the far heap.  Yuck.
 *
 * In small model it usually doesn't kill your application because
 * you would have to be down to your last 4K of memory and this is
 * not as common.
 *
 * The solutions: declare your sockets as static, or put them on the
 * heap, or bump up your stack size by using the global special variable:
 *
 * unsigned _stklen = 16536;
 */

#if defined(__LARGE__)
void watt_largecheck (void *s, int size, char *file, unsigned line)
{
  if ((unsigned)(_FP_OFF(s)) > (unsigned)(-size))
  {
    printf ("%s (%d): user stack size error", file, line);
    exit (3);
  }
}
#endif


#if defined(USE_DEBUG)

/*
 * For tracking down stack overflow bugs.
 *
 * 32-bit
 *
 * Stack checker __CHK is pascal-style with stack-size at [esp+4].
 * __CHK calls __STK which in turn may call _fatal_runtime_error()
 *
 * Compiling with stack-checking on, this prologue is in every function:
 *  (*) push <stack size needed>   <- 68h, dword size at EIP-9
 *      call __CHK                 <- 5 bytes
 *      ...                        <- extracted EIP of return
 *
 * 16-bit (existing implementation is wrong, need rework)
 *
 * All necessary info are not available on the stack
 * some of them are in registers only.
 * Probably it will require to intercept __STACKOVERFLOW code.
 */

#if !defined(__LARGE__)
/*
 * label at TEXT start
 */
extern char __begtext;
#pragma aux __begtext "_*";
#endif

extern  unsigned    _STACKLOW;

    void __cdecl _fatal_runtime_error (UINT stk); /* __cdecl used to have access to stack */
    #define FATAL_HANDLER _fatal_runtime_error
#if !defined(__SW_3S)
    #pragma aux _fatal_runtime_error "_*_"  /* wcc386 -3r and wcc, register-based calling name mangling */
#endif

/* Prevent linker (with 'option eliminate') to drop our
 * '_fatal_runtime_error()' function from .exe-image.
 */
char *dummy_fatal_rte = (char*)&FATAL_HANDLER;

static void stk_overflow (WORD cs, UINT eip)
{
    static char buf[12];

    /*
     * temporary 16-bit code to get some info
     * TO-DO need some work to be correct
     */
#if defined(__I86__)
//    eip -= ???;       /* add appropriate correction if necessary */
#else
    UINT size;

    size = *(UINT*)(eip-9);
    eip -= (UINT)&__begtext - 9; /* print .map-file address of (*) */
#endif

#if 1
    outs ("Stack overflow ");
#if defined(__386__)
    buf[0] = '(';
    itoa (size, buf + 1, 10);
    outs (buf);
    outs (" bytes) ");
#endif
    outs ("detected at ");
    itoa (cs, buf, 16);
    outs (buf);
    buf[0] = ':';
    itoa (eip, buf+1, 16);
    outsnl (buf);
#else
    fprintf (stderr, "Stack overflow (%u bytes) detected at %X:%08lXh\n",
           size, cs, (DWORD)eip);
#endif

    _eth_release();
    _exit (1);     /* do minimal work, no atexit() functions */
}

void FATAL_HANDLER (UINT stk)
{
    /*
     * temporary 16-bit code to get some info
     * TO-DO need some work to be correct
     */

#if defined(__386__)
    /* 32-bit near call */
    _STACKLOW = stk + 4;
    stk_overflow (My_CS(), *(UINT*)(&stk+3));
#elif defined(__SMALL__)
    /* 16-bit near call */
    _STACKLOW = stk + 2;
    stk_overflow (My_CS(), *(UINT*)(&stk+3));
#else
    /* 16-bit far call */
    _STACKLOW = stk + 4;
    stk_overflow (*(WORD*)(&stk+5), *(UINT*)(&stk+3));
#endif
}

#endif  /* USE_DEBUG */
