#ifndef BAG_H
#define BAG_H


/* Bag.h -- declarations for Set of Objects with possible duplicates

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

log:    BAG.H $
Revision 1.2  95/01/29  13:27:00  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:10  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:08  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Collection.h"
#include "Dictionary.h"

class Bag: public Collection {
        DECLARE_MEMBERS(Bag);
        unsigned count;
        Dictionary contents;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Bag(unsigned size =DEFAULT_CAPACITY);
        Bag(const Bag&);
        ~Bag();
        Object* addWithOccurrences(Object&, unsigned);
        bool operator!=(const Bag& a) const             { return !(*this==a); }
        void operator=(const Bag&);
        bool operator==(const Bag&) const;
        virtual Object* add(Object&);
        virtual Object*& at(int);
        virtual const Object *const& at(int) const;
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();
        virtual Object* doNext(Iterator&) const;
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual unsigned occurrencesOf(const Object&) const;
        virtual void reSize(unsigned);
        virtual Object* remove(const Object&);
        virtual void removeAll();
        virtual unsigned size() const;
        virtual const Class* species() const;
private:                            // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#endif
