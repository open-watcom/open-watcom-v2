/* Collection.c -- implementation of abstract Collection class

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
        September, 1985


Function:
        
Collection is an abstract class that actually implements only the
addAll, removeAll, includes, isEmpty, and Collection conversion
functions.  Note that the functions Collection::asBag, asOrderedCltn,
asSet, and asSortedCltn are defined in the file that implements the
respective target Collection so that all of these classes are not loaded
whenever any one Collection is used.

log:    COLLECTI.C $
Revision 1.1  90/05/20  04:19:20  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:18  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Collection.h"
#include "ArrayOb.h"
#include "nihclIO.h"

#define THIS    Collection
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_ABSTRACT_CLASS(Collection,0,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\COLLECTI.C 1.1 90/05/20 04:19:20 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

const unsigned Collection::DEFAULT_CAPACITY = 16;       // default initial collection capacity 
const unsigned Collection::EXPANSION_INCREMENT = 32;    // collection (OrderedCltn) expansion increment 
const unsigned Collection::EXPANSION_FACTOR = 2;        // collection (Set,Bag,Dictionary) expansion factor 

Collection::Collection() {}

Collection::~Collection() {}

Collection::Collection(OIOifd& fd) : BASE(fd) {}

Collection::Collection(OIOin& strm) : BASE(strm) {}

const Collection& Collection::addAll(const Collection& c)
{
        c.addContentsTo(*this);
        return c;
}

Collection& Collection::addContentsTo(Collection& c) const
{
        DO(*this,Object,o) c.add(*o); OD
        return c;
}

const Collection& Collection::removeAll(const Collection& c)
{
        DO(c,Object,o) remove(*o); OD
        return c;
}

void Collection::deepenShallowCopy() {}

bool Collection::includes(const Object& ob) const { return occurrencesOf(ob) != 0; }

bool Collection::isEmpty() const { return size() == 0; }

ArrayOb Collection::asArrayOb() const
{
        ArrayOb a(size());
        Object** q = &(a.elem(0));
        DO(*this,Object,o) *q++ = o; OD
        return a;
}
 
void Collection::doFinish(Iterator& /*pos*/) const {}

void Collection::doReset(Iterator& pos) const
{
        pos.index = 0;
        pos.ptr = NULL;
        pos.num = 0;
}

void Collection::dumpOn(ostream& strm) const
{
        strm << className() << '[';
        DO(*this,Object,o) o->dumpOn(strm); OD
        strm << "]\n";
}

void Collection::printOn(ostream& strm) const
{
        int n = 0;
        DO(*this,Object,o)
                if (n++ != 0) strm << '\n';
                o->printOn(strm);
        OD
}

void Collection::_storer(OIOout& strm) const
{
        strm << capacity() << size();
        DO(*this,Object,o) o->storeOn(strm); OD
}

void Collection::_storer(OIOofd& fd) const
{
        fd << capacity() << size();
        DO(*this,Object,ob) ob->storeOn(fd); OD
}
