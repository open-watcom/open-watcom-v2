#ifndef ERRFACH
#define ERRFACH


/* ErrFac.h -- header file for Class ErrorFacility

        THIS SOFTWARE FITS THE DESCRIPTION IN THE U.S. COPYRIGHT ACT OF A
        "UNITED STATES GOVERNMENT WORK".  IT WAS WRITTEN AS A PART OF THE
        AUTHOR'S OFFICIAL DUTIES AS A GOVERNMENT EMPLOYEE.  THIS MEANS IT
        CANNOT BE COPYRIGHTED.  THIS SOFTWARE IS FREELY AVAILABLE TO THE
        PUBLIC FOR USE WITHOUT A COPYRIGHT NOTICE, AND THERE ARE NO
        RESTRICTIONS ON ITS USE, NOW OR SUBSEQUENTLY.

Author:
        S.M. Orlow
        Systex,Inc.
        Beltsville, MD 20705
        301-474-0111
        June, 1986

Contractor:
        K. E. Gorlen
        Bg. 12A, Rm. 2017
        Computer Systems Laboratory
        Division of Computer Research and Technology
        National Institutes of Health
        Bethesda, Maryland 20892
        Phone: (301) 496-5363
        uucp: uunet!nih-csl!kgorlen
        Internet: kgorlen@alw.nih.gov

log:    ERRFAC.H $
Revision 1.2  95/01/29  13:27:04  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/16  02:36:54  Anthony_Scian
.

 * Revision 3.0  90/05/15  22:36:52  kgorlen
 * Release for 1st edition.
 * 
*/

#include "errors.h"

class ErrorSpecs {
public:
        int severity;           /* severity of this error */
        char* args;             /* error msg args: I=int, S=string,D=double */
        char* text;             /* printf format string for error text */

        ErrorSpecs(int err_sev,char* err_args,char* err_text);
        void printOn(ostream&); 
        };

typedef ErrorSpecs* ErrorSpecsPt;

class ErrorFacility {
        int Code;               // facility code 
        char* shortname;        // facility short name for prefixing 
        char* basename;         // facility basename for source files
        char* longname;         // facility long name for the error msgs 
        ErrorSpecsPt* errlist;  // all errors for this facility 
        int last;               // last error in this facility
        int capacity;           // no. of errors in facility 
        int size;               // current size of error list 
        void resize(int newcapacity);
public:
        ErrorFacility(char* long_name,char* short_name,
                        int fac_code,char* base_name);
        void add(ErrorSpecs&);
        void h_init(ostream&);
        void h_define(ostream&,char*,int);
        void h_finish(ostream&);
        int lastError()         { return last; }
        int code()              { return Code; }
        void printOn(ostream&);
        };
#endif /* ERRFACH */
