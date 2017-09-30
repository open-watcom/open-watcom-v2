#ifndef SEMAPHORE_H
#define SEMAPHORE_H


/* Semaphore.h -- declarations for Semaphore

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

log:    SEMAPHOR.H $
Revision 1.2  95/01/29  13:27:34  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  17:37:32  Anthony_Scian
.

 * Revision 3.0  90/05/20  13:37:16  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"
#include "LinkedList.h"

class Process;

class Semaphore: public VIRTUAL Object {
        DECLARE_MEMBERS(Semaphore);
        LinkedList waitList;
        short count;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Semaphore(int initialCount =0);
#ifndef BUG_TOOBIG
// yacc stack overflow
        Semaphore(const Semaphore&);
#endif
        ~Semaphore();
        virtual Object* copy() const;           // deepCopy()
        virtual void deepenShallowCopy();
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool includes(const Process&) const;
        virtual bool isEqual(const Object& ob) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual Process* remove(Process&);
        virtual Process* removeFirst();
        virtual Process* removeLast();
        virtual void signal(unsigned n=1);  // signal waiting process 
        virtual int value() const;      // read semaphore value 
        virtual void wait();            // wait on semaphore 
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
