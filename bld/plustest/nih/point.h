#ifndef POINT_H
#define POINT_H


/* Point.h -- declarations for X-Y coordinates

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

log:    POINT.H $
Revision 1.2  95/01/29  13:27:32  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:50  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:47  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"
#include <math.h>

class Point: public VIRTUAL Object {
        DECLARE_MEMBERS(Point);
protected:
        short xc,yc;                    // x-y coordinate 
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Point()                         { xc = yc = 0; }
        Point(short newx, short newy)   { xc=newx; yc=newy; }
        short x() const                 { return xc; }
        short x(short newx)             { return xc = newx; }
        short y() const                 { return yc; }
        short y(short newy)             { return yc = newy; }
        Point operator+(const Point& p) const   { return Point(xc+p.xc, yc+p.yc); }
        Point operator-() const                 { return Point(-xc,-yc); }
        Point operator-(const Point& p) const   { return Point(xc-p.xc, yc-p.yc); }
        friend Point operator*(const Point& p, int i) { return Point(i*p.xc, i*p.yc); }
        friend Point operator*(int i, const Point& p) { return Point(i*p.xc, i*p.yc); }
        int operator*(const Point& p) const     { return xc*p.xc + yc*p.yc; }
        bool operator==(const Point& p) const   { return (xc==p.xc && yc==p.yc); }
        bool operator!=(const Point& p) const   { return (xc!=p.xc || yc!=p.yc); }
        bool operator<(const Point& p) const    { return (yc<p.yc && xc<p.xc); }
        bool operator<=(const Point& p) const   { return (yc<=p.yc && xc<=p.xc); }
        bool operator>(const Point& p) const    { return (yc>p.yc && xc>p.xc); }
        bool operator>=(const Point& p) const   { return (yc>=p.yc && xc>=p.xc); }
        void operator+=(const Point& p)         { xc += p.xc; yc += p.yc; }
        void operator-=(const Point& p)         { xc -= p.xc; yc -= p.yc; }
        void operator*=(int s)                  { xc *= s; yc *= s; }
        double dist(const Point& p) const       { return hypot(xc-p.xc, yc-p.yc); }
        Point max(const Point&) const;
        Point min(const Point&) const;
        Point transpose() const                 { return Point(yc,xc); }
        bool isBelow(const Point& p) const      { return yc > p.yc; }
        bool isAbove(const Point& p) const      { return yc < p.yc; }
        bool isLeft(const Point& p) const       { return xc < p.xc; }
        bool isRight(const Point& p) const      { return xc > p.xc; }
        virtual int compare(const Object&) const;       // compare Points 
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;      // equality test 
        virtual void printOn(ostream& strm =cout) const;
        virtual const Class* species() const;
};

#endif
