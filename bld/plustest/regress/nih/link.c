/* Link.c -- implementation of singly-linked list element

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
        October, 1985

Function:
        
Link is an abstract class that is used to construct LinkedLists.  It
contains a pointer to the next Link in the list, or nil if this is the
last Link.

log:    LINK.C $
Revision 1.1  90/05/20  21:12:04  Anthony_Scian
.

 * Revision 3.0  90/05/20  17:11:56  kgorlen
 * Release for 1st edition.
 * 
*/

#include "Link.h"
#include "LinkOb.h"

#define THIS    Link
#define BASE    Object
#define BASE_CLASSES BASE::desc()
#define MEMBER_CLASSES
#define VIRTUAL_BASE_CLASSES Object::desc()

DEFINE_ABSTRACT_CLASS(Link,1,"header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\LINK.C 1.1 90/05/20 21:12:04 Anthony_Scian Exp Locker: NT_Test_Machine $",NULL,NULL);

extern const int NIHCL_DELLNK;

static LinkOb _nilLink;
Link *const Link::nilLink = &_nilLink;

Link::~Link()
{
        if (next != NULL) setError(NIHCL_DELLNK,DEFAULT,className(),next,this);
}

Object* Link::copy() const      { return deepCopy(); }

void Link::deepenShallowCopy()
{
}

Link::Link(OIOifd& fd) : BASE(fd) { next = NULL; }

Link::Link(OIOin& strm) : BASE(strm) { next = NULL; }
