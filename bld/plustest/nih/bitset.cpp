/* Bitset.c -- implementation of set of small integers

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
        
A Bitset is a set of small integers.  It is implemented very efficiently
using a single word.  Each bit of the word indicates if the integer
associated with the bit position is in the set.  Bitsets are
particularly useful in conjunction with enum constants.

log:    BITSET.C $
Revision 1.1  90/05/20  04:19:14  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:12  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Bitset.h"
#include "nihclIO.h"

#define THIS    Bitset
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Bitset,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\BITSET.C 1.1 90/05/20 04:19:14 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

unsigned Bitset::capacity() const       { return sizeof(int)*8; }

void Bitset::deepenShallowCopy()        {}

unsigned Bitset::hash() const   { return m; }
        
bool Bitset::isEmpty() const    { return m==0; }
        
bool Bitset::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* Bitset::species() const { return &classDesc; }

void Bitset::printOn(ostream& s) const
{
        Bitset t = *this;
        for (register unsigned i =0; i<capacity() && !t.isEmpty(); i++) {
                if (t.includes(i)) {
                        s << i;
                        t -= i;
                        if (!t.isEmpty()) s << ',';
                }
        }
}

unsigned Bitset::size() const
{
        register unsigned l=m;
        register unsigned n=0;
        while (l != 0) {
                l &= (l-1);     // removes rightmost 1 
                n++;
        }
        return n;
}

Bitset::Bitset(OIOin& strm)
        : BASE(strm)
{
        strm >> m;
}

void Bitset::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << m;
}

Bitset::Bitset(OIOifd& fd)
        : BASE(fd)
{
        fd >> m;
}

void Bitset::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << m;
}

int Bitset::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
