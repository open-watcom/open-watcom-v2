#ifndef ORDEREDCLTN_H
#define ORDEREDCLTN_H


/* OrderedCltn.h -- declarations for abstract ordered collections

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

log:    ORDEREDC.H $
Revision 1.2  95/01/29  13:27:30  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:46  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:44  kgorlen
 * Release for 1st edition.
 * 
*/

#include "SeqCltn.h"
#include "ArrayOb.h"

class OrderedCltn: public SeqCltn {
        DECLARE_MEMBERS(OrderedCltn);
        void errEmpty(const char* fn) const;
        void errNotFound(const char* fn, const Object& ob) const;
protected:
        int endIndex;
        ArrayOb contents;
        Object* addAtIndex(int i, Object& ob);
        Object* removeAtIndex(int i);
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        OrderedCltn(unsigned size =DEFAULT_CAPACITY);
#ifndef BUG_TOOBIG
// yacc stack overflow
        OrderedCltn(const OrderedCltn&);
#endif
        bool operator!=(const OrderedCltn& a) const { return !(*this==a); }
        void operator=(const OrderedCltn&);
        bool operator==(const OrderedCltn&) const;
        Object*& operator[](int i) {
                if ((unsigned)i >= endIndex) indexRangeErr();
                return contents[i];
        }
        const Object *const& operator[](int i) const {
                if ((unsigned)i >= endIndex) indexRangeErr();
                return contents[i];
        }
        OrderedCltn operator&(const SeqCltn& cltn) const;       // concatenation operator 
        void operator&=(const SeqCltn& cltn);
        virtual Object* add(Object&);
        virtual Object* addAfter(const Object& ob, Object& newob);
        virtual Object* addAllLast(const OrderedCltn&);
        virtual Object* addBefore(const Object& ob, Object& newob);
        virtual Collection& addContentsTo(Collection& cltn) const;
        virtual Object* addLast(Object& ob);
        virtual Object* after(const Object&) const;
        virtual Object*& at(int i);
        virtual const Object *const& at(int i) const;
        virtual void atAllPut(Object& ob);
        virtual Object* before(const Object&) const;
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();
        virtual Object* first() const;
        virtual unsigned hash() const;
        virtual int indexOf(const Object& ob) const;
        virtual int indexOfSubCollection(const SeqCltn& cltn, int start=0) const;
        virtual bool isEmpty() const;
        virtual Object* last() const;
        virtual unsigned occurrencesOf(const Object&) const;
        virtual Object* remove(const Object&);
        virtual void removeAll();
        virtual Object* removeId(const Object&);
        virtual Object* removeLast();
        virtual void replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt =0);
        virtual void reSize(unsigned newSize);
        virtual unsigned size() const;
        virtual void sort();
};

#endif
