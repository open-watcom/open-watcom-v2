#ifndef ITERATOR_H
#define ITERATOR_H


/* Iterator.h -- Declarations for Collection Iterators

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
        December, 1987

log:    ITERATOR.H $
Revision 1.2  95/01/29  13:27:14  NT_Test_Machine
*** empty log message ***

Revision 1.1  92/11/10  10:56:46  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:56  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Collection;

class Iterator: public VIRTUAL Object {
        DECLARE_MEMBERS(Iterator);
        const Collection* cltn; // Collection being iterated over
public:
        int index;              // index of next Object
        Object* ptr;            // pointer to current Object or NULL
        unsigned num;           // object number, used by Bags
        Object* state;          // additional state, e.g., a Stack
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Iterator(const Collection&);
        ~Iterator();
        void reset();                   // reset to beginning of Collection
        Object* operator()() const { return ptr; }      // return current object pointer
        Object* operator++();           // advance to next object in Collection
        /* WATCOM */ Object* operator++(int){return operator++();}
        const Collection* collection() const    { return cltn; }
        bool operator==(const Iterator&) const;
        bool operator!=(const Iterator& a) const { return !(*this==a); }
        virtual void deepenShallowCopy();       // copy with cltn->deepCopy()
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual const Class* species() const;
private:                                // shouldNotImplement()
        virtual int compare(const Object&) const;
};

#define DO(cltn,cls,arg)\
{ cls* arg; Iterator DO_pos(cltn); while (arg = cls::castdown(DO_pos++)) {
#define OD }}

#endif
