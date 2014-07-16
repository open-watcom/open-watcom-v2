/* Bag.c -- implementation of a Set of Objects with possible duplicates

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
        
A Bag is like a Set, except Bags can contain multiple occurrences of
equal objects.  Bags are implemented by using a Dictionary to associate
each object in the Bag with its number of occurrences.

log:    BAG.C $
Revision 1.1  92/11/10  10:56:58  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:06  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Bag.h"
#include "AssocInt.h"
#include "Integer.h"
#include "nihclIO.h"

#define THIS    Bag
#define BASE    Collection
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES Dictionary::desc()
#define VIRTUAL_BASE_CLASSES

DEFINE_CLASS(Bag,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\BAG.C 1.1 92/11/10 10:56:58 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_REMOVEERR;

Bag::Bag(unsigned size) : contents(size)
{
        count = 0;
}

Bag::Bag(const Bag& b) : contents(b.contents)
{
        count = b.count;
        Object* a;
        Iterator i(contents);
        while (a = i++) contents.at(i.index-1) = a->shallowCopy();
}

Bag::~Bag()
{
        DO(contents,AssocInt,a) delete a; OD;
}

void Bag::operator=(const Bag& b)
{
        if (this == &b) return;
        DO(contents,AssocInt,a) delete a; OD;
        contents = b.contents;
        count = b.count;
        Object* a;
        Iterator i(contents);
        while (a = i++) contents.at(i.index-1) = a->shallowCopy();
}
        
void Bag::reSize(unsigned newSize)
{
        contents.reSize(newSize);
}

Object* Bag::addWithOccurrences(Object& ob, unsigned n)
{
        AssocInt* a = (AssocInt*)contents.assocAt(ob);
        Object* o = &ob;
        if (a == 0) {
                a = new AssocInt(ob,n);
                contents.add(*a);
        }
        else {
                Integer& i = *Integer::castdown(a->value());
                o = a->key();
                i.value(i.value()+n);
        }
        count += n;
        return o;
}

Object* Bag::add(Object& ob)
{
        return addWithOccurrences(ob,1);
}

Object*& Bag::at(int i)                         { return contents.at(i); }

const Object *const& Bag::at(int i) const    { return contents.at(i); }

unsigned Bag::capacity() const { return contents.capacity(); }

void Bag::deepenShallowCopy()
{
        BASE::deepenShallowCopy();
        contents.deepenShallowCopy();
}

Object* Bag::doNext(Iterator& pos) const
{
        const AssocInt* a;
        while (YES) {
                if (pos.num == 0) {
                        a = AssocInt::castdown(contents.doNext(pos));
                        if (a == NULL) return NULL;
                }
                else a = AssocInt::castdown(contents.at(pos.index-1));
                if (pos.num++ < (Integer::castdown(a->value()))->value())
                        return a->key();
                pos.num = 0;
        }
}

void Bag::dumpOn(ostream& strm) const
{
        strm << className() << '[';
        DO(contents,AssocInt,a) a->dumpOn(strm); OD
        strm << "]\n";
}

Object* Bag::remove(const Object& ob)
{
        AssocInt* a = (AssocInt*)contents.assocAt(ob);
        Object* rob = 0;                // return NULL until last occurrence removed
        if (a == 0) setError(NIHCL_REMOVEERR,DEFAULT,this,className(),ob.className(),&ob);
        Integer* i = Integer::castdown(a->value());
        unsigned n = (unsigned)(i->value());
        if (--n == 0) {
                rob = a->key();
                delete AssocInt::castdown(contents.remove(*a));
        }
        else i->value(n);
        --count;
        return rob;
}

void Bag::removeAll()
{
        DO(contents,AssocInt,a) delete a; OD;
        contents.removeAll();
        count = 0;
}

bool Bag::operator==(const Bag& b) const
{
        return count==b.count && contents==b.contents;
}

unsigned Bag::hash() const      { return count^contents.hash(); }

bool Bag::isEqual(const Object& p) const
{
        return p.isSpecies(classDesc) && *this==castdown(p);
}

const Class* Bag::species() const { return &classDesc; }

unsigned Bag::occurrencesOf(const Object& ob) const
{
        AssocInt* a = (AssocInt*)contents.assocAt(ob);
        if (a == 0) return 0;
        else return (Integer::castdown(a->value()))->value();
}

unsigned Bag::size() const      { return count; }

Bag Collection::asBag() const
{
        Bag cltn(MAX(size(),DEFAULT_CAPACITY));
        addContentsTo(cltn);
        return cltn;
}

static unsigned bag_capacity;

Bag::Bag(OIOin& strm)
:
#ifdef MI
        Object(strm),
#endif
        BASE(strm),
        contents((strm >> bag_capacity, bag_capacity))
{
        count = 0;
        unsigned i,n;
        strm >> n;              // read bag size 
        while (n--) {
                strm >> i;
                addWithOccurrences(*Object::readFrom(strm),i);
        }
}

void Bag::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << contents.capacity() << contents.size();
        DO(contents,AssocInt,a)
                strm << (Integer::castdown(a->value()))->value();
                (a->key())->storeOn(strm);
        OD
}

Bag::Bag(OIOifd& fd)
:
#ifdef MI
        Object(fd),
#endif
        BASE(fd),
        contents((fd >> bag_capacity, bag_capacity))
{
        count = 0;
        unsigned i,n;
        fd >> n;
        while ( n-- ) {
                fd >> i;
                addWithOccurrences(*Object::readFrom(fd),i);
        }
}

void Bag::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << contents.capacity() << contents.size();
        DO(contents,AssocInt,a)
                fd << (Integer::castdown(a->value()))->value();
                (a->key())->storeOn(fd);
        OD
}

int Bag::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
