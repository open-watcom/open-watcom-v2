/* Float.c -- implementation of Float object

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
        
Provides an object that contains a double.

log:    FLOAT.C $
Revision 1.1  90/05/20  04:19:38  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:36  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Float.h"
#include "nihclIO.h"

#define THIS    Float
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Float,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\FLOAT.C 1.1 90/05/20 04:19:38 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

Float::Float(istream& strm)     { parseFloat(strm); }

int Float::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        register double t = val-castdown(ob).val;
        if (t < 0) return -1;
        if (t > 0) return 1;
        return 0;
}

void Float::deepenShallowCopy() {}

unsigned Float::hash() const
{
        union {
                unsigned asint[2];
                double asdouble;
        };
        asdouble = val;
        return asint[0] ^ asint[1];
}

bool Float::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && val==castdown(ob).val;
}

const Class* Float::species() const { return &classDesc; }

void Float::printOn(ostream& strm) const
{
        strm << val;
}

void Float::scanFrom(istream& strm)     { parseFloat(strm); }

Float::Float(OIOin& strm)
        : BASE(strm)
{
        strm >> val;
}

void Float::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << val;
}

Float::Float(OIOifd& fd)
        : BASE(fd)
{
        fd >> val;
}

void Float::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << val;
}
