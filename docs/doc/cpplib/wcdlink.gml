:CLFNM.WCDLink
:CMT.========================================================================
:LIBF fmt='hdr'.WCDLink
:HFILE.wclcom.h
:DVFML.
:DVFM.WCSLink
:eDVFML.
:CLSS.
The &cls. is the building block for all of the double linked list classes.
It is implemented in terms of the
.MONO WCSLink
base class.
Since no user data is stored directly with it,
the &cls. should only be used as a base class to derive a user defined class.
:P.
When creating a double linked intrusive list, the &cls. is used to derive the
user defined class that holds the data to be inserted into the list.
:P.
The
.MONO wclcom.h
header file is included by the
.MONO wclist.h
header file.
There is no need to explicitly include the
.MONO wclcom.h
header file unless the
.MONO wclist.h
header file is not included.
No errors will result if it is included.
:P.
Note that the destructor is non-virtual so that list elements are of minimum
size.
Objects created as a class derived from the &cls., but destroyed while
typed as a &obj. will not invoke the destructor of the derived class.
:CMT.The
:CMT.:MONO.WCIsvDList
:CMT.and
:CMT.:MONO.WCValDList
:CMT.are examples of classes derived from the &cls.:PERIOD.
:HDG.Public Member Functions
The following public member functions are declared:
:MFNL.
:MFCD cd_idx='c'.WCDLink();
:MFCD cd_idx='d'.~~WCDLink();
:eMFNL.
:eCLSS.
:SALSO.
:SAL typ='cls'.WCSLink
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.WCDLink
:SNPL.
:SNPFLF                    .#include <wclist.h>
:SNPFLF                    .public:
:SNPCD cd_idx='c'.WCDLink();
:eSNPL.
:SMTICS.
The &fn. creates an &obj.:PERIOD.
The &fn. is used implicitly by the compiler
when it generates a constructor for a derived class.
:RSLTS.
The &fn. produces an initialized &obj.:PERIOD.
:SALSO.
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~WCDLink
:SNPL.
:SNPFLF                   .#include <wclist.h>
:SNPFLF                   .public:
:SNPCD cd_idx='d'.~~WCDLink();
:eSNPL.
:SMTICS.
The &fn. does not do anything explicit.
The call to the &fn. is inserted implicitly by the compiler
at the point where the object derived from &cl_name. goes out of scope.
:RSLTS.
The object derived from &cl_name. is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
