#ifndef BITSET_H
#define BITSET_H


/* BitSet.h -- declarations for set of small integers

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

log:    BITSET.H $
Revision 1.2  95/01/29  13:27:02  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:16  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:14  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Bitset: public VIRTUAL Object {
        DECLARE_MEMBERS(Bitset);
public:
        static unsigned bit(unsigned i) { return 1<<i; }
protected:
        unsigned m;
        Bitset(unsigned i, char* dum)   { (dum, m = i); }
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Bitset()                { m = 0; }
        Bitset(int i1)          { m = bit(i1); }
        Bitset(int i1, int i2)
                { m = bit(i1)|bit(i2); }
        Bitset(int i1, int i2, int i3)
                { m = bit(i1)|bit(i2)|bit(i3); }
        Bitset(int i1, int i2, int i3, int i4)
                { m = bit(i1)|bit(i2)|bit(i3)|bit(i4); }
        Bitset(int i1, int i2, int i3, int i4, int i5)
                { m = bit(i1)|bit(i2)|bit(i3)|bit(i4)|bit(i5); }
        Bitset(int i1, int i2, int i3, int i4, int i5, int i6)
                { m = bit(i1)|bit(i2)|bit(i3)|bit(i4)|bit(i5)|bit(i6); }
        Bitset(int i1, int i2, int i3, int i4, int i5, int i6, int i7)
                { m = bit(i1)|bit(i2)|bit(i3)|bit(i4)|bit(i5)|bit(i6)|bit(i7); }
        Bitset operator~() const                { return Bitset(~m, ""); }
        Bitset operator-(const Bitset& n) const { return Bitset(m & ~n.m, ""); }
        bool operator>(const Bitset& n) const   { return m == (m|n.m) && m != n.m; }
        bool operator<(const Bitset& n) const   { return n.m == (m|n.m) && m != n.m; }
        bool operator>=(const Bitset& n) const  { return m == (m|n.m); }
        bool operator<=(const Bitset& n) const  { return n.m == (m|n.m); }
        bool operator==(const Bitset& n) const  { return m == n.m; }
        bool operator!=(const Bitset& n) const  { return m != n.m; }
        Bitset operator&(const Bitset& n) const { return Bitset(m & n.m, ""); }
        Bitset operator^(const Bitset& n) const { return Bitset(m ^ n.m, ""); }
        Bitset operator|(const Bitset& n) const { return Bitset(m | n.m, ""); }
        void operator=(const Bitset& n)         { m = n.m; }
        void operator-=(const Bitset& n)        { m &= ~n.m; }
        void operator&=(const Bitset& n)        { m &= n.m; }
        void operator^=(const Bitset& n)        { m ^= n.m; }
        void operator|=(const Bitset& n)        { m |= n.m; }
        unsigned asMask() const         { return m; }
        bool includes(unsigned i) const { return (m & bit(i)) != 0; }
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual bool isEmpty() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual unsigned size() const;
        virtual const Class* species() const;
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
