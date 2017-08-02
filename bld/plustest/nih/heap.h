#ifndef HEAP_H
#define HEAP_H


/* Heap.h -- declarations for abstract heap          

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

log:    HEAP.H $
Revision 1.2  95/01/29  13:27:10  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:46  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:43  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Collection.h"
#include "OrderedCltn.h"
#include "ArrayOb.h"

class Heap: public SeqCltn {
        DECLARE_MEMBERS(Heap);
        int endIndex;
        ArrayOb contents;
private:                // static member functions
        static int child(int i)         { return (i << 1) + 1; }
        static int grandchild(int i)    { return (child(i) << 1) + 1; }
        static int parent(int i)        { return (i - 1) >> 1; }
        static int level(int);
private:
        void bubbleUp(int);
        void bubbleUpMax(int);
        void bubbleUpMin(int);
        int descendents(int) const;
        void errEmpty(const char* fn) const;
        int largest(int,int) const;
        Object* removeAtIndex(int i);
        int smallest(int,int) const;
        void swap(int a,int b)  {
                Object* temp = contents[a];
                contents[a] = contents[b]; 
                contents[b] = temp;
        }
        void trickleDown(int);
        void trickleDownMax(int);
        void trickleDownMin(int);
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Heap(int size=DEFAULT_CAPACITY);
        Heap(const ArrayOb&);
#ifndef BUG_TOOBIG
// yacc stack overflow
        Heap(const Heap&);
#endif
        void operator=(const Heap&);
        bool operator==(const Heap&) const;
        bool operator!=(const Heap& a) const {  return !(*this==a);  }
        virtual Object* add(Object&);
        virtual Object*& at(int index);
        virtual const Object *const& at(int index) const;
        virtual unsigned capacity() const;
        virtual void deepenShallowCopy();
        virtual void doFinish(Iterator& pos) const;
        virtual Object* doNext(Iterator&) const;
        virtual void doReset(Iterator& pos) const;
        virtual Object* first() const;
        virtual unsigned hash() const;
        virtual bool isEmpty() const;
        virtual Object* last() const;
        virtual unsigned occurrencesOf(const Object&) const;
        virtual Object* remove(const Object&);
        virtual void removeAll();
        virtual Object* removeFirst();
        virtual Object* removeId(const Object&);
        virtual Object* removeLast();
        virtual void reSize(int newSize);
        virtual unsigned size() const;
        OrderedCltn sort();
private:                                        // shouldNotImplement()
        virtual void atAllPut(Object& ob);
        virtual int indexOf(const Object& ob) const;
        virtual int indexOfSubCollection(const SeqCltn& cltn, int start=0) const;
        virtual void replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt =0);
};

#endif
