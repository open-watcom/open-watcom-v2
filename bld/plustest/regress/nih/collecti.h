#ifndef COLLECTION_H
#define COLLECTION_H


/* Collection.h -- declarations for abstract Collection class

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

log:    COLLECTI.H $
Revision 1.2  95/01/29  13:27:02  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:24  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:21  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class ArrayOb;
class Bag;
class Heap;
class Iterator;
class OrderedCltn;
class Set;
class SortedCltn;

class Collection: public VIRTUAL Object {       // abstract class 
        DECLARE_MEMBERS(Collection);
public:
        static const unsigned DEFAULT_CAPACITY;         // default initial collection capacity 
        static const unsigned EXPANSION_INCREMENT;      // collection (OrderedCltn) expansion increment 
        static const unsigned EXPANSION_FACTOR;         // collection (Set,Bag,Dictionary) expansion factor 
protected:
        Collection();
protected:              // _storer() functions for object I/O
        void _storer(OIOofd&) const;            // store collection using Iterator
        void _storer(OIOout&) const;            // store collection using Iterator
public:
        ArrayOb asArrayOb() const;
        Bag asBag() const;
        Heap asHeap() const;
        OrderedCltn asOrderedCltn() const;
        Set asSet() const;
        SortedCltn asSortedCltn() const;
        virtual ~Collection();          // Collection destructors are virtual
        virtual Object* add(Object&) = 0;
        virtual const Collection& addAll(const Collection&);
        virtual Collection& addContentsTo(Collection&) const;
        virtual Object*& at(int) = 0;
        virtual const Object *const& at(int) const = 0;
        virtual int compare(const Object&) const = 0;
        virtual void deepenShallowCopy();   // {}
        virtual void doFinish(Iterator& pos) const;
        virtual Object* doNext(Iterator&) const = 0;
        virtual void doReset(Iterator& pos) const;
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const = 0;
        virtual bool includes(const Object&) const;
        virtual bool isEmpty() const;
        virtual bool isEqual(const Object&) const = 0;
        virtual unsigned occurrencesOf(const Object&) const = 0;
        virtual void printOn(ostream& strm =cout) const;
        virtual Object* remove(const Object&) = 0;
        virtual void removeAll() = 0;
        virtual const Collection& removeAll(const Collection&);
        virtual unsigned size() const = 0;
};

#include "Iterator.h"

#endif
