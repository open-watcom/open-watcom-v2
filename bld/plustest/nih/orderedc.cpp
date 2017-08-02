/* OrderedCltn.c -- implementation of abstract ordered collections

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Bg. 12A, Rm. 2033
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-1111
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov
        September, 1985

Function:
        
OrderedCltns are ordered by the sequence in which objects are added and removed
from them.  Object elements are accessible by index.

log:    ORDEREDC.C $
Revision 1.1  92/11/10  12:49:50  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:42  kgorlen
 * Release for 1st edition.
 * 
*/

#include <libc.h>
#include "OrderedCltn.h"
#include "nihclIO.h"

#define THIS    OrderedCltn
#define BASE    SeqCltn
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES ArrayOb::desc()
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(OrderedCltn,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\ORDEREDC.C 1.1 92/11/10 12:49:50 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_CLTNEMPTY,NIHCL_OBNOTFOUND;

OrderedCltn::OrderedCltn(unsigned size) : contents(size)
{
        endIndex = 0;
}

#ifndef BUG_TOOBIG
// yacc stack overflow

OrderedCltn::OrderedCltn(const OrderedCltn& c) : contents(c.contents)
{
        endIndex = c.endIndex;
}

#endif

void OrderedCltn::operator=(const OrderedCltn& c)
{
        endIndex = c.endIndex;
        contents = c.contents;
}

bool OrderedCltn::operator==(const OrderedCltn& a) const
{
        if (endIndex != a.endIndex) return NO;
        else {
                register int i = endIndex;
                register const Object* /* WATCOM */ const * vv = &contents.elem(0);
                register const Object* /* WATCOM */ const * av = &a.at(0);
                while (i--) if (!((*vv++)->isEqual(**av++))) return NO;
        }
        return YES;
}

OrderedCltn OrderedCltn::operator&(const SeqCltn& cltn) const
{
        OrderedCltn c(size()+cltn.size());
        addContentsTo(c);
        cltn.addContentsTo(c);
        return c;
}

void OrderedCltn::operator&=(const SeqCltn& cltn)
{
        cltn.addContentsTo(*this);
}

Object* OrderedCltn::addAtIndex(int i, Object& ob)
{
        if (endIndex == capacity()) contents.reSize(capacity()+EXPANSION_INCREMENT);
        for (register int j=endIndex; j>i; j--) contents[j] = contents[j-1];
        contents[i] = &ob;
        endIndex++;
        return &ob;
}

Object* OrderedCltn::removeAtIndex(int i)
{
        register Object* obrem = contents[i];
        for (register int j=i+1; j<endIndex; j++) contents[j-1] = contents[j];
        contents[--endIndex] = nil;
        return obrem;
}

Object* OrderedCltn::add(Object& ob)
{
        addAtIndex(endIndex,ob);
        return &ob;
}

Object* OrderedCltn::addAfter(const Object& ob, Object& newob)
{
        register int i = indexOf(ob);
        if (i < 0) errNotFound("addAfter",ob);
        return addAtIndex(i+1,newob);
}

Object* OrderedCltn::addAllLast(const OrderedCltn& cltn)
{
        if (endIndex+cltn.size() >= capacity())
                contents.reSize(endIndex+cltn.size()+EXPANSION_INCREMENT);
        for (register int i=0; i<cltn.size(); i++)
                contents[endIndex++] = (Object*)cltn.contents[i];
        return (Object*) &cltn;
}

Object* OrderedCltn::addBefore(const Object& ob, Object& newob)
{
        register int i = indexOf(ob);
        if (i < 0) errNotFound("addBefore",ob);
        return addAtIndex(i,newob);
}

Collection& OrderedCltn::addContentsTo(Collection& cltn) const
{
        for (register int i=0; i<size(); i++) cltn.add(*(Object*)contents[i]);
        return cltn;
}

Object* OrderedCltn::addLast(Object& ob) { return add(ob); }

Object* OrderedCltn::after(const Object& ob) const
{
        register int i=indexOf(ob);
        if (i<0) errNotFound("after",ob);
        if (++i == endIndex) return nil;
        return (Object*)contents[i];
}

Object*& OrderedCltn::at(int i)                         { return (*this)[i]; }

const Object *const& OrderedCltn::at(int i) const       { return (*this)[i]; }

void OrderedCltn::atAllPut(Object& ob)
{
        for (register int i=0; i<endIndex; i++) contents[i] = &ob;
}

Object* OrderedCltn::before(const Object& ob) const
{
        register int i = indexOf(ob);
        if (i < 0) errNotFound("before",ob);
        if (--i < 0) return nil;
        return (Object*)contents[i];
}

unsigned OrderedCltn::capacity() const  { return contents.capacity(); }

void OrderedCltn::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        register int i = endIndex;
        register Object** vv = &contents.elem(0);
        while (i--) {
                *vv = (*vv)->deepCopy();
                vv++;
        }
}

