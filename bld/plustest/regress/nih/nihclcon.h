#ifndef NIHCLCONFIG_H
#define NIHCLCONFIG_H


/* nihclconfig.h -- NIHCL configuration file

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

log:    NIHCLCON.H $
Revision 1.2  95/01/29  13:27:20  NT_Test_Machine
*** empty log message ***

Revision 1.1  93/02/16  16:03:22  Anthony_Scian
.

 * Revision 3.0  90/05/22  08:58:56  kgorlen
 * Release for 1st edition.
 * 
*/

// nihclconfig.h -- NIHCL configuration file

#include <signal.h>
#include <setjmp.h>

#ifdef u3b
#define att3b
#endif
#ifdef u3b2
#define att3b
#endif
#ifdef u3b5
#define att3b
#endif
#ifdef u3b15
#define att3b
#endif

/* Machine Model Implementation Status:

        sun/UCB42BSD            implemented and tested
*/

// Operating System (only one can be defined)

//#define SYSVR2
//#define SYSVR3
//#define UCB42BSD
//#define UCB43BSD

#if defined(att3b)
#define SYSVR3
#endif

#if defined(hpux)
#define SYSVR3
#endif

#ifdef mc300
// default att universe on MASSCOMP
#define SYSV
// ucb universe on MASSCOMP
//#define UCB42BSD
#endif

#ifdef ibm032
// default AOS 4.3 on RT PC
#define UCB43BSD
// AIX on RT PC
//#define SYSVR2
#endif

#if defined(vax)
#define UCB42BSD
#endif

#if defined(sun)

#include <sys/types.h>
#ifndef howmany
#define UCB42BSD
#define SUNOS3
#else
#define UCB43BSD
#define SUNOS4
#endif

#endif

#if defined(SYSVR2) || defined(SYSVR3)
#define SYSV
#endif

#if defined(UCB42BSD) || defined(UCB43BSD)
#ifndef BSD
#define BSD
#endif
#endif

inline unsigned mod_sizeof_short(unsigned i)    {
  return sizeof(short)&sizeof(short)-1 ? i%sizeof(short) : i&sizeof(short)-1; }
inline unsigned mod_sizeof_int(unsigned i)      {
  return sizeof(int)&sizeof(int)-1 ? i%sizeof(int) : i&sizeof(int)-1; }
inline unsigned mod_sizeof_long(unsigned i)     {
  return sizeof(long)&sizeof(long)-1 ? i%sizeof(long) : i&sizeof(long)-1; }
inline unsigned mod_sizeof_float(unsigned i)    {
  return sizeof(float)&sizeof(float)-1 ? i%sizeof(float) : i&sizeof(float)-1; }
inline unsigned mod_sizeof_double(unsigned i)   {
  return sizeof(double)&sizeof(double)-1 ? i%sizeof(double) : i&sizeof(double)-1; }
inline unsigned mod_sizeof_ptr(unsigned i)      {
  return sizeof(void*)&sizeof(void*)-1 ? i%sizeof(void*) : i&sizeof(void*)-1; }

#if defined(att3b) || defined(mc300) || defined(ibm032) || defined(sun) || defined(vax) || defined(i386) || defined(hpux)

inline unsigned div_bitsize_char(unsigned i)    { return i >> 3; }
inline unsigned mod_bitsize_char(unsigned i)    { return i & 7; }
inline unsigned div_bitsize_int(unsigned i)     { return i >> 5; }
inline unsigned mod_bitsize_int(unsigned i)     { return i & 0x1f; }
inline unsigned div_sizeof_short(unsigned i)    { return i >> 1; }
inline unsigned div_sizeof_int(unsigned i)      { return i >> 2; }
inline unsigned div_sizeof_long(unsigned i)     { return i >> 2; }
inline unsigned div_sizeof_float(unsigned i)    { return i >> 2; }
inline unsigned div_sizeof_double(unsigned i)   { return i >> 3; }
inline unsigned div_sizeof_ptr(unsigned i)      { return i >> 2; }

const int UNINITIALIZED =0xa5a5a5a5;    // data value to flag uninitialized variables 

