/* OIOnih.h -- implementation of "NIH-format" stream Object I/O classes

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

log:    OIONIH.C $
Revision 1.1  90/05/20  04:20:32  Anthony_Scian
.

 * Revision 3.0  90/05/20  00:20:29  kgorlen
 * Release for 1st edition.
 * 
*/

#include "OIOnih.h"
#include "OIOTbl.h"

static char rcsid[] = "header: D:\PLUSTEST\regress\nih\x:\groupdir\cproj\plustest\regress\nih\rcs\OIONIH.C 1.1 90/05/20 04:20:32 Anthony_Scian Exp Locker: NT_Test_Machine $";

extern const int NIHCL_DRVDCLASSRSP,NIHCL_ILLEGALMFCN,NIHCL_BADARGCL,
        NIHCL_BADARGSP,NIHCL_BADARGCLM,NIHCL_BADARGSPM,NIHCL_BADCLASS,NIHCL_BADSPEC,
        NIHCL_RDEOF,NIHCL_RDFAIL,NIHCL_RDSYNERR,NIHCL_RDREFERR,NIHCL_RDWRONGCLASS,
        NIHCL_RDUNKCLASS,NIHCL_RDVERSERR,NIHCL_RDABSTCLASS,NIHCL_RDBADSIG,
        NIHCL_RDBADTYP,NIHCL_READBINERR,NIHCL_STOREBINERR,
        NIHCL_READBINUNDFL;
        
Object* OIOnihin::readObject(const Class& expectedClass)
{
        char delim;                             // delimiter character 
        const Class* readClass = 0;             // class descriptor pointer
        int objectNum;                          // object reference number 
        unsigned short classNum;                // class reference number
        
// read first character of next object on input stream 
        *strm >> delim;
        checkRead();
        
// parse object reference, class reference, or class name & signature
        switch (delim) {
                case '@': {                     // read object reference 
                        *strm >> objectNum;
                        checkRead();
#ifdef DEBUG_OBJIO
cerr << "readFrom: ref to object #" << objectNum
        << ", class " << (Class::readFromTbl->objectAt(objectNum))->className() << '\n';
#endif
                        return Class::readFromTbl->objectAt(objectNum);
                }
                case '#': {                     // read class reference
                        *strm >> classNum >> delim;
                        checkRead();
                        if (delim != '{') syntaxErr("{",delim);
#ifdef DEBUG_OBJIO
cerr << "readFrom: ref to class #" << classNum << ", class ";
#endif
                        readClass = Class::readFromTbl->classAt(classNum);
#ifdef DEBUG_OBJIO
cerr << *readClass << '\n';
#endif
                        break;
                }
                case ':': {                     // read class name and signature
                        unsigned long signature;        // class signature 
                        {
                                char clsName[256];
                                strm->get(clsName, sizeof clsName, '.');  *strm >> delim;
                                checkRead();
                                if (delim != '.') syntaxErr(".",delim);
                                *strm >> signature >> delim;
                                checkRead();
                                if (delim != '{') syntaxErr("{",delim);
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
                default: syntaxErr(":, # or @",delim);
        }
                
// call class object reader to read object
        Object* target = _readObject(*readClass);
        *strm >> delim;
        checkRead();
        if (delim != '}') syntaxErr("}",delim);
        return target;
}

void OIOnihout::storeObject(const Object& obj)
{
        int objectNum;
        if (Class::storeOnTbl->add(obj,objectNum)) {    // object has not been stored 
                unsigned short classNum;
                if (Class::storeOnTbl->addClass(obj.isA(),classNum))    // object of this class has not been previously stored 
                        *strm << ':' << obj.className() << '.' << obj.isA()->signature();
                else *strm << '#' << classNum;          // object of this class already stored, just output class reference
                *strm << "{\n";
                _storer(obj);                   // call storer for this object 
                *strm << "}\n";
        }
        else *strm << '@' << objectNum << '\n'; // object already stored, just output object reference 
}
