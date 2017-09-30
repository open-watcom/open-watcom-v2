#ifndef SORTEDCLTN_H
#define SORTEDCLTN_H


/* SortedCltn.h -- declarations for sorted collection

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

log:    SORTEDCL.H $
Revision 1.2  95/01/29  13:27:38  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:28  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:26  kgorlen
 * Release for 1st edition.
 * 
*/

#include "OrderedCltn.h"
#include "Range.h"

class SortedCltn: public OrderedCltn {
        DECLARE_MEMBERS(SortedCltn);
#ifndef BUG_38
// internal <<AT&T C++ Translator 2.00 06/30/89>> error: bus error (or something nasty like that)
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd& fd) const   { OrderedCltn::storer(fd); };
        virtual void storer(OIOout& strm) const { OrderedCltn::storer(strm); };
#endif
public:
        SortedCltn(unsigned size =DEFAULT_CAPACITY);
        bool operator!=(const SortedCltn& a) const      { return !(*this==a); }
        void operator=(const SortedCltn&);
        virtual Object* add(Object&);
        int findIndexOf(const Object& key) const;
        virtual unsigned occurrencesOf(const Object&) const;
        Range findRangeOf(const Object& key) const;
        Object* remove(const Object&);
protected:
        int findIndexOfFirstKey(const Object&, const int) const;
        int findIndexOfLastKey(const Object&, const int) const;
private:                                // shouldNotImplement()
        virtual Object* addAfter(const Object& ob, Object& newob);
        virtual Object* addAllLast(const OrderedCltn&);
        virtual Object* addBefore(const Object& ob, Object& newob);
        virtual Object* addLast(Object& ob);
        virtual void atAllPut(Object& ob);
        virtual int indexOfSubCollection(const SeqCltn& cltn, int start=0) const;
        virtual void replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt =0);
        virtual void sort();
};

#endif
