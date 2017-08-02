#ifndef ARRAYOB_H
#define ARRAYOB_H


/* ArrayOb.h -- declarations for array of object pointers

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

log:    ARRAYOB.H $
Revision 1.2  95/01/29  13:26:50  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:18:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:18:56  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Collection.h"

class ArrayOb: public Collection {
        DECLARE_MEMBERS(ArrayOb);
        Object** v;
        unsigned sz;
        void allocSizeErr() const;
        void indexRangeErr() const;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        ArrayOb(unsigned size =DEFAULT_CAPACITY);
        ArrayOb(const ArrayOb&);
        ~ArrayOb();
        Object*& elem(int i)                    { return v[i]; }
        const Object *const& elem(int i) const  { return v[i]; }
        bool operator!=(const ArrayOb& a) const { return !(*this==a); }
        void operator=(const ArrayOb&);
        bool operator==(const ArrayOb&) const;
        Object*& operator[](int i)      {
                if ((unsigned)i >= sz) indexRangeErr();
                return v[i];
        }
        const Object *const& operator[](int i) const    {
                if ((unsigned)i >= sz) indexRangeErr();
                return v[i];
        }
        virtual Collection& addContentsTo(Collection&) const;
        virtual Object*& at(int i);
        virtual const Object *const& at(int i) const;
        virtual unsigned capacity() const;
        virtual int compare(const Object&) const;
        virtual void deepenShallowCopy();
        virtual Object* doNext(Iterator&) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void reSize(unsigned);
        virtual void removeAll();
        virtual unsigned size() const;
        virtual void sort();
        virtual const Class* species() const;
private:                            // shouldNotImplement();
        virtual Object* add(Object&);
        virtual unsigned occurrencesOf(const Object&) const;
        virtual Object* remove(const Object&);
};

#endif
