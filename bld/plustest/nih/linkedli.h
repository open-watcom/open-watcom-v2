#ifndef LINKEDLIST_H
#define LINKEDLIST_H


/* LinkedList.h -- declarations for singly-linked list

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

log:    LINKEDLI.H $
Revision 1.2  95/01/29  13:27:18  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  15:23:46  Anthony_Scian
.

 * Revision 3.0  90/05/20  11:23:43  kgorlen
 * Release for 1st edition
 * 
*/

#include "SeqCltn.h"
#include "Link.h"

class LinkedList: public SeqCltn {
        DECLARE_MEMBERS(LinkedList);
private:
        Link* firstLink;                // pointer to first Link of list
        Link* lastLink;                 // pointer to last Link of list
        unsigned count;                 // count of items on list
        void errDblLnk(const char* fn, const Link& lnk) const;
        void errEmpty(const char* fn) const;
        void errNotFound(const char* fn, const Object& ob) const;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
protected:
        virtual Link& linkCastdown(Object&) const;
        Link* linkCastdown(Object* p) const   { return &linkCastdown(*p); }
public:
        LinkedList();
#ifndef BUG_TOOBIG
// yacc stack overflow
        LinkedList(const LinkedList&);
#endif
        bool operator!=(const LinkedList& a) const      { return !(*this==a); }
        bool operator==(const LinkedList&) const;
        Object* operator[](int i);
        const Object *const operator[](int i) const;
        virtual Object* add(Link&);
        virtual Object* add(Object&);
        virtual Object* addAfter(Link&,Link&);
        virtual Object* addAfter(Object&,Object&);
        virtual Collection& addContentsTo(Collection& cltn) const;
        virtual Object* addFirst(Link&);
        virtual Object* addFirst(Object&);
        virtual Object* addLast(Link&);
        virtual Object* addLast(Object&);
        virtual void deepenShallowCopy();
        virtual Object* doNext(Iterator&) const;
        virtual Object* first() const;
        virtual unsigned hash() const;
        virtual bool includes(const Object& ob) const;
        virtual int indexOf(const Object& ob) const;
        virtual bool isEmpty() const;
        virtual bool isEqual(const Object&) const;
        virtual Object* last() const;
        virtual unsigned occurrencesOf(const Object&) const;
        virtual Object* remove(const Link&);
        virtual Object* remove(const Object&);
        virtual void removeAll();
        virtual Object* removeFirst();
        virtual Object* removeLast();
        virtual void reSize(unsigned newSize);
        virtual unsigned size() const;
        virtual const Class* species() const;
private:                                // shouldNotImplement()
        virtual Object*& at(int i);
        virtual const Object *const& at(int i) const;
        virtual void atAllPut(Object& ob);
        virtual int indexOfSubCollection(const SeqCltn& cltn, int start=0) const;
        virtual void replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt =0);
};

#endif
