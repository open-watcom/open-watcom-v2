#ifndef EXCEPTACT_H
#define EXCEPTACT_H


/* ExceptAct.h -- declarations for exception actions

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

log:    EXCEPTAC.H $
Revision 1.2  95/01/29  13:27:06  NT_Test_Machine
*** empty log message ***

Revision 1.1  92/11/10  12:28:30  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:28  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Exception.h"
#include "nihclerrs.h"

class ExceptionAction;
class Process;

class ExceptionActionTbl : public NIHCL {
private:
        static ExceptionActionTbl* active;
        friend class ExceptionAction;
        friend void NIHCL::setError(int error, int sev, ...);
        friend class Process;
private:
#ifndef NESTED_TYPES
        exceptionTrapTy client_exception_trap;
        exceptionActionTy action[NIHCL__LAST_ERROR-NIHCL__FIRSTERROR+1];
#else
        ExceptionTrap::exceptionTrapTy client_exception_trap;
        ExceptionAction::exceptionActionTy
                action[NIHCL__LAST_ERROR-NIHCL__FIRSTERROR+1];
#endif
        friend class ExceptionAction;
        friend class AbortException;
        friend class RaiseException;
        friend class ExceptionTrap;
public:
        ExceptionActionTbl();
        void act(unsigned error, int sev);
};

#endif
