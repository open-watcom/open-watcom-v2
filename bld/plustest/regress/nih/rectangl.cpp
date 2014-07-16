/* Rectangle.c -- implementation of class Rectangle

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
        
A Rectangle is defined by two points: an origin, which specifies the top
left corner, and corner, which specifies the bottom right corner.

log:    RECTANGL.C $
Revision 1.1  90/05/20  04:21:00  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:58  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Rectangle.h"
#include "nihclIO.h"

#define THIS    Rectangle
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES Point::desc(),Point::desc()
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Rectangle,2,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\RECTANGL.C 1.1 90/05/20 04:21:00 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

Rectangle::Rectangle(int left, int top, int height, int width)
{
        tl = Point(left,top);
        br = Point(left+width,top+height);
}

Rectangle::Rectangle(const Point& o, const Point& c)
{
        tl = o;
        br = c;
}

Rectangle::Rectangle(const Rectangle& r) : tl(r.tl), br(r.br) {}

bool Rectangle::operator==(const Rectangle& r) const
{
        return (tl==r.tl && br==r.br);
}

Rectangle Rectangle::operator&&(const Rectangle& r) const
{
        return Rectangle(tl.max(r.tl), br.min(r.br));
}

Rectangle Rectangle::operator||(const Rectangle& r) const
{
        return Rectangle(tl.min(r.tl),
                br.max(r.br));
}

void Rectangle::operator+=(const Point& p)
{
        tl += p;
        br += p;
}

void Rectangle::operator-=(const Point& p)
{
        tl -= p;
        br -= p;
}

bool Rectangle::contains(const Point& p) const
{
        return (tl <= p) && (p <= br);
}

bool Rectangle::contains(const Rectangle& r) const
{
        return (contains(r.tl) && contains(r.br));
}

bool Rectangle::intersects(const Rectangle& r) const
{
        if (tl.max(r.tl) < br.min(r.br)) return YES;
        return NO;
}

void Rectangle::moveTo(const Point& p)
/*
        Move this Rectangle so its origin is at p.
*/
{
        br += p-tl;
        tl = p;
}

void Rectangle::deepenShallowCopy()     {}

unsigned Rectangle::hash() const        { return tl.hash()^br.hash(); }

bool Rectangle::isEqual(const Object& r) const
{
        return r.isSpecies(classDesc) && *this==castdown(r);
}

const Class* Rectangle::species() const { return &classDesc; }

void Rectangle::printOn(ostream& strm) const
{
        strm << tl << " corner: " << br;
}

Rectangle::Rectangle(OIOin& strm)
        : BASE(strm),
        tl(strm), br(strm)
{
}

void Rectangle::storer(OIOout& strm) const
{
        BASE::storer(strm);
        tl.storeMemberOn(strm);
        br.storeMemberOn(strm);
}

Rectangle::Rectangle(OIOifd& fd)
        : BASE(fd),
        tl(fd), br(fd)
{
}

void Rectangle::storer(OIOofd& fd) const 
{
        BASE::storer(fd);
        tl.storeMemberOn(fd);
        br.storeMemberOn(fd);
}

int Rectangle::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
