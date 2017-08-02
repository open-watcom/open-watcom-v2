/* SharedQueue.c -- implementation of shared queues

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Bg. 12A, Rm. 2033
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        December, 1985

Function:

Class SharedQueue provides a queue that can be used for communication
between two separate processes.  Semaphores are used to block a process
if next() is called and there are no objects in the queue, or if
nextPut() is called and there is no space in the queue.

log:    SHAREDQU.C $
Revision 1.1  90/05/20  04:21:22  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:20  kgorlen
 * Release for 1st edition.
 * 
*/

#include "SharedQueue.h"
#include "nihclIO.h"

#define THIS    SharedQueue
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES ArrayOb::desc(),Semaphore::desc(),Semaphore::desc()
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(SharedQueue,2,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\SHAREDQU.C 1.1 90/05/20 04:21:22 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

SharedQueue::SharedQueue(unsigned queueSize)
:
        queue(queueSize),
        valueAvailable(0),
        spaceAvailable(queueSize)
{
        readPosition = writePosition = 0;
}

SharedQueue::SharedQueue(const SharedQueue& q)
:
        queue(q.queue),
        valueAvailable(q.valueAvailable),
        spaceAvailable(q.spaceAvailable)
{
        readPosition = q.readPosition;
        writePosition = q.writePosition;
}

Object* SharedQueue::next()
{
        valueAvailable.wait();
        Object* ob = queue[readPosition];
        queue[readPosition++] = nil;
        if (readPosition >= queue.capacity()) readPosition = 0;
        spaceAvailable.signal();
        return ob;
}

Object* SharedQueue::nextPut(Object& ob)
{
        spaceAvailable.wait();
        queue[writePosition++] = &ob;
        if (writePosition >= queue.capacity()) writePosition = 0;
        valueAvailable.signal();
        return &ob;
}

unsigned SharedQueue::capacity() const  { return queue.capacity(); }

bool SharedQueue::isEmpty() const       { return valueAvailable.value() <= 0; }

unsigned SharedQueue::size() const      { return MAX(valueAvailable.value(),0); }

Object* SharedQueue::copy() const       { return deepCopy(); }

void SharedQueue::deepenShallowCopy()
{
        queue.deepenShallowCopy();
        valueAvailable.deepenShallowCopy();
        spaceAvailable.deepenShallowCopy();
}

void SharedQueue::dumpOn(ostream& strm) const
{
        strm << className() << "[\n";
        strm << "valueAvailable ";  valueAvailable.dumpOn(strm);
        strm << "spaceAvailable ";  spaceAvailable.dumpOn(strm);
        strm << "queue:\n";
        int i = readPosition;
        for (int n = valueAvailable.value(); n>0; n--) {
                queue[i++]->dumpOn(strm);
                if (i == queue.capacity()) i = 0;
        }
        strm << "]\n";
}

unsigned SharedQueue::hash() const      { return (unsigned)this; }

bool SharedQueue::isEqual(const Object& ob) const   { return isSame(ob); }

void SharedQueue::printOn(ostream& strm) const
{
        int i = readPosition;
        for (int n = valueAvailable.value(); n>0; n--) {
                if (i != readPosition) strm << '\n';
                queue[i++]->printOn(strm);
                if (i == queue.capacity()) i = 0;
        }
}

static unsigned sharedqueue_capacity;

SharedQueue::SharedQueue(OIOin& strm)
        : BASE(strm),
        queue((strm >> sharedqueue_capacity, sharedqueue_capacity)),
        valueAvailable(strm),
        spaceAvailable(strm)
{
        readPosition = 0;
        writePosition = valueAvailable.value();
        for (register int i =0; i<writePosition; i++) queue[i] = Object::readFrom(strm);
}

void SharedQueue::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << queue.capacity();
        valueAvailable.storeMemberOn(strm);
        spaceAvailable.storeMemberOn(strm);
        register int i = readPosition;
        while (i != writePosition) {
                queue[i++]->storeOn(strm);
                if (i == queue.capacity()) i = 0;
        }
}

SharedQueue::SharedQueue(OIOifd& fd)
        : BASE(fd),
        queue((fd >> sharedqueue_capacity, sharedqueue_capacity)),
        valueAvailable(fd),
        spaceAvailable(fd)
{
        readPosition = 0;
        writePosition = valueAvailable.value();
        for (register int i=0; i < writePosition; i++)
          queue[i] = Object::readFrom(fd);
}

void SharedQueue::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << queue.capacity();
        valueAvailable.storeMemberOn(fd);
        spaceAvailable.storeMemberOn(fd);
        register int i = readPosition;
        while (i != writePosition) {
          queue[i++]->storeOn(fd);
          if (i == queue.capacity()) i = 0;
          };
}

int SharedQueue::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
