#ifndef RECTANGLE_H
#define RECTANGLE_H


/* Rectangle.h -- declarations for class Rectangle

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

log:    RECTANGL.H $
Revision 1.2  95/01/29  13:27:34  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:02  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:59  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"
#include "Point.h"
#include <math.h>

class Rectangle: public VIRTUAL Object {
        DECLARE_MEMBERS(Rectangle);
        Point tl;               // top left corner (origin)
        Point br;               // bottom right corner (corner)
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Rectangle(int left=0, int top=0, int height=0, int width=0);
        Rectangle(const Point&, const Point&);
        Rectangle(const Rectangle&);
        Point origin() const            { return tl; }
        Point origin(const Point& p)    { return tl = p; }
        Point corner() const            { return br; }
        Point corner(const Point& p)    { return br = p; }
        Point topLeft() const           { return tl; }
        Point topCenter() const         { return Point((br.x()+tl.x())/2,tl.y()); }
        Point topRight() const          { return Point(br.x(),tl.y()); }
        Point rightCenter() const       { return Point(br.x(),(br.y()+tl.y())/2); }
        Point bottomRight() const       { return br; }
        Point bottomCenter() const      { return Point((br.x()+tl.x())/2,br.y()); }
        Point bottomLeft() const        { return Point(tl.x(),br.y()); }
        Point leftCenter() const        { return Point(tl.x(),(br.y()+tl.y())/2); }
        Point center() const            { return Point((br.x()+tl.x())/2,(br.y()+tl.y())/2); }
        Point extent() const            { return Point(br.x()-tl.x(),br.y()-tl.y()); }
        int area() const                { return (br.x()-tl.x())*(br.y()-tl.y()); }
        int width() const               { return br.x()-tl.x(); }
        int height() const              { return br.y()-tl.y(); }
        bool operator==(const Rectangle&) const;
        bool operator!=(const Rectangle& r) const       { return !(*this==r); }
        Rectangle operator&&(const Rectangle&) const;   // intersection 
        Rectangle operator||(const Rectangle&) const;   // union 
        void operator+=(const Point&);                  // translate 
        void operator-=(const Point&);
        bool contains(const Point&) const;
        bool contains(const Rectangle&) const;
        bool intersects(const Rectangle&) const;
        void moveTo(const Point&);
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;      // equality test 
        virtual void printOn(ostream& strm =cout) const;
        virtual const Class* species() const;
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
