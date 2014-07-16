/* SeqCltn.c -- implementation of abstract sequential collections

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
        
SeqCltn is an abstract class representing collections whose elements are
ordered and are externally named by integer indices.

log:    SEQCLTN.C $
Revision 1.1  90/05/20  04:21:14  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:21:12  kgorlen
 * Release for 1st edition.
 * 
*/

#include "SeqCltn.h"

#define THIS    SeqCltn
#define BASE    Collection
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES

DEFINE_ABSTRACT_CLASS(SeqCltn,0,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\SEQCLTN.C 1.1 90/05/20 04:21:14 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_RDABSTCLASS,NIHCL_INDEXRANGE;

SeqCltn::SeqCltn() {}

SeqCltn::SeqCltn(OIOifd& fd) : BASE(fd) {}

SeqCltn::SeqCltn(OIOin& strm) : BASE(strm) {}

int SeqCltn::compare(const Object& arg) const
// Compare two sequenced collections.  If *this > arg return >0,
// *this == arg return 0, and if *this < arg return <0.
{
        assertArgClass(arg,*SeqCltn::desc(),"compare");
        Iterator i(*this);
        Iterator j(castdown(arg));
        Object* p;      // pointer to next object in this SeqCltn
        Object* q;      // pointer to next object in arg SeqCltn

        while ((q = j++, p = i++)) {
// previous elements compared equal; longer SeqCltn is therefore larger
                if (q == 0) return 1;
// compare() != 0 at any element determines ordering
                int val;
                if ((val = p->compare(*q)) != 0) return val;
        }
// all elements in this SeqCltn compare() equal to arg SeqCltn
        if (q == 0) return 0;   // size() == arg.size()
        return -1;
}

Object* SeqCltn::first() const { return (Object*)at(0); }
        
unsigned SeqCltn::hash() const
{
        unsigned h = size();
        DO(*this,Object,p) h ^= p->hash(); OD
        return h;
}

int SeqCltn::indexOf(const Object& ob) const
{
        int i = 0;
        DO(*this,Object,p)
                if (p->isEqual(ob)) return i;
                i++;
        OD
        return -1;
}
        
bool SeqCltn::isEqual(const Object& ob) const
{
        assertArgClass(ob,*SeqCltn::desc(),"isEqual");
        if (size() != ob.size()) return NO;
        Iterator i(*this);
        Iterator j(castdown(ob));
        Object* p;
        while (p = i++) if (!p->isEqual(*(j++))) return NO;
        return YES;
}

const Class* SeqCltn::species() const   { return &classDesc; }

Object* SeqCltn::last() const { return (Object*)at(size()-1); }

unsigned SeqCltn::occurrencesOf(const Object& ob) const
{
        unsigned n = 0;
        DO(*this,Object,p) if (p->isEqual(ob)) n++; OD
        return n;
}
        
Object* SeqCltn::doNext(Iterator& pos) const
{
        if (pos.index < size()) return (Object*)at(pos.index++);
        return 0;
}

void SeqCltn::indexRangeErr() const
{
        setError(NIHCL_INDEXRANGE,DEFAULT,this,className());
}
