#ifndef HEAPPROC_H
#define HEAPPROC_H


/* HeapProc.h -- Specification for Process with stack on free storage heap

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

log:    HEAPPROC.H $
Revision 1.2  95/01/29  13:27:10  NT_Test_Machine
*** empty log message ***

Revision 1.1  92/11/10  12:42:16  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:47  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Process.ho"

class HeapProc: public Process {
        DECLARE_MEMBERS(HeapProc);
        unsigned long stack_size;       // size of stack in words
        void setupStack();
protected:
        HeapProc(const char* name, stackTy* bottom, int priority =0, unsigned long size =1024);
        virtual void switchContext(Process*);
        virtual void switchFrom(HeapProc*);
        virtual void switchFrom(StackProc*);
        bool FORK() {
                add();          // add this process to Scheduler runlist
                if (_SETJMP(env) == 0) {
                        setupStack();
                        return YES;
                }
                else return NO;
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
        ~HeapProc();
        virtual unsigned size() const;
};

#endif
