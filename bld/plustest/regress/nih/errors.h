#ifndef ERRORSH
#define ERRORSH


/* errors.h -- Error facility declarations

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

log:    ERRORS.H $
Revision 1.2  95/01/29  13:27:06  NT_Test_Machine
*** empty log message ***

Revision 1.1  90/05/16  02:37:04  Anthony_Scian
.

 * Revision 3.0  90/05/15  22:37:02  kgorlen
 * Release for 1st edition.
 * 
*/

#include <stdio.h>
#include <iostream.h>

const int MAX_MSG_ARG = 8;      /* max numbers of args per msg */
const int MAX_FACILITIES = 32;  /* max no. of facilities */
const int SYS_LAST_ERROR = 34;  /* last UNIX system error */

/* severity levels */
enum severity_level { SUCCESS = 0,
                      INFORMATION = -1,
                      INFO = -1,
                      WARNING = -2,
                      ERROR = -3,
                      FATAL = -4,
                      DEFAULT = 1 };

/* Error Code Definition */
const int FAC_SHIFT =    12;        // right shift count to get facility code
const int OFFSET_MASK =  0xfff;     // mask for offset portion of error 
const int FAC_MASK =     0xfff000;  // mask for facility portion 

inline int FACILITY_CODE(int ER) { return ((ER&FAC_MASK)>>FAC_SHIFT); }
inline int OFFSET_INDEX(int ER)  { return (ER&OFFSET_MASK); }

class ErrSpecs {
public:
        int severity;           /* severity of this error */
        char* args;             /* error msg args: I=int, S=string,D=double */
        char* text;             /* printf format string for error text */
        };


class ErrFac {
public:
        char* longname;         /* facility long name for the error msgs */
        ErrSpecs* errlist;      /* all errors for this facility */
        int last;               /* last error in this facility */
};

extern "C" {
void seterropt(int esev, int psev, int dump, int fmt, FILE*);
void geterropt(int& esev,int& psev,int& dump,int& fmt,FILE*&);
int seterror(int err, int sev, ...);

ErrFac* errfac_at(int err);
int errfac_severity(int err);
char* errfac_errorText(int err, char* buf =0);
void errfac_prepText(char* p, char* q);
void errfac_prepNextArg(char* p,char* q);
char* errfac_argumentCodes(int err);
void errfac_errorMessage(int sev,char* msg);
int errfac_printSeverity();
int errfac_exitSeverity();
int errfac_isPrintSeverity(int s);
int errfac_isExitSeverity(int s);
}
#endif /* ERRORSH */
