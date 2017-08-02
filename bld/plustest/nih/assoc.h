#ifndef ASSOC_H
#define ASSOC_H


/* Assoc.h -- declarations for key-value association

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

log:    ASSOC.H $
Revision 1.2  95/01/29  13:26:58  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:06  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:02  kgorlen
 * Release for 1st edition.
 * 
*/

#include "LookupKey.h"

class Assoc: public LookupKey {
        DECLARE_MEMBERS(Assoc);
        Object* avalue;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        Assoc(Object& newKey =*nil, Object& newValue =*nil);
        virtual void deepenShallowCopy();
        virtual Object* value();
        virtual const Object* value() const;
        virtual Object* value(Object& newvalue);
};

#endif
