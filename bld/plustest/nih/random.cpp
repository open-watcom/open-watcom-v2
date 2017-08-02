/* Random.c -- implementation of pseudo-random number generator

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
        December, 1985

Function:
        
A psuedo-random number generator.  The function next() returns random
numbers uniformly distributed over the interval (0.0,1.0).

Reference:

Pierre L'ecuyer, "Efficient and Portable Combined Random Number
Generators", Commun. ACM 31, 6 (June 1988), 742-749.

log:    RANDOM.C $
Revision 1.1  92/11/10  12:51:46  Anthony_Scian
.

 * Revision 3.0  90/05/20  17:40:49  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Random.h"
#include <time.h>
#include <stdlib.h>
#include "nihclIO.h"

#define THIS    Random
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Random,2,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\RANDOM.C 1.1 92/11/10 12:51:46 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

const long s1max = 2147483562;
const long s2max = 2147483398;

void Random::checkSeeds()
{
        if (sizeof(long) != 4) {
                cerr << "\nRandom only works on 32-bit machines\n";
                exit(1);
        }
        if (s1 < 1 || s1 > s1max) s1 = (ABS(s1) % s1max) + 1;
        if (s2 < 1 || s2 > s2max) s2 = (ABS(s2) % s2max) + 1;
}

Random::Random()
{
        time((time_t *)&s1);
        s2 = (long)this;
        checkSeeds();
}

Random::Random(long seed1, long seed2)
{
        s1 = seed1;  s2 = seed2;
        checkSeeds();
}

float Random::next()
{
        long Z,k;

        k = s1/53668;
        s1 = 40014 * (s1 - k * 53668) - k * 12211;
        if (s1 < 0) s1 += s1max + 1;

        k = s2/52774;
        s2 = 40692 * (s2 - k * 52774) - k * 3791;
        if (s2 < 0) s2 += s2max + 1;

        Z = s1 - s2;
        if (Z < 1) Z += s1max;

        return Z * 4.656613E-10;
}

void Random::deepenShallowCopy()        {}

unsigned Random::hash() const   { return (unsigned)this; }

bool Random::isEqual(const Object& ob) const   { return isSame(ob); }

void Random::printOn(ostream& strm) const
{
        strm << s1 << ' ' << s2;
}

Random::Random(OIOin& strm)
        : BASE(strm)
{
        strm >> s1 >> s2;
}

void Random::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << s1 << s2;
}

Random::Random(OIOifd& fd)
        : BASE(fd)
{
        fd >> s1 >> s2;
}

void Random::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << s1 << s2;
}

int Random::compare(const Object&) const
{
        shouldNotImplement("compare");
        return 0;
}
