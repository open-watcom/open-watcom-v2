/* ArrayOb.c -- member functions of class ArrayOb

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
        
Member function definitions for class ArrayOb (Array of Object*).
Objects of class ArrayOb are used in the implementations of several
other Collection classes such as: Bag, Dictionary, Set, and
OrderedCltn.  Note that the ArrayOb constructor initializes the array
with pointers to the nil object.

log:    ARRAYOB.C $
Revision 1.1  90/05/20  04:18:56  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:18:54  kgorlen
 * Release for 1st edition.
 * 
*/

#include <libc.h>
#include <malloc.h>
#include "ArrayOb.h"
#include "nihclIO.h"

#define THIS    ArrayOb
#define BASE    Collection
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(ArrayOb,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\ARRAYOB.C 1.1 90/05/20 04:18:56 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_ALLOCSIZE,NIHCL_INDEXRANGE;

/*
Note: we use malloc()/free() here instead of new/delete because
we use realloc() to implement reSize().
*/

#define NEW(type,size)  ((type*)malloc(sizeof(type)*(size)))

inline void DELETE(Object** ptr) { free((char*)ptr); }

inline Object** REALLOC(Object** ptr, unsigned size)
{
        return (Object**)realloc((char*)ptr,sizeof(Object*)*size);
}

ArrayOb::ArrayOb(unsigned size)
{
        sz = size;
        if (sz==0) allocSizeErr();
        v = NEW(Object*,sz);
        register int i = sz;
        register Object** vp = v;
        while (i--) *vp++ = nil;
}
        
ArrayOb::ArrayOb(const ArrayOb& a)
{
        register int i = a.sz;
        sz = i;
        v = NEW(Object*,i);
        register Object** vp = v;
        register Object** av = a.v;
        while (i--) *vp++ = *av++;
}

ArrayOb::~ArrayOb()     { DELETE(v); }

void ArrayOb::operator=(const ArrayOb& a)
{
        if (v != a.v) {
                DELETE(v);
                v = NEW(Object*,sz=a.sz);
                register int i = a.sz;
                register Object** vp = v;
                register Object** av = a.v;
                while (i--) *vp++ = *av++;
        }
}

bool ArrayOb::operator==(const ArrayOb& a) const
{
        if (sz != a.sz) return NO;
        register unsigned i = sz;
        register Object** vp = v;
        register Object** av = a.v;
        while (i--) { if (!((*vp++)->isEqual(**av++))) return NO; }
        return YES;
}

Object*& ArrayOb::at(int i)                     { return (*this)[i]; }

const Object *const& ArrayOb::at(int i) const   { return (*this)[i]; }

unsigned ArrayOb::capacity() const      { return sz; }
        
bool ArrayOb::isEqual(const Object& a) const
{
        return a.isSpecies(classDesc) && *this==castdown(a);
}

const Class* ArrayOb::species() const { return &classDesc; }

void ArrayOb::reSize(unsigned newsize)
{
        if (newsize == 0) allocSizeErr();
        v = REALLOC(v,newsize);
        if (newsize > sz) {     // initialize new space to nil
                Object** vp = &v[sz];
                while (newsize > sz) {
                        *vp++ = nil;
                        sz++;
                }
        }
        else sz = newsize;
}

void ArrayOb::removeAll()
{
        register Object** vp = v;
        register unsigned i = sz;
        while (i--) *vp++ = nil;
}

Collection& ArrayOb::addContentsTo(Collection& cltn) const
{
        register Object** vp = v;
        register unsigned i = sz;
        while (i--) cltn.add(**vp++);
        return cltn;
}

Object* ArrayOb::doNext(Iterator& pos) const
{
        if (pos.index < size()) return v[pos.index++];
        return 0;
}

void ArrayOb::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        register int i = sz;
        register Object** vp = v;
        while (i--) {
                *vp = (*vp)->deepCopy();
                vp++;
        }
}

unsigned ArrayOb::hash() const
{
        register unsigned h = sz;
        register unsigned i = sz;
        register Object** vp = v;
        while (i--) h^=(*vp++)->hash();
        return h;
}

ArrayOb::ArrayOb(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm)
{
        strm >> sz;
        v = NEW(Object*,sz);
        for (register unsigned i=0; i<sz; i++) v[i] = Object::readFrom(strm);
}

void ArrayOb::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << sz;
        for (register unsigned i=0; i<sz; i++) v[i]->storeOn(strm);
}

unsigned ArrayOb::size() const  { return sz; }

static int compare_ob(const void* a, const void* b)
{
        return (*(const Object**)a)->compare(**(const Object**)b);
}

void ArrayOb::sort()
{
        qsort(v,sz,sizeof(Object*),compare_ob);
}

void ArrayOb::allocSizeErr() const
{
        setError(NIHCL_ALLOCSIZE,DEFAULT,this,className());
}

void ArrayOb::indexRangeErr() const
{
        setError(NIHCL_INDEXRANGE,DEFAULT,this,className());
}

ArrayOb::ArrayOb(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd)
{
        fd >> sz;
        v = NEW(Object*,sz);
        for (register unsigned i=0; i<sz; i++ )
           v[i] = Object::readFrom(fd);
}

void ArrayOb::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << sz;
        for (register unsigned i=0; i<sz; i++) 
                v[i]->storeOn(fd);
}

int ArrayOb::compare(const Object& arg) const
// Compare two arrays of objects.  If *this > arg return >0,
// *this == arg return 0, and if *this < arg return <0.
{
        assertArgSpecies(arg,classDesc,"compare");
        const ArrayOb& a = castdown(arg);
        for (int i=0; i<sz; i++) {
// previous elements compared equal; longer ArrayOb is therefore larger
                if (i == a.sz) return 1;
// compare() != 0 at any element determines ordering
                int val;
                if ((val = v[i]->compare(*a.v[i])) != 0) return val;
        }
// all elements in this ArrayOb compare() equal to arg ArrayOb
        if (sz == a.sz) return 0;
        return -1;
}

Object* ArrayOb::add(Object& ob)
{
        shouldNotImplement("add");
        return &ob;
}

unsigned ArrayOb::occurrencesOf(const Object&) const
{
        shouldNotImplement("occurrencesOf");
        return 0;
}

Object* ArrayOb::remove(const Object&)
{
        shouldNotImplement("remove");
        return 0;
}
