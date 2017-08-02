#ifndef OIONIH_H
#define OIONIH_H


/* OIOnih.h -- declarations for "NIH-format" stream Object I/O classes

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
        May, 1989

Modification History:

log:    OIONIH.H $
Revision 1.2  95/01/29  13:27:28  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/20  04:20:32  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:30  kgorlen
 * Release for 1st edition.
 * 
*/
#include "OIOstream.h"

class OIOnihin : public OIOistream {
protected:
        virtual Object* readObject(const Class&);
public:
        OIOnihin(istream& s) : OIOistream(s) {}
};

class OIOnihout : public OIOostream {
protected:
        virtual void storeObject(const Object&);
public:
        OIOnihout(ostream& s) : OIOostream(s) {}
};

#endif