#endif

#if defined(att3b) || defined(mc300) || defined(vax) || defined(ibm032) || defined(sun) || defined(i386)

#define STACK_GROWS_DOWN 1

#endif

#if defined(BSD) || defined(mc300) || defined(hpux)
// defined if select(2) implemented
#define HAVE_SELECT
#endif

// defines for interfacing with ASTs or signals

#if defined(mc300)

#ifndef DEBUG_PROCESS
#define AST_DISABLE     int prior_AST_state = (Scheduler::astActive()? 127 : setpri(127))
#else
#define AST_DISABLE     int prior_AST_state = (Scheduler::astActive()? 0 : setpri(0))
#endif

#define AST_ENABLE      if (!Scheduler::astActive()) setpri(prior_AST_state)
#define AST_STATE       setpri(-1)
#define AST_SAVE(m)     m = prior_AST_state
#define AST_RESTORE(m)  setpri(m)
#define AST_PAUSE       astpause(0,1000)

#else
#if defined(BSD) || defined(hpux)

#ifndef DEBUG_PROCESS
#define AST_DISABLE     int prior_AST_state = (Scheduler::astActive()? 0xFFFFFFFF : sigblock(0xFFFFFFFF))
#else
#define AST_DISABLE     int prior_AST_state = (Scheduler::astActive()? 0x0 : sigblock(0x0))
#endif

#define AST_ENABLE      if (!Scheduler::astActive()) sigsetmask(prior_AST_state)
#define AST_STATE       sigblock(0)
#define AST_SAVE(m)     m = prior_AST_state
#define AST_RESTORE(m)  sigsetmask(m)
#define AST_PAUSE       sigpause(0)

#endif /* WATCOM        BSD */

#if defined(SYSVR2)

// Signals do not queue under System V R2, so they are unusable

#define AST_DISABLE
#define AST_ENABLE
#define AST_STATE       0
#define AST_SAVE(m)
#define AST_RESTORE(m)
#define AST_PAUSE

#endif /* WATCOM        SYSVR2 */
#endif  /* WATCOM mc300 */

// setjmp()/longjmp() context switching for Processes -- see HeapProc.c
//  for additional machine-dependent code.

#if defined(ibm032) && defined(UCB43BSD)
typedef jmp_buf JMP_BUF;
inline int SETJMP(JMP_BUF env)                  { return setjmp(env); }
inline void LONGJMP(JMP_BUF env, int val)       { longjmp(env,val); }
inline int _SETJMP(JMP_BUF env)                 { return _setjmp(env); }
inline void _LONGJMP(JMP_BUF env, int val)      { _longjmp(env,val); }
inline unsigned& ENV_PC(JMP_BUF env)            { return (unsigned&)env[10]; }
inline unsigned& ENV_SP(JMP_BUF env)            { return (unsigned&)env[0]; }
inline unsigned& ENV_FP(JMP_BUF env)            { return (unsigned&)env[0]; }   // register used for FP varies
#endif

#if defined(__WATCOM_CPLUSPLUS__)
typedef jmp_buf JMP_BUF;
inline int SETJMP(JMP_BUF env)                  { return setjmp(env); }
inline void LONGJMP(JMP_BUF env, int val)       { longjmp(env,val); }
inline int _SETJMP(JMP_BUF env)                 { return _setjmp(env); }
inline void _LONGJMP(JMP_BUF env, int val)      { _longjmp(env,val); }
inline unsigned& ENV_PC(JMP_BUF env)            { return (unsigned&)env[10]; }
inline unsigned& ENV_SP(JMP_BUF env)            { return (unsigned&)env[0]; }
inline unsigned& ENV_FP(JMP_BUF env)            { return (unsigned&)env[0]; }   // register used for FP varies
#endif

#if defined(sun)

