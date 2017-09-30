#ifndef ASSOCINT_H
#define ASSOCINT_H


/* AssocInt.h -- declarations for key-Integer association

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

log:    ASSOCINT.H $
Revision 1.2  95/01/29  13:27:00  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:08  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:05  kgorlen
 * Release for 1st edition.
 * 
*/

#include "LookupKey.h"
#include "Integer.h"

class AssocInt: public LookupKey {
        DECLARE_MEMBERS(AssocInt);
        Integer avalue;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        AssocInt(Object& newKey =*nil, int newValue =0);
        virtual void deepenShallowCopy();
        virtual Object* value();
        virtual const Object* value() const;
        virtual Object* value(Object& newValue);
};

#endif
