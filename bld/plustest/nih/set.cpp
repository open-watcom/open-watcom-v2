/* Set.c -- implemenation of hash tables

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
        
A Set is an unordered collection of objects in which no object is
duplicated.  Duplicate objects are defined by the function isEqual.
Sets are implemented using a hash table.  The capacity() function
returns the 1/2 the capacity of the hash table and the size() function
returns the number of objects currently in the Set.  For efficiency, the
capacity is always a power of two and is doubled whenever the table
becomes half full.

log:    SET.C $
Revision 1.1  92/11/10  13:04:08  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:14  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Set.h"
#include "OrderedCltn.h"
#include "nihclIO.h"

#define THIS    Set
#define BASE    Collection
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES ArrayOb::desc()
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(Set,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\SET.C 1.1 92/11/10 13:04:08 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_ALLOCSIZE,NIHCL_REMOVEERR;

unsigned Set::setCapacity(unsigned int size)
/*
        Establish the Set capacity.  Round size up to the next highest
        power of two, if necessary.
*/
{
        if (size==0) setError(NIHCL_ALLOCSIZE,DEFAULT,this,className());
        count = 0;
        nbits = 0;
        for (register unsigned s=size; s!=0; s>>=1) nbits++;
        if ((size&(size-1)) != 0) nbits++;      // round up if not power of 2
        size = 1<<nbits;
        mask = size-1;
        return size;            // return hash table size
}

Set::Set(unsigned size) : contents(setCapacity(size)) {}

int Set::h(unsigned long K) const
/*
multiplicative hash function

Enter:
        K = key to be hashed
        
Returns:
        hash table index
        
Knuth Vol. 3, Section 6.4, pp. 508-512
*/
{
//mjs:  const unsigned long Aw = 2654435769UL;
        const unsigned long Aw = 0x9E3779B9;
//      const unsigned long Aw = 40503;         use for 16 bit machines? 
        return ((Aw*K)>>((8*sizeof(unsigned))-nbits)) & mask;
}

int Set::findIndexOf(const Object& ob) const
/*
Search this set for the specified object

Enter:
        ob = pointer to object to search for

Returns:
        index of object if found or of nil slot if not found
        
Algorithm L, Knuth Vol. 3, p. 519
*/
{
        register int i;
        for (i = h(ob.hash()); contents[i]!=nil; i = (i-1)&mask) {
                if (contents[i]->isEqual(ob)) return i;
        }
        return i;
}

void Set::reSize(unsigned newSize)
/*
        Change the capacity of this Set to newSize.
*/
{
        if (newSize <= count) return;
        if (count > 0) {
                OrderedCltn oldcontents(count);
                unsigned n = contents.size();
                for (int i=0; i<n; i++) {
                        Object* ob = contents[i];
                        if (ob != nil) {
                                oldcontents.add(*ob);
                                contents.elem(i) = nil;
                        }
                }
                contents.reSize(setCapacity(newSize));
                addAll(oldcontents);
        }
        else contents.reSize(setCapacity(newSize));
}

Object* Set::add(Object& ob)
/*
        Add an object to this Set, making the Set larger if it
        becomes half full.
*/
{
        register int i = findIndexOf(ob);
        if (contents[i]==nil) {         // add new object to set 
                contents[i] = &ob;
                if (++count > capacity()) reSize(capacity()*EXPANSION_FACTOR);
                return &ob;             // successful add 
        }
        else return contents[i];        // object already in set 
}

Object* Set::remove(const Object& ob)
/*
remove object from set

Enter:
        ob = reference to object to be removed

Returns:
        pointer to removed object

Algorithm R, Knuth Vol. 3 p. 527
*/
{
        register int i = findIndexOf(ob);
        Object* rob = contents[i];
        if (rob==nil) setError(NIHCL_REMOVEERR,DEFAULT,this,className(),ob.className(),&ob);
        else {
                register int j,r;
                while (YES) {
                        contents[j=i] = nil;
                        do {
                                i = (i-1)&mask;
                                if (contents[i]==nil) {
                                        count--;
                                        return rob;
                                }
                                r = h(contents[i]->hash());
                        } while ((i<=r&&r<j) || (r<j&&j<i) || (j<i&&i<=r));
                        contents[j] = contents[i];
                }
        }
        return 0;
}

