/* LinkOb.c -- implementation of singly-linked list Object pointer element

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
        October, 1985

Function:

Objects of class LinkOb are used to link Objects into a LinkedList.

log:    LINKOB.C $
Revision 1.1  90/05/20  04:20:06  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:03  kgorlen
 * Release for 1st edition.
 * 
*/

#include "LinkOb.h"
#include "nihclIO.h"

#define THIS    LinkOb
#define BASE    Link
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(LinkOb,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\LINKOB.C 1.1 90/05/20 04:20:06 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

LinkOb::LinkOb(Object& newval)
{
        val = &newval;
}

unsigned LinkOb::capacity() const               { return val->capacity(); }

int LinkOb::compare(const Object& ob) const     { return ob.compare(*val); }

void LinkOb::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        val = val->deepCopy();
}

unsigned LinkOb::hash() const                   { return val->hash(); }

bool LinkOb::isEqual(const Object& ob) const    { return ob.isEqual(*val); }

void LinkOb::dumpOn(ostream& strm) const
{
        val->dumpOn(strm);
}

void LinkOb::printOn(ostream& strm) const
{
        val->printOn(strm);
}

unsigned LinkOb::size() const                   { return val->size(); }

Object* LinkOb::value() const   { return val; }

Object* LinkOb::value(Object& newval)
{
        Object* temp = val;
        val = &newval;
        return temp;
}

LinkOb::LinkOb(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
        val = Object::readFrom(strm);
}

void LinkOb::storer(OIOout& strm) const
{
        BASE::storer(strm);
        val->storeOn(strm);
}

LinkOb::LinkOb(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
        val = Object::readFrom(fd);
}

void LinkOb::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        val->storeOn(fd);
}
