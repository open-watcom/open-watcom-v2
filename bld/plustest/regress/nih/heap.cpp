/* Heap.c  -- implementation of abstract Heap class

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        C. J. Eppich
        Bg. 12A, Rm. 2025
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-5361
        March, 1987

Function:  The Min-Max Heap is implemented as described by Atkinson,
Sack, Santoro, and Strothotte (1986).  Objects may be added; the min
or max may be accessed with first() or last(), respectively, or removed
with removeFirst() or removeLast(), respectively.
        
log:    HEAP.C $
Revision 1.1  92/11/10  12:41:00  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:42  kgorlen
 * Release for 1st edition.
 * 
*/

#include <libc.h>
#include "Heap.h"
#include "nihclIO.h"

#define THIS    Heap
#define BASE    SeqCltn
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES ArrayOb::desc()
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(Heap,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\HEAP.C 1.1 92/11/10 12:41:00 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_CLTNEMPTY;

static int heap_capacity;


// CONSTRUCTORS:

Heap::Heap(int size) : contents(size)
{
        endIndex = 0;
}

Heap::Heap(const ArrayOb &a) : contents(a)
{
        endIndex = a.size();
        for (int i = endIndex - 2; i >= 0; i--)
                trickleDown(i);
}

#ifndef BUG_TOOBIG
// yacc stack overflow
Heap::Heap(const Heap& c) : contents(c.contents)
{
        endIndex = c.endIndex;
}
#endif

Heap::Heap(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm),
        contents((strm >> heap_capacity, heap_capacity))
{
        endIndex = 0;
        int n;
        strm >> n;              // read collection size 
        while (n--)
                add(*Object::readFrom(strm));
}

Heap::Heap(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd),
        contents((fd >> heap_capacity, heap_capacity))
{
        endIndex = 0;
        int n;
        fd >> n;
        while (n--) add(*Object::readFrom(fd));
}

void Heap::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        _storer(fd);
}

void Heap::storer(OIOout& strm) const
{
        BASE::storer(strm);
        _storer(strm);
}

// OPERATORS:

void Heap::operator=(const Heap& a)
{
        endIndex = a.endIndex;
        contents = a.contents;
}

bool Heap::operator==(const Heap& a) const
{
        if (endIndex != a.endIndex) 
                return NO;
        else 
        {
                Heap heap1 = *this;
                Heap heap2 = a;
                while (heap1.endIndex)
                     if (!(heap1.removeFirst()->isEqual(*heap2.removeFirst())))
                                return NO;
        }
        return YES;
}

// NonPublic MEMBER FUNCTIONS

void Heap:: bubbleUp(int i)
//Atkinson,et.al.(1986) -- determine correct position of last element of heap.
{
        int father;     
        if ((level(i) & 1) == 0)        // i is on min level

                if ( (i > 0)    // then i has a parent
                 && (contents[i]->compare(*contents[father=parent(i)])>0))
                {
                        swap(i,father);
                        bubbleUpMax(father);
                }
                else bubbleUpMin(i);
        
        else    // i is on max level

                if ( (i > 0)
                  && (contents[i]->compare(*contents[father=parent(i)])<0))
                {
                        swap(father,i);
                        bubbleUpMin(father);
                }
                else bubbleUpMax(i);
}

void Heap::bubbleUpMax(int i)
{
        int grandparent;
        if ( (i > 6)    //then i has a grandparent
                && (contents[i]->compare
                  (*contents[grandparent=parent(parent(i))]) > 0))
        {
                swap(i,grandparent);
                bubbleUpMax(grandparent);
        }
}

void Heap::bubbleUpMin(int i)
{
        int grandparent;
        if ( (i > 2)    //then i has a grandparent
                && (contents[i]->compare
                  (*contents[grandparent=parent(parent(i))]) < 0))
        {
                swap(i,grandparent);
                bubbleUpMin(grandparent);
        }
}


int Heap::descendents(int i) const
// Returns number of children and grandchildren of heap element with index i.
{
        if (endIndex <= child(i))       return(0);
        else if (endIndex <= grandchild(i))     // No grandchildren
                return MIN(2,(endIndex - child(i)));
        else return MIN(6,(2 + endIndex - grandchild(i)));
}

