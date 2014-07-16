/* Fraction.c -- implementation of fractions

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
        
Implements a fraction as two integers, the numerator and the
denominator.  WARNING -- this implementation is not suitable for serious
numeric applications.  Reference: Knuth, "The Art of Computer
Programming", Vol. 2, Section 4.5.

log:    FRACTION.C $
Revision 1.1  90/05/20  04:19:40  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:38  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Fraction.h"
#include <libc.h>
#include <math.h>
#include "nihclIO.h"

#define THIS    Fraction
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_CLASS(Fraction,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\FRACTION.C 1.1 90/05/20 04:19:40 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_ZERODEN,NIHCL_FCTNOV,NIHCL_FCTNUN;

int Fraction::gcd(int uu, int vv)
/* gcd -- binary greatest common divisor algorithm

Algorithm B, p. 321.

*/
{
        register int u=ABS(uu), v=ABS(vv);
        register int k=0;
        register int t;
        if (u == 0) return v;
        if (v == 0) return u;
        while ((u&1) == 0 && (v&1) == 0) { u>>=1; v>>=1; k++; }
        if (u&1) { t = -v; goto B4; }
        else t = u;
        do {
B3:             t/=2;
B4:             if ((t&1) == 0) goto B3;
                if (t>0) u = t;
                else v = -t;
                t = u-v;
        } while (t != 0);
        return u<<k;
}

Fraction::Fraction(int num, int den)
/*
        Construct a Fraction from the specified numerator and denominator.
*/
{
        n = num; d = den;
        if (d == 0) setError(NIHCL_ZERODEN,DEFAULT,this,num,den);
        if (n == 0) { d = 1; return; }
        if (d < 0) { n = -n; d = -d; }
        reduce();
}

inline char* gcvt(double val, int dig, char* buf)
{
        sprintf(buf,"%*.lg",dig,val);
        return buf;
}

Fraction::Fraction(double x)
/*
        Construct a Fraction from a double.
*/
{
        char buf[20];
        int exp;
        double m = frexp(x,&exp);
        register int k;
        if (exp>=0) {
                if (exp > (sizeof(int)*8-2)) setError(NIHCL_FCTNOV,DEFAULT,this,gcvt(x,20,buf));
                k = (sizeof(int)*8-2);
        }
        else {
                k = exp+(sizeof(int)*8-2);
                if (k < 0) setError(NIHCL_FCTNUN,DEFAULT,this,gcvt(x,20,buf));
        }
        n = (int)(m*(1<<k));
        d = 1 << (k-exp);
        reduce();
}

void Fraction::parseFraction(istream& strm)
/*
        Read a Fraction from an istream.
*/
{
        n = 0; d = 1;
        strm >> n;
        char slash;
        strm >> slash;
        if (slash == '/') {
                strm >> d;
                reduce();
        }
        else strm.putback(slash);
}

Fraction::Fraction(istream& strm)       { parseFraction(strm); }

void Fraction::reduce()
/*
        Reduce a Fraction to lowest terms by dividing the numerator and
        denominator by their gcd.
*/
{
        register int d1 = gcd(n,d);
        if (d1 == 1) return;
        n /= d1; d /= d1;
}

Fraction operator+(const Fraction& u, const Fraction& v)
{
        register int d1 = Fraction::gcd(u.d,v.d);
        if (d1 == 1) return Fraction(u.n*v.d+u.d*v.n, u.d*v.d, 0);
        register int t = u.n*(v.d/d1) + v.n*(u.d/d1);
        register int d2 = Fraction::gcd(t,d1);
        return Fraction(t/d2, (u.d/d1)*(v.d/d2), 0);
}

Fraction operator-(const Fraction& u, const Fraction& v)
{
        register int d1 = Fraction::gcd(u.d,v.d);
        if (d1 == 1) return Fraction(u.n*v.d-u.d*v.n, u.d*v.d, 0);
        register int t = u.n*(v.d/d1) - v.n*(u.d/d1);
        register int d2 = Fraction::gcd(t,d1);
        return Fraction(t/d2, (u.d/d1)*(v.d/d2), 0);
}

bool operator<(const Fraction& u, const Fraction& v)
{
        register int d1 = Fraction::gcd(u.d,v.d);
        if (d1 == 1) return u.n*v.d < u.d*v.n;
        return u.n*(v.d/d1) < v.n*(u.d/d1);
}

bool operator<=(const Fraction& u, const Fraction& v)
{
        if (u==v) return YES;
        return u<v;
}

Fraction operator*(const Fraction& u, const Fraction& v)
{
        register int d1 = Fraction::gcd(u.n, v.d);
        register int d2 = Fraction::gcd(u.d, v.n);
        return Fraction((u.n/d1)*(v.n/d2), (u.d/d2)*(v.d/d1), 0);
}

Fraction operator/(const Fraction& u, const Fraction& v)
{
        if (v.n < 0) return u*Fraction(-v.d,-v.n, 0);
        return u*Fraction(v.d,v.n,0);
}

bool Fraction::between(const Fraction& min, const Fraction& max) const
/*
        Return YES if this Fraction is <= to max and >= to min.
*/
{
        return *this >= min && *this <= max;
}

Fraction Fraction::max(const Fraction& f) const
{
        if (f < *this) return *this;
        else return f;
}

Fraction Fraction::min(const Fraction& f) const
{
        if (f > *this) return *this;
        else return f;
}

unsigned Fraction::hash() const { return n^d; }

bool Fraction::isEqual(const Object& ob) const
{
        return ob.isSpecies(classDesc) && *this==castdown(ob);
}

const Class* Fraction::species() const { return &classDesc; }

int Fraction::compare(const Object& ob) const
{
        assertArgSpecies(ob,classDesc,"compare");
        const Fraction& f = castdown(ob);
        if (*this == f) return 0;
        if (*this < f) return -1;
        return 1;
}

void Fraction::deepenShallowCopy()      {}

void Fraction::printOn(ostream& strm) const
{
        if (n == 0 || d == 1) strm << n;
        else strm << n << '/' << d;
}

void Fraction::scanFrom(istream& strm)  { parseFraction(strm); }

Fraction::Fraction(OIOin& strm)
        : BASE(strm)
{
        strm >> n >> d;
}

void Fraction::storer(OIOout& strm) const
{
        BASE::storer(strm);
        strm << n << d;
}

Fraction::Fraction(OIOifd& fd)
        : BASE(fd)
{
        fd >> n >> d;
}

void Fraction::storer(OIOofd& fd) const
{
        BASE::storer(fd);
        fd << n << d;
}