Object* OrderedCltn::first() const
{
        if (endIndex==0) errEmpty("first");
        else return (Object*)contents.elem(0);
        return 0;
}

unsigned OrderedCltn::hash() const
{
        register unsigned h = endIndex;
        register int i = endIndex;
        register const Object* /* WATCOM */ const * vv = &contents.elem(0);
        while (i--) h^=(*vv++)->hash();
        return h;
}

int OrderedCltn::indexOf(const Object& ob) const
{
        for (register int i=0; i<endIndex; i++)
                if (contents[i]->isEqual(ob)) return i;
        return -1;
}

int OrderedCltn::indexOfSubCollection(const SeqCltn& cltn, int start) const
{
        int subsize = cltn.size();
        for (register int i=start; i<(endIndex-subsize); i++) {
                for (register int j=0; j<subsize; j++)
                        if (!(contents[i+j]->isEqual(*cltn.at(j)))) goto next;
                return i;
next:;  }
        return -1;
}

bool OrderedCltn::isEmpty() const { return endIndex==0; }
        
Object* OrderedCltn::last() const
{
        if (endIndex==0) errEmpty("last");
        else return (Object*)contents.elem(endIndex-1);
        return  0;
}

unsigned OrderedCltn::occurrencesOf(const Object& ob) const
{
        register unsigned n=0;
        for (register int i=0; i<endIndex; i++)
                if (contents[i]->isEqual(ob)) n++;
        return n;
}

Object* OrderedCltn::remove(const Object& ob)
{
        for (register int i=0; i<endIndex; i++) {
                if (contents[i]->isEqual(ob)) {
                        return removeAtIndex(i);
                }
        }
        return nil;
}

void OrderedCltn::removeAll()
{
        for (register int i=0; i<endIndex; i++) contents[i] = nil;
        endIndex = 0;
}

Object* OrderedCltn::removeId(const Object& ob)
{
        for (register int i=0; i<endIndex; i++) {
                if (contents[i] == &ob) return removeAtIndex(i);
        }
        return nil;
}

Object* OrderedCltn::removeLast()
{
        if (endIndex==0) errEmpty("removeLast");
        else return removeAtIndex(endIndex-1);
        return 0;
}

void OrderedCltn::reSize(unsigned newSize)
{
        if (newSize > size()) contents.reSize(newSize);
}

void OrderedCltn::replaceFrom(int start, int stop, const SeqCltn& replacement, int startAt)
{
        register int j=startAt;
        for (register int i=start; i<=stop; i++,j++)
                contents[i] = ((SeqCltn&)replacement).at(j);
}

static int compare_ob(const void* a, const void* b)
{
        return (*(const Object**)a)->compare(**(const Object**)b);
}

unsigned OrderedCltn::size() const              { return endIndex; }

void OrderedCltn::sort()
{
     qsort(&contents.elem(0),size(),sizeof(Object*),compare_ob);
}

void OrderedCltn::errEmpty(const char* fn) const
{
        setError(NIHCL_CLTNEMPTY,DEFAULT,this,className(),fn);
}

void OrderedCltn::errNotFound(const char* fn, const Object& ob) const
{
        setError(NIHCL_OBNOTFOUND,DEFAULT,this,className(),fn,ob.className(),&ob);
}

OrderedCltn Collection::asOrderedCltn() const
{
        OrderedCltn cltn(MAX(size(),DEFAULT_CAPACITY));
        addContentsTo(cltn);
        return cltn;
}

static unsigned orderedcltn_capacity;

OrderedCltn::OrderedCltn(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm),
        contents((strm >> orderedcltn_capacity, orderedcltn_capacity))
{
        endIndex = 0;
        unsigned n;
        strm >> n;              // read collection capacity 
        while (n--) add(*Object::readFrom(strm));
}

OrderedCltn::OrderedCltn(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd),
        contents((fd >> orderedcltn_capacity, orderedcltn_capacity))
{
        endIndex = 0;
        unsigned n;
        fd >> n;
        while (n--) add(*Object::readFrom(fd));
}

void OrderedCltn::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        _storer(fd);
}

void OrderedCltn::storer(OIOout& strm) const
{
        BASE::storer(strm);
        _storer(strm);
}
