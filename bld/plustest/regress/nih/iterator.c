/* Iterator.c -- Implementation of Collection Iterators

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
        December, 1987

Function:

Instances of class Iterator are used to iterate over (i.e. sequence
through) the objects contained in a Collection.  For example:

        OrderedCltn c;
        c.add(*new Point(0,0));
        c.add(*new Point(1,1));
//      ...
        Iterator i(c);
        Object* p;
        while (p = i++) cout << *p;

will print all the Point objects in the OrderedCltn c in the same
order in which they were added.
        
Iterators may be used on any derived class of Collection, and several
Iterators may be active on the same Collection at the same time
without interference.

Iterator is an NIHCL class and implements the usual Object
functionality with the following restriction: deepCopy() and
storeOn() work only for Iterators bound to classes derived from
SeqCltn.

log:    ITERATOR.C $
Revision 1.1  90/05/20  04:19:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:55  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Iterator.h"
#include "Collection.h"
#include "SeqCltn.h"
#include "nihclIO.h"

#define THIS    Iterator
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Iterator,2,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\ITERATOR.C 1.1 90/05/20 04:19:58 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

Iterator::Iterator(const Collection& c)
        : cltn(&c)
{
        index = 0;
        ptr = NULL;
        num = 0;
        state = nil;
        cltn->doReset(*this);
}

void Iterator::reset()
{
        cltn->doReset(*this);
}

Iterator::~Iterator()
{
        cltn->doFinish(*this);
}

Object* Iterator::operator++()
{
        return ptr = cltn->doNext(*this);
}

bool Iterator::operator==(const Iterator& a) const
{
        return cltn->isSame(*a.cltn)
                && index == a.index && num == a.num
                && state->isEqual(*a.state);
}

const Class* Iterator::species() const
{
        return Iterator::desc();
}

int Iterator::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}

bool Iterator::isEqual(const Object& p) const
{
        return p.isSpecies(classDesc) && *this==castdown(p);
}

unsigned Iterator::hash() const
{
        return (const unsigned)cltn ^ index ^ num ^ state->hash();
}

void Iterator::deepenShallowCopy()
{
// Can only deepCopy() Iterators over SeqCltns
// -- not Sets, Bags, Dictionaries, etc.
        cltn->assertClass(*SeqCltn::desc());
        cltn = Collection::castdown(cltn->deepCopy());
        state = state->deepCopy();
        if (ptr) ptr = ptr->deepCopy();
}

void Iterator::dumpOn(ostream& strm) const
{
        strm << className() << '[';
        strm << cltn->className() << '[' << index << "]#" << num;
        if (state != nil) {
                strm << ' ';
                state->dumpOn(strm);
        }
        strm << "]\n";
}

void Iterator::printOn(ostream& strm) const
{
        if (ptr) strm << *ptr;
}

Iterator::Iterator(OIOin& strm)
        : BASE(strm)
{
        bool ptrflag;
        strm >> index >> num >> ptrflag;
        cltn = Collection::readFrom(strm);
        cltn->assertClass(*SeqCltn::desc());
        state = Object::readFrom(strm);
        if (ptrflag) ptr = Object::readFrom(strm);
}

void Iterator::storer(OIOout& strm) const
{
// Can only storeOn() Iterators over SeqCltns
// -- not Sets, Bags, Dictionaries, etc.
        cltn->assertClass(*SeqCltn::desc());
        BASE::storer(strm);
        strm << index << num << (ptr ? YES : NO);
        cltn->storeOn(strm);
        state->storeOn(strm);
        if (ptr) ptr->storeOn(strm);
}

Iterator::Iterator(OIOifd& fd)
        : BASE(fd)
{
        bool ptrflag;
        fd >> index;
        fd >> num;
        fd >> ptrflag;
        cltn = Collection::readFrom(fd);
        cltn->assertClass(*SeqCltn::desc());
        state = Object::readFrom(fd);
        if (ptrflag) ptr = Object::readFrom(fd);
}

void Iterator::storer(OIOofd& fd) const
{
// Can only storeOn() Iterators over SeqCltns
// -- not Sets, Bags, Dictionaries, etc.
        cltn->assertClass(*SeqCltn::desc());
        BASE::storer(fd);
        fd << index;
        fd << num;
        fd << (ptr ? YES : NO);
        cltn->storeOn(fd);
        state->storeOn(fd);
        if (ptr) ptr->storeOn(fd);
}
