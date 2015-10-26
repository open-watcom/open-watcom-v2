#ifndef __MISC_H
#define __MISC_H

#include <sys/swap.h>     /* intel() etc. */
#include <sys/wtime.h>    /* struct timeval */

/*
 * Implemented in misc.c
 *
 * Fixup namespace for "internal" symbols
 */
#define cpu_type    NAMESPACE(cpu_type)
#define init_misc   NAMESPACE(init_misc)
#define Random      NAMESPACE(Random)
#define RandomWait  NAMESPACE(RandomWait)
#define valid_addr  NAMESPACE(valid_addr)
#define hex_chars   NAMESPACE(hex_chars)
#define hex_CHARS   NAMESPACE(hex_CHARS)

extern int __bss_count, cpu_type;

extern char hex_chars[];
extern char hex_CHARS[];

extern void init_misc (void);
extern int  Random    (unsigned a, unsigned b);
extern void RandomWait(unsigned a, unsigned b);

extern BOOL valid_addr(DWORD addr, int len);

#if defined(USE_DEBUG)
  extern void unfinished (const char *file, unsigned line);
  #define UNFINISHED() unfinished(__FILE__, __LINE__)
#else
  #define UNFINISHED() ((void)0)
#endif

#if defined(__LARGE__)
  extern void watt_largecheck (void *s, int size, char *file, unsigned line);
#else
  #define watt_largecheck(s,ss,f,l) ((void)0)
#endif

/*
 * Implemented in timer.c
 */
#define has_8254     NAMESPACE (has_8254)
#define has_rdtsc    NAMESPACE (has_rdtsc)
#define init_timers  NAMESPACE (init_timers)
#define set_timeout  NAMESPACE (set_timeout)
#define chk_timeout  NAMESPACE (chk_timeout)
#define hires_timer  NAMESPACE (hires_timer)
#define set_timediff NAMESPACE (set_timediff)
#define get_timediff NAMESPACE (get_timediff)
#define timeval_diff NAMESPACE (timeval_diff)

#define time_str     NAMESPACE (time_str)
#define dword_str    NAMESPACE (dword_str)

extern int has_8254, has_rdtsc;

extern void   init_timers  (void);
extern DWORD  set_timeout  (DWORD msec);        /* return value for timeout */
extern int    chk_timeout  (DWORD timeout);     /* check if time expired    */
extern int    cmp_timers   (DWORD t1, DWORD t2);
extern void   hires_timer  (int on);
extern int    set_timediff (long msec);
extern DWORD  get_timediff (DWORD now, DWORD t);
extern struct timeval timeval_diff (const struct timeval *a, const struct timeval *b);

extern const char *time_str  (DWORD val);
extern const char *dword_str (DWORD val);


#if defined(HAVE_UINT64)
  union ulong_long {
          struct {
            DWORD hi;
            DWORD lo;
          } s;
          uint64 ull;
        };

  #define microsec_clock NAMESPACE (microsec_clock)
  extern uint64 microsec_clock (void);

#if defined(__GNUC__) && defined(__i386__) /* also for gcc -m486 and better */
 /*
  * This is not used yet since the benefit/drawbacks are unknown.
  * Define 32-bit multiplication asm macros.
  *
  * umul_ppmm (high_prod, low_prod, multiplier, multiplicand)
  * multiplies two unsigned long integers multiplier and multiplicand,
  * and generates a two unsigned word product in high_prod and
  * low_prod.
  */
  #define umul_ppmm(w1,w0,u,v)                            \
          __asm__ __volatile__ (                          \
                 "mull %3"                                \
                 : "=a" ((DWORD)(w0)), "=d" ((DWORD)(w1)) \
                 : "%0" ((DWORD)(u)),  "rm" ((DWORD)(v)))

  #define mul32(u,v) ({ union ulong_long w;               \
                        umul_ppmm (w.s.hi, w.s.lo, u, v); \
                        w.ull; })
  /* Use as:
   *  DWORD x,y;
   *  ..
   *  uint64 z = mul32 (x,y);
   */
#endif /* __GNUC__ && __i386__ */
#endif /* HAVE_UINT64 */

/*
 * Some prototypes not found in lc-lint's libraries
 */
#if defined(lint) || defined(_lint)
  extern char *strupr (char*);
  extern char *strlwr (char*);
  extern char *strdup (const char*);
  extern int   stricmp (const char*, const char*);
  extern int   strnicmp (const char*, const char*, size_t);
  extern char *itoa (int, char*, int);
  extern void *alloca (size_t);
  extern int   gettimeofday (struct timeval*, struct timezone*);
  extern int   fileno (FILE*);
  extern int   isascii (int);
  extern void  psignal (int, const char*);
  extern int   vsscanf (const char*, const char*, va_list);

  /*@constant int SIGALRM; @*/
  /*@constant int SIGTRAP; @*/
#endif

#endif /* __MISC_H */

