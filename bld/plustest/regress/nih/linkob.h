#ifndef LINKOB_H
#define LINKOB_H


/* LinkOb.h -- declarations for singly-linked list Object element

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

log:    LINKOB.H $
Revision 1.2  95/01/29  13:27:20  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  15:23:44  Anthony_Scian
.

 * Revision 3.0  90/05/20  11:23:39  kgorlen
 * Release for 1st edition
 * 
*/

#include "Link.h"

class LinkOb: public Link {
        DECLARE_MEMBERS(LinkOb);
        Object* val;
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd&) const;
        virtual void storer(OIOout&) const;
public:
        LinkOb(Object& newval =*nil);
        virtual unsigned capacity() const;
        virtual int compare(const Object&) const;
        virtual void deepenShallowCopy();
        virtual void dumpOn(ostream& strm =cerr) const;
        virtual unsigned hash() const;
        virtual bool isEqual(const Object&) const;
        virtual void printOn(ostream& strm =cout) const;
        virtual unsigned size() const;
        virtual Object* value() const;
        virtual Object* value(Object& newval);
};

#endif
