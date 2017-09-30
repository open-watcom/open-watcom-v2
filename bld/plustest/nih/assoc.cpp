/* Assoc.c -- implementation of key-value association

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
        Division of Computer Reearch and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        September, 1985

Function:

Objects of class Assoc associate a key object with a value object.  They
are used to implement Dictionaries, which are Sets of Associations.

log:    ASSOC.C $
Revision 1.1  90/05/20  04:19:02  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:01  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Assoc.h"
#include "nihclIO.h"

#define THIS    Assoc
#define BASE    LookupKey
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(Assoc,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\ASSOC.C 1.1 90/05/20 04:19:02 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

Assoc::Assoc(Object& newKey, Object& newValue) : BASE(newKey)
{
        avalue = &newValue;
}

Object* Assoc::value() { return avalue; }

const Object* Assoc::value() const { return avalue; }

Object* Assoc::value(Object& newvalue)
{
        Object* temp = avalue;
        avalue = &newvalue;
        return temp;
}

void Assoc::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        avalue = avalue->deepCopy();
}

Assoc::Assoc(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
        avalue = Object::readFrom(strm);
}

void Assoc::storer(OIOout& strm) const
{
        BASE::storer(strm);
        avalue->storeOn(strm);
}

Assoc::Assoc(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
        avalue = Object::readFrom(fd);
}

void Assoc::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        avalue->storeOn(fd);
}
