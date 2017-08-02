/* Point.c  -- implementation of X-Y coordinates

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
        August, 1985

Function:
        
A Point represents an x-y coordinate pair.  By convention, Point(0,0) is
the top left corner of the display, with x increasing to the right and y
increasing to the bottom.

log:    POINT.C $
Revision 1.1  90/05/20  04:20:48  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:45  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Point.h"
#include "nihclIO.h"

#define THIS    Point
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Point,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\POINT.C 1.1 90/05/20 04:20:48 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

Point Point::max(const Point& p) const
{
        return Point(MAX(xc,p.xc),MAX(yc,p.yc));
}

Point Point::min(const Point& p) const
{
        return Point(MIN(xc,p.xc),MIN(yc,p.yc));
}

bool Point::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* Point::species() const     { return &classDesc; }

unsigned Point::hash() const    { return xc^yc; }

int Point::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        const Point& p = castdown(ob);
        int t = yc - p.yc;;
        if (t != 0) return t;
        else return xc - p.xc;
}

void Point::deepenShallowCopy() {}

void Point::printOn(ostream& strm) const
{
        strm << '(' << xc << ',' << yc << ')';
}

Point::Point(OIOin& strm)
        : BASE(strm)
{
        strm >> xc >> yc;
}

void Point::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << xc << yc;
}

Point::Point(OIOifd& fd)
        : BASE(fd)
{
        fd >> xc >> yc;
}

void Point::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << xc << yc;
}
