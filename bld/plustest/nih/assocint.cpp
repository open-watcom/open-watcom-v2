/* AssocInt.c -- implementation of key-Integer association

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

Objects of class AssocInt associate a key object with an Integer value
object.  They are used to implement Bags, which use a Dictionary to
associate objects with their occurrence counts.

log:    ASSOCINT.C $
Revision 1.1  90/05/20  04:19:06  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:04  kgorlen
 * Release for 1st edition.
 * 
*/

#include "AssocInt.h"
#include "nihclIO.h"

#define THIS    AssocInt
#define BASE    LookupKey
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES Integer::desc()
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(AssocInt,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\ASSOCINT.C 1.1 90/05/20 04:19:06 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

AssocInt::AssocInt(Object& newKey, int newValue)
        : BASE(newKey), avalue(newValue)
{
}

Object* AssocInt::value() { return &avalue; }

const Object* AssocInt::value() const { return &avalue; }

Object* AssocInt::value(Object& newValue)
{
        assertArgClass(newValue,*Integer::desc(),"value");
        avalue = Integer::castdown(newValue);
        return &avalue;
}

void AssocInt::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        avalue.deepenShallowCopy();
}

static int intval;

AssocInt::AssocInt(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm),
        avalue((strm >> intval, intval))
{
}

void AssocInt::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << avalue.value();
}

AssocInt::AssocInt(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd),
        avalue((fd >> intval, intval))
{
}

void AssocInt::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << avalue.value();
}
