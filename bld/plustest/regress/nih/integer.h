#ifndef INTEGER_H
#define INTEGER_H


/* Integer.h -- declarations for Integer object

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

log:    INTEGER.H $
Revision 1.2  95/01/29  13:27:12  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:56  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:54  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Integer: public VIRTUAL Object {
        DECLARE_MEMBERS(Integer);
        long val;
        void parseInteger(istream& strm)        { strm >> val; }
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Integer(long v =0)              { val = v; }
        Integer(istream&);
        long value() const              { return val; }
        long value(long newval)         { return val = newval; }
        virtual int compare(const Object&) const;
        virtual void deepenShallowCopy();       // {}
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual void scanFrom(istream& strm);
        virtual const Class* species() const;
};

#endif
