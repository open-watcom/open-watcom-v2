#ifndef FRACTION_H
#define FRACTION_H


/* Fraction.h -- declarations for fractions

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        K. E. Gorlen
        Computer Systems Laboratory, DCRT
        National Institutes of Health
        Bethesda, MD 20892

log:    FRACTION.H $
Revision 1.2  95/01/29  13:27:08  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:42  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:40  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Fraction: public VIRTUAL Object {
        DECLARE_MEMBERS(Fraction);
public:                 // static member functions
        static int gcd(int uu, int vv);
private:
        int n,d;
        Fraction(int num, int den, int dum) {
                n = (dum,num); d = den;
        }
        void parseFraction(istream&);
        void reduce();
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Fraction(int num =0, int den =1);
        Fraction(double);
        Fraction(istream&);
        operator double() const         { return (double)n/d; }
        int denominator() const         { return d; }
        int numerator() const           { return n; }
        
        friend Fraction operator+(const Fraction&, const Fraction&);
        friend Fraction operator-(const Fraction& u)  { return Fraction(-u.n,u.d); }
        friend Fraction operator-(const Fraction&, const Fraction&);
        friend Fraction operator*(const Fraction&, const Fraction&);
        friend Fraction operator/(const Fraction&, const Fraction&);
        friend bool     operator<(const Fraction& u, const Fraction& v);
        friend bool     operator>(const Fraction& u, const Fraction& v)         { return v<u; }
        friend bool     operator<=(const Fraction& u, const Fraction& v);
        friend bool     operator>=(const Fraction& u, const Fraction& v)        { return v<=u; }
        friend bool     operator==(const Fraction& u, const Fraction& v)        { return u.n == v.n && u.d == v.d; }
        friend bool     operator!=(const Fraction& u, const Fraction& v)        { return !(u==v); }
        
        void operator+=(const Fraction& u)      { *this = *this + u; }
        void operator-=(const Fraction& u)      { *this = *this - u; }
        void operator*=(const Fraction& u)      { *this = *this * u; }
        void operator/=(const Fraction& u)      { *this = *this / u; }
        
        bool between(const Fraction& min, const Fraction& max) const;
        Fraction max(const Fraction&) const;
        Fraction min(const Fraction&) const;
        
        virtual int compare(const Object&) const;
        virtual void    deepenShallowCopy();    // {}
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual void scanFrom(istream& strm);
        virtual const Class* species() const;
};

#endif
