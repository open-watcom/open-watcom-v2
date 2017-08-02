#ifndef SET_H
#define SET_H


/* Set.h -- declarations for hash tables

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

log:    SET.H $
Revision 1.2  95/01/29  13:27:36  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:21:20  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:18  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Collection.h"
#include "ArrayOb.h"

class Set: public Collection {
        DECLARE_MEMBERS(Set);
        unsigned count;         // number of objects in set 
        unsigned nbits;         // log base 2 of contents.capacity() 
protected:
        unsigned mask;          // contents.capacity()-1 
        ArrayOb contents;       // array of set objects 
        unsigned setCapacity(unsigned); // compute set allocation size 
        int h(unsigned long) const;     // convert hash key into contents index 
        virtual int findIndexOf(const Object&) const;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Set(unsigned size =DEFAULT_CAPACITY);
        bool operator==(const Set&) const;
        bool operator!=(const Set& a) const     { return !(*this==a); }
        Set operator&(const Set&) const;        // intersection 
        Set operator|(const Set&) const;        // union 
        Set operator-(const Set&) const;        // difference 
        virtual Object* add(Object&);
        virtual Object*& at(int);
        virtual const Object *const& at(int) const;
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();
        virtual Object* doNext(Iterator&) const;
        virtual Object* findObjectWithKey(const Object&) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual unsigned occurrencesOf(const Object&) const;
        virtual void reSize(unsigned);
        virtual Object* remove(const Object&);
        virtual void removeAll();
        virtual unsigned size() const;
        virtual const Class* species() const;
private:                                // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
