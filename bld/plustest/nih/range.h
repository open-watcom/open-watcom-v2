#ifndef RANGE_H
#define RANGE_H


/* Range.h -- header file for class Range

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        C. J. Eppich
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

log:    RANGE.H $
Revision 1.2  95/01/29  13:27:32  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:57  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Range: public VIRTUAL Object {
        DECLARE_MEMBERS(Range);
        int first,len;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Range()                 { first = 0; len = -1; }
        Range(int f, int l)     { first = f; len = l; }
        int length() const      { return len; }
        int length(int l)       { return len = l; }
        int firstIndex() const  { return first; }
        int firstIndex(int f)   { return first = f; }
        int lastIndex() const   { return (first + len - 1); }
        int lastIndex(int i)    { len = i - first + 1;  return i; }
        bool valid() const      { return (len >= 0); }
        void operator=(const Range& r)  { first = r.first;  len = r.len; }
        bool operator==(const Range& r) const { return ((first == r.first) && (len == r.len)); }
        bool operator!=(const Range& r) const { return !(*this==r); }
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual const Class* species() const;
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
