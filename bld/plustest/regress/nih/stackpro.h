#ifndef STACKPROC_H
#define STACKPROC_H


/* StackProc.h -- Specification for Process with stack shared with main() process

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
        January, 1989

Modification History:

log:    STACKPRO.H $
Revision 1.2  95/01/29  13:26:42  NT_Test_Machine
*** empty log message ***

Revision 1.1  92/11/10  12:42:12  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:31  kgorlen
 * Release for 1st edition.
 * 
*/

#ifdef sparc
#include <alloca.h>
#endif
#include <malloc.h>
#include "Process.ho"

class Scheduler;

class StackProc: public Process {
        DECLARE_MEMBERS(StackProc);
public:
        stackTy* stack_top;             // pointer to stack top -- public for MAIN_PROCESS
private:
        stackTy* stack_save;            // stack save area
        unsigned long stack_save_size;  // size of stack save area in words

        StackProc(stackTy* bottom, int priority);       // MAIN process constructor 
        void saveStack();
        friend Scheduler;       // friend void Scheduler::initialize(stackTy* \*bottom\*, int \*priority*\)
public:
        void restoreStack(const stackTy*);      // public so MAIN_PROCESS macro can access
protected:
        StackProc(const char* name, stackTy* bottom, int priority =0, unsigned long size =0);
        virtual void switchContext(Process*);
        virtual void switchFrom(HeapProc*);
        virtual void switchFrom(StackProc*);
        bool FORK() {
                add();          // add this process to Scheduler runlist
                return (_SETJMP(env) == 0);
        }
#ifndef BUG_38
// internal <<AT&T C++ Translator 2.00 06/30/89>> error: bus error (or something nasty like that)
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const      { shouldNotImplement("storer"); }
        virtual void storer(OIOout&) const      { shouldNotImplement("storer"); }
public:
        virtual void deepenShallowCopy()        { shouldNotImplement("deepenShallowCopy"); }
#endif
public:
        ~StackProc();
        virtual unsigned size() const;
};

#endif
