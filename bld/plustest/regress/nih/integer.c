/* Integer.c -- implementation of Integer object

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
        
Provides an object that contains an int.

log:    INTEGER.C $
Revision 1.1  90/05/20  04:19:54  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:53  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Integer.h"
#include "nihclIO.h"

#define THIS    Integer
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Integer,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\INTEGER.C 1.1 90/05/20 04:19:54 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

Integer::Integer(istream& strm)         { parseInteger(strm); }

unsigned Integer::hash() const { return val; }

bool Integer::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && val==castdown(ob).val;
}

const Class* Integer::species() const { return &classDesc; }

int Integer::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        long t = val-castdown(ob).val;
        if (sizeof(long) > sizeof(int)) {
                if (t < 0) return -1;
                return (t > 0);
        }
        return t;
}

void Integer::deepenShallowCopy()       {}

void Integer::printOn(ostream& strm) const
{
        strm << val;
}

void Integer::scanFrom(istream& strm)   { parseInteger(strm); }

Integer::Integer(OIOin& strm)
        : BASE(strm)
{
        strm >> val;
}

void Integer::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << val;
}

Integer::Integer(OIOifd& fd)
        : BASE(fd)
{
        fd >> val;
}

void Integer::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << val;
}
