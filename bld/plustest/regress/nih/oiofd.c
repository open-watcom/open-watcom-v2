/* OIOfd.c -- Implementation of file Object I/O classes

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
        May, 1989

Function:
        
Modification History:

log:    OIOFD.C $
Revision 1.1  90/05/20  04:20:28  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:26  kgorlen
 * Release for 1st edition.
 * 
*/

#include <ctype.h>
#include "OIOfd.h"
#include "OIOTbl.h"

static char rcsid[] = "header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\OIOFD.C 1.1 90/05/20 04:20:28 Anthony_Scian Exp Locker: NT_Test_Machine $";

extern const int NIHCL_RDBADSIG,NIHCL_RDBADTYP,NIHCL_RDEOF,NIHCL_RDUNKCLASS,NIHCL_RDWRONGCLASS,NIHCL_READBINERR,NIHCL_READBINUNDFL,NIHCL_STOREBINERR,NIHCL_STROV;
        
Object* OIOifd::readObject(const Class& expectedClass)
{
        char recordType =-1;                    // object I/O record type
        const Class* readClass = 0;             // class descriptor pointer
        int objectNum;                          // object reference number 
        unsigned short classNum;                // class reference number

// read record type byte from file descriptor
        *this >> recordType;
        
// parse object reference, class reference, or class name & signature
        switch (recordType) {
                case OIOofd::storeOnObjectRef: {        // read object reference
                        *this >> objectNum;
#ifdef DEBUG_OBJIO
cerr << "readFrom: ref to object #" << objectNum
        << ", class " << (Class::readFromTbl->objectAt(objectNum))->className() << '\n';
#endif
                        return Class::readFromTbl->objectAt(objectNum);
                }
                case OIOofd::storeOnClassRef: {         // read class reference
                        unsigned short classNum;  // class reference number
                        *this >> classNum;
#ifdef DEBUG_OBJIO
cerr << "readFrom: ref to class #" << classNum << ", class ";
#endif
                        readClass = Class::readFromTbl->classAt(classNum);
#ifdef DEBUG_OBJIO
cerr << *readClass << '\n';
#endif
                        break;
                }
                case OIOofd::storeOnClass: {            // read class name and class signature
                        unsigned long signature;        // class signature 
                        {
                                char clsName[256];
                                unsigned char namelen = 0;
                                *this >> namelen;
                                this->get(clsName,namelen);
                                clsName[namelen] = '\0';
                                *this >> signature;
                                if ((readClass = Class::lookup(clsName)) == 0)
                                        setError(NIHCL_RDUNKCLASS, DEFAULT, clsName);
                                if (!readClass->_isKindOf(expectedClass))
                                        setError(NIHCL_RDWRONGCLASS, DEFAULT, expectedClass.name(), clsName);
                                if (readClass->signature() != signature)
                                    setError(NIHCL_RDBADSIG, DEFAULT, clsName, readClass->signature(), signature);
                        }
                        classNum = Class::readFromTbl->add(readClass);
#ifdef DEBUG_OBJIO
cerr << "readFrom: class " << *readClass << '\n';
#endif
                        break;
                }
                default: setError(NIHCL_RDBADTYP,DEFAULT,recordType,fd);
        }

// call class object reader to read object
        return readClass->readObject(*this);
}

void OIOofd::storeObject(const Object& obj)
{
        int objectNum;
        if (Class::storeOnTbl->add(obj,objectNum)) {    // object has not been stored 
                unsigned short classNum;
                if (Class::storeOnTbl->addClass(obj.isA(),classNum)) {  // object of this class has not been previously stored 
                        char recordType = storeOnClass;
                        *this << recordType;
                        unsigned char classNameLen = strlen(obj.className());
                        *this << classNameLen;
                        put(obj.className(),classNameLen);
                        *this << obj.isA()->signature();
                }
                else {                  // object of this class already stored, just output class reference
                        char recordType = storeOnClassRef;
                        *this << recordType;
                        *this << classNum;
                }
                obj.storer(*this);              // call storer for this object 
        }
        else {          // object already stored, just output object reference
                char recordType = storeOnObjectRef; 
                *this << recordType;
                *this << objectNum;
        }
}

extern int errno;
extern char* sys_errlist[];

void OIOofd::writeErr()
{
        setError(NIHCL_STOREBINERR,DEFAULT,sys_errlist[errno]);
}

OIOifd& OIOifd::get(char* buf,unsigned nbyte)
{
        if (nbyte > 0) {
                int retval = read(fd,buf,nbyte);
                if (retval == -1) 
                        setError(NIHCL_READBINERR,DEFAULT,sys_errlist[errno]);
                if (retval == 0) 
                        setError(NIHCL_RDEOF,DEFAULT);
                if (retval < nbyte)
                        setError(NIHCL_READBINUNDFL,DEFAULT,nbyte,retval);
        }
        return *this;
}

OIOifd& OIOifd::getCString(char* s, unsigned maxlen)
{
        unsigned n;
        *this >> n;
        if (n >= maxlen) {
                get(s,maxlen);
                setError(NIHCL_STROV,DEFAULT,maxlen,s,maxlen);
        }
        get(s,n);
        s[n] = '\0';
        return *this;
}

OIOofd& OIOofd::putCString(const char* s)
{
        unsigned n = strlen(s);
        *this << n;
        return put(s,n);
}