#ifdef SUNOS3
typedef jmp_buf JMP_BUF;
inline int SETJMP(JMP_BUF env)                  { return setjmp(env); }
inline void LONGJMP(JMP_BUF env, int val)       { longjmp(env,val); }
inline int _SETJMP(JMP_BUF env)                 { return setjmp(env); }
inline void _LONGJMP(JMP_BUF env, int val)      { longjmp(env,val); }
inline unsigned& ENV_PC(JMP_BUF env)            { return (unsigned&)env[0]; }
inline unsigned& ENV_SP(JMP_BUF env)            { return (unsigned&)env[14]; }
inline unsigned& ENV_FP(JMP_BUF env)            { return (unsigned&)env[13]; }
#endif

#ifdef SUNOS4

#ifdef mc68000
typedef jmp_buf JMP_BUF;
inline int SETJMP(JMP_BUF env)                  { return setjmp(env); }
inline void LONGJMP(JMP_BUF env, int val)       { longjmp(env,val); }
inline int _SETJMP(JMP_BUF env)                 { return _setjmp(env); }
inline void _LONGJMP(JMP_BUF env, int val)      { _longjmp(env,val); }
inline unsigned& ENV_PC(JMP_BUF env)            { return (unsigned&)env[3]; }
inline unsigned& ENV_SP(JMP_BUF env)            { return (unsigned&)env[2]; }
inline unsigned& ENV_FP(JMP_BUF env)            { return (unsigned&)env[15]; }
#endif

#ifdef sparc
typedef jmp_buf JMP_BUF;
inline int SETJMP(JMP_BUF env)                  { return setjmp(env); }
inline void LONGJMP(JMP_BUF env, int val)       { longjmp(env,val); }
inline int _SETJMP(JMP_BUF env)                 { return _setjmp(env); }
inline void _LONGJMP(JMP_BUF env, int val)      { _longjmp(env,val); }
inline unsigned& ENV_PC(JMP_BUF env)            { return (unsigned&)env[3]; }
inline unsigned& ENV_SP(JMP_BUF env)            { return (unsigned&)env[2]; }
inline unsigned& ENV_FP(JMP_BUF env)            { return ((unsigned*)ENV_SP(env))[14]; }
#endif

#ifdef i386
NOT YET IMPLEMENTED
#endif

#endif
#endif

#if defined(mc300)
#ifdef SYSV
struct JMP_BUF {
        jmp_buf jbuf;
        long sigmask;
};
inline int SETJMP(JMP_BUF& env)                 { env.sigmask = AST_STATE; return setjmp(env.jbuf); }
inline void LONGJMP(JMP_BUF& env, int val)      { AST_RESTORE(env.sigmask); longjmp(env.jbuf,val); }
inline int _SETJMP(JMP_BUF& env)                { return setjmp(env.jbuf); }
inline void _LONGJMP(JMP_BUF& env, int val)     { longjmp(env.jbuf,val); }
inline unsigned& ENV_PC(JMP_BUF& env)           { return (unsigned&)env.jbuf[0]; }
inline unsigned& ENV_SP(JMP_BUF& env)           { return (unsigned&)env.jbuf[12]; }
inline unsigned& ENV_FP(JMP_BUF& env)           { return (unsigned&)env.jbuf[11]; }
#endif
#ifdef UCB42BSD
typedef jmp_buf JMP_BUF;
inline int SETJMP(JMP_BUF env)                  { return setjmp(env); }
inline void LONGJMP(JMP_BUF env, int val)       { longjmp(env,val); }
inline int _SETJMP(JMP_BUF env)                 { return _setjmp(env); }
inline void _LONGJMP(JMP_BUF env, int val)      { _longjmp(env,val); }
inline unsigned& ENV_PC(JMP_BUF env)            { return (unsigned&)env[0]; }
inline unsigned& ENV_SP(JMP_BUF env)            { return (unsigned&)env[12]; }
inline unsigned& ENV_FP(JMP_BUF env)            { return (unsigned&)env[11]; }
#endif
#endif

inline unsigned ENV_PC(const JMP_BUF& env)      { return ENV_PC((JMP_BUF)env); }
inline unsigned ENV_SP(const JMP_BUF& env)      { return ENV_SP((JMP_BUF)env); }
inline unsigned ENV_FP(const JMP_BUF& env)      { return ENV_FP((JMP_BUF)env); }

#endif
