:CLFNM.manipulator
:CMT.========================================================================
:LIBF fmt='hdrmnp'.Manipulators
:HFILE.iostream.h and iomanip.h
:CLSS
Manipulators are designed to be inserted into or extracted from a stream.
Manipulators come in two forms, non-parameterized and parameterized.
The non-parameterized manipulators are simpler and are declared in
:MONO.<iostream.h>
:PERIOD.
The parameterized manipulators require more complexity and are declared in
:MONO.<iomanip.h>
:PERIOD.
:P.
:MONO.<iomanip.h>
defines two macros
:MONO.SMANIP_define
and
:MONO.SMANIP_make
to implement parameterized manipulators. The workings of the
:MONO.SMANIP_define
and
:MONO.SMANIP_make
macros are disclosed in the header file and are not discussed here.
:HDG.Non-parameterized Manipulators
The following non-parameterized manipulators are declared in
:MONO.<iostream.h>
:CONT.:
:MANIPL.
:MANIP index='dec'  .ios &amp.dec( ios & );
:MANIP index='hex'  .ios &amp.hex( ios & );
:MANIP index='oct'  .ios &amp.oct( ios & );
:MANIP index='ws'   .istream &amp.ws( istream & );
:MANIP index='endl' .ostream &amp.endl( ostream & );
:MANIP index='ends' .ostream &amp.ends( ostream & );
:MANIP index='flush'.ostream &amp.flush( ostream & );
:eMANIPL.
:HDG.Parameterized Manipulators
The following parameterized manipulators are declared in
:MONO.<iomanip.h>
:CONT.:
:MANIPL.
:MANIP index='resetiosflags'.SMANIP_define( long ) resetiosflags( long );
:MANIP index='setbase'      .SMANIP_define( int )  setbase( int );
:MANIP index='setfill'      .SMANIP_define( int )  setfill( int );
:MANIP index='setiosflags'  .SMANIP_define( long ) setiosflags( long );
:MANIP index='setprecision' .SMANIP_define( int )  setprecision( int );
:MANIP index='setw'         .SMANIP_define( int )  setw( int );
:MANIP index='setwidth'     .SMANIP_define( int )  setwidth( int );
:eMANIPL.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.dec
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPM index='dec'.ios &amp.dec( ios &amp.strm );
:eSNPL.
:SMTICS.
The &fn. sets the
:MONO.ios::basefield
bits for decimal formatting in &fmtflags. in the
:ARG.strm
:MONO.ios
object.
:SALSO.
:SAL typ='omtyp' ocls='ios'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.endl
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPM index='endl'.ostream &amp.endl( ostream &amp.ostrm );
:eSNPL.
:SMTICS.
The &fn. writes a new-line character to the stream specified by the
:ARG.ostrm
parameter and performs a flush.
:SALSO.
:SAL typ='ofun' ocls='ostream'.flush
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.ends
:SNPL.
:SNPFLF           .#include <iostream.h>
:SNPM index='ends'.ostream &amp.ends( ostream &amp.ostrm );
:eSNPL.
:SMTICS.
The &fn. writes a null character to the stream specified by the
:ARG.ostrm
parameter.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.flush
:SNPL.
:SNPFLF            .#include <iostream.h>
:SNPM index='flush'.ostream &amp.flush( ostream &amp.ostrm );
:eSNPL.
:SMTICS.
The &fn. flushes the stream specified by the
:ARG.ostrm
parameter.
The flush is performed in the same manner as the
:MONO.flush
member function.
:SALSO.
:SAL typ='ofun' ocls='ostream'.flush
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.hex
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPM index='hex'.ios &amp.hex( ios &amp.strm );
:eSNPL.
:SMTICS.
The &fn. sets the
:MONO.ios::basefield
bits for hexadecimal formatting in &fmtflags. in the
:ARG.strm
:MONO.ios
object.
:SALSO.
:SAL typ='omtyp' ocls='ios'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.oct
:SNPL.
:SNPFLF          .#include <iostream.h>
:SNPM index='oct'.ios &amp.oct( ios &amp.strm );
:eSNPL.
:SMTICS.
The &fn. sets the
:MONO.ios::basefield
bits for octal formatting in &fmtflags. in the
:ARG.strm
:MONO.ios
object.
:SALSO.
:SAL typ='omtyp' ocls='ios'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.resetiosflags
:SNPL.
:SNPFLF                    .#include <iomanip.h>
:SNPM index='resetiosflags'.SMANIP_define( long ) resetiosflags( long flags )
:eSNPL.
:SMTICS.
The &fn. turns off the bits in &fmtflags. that correspond to the bits
that are on in the
:ARG.flags
parameter.
No other bits are affected.
:SALSO.
:SAL typ='ofun' ocls='ios'.flags
:SAL typ='omtyp' ocls='ios'.fmtflags
:SAL typ='ofun' ocls='ios'.setf
:SAL typ='ofun' ocls='ios'.unsetf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.setbase
:SNPL.
:SNPFLF              .#include <iomanip.h>
:SNPM index='setbase'.SMANIP_define( int ) setbase( int base );
:eSNPL.
:SMTICS.
The &fn. sets the
:MONO.ios::basefield
bits in &fmtflags. to the value specified by the
:ARG.base
parameter within the stream that the &fn. is operating upon.
:SALSO.
:SAL typ='omtyp' ocls='ios'.fmtflags
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.setfill
:SNPL.
:SNPFLF              .#include <iomanip.h>
:SNPM index='setfill'.SMANIP_define( int ) setfill( int fill )
:eSNPL.
:SMTICS.
The &fn. sets the &fillchar. to the value specified by the
:ARG.fill
parameter within the stream that the &fn. is operating upon.
:SALSO.
:SAL typ='ofun' ocls='ios'.fill
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.setiosflags
:SNPL.
:SNPFLF                  .#include <iomanip.h>
:SNPM index='setiosflags'.SMANIP_define( long ) setiosflags( long flags );
:eSNPL.
:SMTICS.
The &fn. turns on the bits in &fmtflags. that correspond to the bits
that are on in the
:ARG.flags
parameter.
No other bits are affected.
:SALSO.
:SAL typ='ofun' ocls='ios'.flags
:SAL typ='omtyp' ocls='ios'.fmtflags
:SAL typ='ofun' ocls='ios'.setf
:SAL typ='ofun' ocls='ios'.unsetf
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.setprecision
:SNPL.
:SNPFLF                   .#include <iomanip.h>
:SNPM index='setprecision'.SMANIP_define( int ) setprecision( int prec );
:eSNPL.
:SMTICS.
The &fn. sets the &fmtprec. to the value specified by the
:ARG.prec
parameter within the stream that the &fn. is operating upon.
:SALSO.
:SAL typ='ofun' ocls='ios'.precision
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.setw
:SNPL.
:SNPFLF           .#include <iomanip.h>
:SNPM index='setw'.SMANIP_define( int ) setw( int wid );
:eSNPL.
:SMTICS.
The &fn. sets the &fmtwidth. to the value specified by the
:ARG.wid
parameter within the stream that the &fn. is operating upon.
:SALSO.
:SAL typ='ofun' ocls='ios'.width
:SAL typ='mnp'.setwidth
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.setwidth
:SNPL.
:SNPFLF               .#include <iomanip.h>
:SNPM index='setwidth'.SMANIP_define( int ) setwidth( int wid );
:eSNPL.
:SMTICS.
The &fn. sets the &fmtwidth. to the value specified by the
:ARG.wid
parameter within the stream that the &fn. is operating upon.
:P.
This function is a WATCOM extension.
:SALSO.
:SAL typ='ofun' ocls='ios'.width
:SAL typ='mnp'.setw
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mnp'.ws
:SNPL.
:SNPFLF         .#include <iostream.h>
:SNPM index='ws'.istream &amp.ws( istream &amp.istrm );
:eSNPL.
:SMTICS.
The &fn. extracts and discards whitespace characters from the
:ARG.istrm
parameter, leaving
the stream positioned at the next non-whitespace character.
:P.
The &fn. is needed particularly when the
:MONO.ios::skipws
bit is not set in &fmtflags. in the
:ARG.istrm
object. In this case, whitespace characters must be explicitly removed
from the stream, since the formatted input operations will not
automatically remove them.
:SALSO.
:SAL typ='ofun' ocls='istream'.eatwhite
:SAL typ='ofun' ocls='istream'.ignore
:eSALSO.
:eLIBF.
