#ifndef IDENTSET_H
#define IDENTSET_H


/* IdentSet.h -- declarations for Identity Set

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

log:    IDENTSET.H $
Revision 1.2  95/01/29  13:27:12  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:19:54  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:19:51  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Set.h"

class IdentSet: public Set {
        DECLARE_MEMBERS(IdentSet);
        virtual int findIndexOf(const Object&) const;
#ifndef BUG_38
// internal <<AT&T C++ Translator 2.00 06/30/89>> error: bus error (or something nasty like that)
protected:              // storer() functions for object I/O
        virtual void storer(OIOofd& fd) const   { Set::storer(fd); };
        virtual void storer(OIOout& strm) const { Set::storer(strm); };
#endif
public:
        IdentSet(unsigned size =DEFAULT_CAPACITY);
};

#endif