void Heap::errEmpty(const char* fn) const
{
        setError(NIHCL_CLTNEMPTY,DEFAULT,this,className(),fn);
}

int Heap::largest(int a, int i) const
// Return Heap index with largest value from children and grandchildren
// of i. a is number of children and grandchildren & must be > 0.
{
        int maxIndex = child(i);
        Object* max = (Object*)contents[maxIndex];
        if (a > 1)
        {       if (contents[child(i)+1]->compare(*max) > 0)
                        max = (Object*)contents[++maxIndex];
                if (a > 2)
                {
                        Object** grandcPtr = (Object**)&contents[grandchild(i)];
                        for (int k=0; k < (a - 2); k++)
                        {
                                if ((*grandcPtr)->compare(*max) > 0)
                                {
                                        maxIndex = grandchild(i) + k;
                                        max = *grandcPtr;
                                }
                                grandcPtr++;
                        }
                }
        }
        return(maxIndex);
}

int Heap::level(int i)
//returns level that ith element is on
{
        int l = 0;
        for (int index = i+1; index > 1; index >>=1)  l++;
        return(l);
}

Object* Heap::removeAtIndex(int i)
{
        register Object* obrem = contents[i];
        contents[i] = contents[--endIndex];
        bubbleUp(i);
        trickleDown(i);
        return obrem;
}

int Heap::smallest(int a, int i) const
// Return Heap index with smallest value from children and grandchildren
// of i. a is number of children and grandchildren & must be > 0.
{
        int minIndex = child(i);
        Object* min = (Object*)contents[minIndex];
        if (a > 1) 
        {       if (contents[child(i)+1]->compare(*min) < 0)
                        min = (Object*)contents[++minIndex];
                if (a > 2)
                {
                        Object** grandcPtr = (Object**)&contents[grandchild(i)];
                        for (int k=0; k < (a - 2); k++)
                        {
                                if ((*grandcPtr)->compare(*min) < 0)
                                {
                                        minIndex = grandchild(i) + k;
                                        min = *grandcPtr;
                                }
                                grandcPtr++;
                        }
                }
        }
        return(minIndex);
}

void Heap::trickleDown(int i)
{
        if ((level(i) & 1) == 0)        // i is on min level
                trickleDownMin(i);
        else    trickleDownMax(i);
}

void Heap::trickleDownMax(int i)
{
        if (descendents(i))
        {       int max = largest(descendents(i),i);
                if (max >= grandchild(i))
                {   if (contents[max]->compare(*contents[i]) > 0)
                    {
                        swap(i,max);
                        if (contents[max]->compare(*contents[parent(max)]) < 0)
                                swap(max,parent(max));
                        trickleDownMax(max);
                    }
                }
                else  // max is child of i
                    if (contents[max]->compare(*contents[i]) > 0)
                        swap(i,max);
        }
}

void Heap::trickleDownMin(int i)
{
        if (descendents(i))
        {
                int min = smallest(descendents(i),i);
                if (min >= grandchild(i))
                {   if (contents[min]->compare(*contents[i]) < 0)
                    {
                        swap(i,min);
                        if (contents[min]->compare(*contents[parent(min)]) > 0)
                                swap(min,parent(min));
                        trickleDownMin(min);
                    }
                }
                else  // min is child of i
                    if (contents[min]->compare(*contents[i]) < 0)
                        swap(i,min);
        }
}


// PUBLIC MEMBER FUNCTIONS:

Object* Heap::add(Object& ob)
{
        if (endIndex == capacity())
                contents.reSize(capacity() + EXPANSION_INCREMENT);
        contents[endIndex++] = &ob;
        bubbleUp(endIndex-1);
        return this;
}

Heap Collection::asHeap() const // declaration in Collection.h
{
        Heap cltn(asArrayOb());
        return cltn;
}

Object*& Heap::at(int index)
{
    return contents.at(index);
}

const Object *const& Heap::at(int index) const
{
    return contents.at(index);
}

void Heap::atAllPut(Object&) { shouldNotImplement("atAllPut"); }

unsigned Heap::capacity() const { return contents.capacity(); }

