#ifndef FLOAT_H
#define FLOAT_H


/* Float.h -- declarations for Float object

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

log:    FLOAT.H $
Revision 1.2  95/01/29  13:27:08  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:40  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:37  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Object.h"

class Float: public VIRTUAL Object {
        DECLARE_MEMBERS(Float);
        double val;
        void parseFloat(istream& strm)  { strm >> val; }
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Float(double v =0)              { val = v; }
        Float(istream&);
        double value() const            { return val; }
        double value(double newval)     { return val = newval; }
        virtual int compare(const Object&) const;
        virtual void    deepenShallowCopy();    // {}
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual void scanFrom(istream& strm);
        virtual const Class* species() const;
};

#endif
