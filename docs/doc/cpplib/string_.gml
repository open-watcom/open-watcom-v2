:P.
This class is used to store arbitrarily long sequences of characters in
memory.
Objects of this type may be concatenated, substringed, compared and
searched without the need for memory management by the user.
Unlike a C string, this object has no delimiting character, so any character
in the collating sequence, or character set, may be stored in an object.
:P.
The class documented here is the Open Watcom legacy string class. It
is not related to the
:MONO.std::basic_string
class template nor to its corresponding specialization
:MONO.std::string.
.*
:CLFNM.String
:CMT.========================================================================
:LIBF fmt='hdr'.String
:HFILE.string.hpp
:CLSS.
The &cls. is used to store arbitrarily long sequences of characters in memory.
Objects of this type may be concatenated, substringed, compared and
searched without the need for memory management by the user.
Unlike a C string, a &obj. has no delimiting character, so any character
in the collating sequence, or character set, may be stored in a &obj.:PERIOD.
:HDG.Public Functions
The following constructors and destructors are declared:
:MFNL.
:MFCD cd_idx='c'.String();
:MFCD cd_idx='c'.String( size_t, capacity );
:MFCD cd_idx='c'.String( String const &, size_t = 0, size_t = NPOS );
:MFCD cd_idx='c'.String( char const *, size_t = NPOS );
:MFCD cd_idx='c'.String( char, size_t = 1 );
:MFCD cd_idx='d' .~~String();
:eMFNL.
The following member functions are declared:
:MFNL.
:MFN index='operator char const *'.operator char const *();
:MFN index='operator char'        .operator char() const;
:MFN index='operator ='           .String     &amp.operator  =( String const & );
:MFN index='operator ='           .String     &amp.operator  =( char const * );
:MFN index='operator +='          .String     &amp.operator +=( String const & );
:MFN index='operator +='          .String     &amp.operator +=( char const * );
:MFN index='operator ()'          .String      operator ()( size_t, size_t ) const;
:MFN index='operator ()'          .char       &amp.operator ()( size_t );
:MFN index='operator []'          .char const &amp.operator []( size_t ) const;
:MFN index='operator []'          .char       &amp.operator []( size_t );
:MFN index='operator !'           .int         operator  !() const;
:MFN index='length'               .size_t length() const;
:MFN index='get_at'               .char const &amp.get_at( size_t ) const;
:MFN index='put_at'               .void        put_at( size_t, char );
:MFN index='match'                .int    match( String const & ) const;
:MFN index='match'                .int    match( char const * ) const;
:MFN index='index'                .int    index( String const &, size_t = 0 ) const;
:MFN index='index'                .int    index( char const *, size_t = 0 ) const;
:MFN index='upper'                .String upper() const;
:MFN index='lower'                .String lower() const;
:MFN index='valid'                .int    valid() const;
:MFN index='alloc_mult_size'      .int    alloc_mult_size() const;
:MFN index='alloc_mult_size'      .int    alloc_mult_size( int );
:eMFNL.
The following friend functions are declared:
:RFNL.
:RFN index='operator =='          .friend int    operator ==( String const &, String const & );
:RFN index='operator =='          .friend int    operator ==( String const &,   char const * );
:RFN index='operator =='          .friend int    operator ==(   char const *, String const & );
:RFN index='operator =='          .friend int    operator ==( String const &,   char         );
:RFN index='operator =='          .friend int    operator ==(   char        , String const & );
:RFN index='operator !='          .friend int    operator !=( String const &, String const & );
:RFN index='operator !='          .friend int    operator !=( String const &,   char const * );
:RFN index='operator !='          .friend int    operator !=(   char const *, String const & );
:RFN index='operator !='          .friend int    operator !=( String const &,   char         );
:RFN index='operator !='          .friend int    operator !=(   char        , String const & );
:RFN index='operator <'           .friend int    operator  <( String const &, String const & );
:RFN index='operator <'           .friend int    operator  <( String const &,   char const * );
:RFN index='operator <'           .friend int    operator  <(   char const *, String const & );
:RFN index='operator <'           .friend int    operator  <( String const &,   char         );
:RFN index='operator <'           .friend int    operator  <(   char        , String const & );
:RFN index='operator <='          .friend int    operator <=( String const &, String const & );
:RFN index='operator <='          .friend int    operator <=( String const &,   char const * );
:RFN index='operator <='          .friend int    operator <=(   char const *, String const & );
:RFN index='operator <='          .friend int    operator <=( String const &,   char         );
:RFN index='operator <='          .friend int    operator <=(   char        , String const & );
:RFN index='operator >'           .friend int    operator  >( String const &, String const & );
:RFN index='operator >'           .friend int    operator  >( String const &,   char const * );
:RFN index='operator >'           .friend int    operator  >(   char const *, String const & );
:RFN index='operator >'           .friend int    operator  >( String const &,   char         );
:RFN index='operator >'           .friend int    operator  >(   char        , String const & );
:RFN index='operator >='          .friend int    operator >=( String const &, String const & );
:RFN index='operator >='          .friend int    operator >=( String const &,   char const * );
:RFN index='operator >='          .friend int    operator >=(   char const *, String const & );
:RFN index='operator >='          .friend int    operator >=( String const &,   char         );
:RFN index='operator >='          .friend int    operator >=(   char        , String const & );
:RFN index='operator +'           .friend String operator  +(       String &, String const & );
:RFN index='operator +'           .friend String operator  +(       String &,   char const * );
:RFN index='operator +'           .friend String operator  +(   char const *, String const & );
:RFN index='operator +'           .friend String operator  +(       String &,   char         );
:RFN index='operator +'           .friend String operator  +(   char        , String const & );
:RFN index='valid'                .friend int valid( String const & );
:eRFNL.
The following I/O Stream inserter and extractor functions are declared:
:RFNL.
:RFN index='operator >>'.friend istream &amp.operator >>( istream &, String & );
:RFN index='operator <<'.friend ostream &amp.operator <<( ostream &, String const & );
:eRFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.alloc_mult_size
:SNPL.
:SNPFLF                      .#include <string.hpp>
:SNPFLF                      .public:
:SNPF index='alloc_mult_size'.int String::alloc_mult_size() const;
:SNPF index='alloc_mult_size'.int String::alloc_mult_size( int mult );
:eSNPL.
:SMTICS.
The &fn. is used to query and/or change the allocation multiple size.
:P.
The first form of the &fn. queries the current setting.
:P.
The second form of the &fn. sets the value to a multiple of 8 based on the
:ARG.mult
parameter. The value of
:ARG.mult
is rounded down to a multiple of 8 characters. If
:ARG.mult
is less than 8, the new multiple size is 1 and allocation sizes are exact.
:P.
The scheme used to store a &obj. allocates the memory for the characters in
multiples of some size. By default, this size is 8 characters. A &obj.
with a length of 10 actually has 16 characters of storage allocated
for it. Concatenating more characters on the end of the &obj. only
allocates a new storage block if more than 6 (16-10) characters are
appended. This scheme tries to find a balance between reallocating
frequently (multiples of a small value) and creating a large amount of
unused space (multiples of a large value).
:RSLTS.
The &fn. returns the previous allocation multiple size.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.get_at
:SNPL.
:SNPFLF             .#include <string.hpp>
:SNPFLF             .public:
:SNPF index='get_at'.char const &amp.String::get_at( size_t pos );
:eSNPL.
:SMTICS.
.'se str_sub_pm='constant'
:INCLUDE file='str_subs'.
:SALSO.
:SAL typ='mfun'.put_at
:SAL typ='mfun'.operator~b[]
:SAL typ='mfun'.operator~b()
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.index
:SNPL.
:SNPFLF            .#include <string.hpp>
:SNPFLF            .public:
:SNPF index='index'.int String::index( String const &amp.str, size_t pos = 0 ) const;
:SNPF index='index'.int String::index( char const   *pch, size_t pos = 0 ) const;
:eSNPL.
:SMTICS.
The &fn. computes the offset at which a sequence of characters in the &obj.
is found.
:P.
The first form searches the &obj. for the contents of the
:ARG.str
&obj.:PERIOD.
:P.
The second form searches the &obj. for the sequence of characters pointed
at by
:ARG.pch
:PERIOD.
:P.
If
:ARG.pos
is specified, the search begins at that offset from the start of the
&obj.:PERIOD.
Otherwise, the search begins at offset zero (the first character).
:P.
The &fn. treats upper and lower case letters as not equal.
:RSLTS.
The &fn. returns the offset at which the sequence of characters is found.
If the substring is not found, -1 is returned.
:SALSO.
:SAL typ='mfun'.lower
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.match
:SAL typ='mfun'.upper
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.length
:SNPL.
:SNPFLF             .#include <string.hpp>
:SNPFLF             .public:
:SNPF index='length'.size_t String::length() const;
:eSNPL.
:SMTICS.
The &fn. computes the number of characters contained in the &obj.:PERIOD.
:RSLTS.
The &fn. returns the number of characters contained in the &obj.:PERIOD.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.lower
:SNPL.
:SNPFLF            .#include <string.hpp>
:SNPFLF            .public:
:SNPF index='lower'.String String::lower() const;
:eSNPL.
:SMTICS.
The &fn. creates a &obj. whose value is the same as the original
object's value, except that all upper-case letters have been converted
to lower-case.
:RSLTS.
The &fn. returns a lower-case &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.upper
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.match
:SNPL.
:SNPFLF            .#include <string.hpp>
:SNPFLF            .public:
:SNPF index='match'.int String::match( String const &amp.str ) const;
:SNPF index='match'.int String::match( char const   *pch ) const;
:eSNPL.
:SMTICS.
The &fn. compares two character sequences to find the offset
where they differ.
:P.
The first form compares the &obj. to the
:ARG.str
&obj.:PERIOD.
:P.
The second form compares the &obj. to the
:ARG.pch
C string.
:P.
The first character is at offset zero.
The &fn. treats upper and lower case letters as not equal.
:RSLTS.
The &fn. returns the offset at which the two character sequences differ.
If the character sequences are equal, -1 is returned.
:SALSO.
:SAL typ='mfun'.index
:SAL typ='mfun'.lower
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.upper
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator !
:SNPL.
:SNPFLF                 .#include <string.hpp>
:SNPFLF                 .public:
:SNPF index='operator !'.int String::operator !() const;
:eSNPL.
:SMTICS.
The &fn. tests the validity of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if the &obj. is invalid,
otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.valid
:SAL typ='fun'.valid
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator !=
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPR index='operator !='.friend int operator !=( String const &amp.lft, String const &amp.rht );
:SNPR index='operator !='.friend int operator !=( String const &amp.lft, char const   *rht );
:SNPR index='operator !='.friend int operator !=( char const   *lft, String const &amp.rht );
:SNPR index='operator !='.friend int operator !=( String const &amp.lft, char          rht );
:SNPR index='operator !='.friend int operator !=( char          lft, String const &amp.rht );
:eSNPL.
:SMTICS.
The &fn. compares two sequences of characters in terms of an
:ITALICS.inequality
relationship.
:P.
A &obj. is different from another &obj. if the lengths are
different or they contain different sequences of characters.
A &obj. and a C
string are different if their lengths are different or they contain a
different sequence of characters.  A C string is terminated by a null
character.
A &obj. and a character are different if the &obj. does not contain
only the character.
Upper-case and lower-case characters are considered different.
:RSLTS.
The &fn. returns a non-zero value if the
lengths or sequences of characters in the
:ARG.lft
and
:ARG.rht
parameter are different, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.operator~b<
:SAL typ='mfun'.operator~b<=
:SAL typ='mfun'.operator~b>
:SAL typ='mfun'.operator~b>=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator ()
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPF index='operator ()'.char &amp.String::operator ()( size_t pos );
:eSNPL.
:SMTICS.
.'se str_sub_pm='dynamic'
:INCLUDE file='str_subs'.
:SALSO.
:SAL typ='mfun'.operator~b[]
:SAL typ='mfun'.operator~bchar
:SAL typ='mfun'.operator~bchar~bconst~b*
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator ()
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPF index='operator ()'.String String::operator ()( size_t pos, size_t len ) const;
:eSNPL.
:SMTICS.
This form of the &fn. extracts a sub-sequence of characters from the &obj.:PERIOD.
A new &obj. is created that contains the sub-sequence of characters.
The sub-sequence begins at offset
:ARG.pos
within the &obj. and continues for
:ARG.len
characters.
The first character of a &obj. is at position zero.
:P.
If
:ARG.pos
is greater than or equal to the length of the &obj.,
the result is empty.
:P.
If
:ARG.len
is such that
:ARG.pos
+
:ARG.len
exceeds the length of the object, the result is
the sub-sequence of characters from the &obj. starting at offset
:ARG.pos
and running to the end of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.operator~b[]
:SAL typ='mfun'.operator~bchar
:SAL typ='mfun'.operator~bchar~bconst~b*
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator +
:SNPL.
:SNPFLF                 .#include <string.hpp>
:SNPFLF                 .public:
:SNPR index='operator +'.friend String operator +( String       &amp.lft, String const &amp.rht );
:SNPR index='operator +'.friend String operator +( String       &amp.lft, char const   *rht );
:SNPR index='operator +'.friend String operator +( char const   *lft, String const &amp.rht );
:SNPR index='operator +'.friend String operator +( String       &amp.lft, char          rht );
:SNPR index='operator +'.friend String operator +( char          lft, String const &amp.rht );
:eSNPL.
:SMTICS.
The &fn. concatenates two sequences of characters into a new &obj.:PERIOD.
The new &obj. contains the sequence of characters from the
:ARG.lft
parameter followed by the sequence of characters from the
:ARG.rht
parameter.
:P.
A &null. pointer to a C string is treated as a pointer to an empty C string.
:RSLTS.
The &fn. returns a new &obj. that contains the characters from the
:ARG.lft
parameter followed by the characters from the
:ARG.rht
parameter.
:SALSO.
:SAL typ='mfun'.operator~b+=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator +=
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPF index='operator +='.String &amp.String::operator +=( String const &amp.str );
:SNPF index='operator +='.String &amp.String::operator +=( char const   *pch );
:eSNPL.
:SMTICS.
The &fn. appends the contents of the parameter to the end of the &obj.:PERIOD.
:P.
The first form of the &fn. appends the contents of the
:ARG.str
&obj. to the &obj.:PERIOD.
:P.
The second form appends the null-terminated sequence of characters stored at
:ARG.pch
to the &obj.:PERIOD.
If the
:ARG.pch
parameter is &null., nothing is appended.
:RSLTS.
The &fn. returns a reference to the &obj. that was the target of the assignment.
:SALSO.
:SAL typ='mfun'.operator~b=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator <
:SNPL.
:SNPFLF                 .#include <string.hpp>
:SNPFLF                 .public:
:SNPR index='operator <'.friend int operator <( String const &amp.lft, String const &amp.rht );
:SNPR index='operator <'.friend int operator <( String const &amp.lft, char const   *rht );
:SNPR index='operator <'.friend int operator <( char const   *lft, String const &amp.rht );
:SNPR index='operator <'.friend int operator <( String const &amp.lft, char          rht );
:SNPR index='operator <'.friend int operator <( char          lft, String const &amp.rht );
:eSNPL.
:eSNPL.
:SMTICS.
The &fn. compares two sequences of characters in terms of a
:ITALICS.less-than
relationship.
:P.
:ARG.lft
is less-than
:ARG.rht
if
:ARG.lft
if the characters of
:ARG.lft
occur before the characters of
:ARG.rht
in the collating sequence.
Upper-case and lower-case characters are considered different.
:RSLTS.
The &fn. returns a non-zero value if the
:ARG.lft
sequence of characters is less than the
:ARG.rht
sequence, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.operator~b<=
:SAL typ='mfun'.operator~b>
:SAL typ='mfun'.operator~b>=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator <<
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPR index='operator <<'.friend ostream &amp.operator <<( ostream &amp.strm, String const &amp.str );
:eSNPL.
:SMTICS.
The &fn. is used to write the sequence of characters in the
:ARG.str
&obj. to the
:ARG.strm
:MONO.ostream
object. Like C strings, the value of the
:ARG.str
&obj. is written to
:ARG.strm
without the addition of any characters. No special processing occurs for any
characters in the &obj. that have special meaning for the
:ARG.strm
object, such as carriage-returns. 
:P.
The underlying implementation of the &fn. uses the ostream write method, which 
writes unformatted characters to the output stream. If formatted output is required, 
then the programmer should make use of the classes accessor methods, such as 
c_str(), and pass the resulting data item to the stream using the appropriate 
insert operator.
:RSLTS.
The &fn. returns a reference to the
:ARG.strm
parameter.
:SALSO.
:SAL typ='cls'.ostream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator <=
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPR index='operator <='.friend int operator <=( String const &amp.lft, String const &amp.rht );
:SNPR index='operator <='.friend int operator <=( String const &amp.lft, char const   *rht );
:SNPR index='operator <='.friend int operator <=( char const   *lft, String const &amp.rht );
:SNPR index='operator <='.friend int operator <=( String const &amp.lft, char          rht );
:SNPR index='operator <='.friend int operator <=( char          lft, String const &amp.rht );
:eSNPL.
:SMTICS.
The &fn. compares two sequences of characters in terms of a
:ITALICS.less-than or equal
relationship.
:P.
:ARG.lft
is less-than or equal to
:ARG.rht
if the characters of
:ARG.lft
are equal to or occur before the characters of
:ARG.rht
in the collating sequence.
Upper-case and lower-case characters are considered different.
:RSLTS.
The &fn. returns a non-zero value if the
:ARG.lft
sequence of characters is less than or equal to the
:ARG.rht
sequence, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.operator~b<
:SAL typ='mfun'.operator~b>
:SAL typ='mfun'.operator~b>=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator =
:SNPL.
:SNPFLF                 .#include <string.hpp>
:SNPFLF                 .public:
:SNPF index='operator ='.String &amp.String::operator =( String const &amp.str );
:SNPF index='operator ='.String &amp.String::operator =( char const   *pch );
:eSNPL.
:SMTICS.
The &fn. sets the contents of the &obj. to be the same as the
parameter.
:P.
The first form of the &fn. sets the value of the
&obj. to be the same as the value of the
:ARG.str
&obj.:PERIOD.
:P.
The second form sets the value of the &obj. to the null-terminated
sequence of characters stored at
:ARG.pch
:PERIOD.
If the
:ARG.pch
parameter is &null., the &obj. is empty.
:RSLTS.
The &fn. returns a reference to the &obj. that was the target
of the assignment.
:SALSO.
:SAL typ='mfun'.operator~b+=
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator ==
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPR index='operator =='.friend int operator ==( String const &amp.lft, String const &amp.rht );
:SNPR index='operator =='.friend int operator ==( String const &amp.lft, char const   *rht );
:SNPR index='operator =='.friend int operator ==( char const   *lft, String const &amp.rht );
:SNPR index='operator =='.friend int operator ==( String const &amp.lft, char          rht );
:SNPR index='operator =='.friend int operator ==( char          lft, String const &amp.rht );
:eSNPL.
:SMTICS.
The &fn. compares two sequences of characters in terms of an
:ITALICS.equality
relationship.
:P.
A &obj. is equal to another &obj. if they have the same length and they
contain the same sequence of characters.  A &obj. and a C string are equal
if their lengths are the same and they contain the same sequence of
characters.  The C string is terminated by a null character.
A &obj. and a character are equal if the &obj. contains only that
character.
Upper-case and lower-case characters are considered different.
:RSLTS.
The &fn. returns a non-zero value if the lengths and sequences of
characters in the
:ARG.lft
and
:ARG.rht
parameter are identical, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b<
:SAL typ='mfun'.operator~b<=
:SAL typ='mfun'.operator~b>
:SAL typ='mfun'.operator~b>=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator >
:SNPL.
:SNPFLF                 .#include <string.hpp>
:SNPFLF                 .public:
:SNPR index='operator >'.friend int operator >( String const &amp.lft, String const &amp.rht );
:SNPR index='operator >'.friend int operator >( String const &amp.lft, char const   *rht );
:SNPR index='operator >'.friend int operator >( char const   *lft, String const &amp.rht );
:SNPR index='operator >'.friend int operator >( String const &amp.lft, char          rht );
:SNPR index='operator >'.friend int operator >( char          lft, String const &amp.rht );
:eSNPL.
:SMTICS.
The
&fn. compares two sequences of characters in terms of a
:ITALICS.greater-than
relationship.
:P.
:ARG.lft
is greater-than
:ARG.rht
if the characters of
:ARG.lft
occur after the characters of
:ARG.rht
in the collating sequence.
Upper-case and lower-case characters are considered different.
:RSLTS.
The &fn. returns a non-zero value if the
:ARG.lft
sequence of characters is greater than the
:ARG.rht
sequence, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.operator~b<
:SAL typ='mfun'.operator~b<=
:SAL typ='mfun'.operator~b>=
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.operator >=
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPR index='operator >='.friend int operator >=( String const &amp.lft, String const &amp.rht );
:SNPR index='operator >='.friend int operator >=( String const &amp.lft, char const   *rht );
:SNPR index='operator >='.friend int operator >=( char const   *lft, String const &amp.rht );
:SNPR index='operator >='.friend int operator >=( String const &amp.lft, char          rht );
:SNPR index='operator >='.friend int operator >=( char          lft, String const &amp.rht );
:eSNPL.
:SMTICS.
The &fn. compares two sequences of characters in terms of a
:ITALICS.greater-than or equal
relationship.
:P.
:ARG.lft
is greater-than or equal to
:ARG.rht
if the characters of
:ARG.lft
are equal to or occur after the characters of
:ARG.rht
in the collating sequence.
Upper-case and lower-case characters are considered different.
:RSLTS.
The &fn. returns a non-zero value if the
:ARG.lft
sequence of characters is greater than or equal to the
:ARG.rht
sequence, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!=
:SAL typ='mfun'.operator~b==
:SAL typ='mfun'.operator~b<
:SAL typ='mfun'.operator~b<=
:SAL typ='mfun'.operator~b>
:eSALSO.
:eLIBF.
:P.
:CMT.========================================================================
:LIBF fmt='fun'.operator >>
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPR index='operator >>'.friend istream &amp.operator >>( istream &amp.strm, String &amp.str );
:eSNPL.
:SMTICS.
The &fn. is used to read a sequence of characters from the
:ARG.strm
:MONO.istream
object into the
:ARG.str
&obj.:PERIOD.
Like C strings, the gathering of characters for a
:ARG.str
&obj. ends at the first whitespace encountered, so that the last
character placed in
:ARG.str
is the character before the whitespace.
:RSLTS.
The &fn. returns a reference to the
:ARG.strm
parameter.
:SALSO.
:SAL typ='cls'.istream
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator []
:SNPL.
:SNPFLF                  .#include <string.hpp>
:SNPFLF                  .public:
:SNPF index='operator []'.char const &amp.String::operator []( size_t pos ) const;
:SNPF index='operator []'.char       &amp.String::operator []( size_t pos );
:eSNPL.
:SMTICS.
The &fn. creates either a const or a non-const reference to the character
at offset
:ARG.pos
within the &obj.:PERIOD.
The non-const reference may be used to modify that character.
The first character of a &obj. is at position zero.
:P.
If
:ARG.pos
is greater than or equal to the length of the &obj.,
and the resulting reference is used, the behavior is undefined.
:P.
If the non-const reference is used to modify other characters within the
&obj., the behavior is undefined.
:P.
The reference is associated with the &obj., and therefore has meaning only
as long as the &obj. is not modified (or destroyed).
If the &obj. has been modified and an old reference is used, the
behavior is undefined.
:RSLTS.
The &fn. returns either a const or a non-const reference to a character.
:SALSO.
:SAL typ='mfun'.operator~b()
:SAL typ='mfun'.operator~bchar
:SAL typ='mfun'.operator~bchar~bconst~b*
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator char
:SNPL.
:SNPFLF                    .#include <string.hpp>
:SNPFLF                    .public:
:SNPF index='operator char'.String::operator char();
:eSNPL.
:SMTICS.
The &fn. converts a &obj. into the first character it contains.
If the &obj. is empty, the result is the null character.
:RSLTS.
The &fn. returns the first character contained in the &obj.:PERIOD.
If the
&obj. is empty, the null character is returned.
:SALSO.
:SAL typ='mfun'.operator~b()
:SAL typ='mfun'.operator~b[]
:SAL typ='mfun'.operator~bchar~bconst~b*
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.operator char const *
:SNPL.
:SNPFLF                            .#include <string.hpp>
:SNPFLF                            .public:
:SNPF index='operator char const *'.String::operator char const *();
:eSNPL.
:SMTICS.
The &fn. converts a &obj. into a C string containing the same length and
sequence of characters, terminated by a null character. If the &obj.
contains a null character the resulting C string is terminated by that
null character.
:P.
The returned pointer is associated with the &obj., and therefore has meaning
only as long as the &obj. is not modified. If the intention is
to be able to refer to the C string after the &obj. has been modified, a
copy of the string should be made, perhaps by using the C library
:MONO.strdup
function.
:P.
The returned pointer is a pointer to a constant C string. If the pointer is
used in some way to modify the C string, the behavior is undefined.
:RSLTS.
The &fn. returns a pointer to a null-terminated constant C string
that contains the same characters as the &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.operator~b()
:SAL typ='mfun'.operator~b[]
:SAL typ='mfun'.operator~bchar
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.put_at
:SNPL.
:SNPFLF             .#include <string.hpp>
:SNPFLF             .public:
:SNPF index='put_at'.void String::put_at( size_t pos, char chr );
:eSNPL.
:SMTICS.
The &fn. modifies the character at offset
:ARG.pos
within the &obj.:PERIOD.
The character at the specified offset is set to the value of
:ARG.chr
:PERIOD.
If
:ARG.pos
is greater than the number of characters within the &obj.,
:ARG.chr
is appended to the &obj.:PERIOD.
:P.
:RSLTS.
The &fn. has no return value.
:SALSO.
:SAL typ='mfun'.get_at
:SAL typ='mfun'.operator~b[]
:SAL typ='mfun'.operator~b()
:SAL typ='mfun'.operator~b+=
:SAL typ='mfun'.operator~b+
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.String
:SNPL.
:SNPFLF          .#include <string.hpp>
:SNPFLF          .public:
:SNPCD cd_idx='c'.String::String();
:eSNPL.
:SMTICS.
This form of the &fn. creates a default &obj. containing no characters.
The created &obj. has length zero.
:RSLTS.
This form of the &fn. produces a &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.operator~b=
:SAL typ='mfun'.operator~b+=
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.String
:SNPL.
:SNPFLF          .#include <string.hpp>
:SNPFLF          .public:
:SNPCD cd_idx='c'.String::String( size_t size, String::capacity cap );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj.
:PERIOD.
The function constructs a &obj. of length
:ARG.size
if
:ARG.cap
is equal to the enumerated
:ITALICS.default_size
:PERIOD.
The function reserves
:ARG.size
bytes of memory and sets the length of the &obj. to be zero if
:ARG.cap
is equal to the enumerated
:ITALICS.reserve
:PERIOD.
:RSLTS.
This form of the &fn. produces a &obj. of size
:ARG.size
:PERIOD.
:SALSO.
:SAL typ='mfun'.operator~b=
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.String
:SNPL.
:SNPFLF          .#include <string.hpp>
:SNPFLF          .public:
:SNPCD cd_idx='c'.String::String( String const &amp.str, size_t pos = 0, size_t num = NPOS );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. which contains a sub-string of the
:ARG.str
parameter.
The sub-string starts at position
:ARG.pos
within
:ARG.str
and continues for
:ARG.num
characters or until the end of the
:ARG.str
parameter, whichever comes first.
:RSLTS.
This form of the &fn. produces a sub-string or duplicate of the
:ARG.str
parameter.
:SALSO.
:SAL typ='mfun'.operator~b=
:SAL typ='mfun'.operator~b()
:SAL typ='mfun'.operator~b[]
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.String
:SNPL.
:SNPFLF          .#include <string.hpp>
:SNPFLF          .public:
:SNPCD cd_idx='c'.String::String( char const *pch, size_t num = NPOS );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. from a C string.
The &obj. contains the sequence of characters located at the
:ARG.pch
parameter.
Characters are included up to
:ARG.num
or the end of the C string pointed at by
:ARG.pch
:PERIOD.
Note that C strings are terminated by a null character and that
the value of the created &obj. does not contain that character, nor any
following it.
:RSLTS.
This form of the &fn. produces a &obj. of at most length
:ARG.n
containing the characters in the C string starting at the
:ARG.pch
parameter.
:SALSO.
:SAL typ='mfun'.operator~b=
:SAL typ='mfun'.operator~bchar~bconst~b*
:SAL typ='mfun'.operator~b()
:SAL typ='mfun'.operator~b[]
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='ctor' prot='public'.String
:SNPL.
:SNPFLF          .#include <string.hpp>
:SNPFLF          .public:
:SNPCD cd_idx='c'.String::String( char ch, size_t rep = 1 );
:eSNPL.
:SMTICS.
This form of the &fn. creates a &obj. containing
:ARG.rep
copies of the
:ARG.ch
parameter.
:RSLTS.
This form of the &fn. produces a &obj. of length
:ARG.rep
containing only the character specified by the
:ARG.ch
parameter.
:SALSO.
:SAL typ='mfun'.operator~b=
:SAL typ='mfun'.operator~bchar
:SAL typ='dtor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='dtor' prot='public'.~~String
:SNPL.
:SNPFLF          .#include <string.hpp>
:SNPFLF          .public:
:SNPCD cd_idx='d'.String::~~String();
:eSNPL.
:SMTICS.
The &fn. destroys the &obj.:PERIOD.
The call to the &fn. is inserted implicitly by the compiler
at the point where the &obj. goes out of scope.
:RSLTS.
The &obj. is destroyed.
:SALSO.
:SAL typ='ctor'.
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.upper
:SNPL.
:SNPFLF            .#include <string.hpp>
:SNPFLF            .public:
:SNPF index='upper'.String String::upper() const;
:eSNPL.
:SMTICS.
The &fn. creates a new &obj. whose value is the same as the original &obj.,
except that all lower-case letters have been converted to upper-case.
:RSLTS.
The &fn. returns a new upper-case &obj.:PERIOD.
:SALSO.
:SAL typ='mfun'.lower
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='fun'.valid
:SNPL.
:SNPFLF            .#include <string.hpp>
:SNPFLF            .public:
:SNPR index='valid'.friend int valid( String const &amp.str );
:eSNPL.
:SMTICS.
The &fn. tests the validity of the
:ARG.str
&obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if the
:ARG.str
&obj. is valid, otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!
:SAL typ='mfun'.valid
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.valid
:SNPL.
:SNPFLF            .#include <string.hpp>
:SNPFLF            .public:
:SNPF index='valid'.int String::valid() const;
:eSNPL.
:SMTICS.
The &fn. tests the validity of the &obj.:PERIOD.
:RSLTS.
The &fn. returns a non-zero value if the &obj. is valid,
otherwise zero is returned.
:SALSO.
:SAL typ='mfun'.operator~b!
:SAL typ='fun'.valid
:eSALSO.
:eLIBF.
