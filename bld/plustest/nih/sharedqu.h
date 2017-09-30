#ifndef SHAREDQUEUE_H
#define SHAREDQUEUE_H


/* SharedQueue.h -- declarations for shared queues

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

log:    SHAREDQU.H $
Revision 1.2  95/01/29  13:27:36  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:24  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:23  kgorlen
 * Release for 1st edition.
 * 
*/

#include "ArrayOb.h"
#include "Semaphore.h"

class SharedQueue: public VIRTUAL Object {
        DECLARE_MEMBERS(SharedQueue);
        ArrayOb queue;
        int readPosition,writePosition;
        Semaphore valueAvailable;
        Semaphore spaceAvailable;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        SharedQueue(unsigned queueSize =Collection::DEFAULT_CAPACITY);
        SharedQueue(const SharedQueue&);
        bool isFull() const     { return spaceAvailable.value() == 0; }
        virtual unsigned capacity() const;
        virtual Object* copy() const;   // deepCopy()
        virtual void deepenShallowCopy();
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool isEmpty() const;
        virtual bool isEqual(const Object&) const;
        virtual Object* next();
        virtual Object* nextPut(Object&);
        virtual void printOn(ostream& strm =cout) const;
        virtual unsigned size() const;
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
