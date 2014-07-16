/* IdentSet.c -- implementation of Identity Set

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
        April, 1988

Function:
        
An IdentSet is like a Set, except keys are compared using
isSame() rather than isEqual().

log:    IDENTSET.C $
Revision 1.1  90/05/20  04:19:52  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:50  kgorlen
 * Release for 1st edition.
 * 
*/

#include "IdentSet.h"

#define THIS    IdentSet
#define BASE    Set
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(IdentSet,0,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\IDENTSET.C 1.1 90/05/20 04:19:52 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

IdentSet::IdentSet(unsigned size) : BASE(size) {}

int IdentSet::findIndexOf(const Object& ob) const
/*
Search this IdentSet for the argument object.

Enter:
        ob = object to search for

Returns:
        index of object if found or of nil slot if not found
        
Algorithm L, Knuth Vol. 3, p. 519
*/
{
        register int i;
        for (i = h((const int)&ob); contents[i]!=nil; i = (i-1)&mask) {
                if (contents[i]->isSame(ob)) return i;
        }
        return i;
}

IdentSet::IdentSet(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
}

IdentSet::IdentSet(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
}