void Set::removeAll()
{
        contents.removeAll();
        count = 0;
}

bool Set::operator==(const Set& s) const
/*
        Return YES if the specified Set equals this Set.
*/
{
        if (count!=s.count) return NO;
        unsigned n = contents.capacity();
        for (register int i=0; i<n; i++) {
                if (contents[i]!=nil && !s.includes(*contents[i])) return NO;
        }
        return YES;
}

Set Set::operator-(const Set& s) const
/*
        Returns a Set of all of the objects that are contained in this
        Set but not in the specified Set.
*/
{
        Set diff = *this;
        unsigned n = contents.capacity();
        for (register int i=0; i<n; i++) {
                if (contents[i]!=nil && s.includes(*contents[i])) diff.remove(*contents[i]);
        }
        return diff;
}

Set Set::operator&(const Set& s) const
/*
        Returns a Set of all objects that are in both this Set and
        the specified Set.
*/
{
        Set intersection = *this;
        unsigned n = contents.capacity();
        for (register int i=0; i<n; i++) {
                if (contents[i]!=nil && !s.includes(*contents[i])) intersection.remove(*contents[i]);
        }
        return intersection;
}

Set Set::operator|(const Set& s) const
/*
        Returns a Set of all objects that are in either this Set
        or the specified Set.
*/
{
        Set u = *this;
        u.addAll(s);
        return u;
}

Object*& Set::at(int i)                         { return contents[i]; }

const Object *const& Set::at(int i) const       { return contents[i]; }

bool Set::isEqual(const Object& p) const
/*
        Returns YES if this Set equals the specified object.
*/
{
        return p.isSpecies(classDesc) && *this==castdown(p);
}

const Class* Set::species() const { return &classDesc; }

void Set::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        contents.deepenShallowCopy();
}

Object* Set::doNext(Iterator& pos) const
{
        Object* ob;
        unsigned n = contents.capacity();
        while (pos.index < n) {
                if ((ob = (Object*)contents[pos.index++]) != nil) return ob;
        }
        return 0;
}

unsigned Set::capacity() const  { return contents.capacity()>>1; }

unsigned Set::hash() const
{
        unsigned h = 0;
        DO(*this,Object,o) h ^= o->hash(); OD
        return h;
}

unsigned Set::occurrencesOf(const Object& ob) const
/*
        Return the number of occurences of thw specified object
        in this Set (either 0 or 1).
*/
{
        if (contents[findIndexOf(ob)]!=nil) return 1;
        else return 0;
}

Object* Set::findObjectWithKey(const Object& ob) const
{
        return (Object*)contents[findIndexOf(ob)];
}

unsigned Set::size() const              { return count; }

Set Collection::asSet() const
/*
        Convert this Collection to a Set.
*/
{
        Set cltn(MAX(size(),DEFAULT_CAPACITY));
        addContentsTo(cltn);
        return cltn;
}

static unsigned set_capacity;

Set::Set(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm),
        contents((strm >> set_capacity, setCapacity(set_capacity)))
{
        unsigned n;
        strm >> n;              // read Set size 
        while (n--) add(*Object::readFrom(strm));
}

Set::Set(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd),
        contents((fd >> set_capacity, setCapacity(set_capacity) ))
{
        unsigned n;
        fd >> n;
        while (n--) add(*Object::readFrom(fd));
}

void Set::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        _storer(fd);
}

void Set::storer(OIOout& strm) const
{
        BASE::storer(strm);
        _storer(strm);
}

int Set::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
