#ifndef EXCEPTION_H
#define EXCEPTION_H


/* Exception.h -- declarations for NIHCL exception handling

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

log:    EXCEPTIO.H $
Revision 1.2  95/01/29  13:27:06  NT_Test_Machine
*** empty log message ***

Revision 1.1  92/11/10  12:17:24  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:31  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"
#include <setjmp.h>

class ExceptionAction : public NIHCL {
public:                 // type definitions
        enum exceptionActionTy { ABORT, RAISE };
private:
        unsigned error_code;
        exceptionActionTy old_action;
public:
        ExceptionAction(unsigned error);
        virtual ~ExceptionAction();
};

class RaiseException : public ExceptionAction {
public:
        RaiseException(unsigned error);
};

class AbortException : public ExceptionAction {
public:
        AbortException(unsigned error);
};

#ifndef NESTED_TYPES
typedef void (*exceptionTrapTy)(unsigned&, int&, ...);
#endif

class ExceptionTrap : public NIHCL {
public:                 // type definitions
#ifdef NESTED_TYPES
        typedef void (*exceptionTrapTy)(unsigned&, int&, ...);
#endif
private:
        exceptionTrapTy old_trap;
public:
        ExceptionTrap(exceptionTrapTy xtrap =NULL);
        ~ExceptionTrap();
};

class Process;
class Catch;

class ExceptionEnv : public NIHCL {
private:                // static member variables
        static ExceptionEnv* stackTop;
        static ExceptionEnv* lastResort;
        friend class Catch;
        friend void RAISE(int exception);
        friend class Process;
private:
        ExceptionEnv* prev;
        int exceptionCode;
        jmp_buf env;
public:
        ExceptionEnv() {        // MUST be inline
                prev = stackTop;
                stackTop = this;
                exceptionCode = setjmp(env);
        }
        ~ExceptionEnv() { if (stackTop == this) pop(); }
        int code() const { return exceptionCode; }
        void pop()      { stackTop = prev; }
        void raise(int exception);
};

#define EXCEPTION_CODE  exception_environment.code()

#define BEGINX { \
        ExceptionEnv exception_environment; \
        if (EXCEPTION_CODE == 0) { \
                
// Statements in the scope of this exception handler block go here.

#define EXCEPTION \
        } \
        else switch(EXCEPTION_CODE) { \

/*
Exception handlers go here; the syntax is that of a switch statement
body.  The exception code that caused this EXCEPTION block to be entered
may be accessed via the macro EXCEPTION_CODE.  The statement
"default:RAISE(EXCEPTION_CODE);" will propagate the current exception up
to the next exception handler block if the exception is not handled by
this block; otherwise, execution continues with the first statement
after this exception block.
*/

#define ENDX \
        }; \
} \

inline void RAISE(int exception)
{
        ExceptionEnv::stackTop->raise(exception);
}

class Catch : public NIHCL {
        static Catch* stackTop; // top of catch frame stack
        friend class Process;
private:
        Catch*  next;
        Catch*  prev;
        Object* obj;
        friend class ExceptionEnv;
public:
        Catch(Object*);
        Catch();                // for construction of catch_stack_top only!
        ~Catch();
};

#endif
