#ifndef SCHEDULER_H
#define SCHEDULER_H


/* Scheduler.h -- declarations for the Process Scheduler

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

log:    SCHEDULE.H $
Revision 1.2  95/01/29  13:27:34  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:08  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:06  kgorlen
 * Release for 1st edition.
 * 
*/
#include "LinkedList.h"
#include "StackProc.h"
#include "nihclconfig.h"
#ifdef HAVE_SELECT
#include <sys/time.h>
#endif

class HeapProc;

class Scheduler : public NIHCL {
public:                 // type definitions
        enum setjmp_val { schedule_process=0, resume_current_process, resume_new_process };
public:                 // static member functions
        static unsigned char activePriority()   { return active_process->priority(); }
        static Process& activeProcess()         { return *active_process; }
        static bool astActive()                 { return ast_level != 0; }
        static const char* className();
        static void dumpOn(ostream& strm =cerr);
#ifndef NESTED_TYPES
        static void initialize(stackTy* bottom, int priority =0);
#else
        static void initialize(Process::stackTy* bottom, int priority =0);
#endif
        static void printOn(ostream& strm =cout);
        static void terminateActive()           { activeProcess().terminate(); }
        static void schedule();
        static void yield();
public:                 // static member variables
        static JMP_BUF switcher;        // environment for stack switcher
// The following must be public so they can be accessed from the MAIN_PROCESS macro
        static Process* active_process;
        static StackProc* main_stack_process;
#ifndef NESTED_TYPES
        static stackTy* main_stack_bottom;
#else
        static Process::stackTy* main_stack_bottom;
#endif
private:                // static member variables
        static Process* previous_process;
        static unsigned runCount;       // total # of runnable processes 
        static LinkedList runList[MAXPRIORITY+1];
#ifdef HAVE_SELECT
        static LinkedList selectList;   // Process waiting for select
        static struct timeval selectTimeout;    // timeout on select
        static int selectfd();                  // do select(2) system call
        void timeOutSelect()    { selectTimeout.tv_sec = selectTimeout.tv_usec = 0; }
#endif
protected:              // static member variables
        static int ast_level;   // AST nesting level 
        Scheduler() {}          // to prevent construction of instances
private:                // member functions
        Scheduler(const Scheduler&) {}  // to prevent copy of instances
        static void addProcess(Process& p);
        friend Process;
        friend StackProc::StackProc(stackTy* bottom, int priority);
        friend void StackProc::switchFrom(HeapProc*);
        friend void StackProc::switchFrom(StackProc*);
};

#ifndef NESTED_TYPES
inline void Scheduler::initialize(stackTy* bottom, int priority)
#else
inline void Scheduler::initialize(Process::stackTy* bottom, int priority)
#endif
{
        Scheduler::main_stack_bottom = bottom;
// Create MAIN process
        Scheduler::active_process = new StackProc(bottom,priority);
}

/*
main() must call MAIN_PROCESS first thing to initialize the scheduler.

Note: MAIN_PROCESS must avoid using blocks in code executed as the
result of a longjmp() because such blocks may contain
compiler-generated local variables that share storage with local
variables used elsewhere in main().  Executing such a block after a
longjmp() would cause these local variables to be overwritten.  Inline
functions must be avoided for the same reason.

*/

#ifndef NESTED_TYPES
#define _STACKTY stackTy
#else
#define _STACKTY Process::stackTy
#endif

#if STACK_GROWS_DOWN

#define _STACKPROC_ALLOCATE(active) \
        alloca((Scheduler::main_stack_bottom - (active)->stack_top) * sizeof(_STACKTY)) \

#else

#define _STACKPROC_ALLOCATE(active) \
        alloca(((active)->stack_top - Scheduler::main_stack_bottom) * sizeof(_STACKTY)) \

#endif

#define MAIN_PROCESS(priority) \
        if (_SETJMP(Scheduler::switcher) == 0) goto _begin_main_process; \
/* longjmp() here to restore a process's stack */ \
        ((StackProc*)Scheduler::active_process)-> \
                restoreStack((_STACKTY*)_STACKPROC_ALLOCATE((StackProc*)Scheduler::active_process)); \
_begin_main_process: \
        Scheduler::initialize((_STACKTY*)alloca(sizeof(_STACKTY)),priority); \

class AST_LEVEL : public Scheduler {
public:
        AST_LEVEL()     { ast_level++; }
        ~AST_LEVEL()    { ast_level--; }
};

// AST/signal handlers must begin with an AST_ENTER
#define AST_ENTER       AST_LEVEL ast_level_dummy

#endif
