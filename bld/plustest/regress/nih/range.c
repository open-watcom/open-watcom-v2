/* Range.c  -- implementation of NIHCL class Range

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        C. J. Eppich
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-5361
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        September, 1987

Function:

Class Range implements an ordered pair of ints that can be used to indicate
a segment of some array (possibly a character string or vector).

log:    RANGE.C $
Revision 1.1  90/05/20  04:20:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:55  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Range.h"
#include "nihclIO.h"

#define THIS    Range
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Range,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\RANGE.C 1.1 90/05/20 04:20:58 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

//======= Protected member functions:

Range::Range(OIOin& strm)
        : BASE(strm)
{
        strm >> first >> len;
}

void Range::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << first << len;
}

Range::Range(OIOifd& fd)
        : BASE(fd)
{
        fd >> first >> len;
}

void Range::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << first << len;
}


//======= Public member functions:

void Range::deepenShallowCopy()  {}

unsigned Range::hash() const
{
        return (first^len);
}

bool Range::isEqual(const Object& p) const
// Test two objects for equality
{
        return p.isSpecies(classDesc) && *this==castdown(p);
}

void Range::printOn(ostream& strm) const
{
        strm << first << ':' << len;
}

const Class* Range::species() const
// Return a pointer to the descriptor of the species of this class
{
        return Range::desc();
}

int Range::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