void Heap::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        register int i = endIndex;
        register Object** vv = &contents.elem(0);
        while (i--) {
                *vv = (*vv)->deepCopy();
                vv++;
        }
}

void Heap::doReset(Iterator& pos) const
{
        BASE::doReset(pos);
        if (pos.state != nil) {
                Heap* hp = castdown(pos.state);
                delete hp;
        }
        pos.state = new Heap(*this);
}

Object* Heap::doNext(Iterator& pos) const
{
        Heap& h = *castdown(pos.state);
        if (h.isEmpty()) return 0;
        pos.index++;
        return h.removeFirst();
}

void Heap::doFinish(Iterator& pos) const
{
        if (pos.state != nil) {
                Heap* hp = castdown(pos.state);
                delete hp;
        }
        BASE::doFinish(pos);
}

Object* Heap::first() const     // returns minimum object 
{
        if (endIndex==0) errEmpty("first");
        else return (Object*)contents.elem(0);
        return 0;
}

unsigned Heap::hash() const
{
        register unsigned h = endIndex;
        register int i = endIndex;
        register const Object* /* WATCOM */ const * vv = &contents.elem(0);
        while (i--) h^=(*vv++)->hash();
        return h;
}


int Heap::indexOf(const Object&) const
{
        shouldNotImplement("indexOf");
        return(0);
}

int Heap::indexOfSubCollection(const SeqCltn&, int) const
{
        shouldNotImplement("indexOfSubCollection");
        return(0);
}

bool Heap::isEmpty() const      { return endIndex==0; }

Object* Heap::last() const      // returns maximum object
{
        if (endIndex==0)  errEmpty("last");
        else if (endIndex==1) return (Object*)contents[0];
        else if (endIndex==2) return (Object*)contents[1];
        else if (contents[1]->compare(*contents[2]) > 0)
                return (Object*)contents[1];
                else return (Object*)contents[2];
        return 0;
}

unsigned Heap::occurrencesOf(const Object& ob) const
{
    register unsigned n=0;
    for (register int i=0; i<endIndex; i++)
        if (contents[i]->isEqual(ob)) n++;
    return n;
}

Object* Heap::remove(const Object& ob)
{
        for (register int i=0; i<endIndex; i++) {
                if (contents[i]->isEqual(ob)) {
                        return removeAtIndex(i);
                }
        }
        return nil;
}

Object* Heap::removeId(const Object& ob)
{
        for (register int i=0; i<endIndex; i++) {
                if (contents[i]->isSame(ob)) {
                        return removeAtIndex(i);
                }
        }
        return nil;
}

void Heap::removeAll()
{
        contents.removeAll();
        endIndex = 0;
}

Object* Heap::removeFirst()     // remove the minimum element of the Heap
{
        Object* min = first();
        contents[0] = contents[--endIndex];
        trickleDownMin(0);
        return(min);
}

Object* Heap::removeLast()      // remove the maximum element of the Heap
{
        if (endIndex == 0)  errEmpty("removeLast");
        else if (endIndex == 1) return(removeFirst());
        else 
        {       Object* max;
                if (endIndex == 2)
                {
                        max = contents[1];
                        contents[1] = contents[--endIndex];
                        trickleDownMax(1);
                }
                else    // endIndex >= 3
                {
                        if (contents[1]->compare(*contents[2]) > 0)
                        {
                                max = contents[1];
                                contents[1] = contents[--endIndex];
                                trickleDownMax(1);
                        }
                        else
                        {       
                                max = contents[2];
                                contents[2] = contents[--endIndex];
                                trickleDownMax(2);
                        }
                }
                return(max);
        }
        return 0;
}

void Heap::reSize(int newSize)
{
        if (newSize > size())   contents.reSize(newSize);
}

unsigned Heap::size() const     { return endIndex; }

OrderedCltn Heap::sort()
//Returns heap as OrderedCltn, sorted min to max.
{
        Heap heap = *this;
        OrderedCltn cltn(size());
        for (register unsigned n=size(); n > 0; n--)
                cltn.add(*heap.removeFirst());
        return cltn;
}

void Heap::replaceFrom(int /* start */, int /* stop */, const SeqCltn&  /* replacement */, int /* startAt */)
{
        shouldNotImplement("replaceFrom");
}
