.DM ITERATOR_UNDEFINED BEGIN
If the iterator is not associated with a hash, or the iterator
position is either before the first element or past the last element in
the hash, the current iterator position is undefined.
.DM ITERATOR_UNDEFINED END
.*
.se *cl_cl2='&clfnm_cl2.&tmplat.'
:CMT.========================================================================
.if '&clfnm_cl1.' eq '' .th .do begin
:CLFNM.&clfnm_cl2.&tmplat.
:LIBF fmt='hdr'.&clfnm_cl2.&tmplat.
.do end
.el .do begin
:CLFNM cl2='&clfnm_cl1.&tmplat.'.&clfnm_cl2.&tmplat.
:LIBF fmt='hdr'.&clfnm_cl2.&tmplat., &clfnm_cl1.&tmplat.
.do end
:HFILE.wchiter.h
:CLSS.
.if '&clfnm_cl1.' eq '' .th .do begin
The &cls. is the templated class used to create iterator objects for
.mno &clobj_cl2.&tmplat.
objects.
.do end
.el .do begin
The &cls. are the templated classes used to create iterator objects for
.mno &clobj_cl1.&tmplat.
and
.mno &clobj_cl2.&tmplat.
objects.
.do end
In the description of each member function, the text
.if '&clfnm_cl1.' ne '' .th .do begin
.MONO Type
is used to indicate the hash element type specified as the template parameter.
.do end
.el .do begin
.MONO Key
is used to indicate the template parameter defining
.if &lpref. eq Val .do begin
the type of the indices used to store data in the dictionary.
.do end
.el .do begin
the type of the indices pointed to by the pointers stored in the dictionary.
.do end
The text
.MONO Value
is used to indicate the template parameter defining
.if &lpref. eq Val .do begin
the type of the data stored in the dictionary.
.do end
.el .do begin
the type of the data pointed to by the pointers stored in the dictionary.
.do end
.do end
:INCLUDE file='_IEXT_BC'.
:HDG.Public Member Functions
The following member functions are declared in the public interface:
:MFNL.
:MFCD cd_idx='c' .&clfnm_cl2.();
:MFCD cd_idx='c' .&clfnm_cl2.( const &clobj_cl2.&tmplat. & );
:MFCD cd_idx='d' .~~&clfnm_cl2.();
.if '&clfnm_cl1.' ne '' .th .do begin
:MFCD cd_idx='c' .&clfnm_cl1.();
:MFCD cd_idx='c' .&clfnm_cl1.( const &clobj_cl1.&tmplat. & );
:MFCD cd_idx='d' .~~&clfnm_cl1.();
:MFN index='container'      .const &clobj_cl1.&tmplat. *container() const;
.do end
:MFN index='container'      .const &clobj_cl2.&tmplat. *container() const;
.if '&clfnm_cl1.' ne '' .th .do begin
.if '&lpref.' eq 'Val' .th .do begin
:MFN index='current'        .Type current() const;
.do end
.el .do begin
:MFN index='current'        .Type *current() const;
.do end
.do end
.if '&clfnm_cl1.' eq '' .th .do begin
.if '&lpref.' eq 'Val' .th .do begin
:MFN index='key'            .Key key();
.do end
.el .do begin
:MFN index='key'            .Key *key();
.do end
.do end
:MFN index='reset'          .void reset();
.if '&clfnm_cl1.' eq '' .th .do begin
:MFN index='reset'          .void reset( &clobj_cl2.&tmplat. & );
.do end
.el .do begin
:MFN index='reset'          .void &clfnm_cl2.&tmplat.::reset( &clobj_cl2.&tmplat. & );
:MFN index='reset'          .void &clfnm_cl1.&tmplat.::reset( &clobj_cl1.&tmplat. & );
.do end
.if '&clfnm_cl1.' eq '' .th .do begin
.if '&lpref.' eq 'Val' .th .do begin
:MFN index='value'          .Value value();
.do end
.el .do begin
:MFN index='value'          .Value * value();
.do end
.do end
:eMFNL.
:HDG.Public Member Operators
The following member operators are declared in the public interface:
:MFNL.
:MFN index='operator ()'     .int operator ()();
:MFN index='operator ++'     .int operator ++();
:eMFNL.
:eCLSS.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='&*cl_cl2.' fmt='ctor' prot='public'.&clfnm_cl2.
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='&clfnm_cl2.'.&clfnm_cl2.();
:eSNPL.
:SMTICS.
The &fn. is the default constructor for the class and initializes the
iterator with no hash to operate on.  The
.MONO reset
member function must be called to provide the iterator with a hash to
iterate over.
:RSLTS.
The &fn. creates an initialized
.MONO &clfnm_cl2.
hash iterator object.
:SALSO.
:SAL typ='fun'.~~&clfnm_cl2.
.if '&clfnm_cl1.' ne '' .th .do begin
:SAL typ='fun'.&clfnm_cl1.
.do end
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='&*cl_cl2.' fmt='ctor' prot='public'.&clfnm_cl2.
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='&clfnm_cl2.'.&clfnm_cl2.( &clobj_cl2.&tmplat. & );
:eSNPL.
:SMTICS.
The &fn. is a constructor for the class.
The value passed as a parameter is a
.MONO &clobj_cl2.
hash object.
The iterator will be initialized for that hash object and positioned
before the first hash element.
To position the iterator to a valid element within the hash, increment
it using one of the
.MONO operator ++
or
.MONO operator ()
operators.
:RSLTS.
The &fn. creates an initialized
.MONO &clfnm_cl2.
hash iterator object positioned before the first element in the hash.
:SALSO.
:SAL typ='fun'.~~&clfnm_cl2.
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='&*cl_cl2.' fmt='dtor' prot='public'.~~&clfnm_cl2.
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='~~&clfnm_cl2.'.~~&clfnm_cl2.();
:eSNPL.
:SMTICS.
The &fn. is the destructor for the class.
The call to the destructor is inserted implicitly by the compiler at
the point where the
.MONO &clfnm_cl2.
hash iterator object goes out of scope.
:RSLTS.
The
.MONO &clfnm_cl2.
hash iterator object is destroyed.
:SALSO.
:SAL typ='fun'.&clfnm_cl2.
.if '&clfnm_cl1.' ne '' .th .do begin
:SAL typ='fun'.&clfnm_cl1.
.do end
:eSALSO.
:eLIBF.

.if '&clfnm_cl1.' ne '' .th .do begin
:CMT.========================================================================
:LIBF cltype='&clfnm_cl1.&tmplat.' fmt='ctor' prot='public'.&clfnm_cl1.
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='&clfnm_cl1.'.&clfnm_cl1.();
:eSNPL.
:SMTICS.
The public
.MONO &clfnm_cl1.&tmplat.
constructor is the default constructor for the class and initializes the
iterator with no hash to operate on.  The
.MONO reset
member function must be called to provide the iterator with a hash to
iterate over.
:RSLTS.
The public
.MONO &clfnm_cl1.&tmplat.
constructor creates an initialized
.MONO &clfnm_cl1.
hash iterator object.
:SALSO.
:SAL typ='fun'.~~&clfnm_cl1.
:SAL typ='fun'.&clfnm_cl2.
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='&clfnm_cl1.&tmplat.' fmt='ctor' prot='public'.&clfnm_cl1.
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='&clfnm_cl1.'.&clfnm_cl1.( &clobj_cl1.&tmplat. & );
:eSNPL.
:SMTICS.
The public
.MONO &clfnm_cl1.&tmplat.
constructor is a constructor for the class.
The value passed as a parameter is a
.MONO &clobj_cl1.
hash object.
The iterator will be initialized for that hash object and positioned before the
first hash element.
To position the iterator to a valid element within the hash, increment
it using one of the
.MONO operator ++
or
.MONO operator ()
operators.
:RSLTS.
The public
.MONO &clfnm_cl1.&tmplat.
constructor creates an initialized
.MONO &clfnm_cl1.
hash iterator object positioned before the first element in the hash.
:SALSO.
:SAL typ='fun'.~~&clfnm_cl1.
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.reset
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF cltype='&clfnm_cl1.&tmplat.' fmt='dtor' prot='public'.~~&clfnm_cl1.
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='~~&clfnm_cl1.'.~~&clfnm_cl1.();
:eSNPL.
:SMTICS.
The
.MONO &clfnm_cl1.&tmplat.
destructor is the destructor for the class.
The call to the destructor is inserted implicitly by the compiler at
the point where the
.MONO &clfnm_cl1.
hash iterator object goes out of scope.
:RSLTS.
The
.MONO &clfnm_cl1.
hash iterator object is destroyed.
:SALSO.
:SAL typ='fun'.&clfnm_cl2.
:SAL typ='fun'.&clfnm_cl1.
:eSALSO.
:eLIBF.
.do end

:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.container
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
.if '&clfnm_cl1.' eq '' .th .do begin
:SNPF index='container'.&clobj_cl2.&tmplat. *container() const;
.do end
.el .do begin
:SNPF index='container'.&clobj_cl1.&tmplat. *&clfnm_cl1.&tmplat.::container() const;
:SNPF index='container'.&clobj_cl2.&tmplat. *&clfnm_cl2.&tmplat.::container() const;
.do end
:eSNPL.
:SMTICS.
The &fn. returns a pointer to the hash container object.
If the iterator has not been initialized with a hash object, and the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is enabled, the exception is thrown.
:RSLTS.
A pointer to the hash object associated with the iterator is returned,
or NULL(0) if the iterator has not been initialized with a hash.
:SALSO.
:SAL typ='fun'.&clfnm_cl2.
.if '&clfnm_cl1.' ne '' .th .do begin
:SAL typ='fun'.&clfnm_cl1.
.do end
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.

.if '&clfnm_cl1.' ne '' .th .do begin
.if &lpref. eq Val .th .do begin
.se lret='Type '
.do end
.el .do begin
.se lret='Type *'
.do end
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.current
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='current'.&lret.current();
:eSNPL.
:SMTICS.
.if &lpref. eq Val .th .do begin
The &fn. returns the value of the hash element at the current iterator
position.
.do end
.el .do begin
The &fn. returns a pointer to the hash item at the current iterator
position.
.do end
:P.
.ITERATOR_UNDEFINED
In this case the
.MONO undef_item
.ix 'undef_item'
exception is thrown, if enabled.
:RSLTS.
.if &lpref. eq Val .th .do begin
The value at the current iterator element is returned.
If the current element is undefined, a default initialized object is returned.
.do end
.el .do begin
A pointer to the current iterator element is returned.
If the current element is undefined,
NULL(0) is returned.
.do end
:SALSO.
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_item
:eSALSO.
:eLIBF.
.do end

.if '&clfnm_cl1.' eq '' .th .do begin
.if &lpref. eq Val .th .do begin
.se lret='Key '
.do end
.el .do begin
.se lret='Key *'
.do end
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.key
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='key'.&lret.key();
:eSNPL.
:SMTICS.
.if &lpref. eq Val .th .do begin
The &fn. returns the value of
.MONO Key
at the current iterator position.
.do end
.el .do begin
The &fn. returns a pointer to the
.MONO Key
value of the hash item at the current iterator position.
.do end
:P.
.ITERATOR_UNDEFINED
In this case the
.MONO undef_item
.ix 'undef_item'
exception is thrown, if enabled.
:RSLTS.
.if &lpref. eq Val .th .do begin
The value of
.MONO Key
at the current iterator element is returned.
If the current element is undefined, a default initialized object is returned.
.do end
.el .do begin
A pointer to
.MONO Key
at the current iterator element is returned.
If the current element is undefined,
an undefined pointer is returned.
.do end
:SALSO.
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_item
:eSALSO.
:eLIBF.
.do end

:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator ()
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='operator ()'.int operator ()();
:eSNPL.
:SMTICS.
The &fn. is the call operator for the class.
The hash element which follows the current item is set to be the
new current item.  If the previous current item was the last element in the
hash, the iterator is positioned after the end of the hash.
:P.
The &fn. has the same semantics as the pre-increment operator,
.MONO operator ++.
:P.
If the iterator was positioned before the first hash element,
the current item will be set to the first element.
If the hash is empty, the iterator will be positioned after the end of
the hash.
:P.
If the iterator is not associated with a hash or the iterator
position before the increment was past the last element the hash, the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is thrown, if enabled.
:RSLTS
The &fn. returns a non-zero value if the iterator is positioned on a
hash item.
Zero(0) is returned when the iterator is incremented past the end of the hash.
:SALSO.
:SAL typ='fun'.operator~b++
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' cllong='yes' prot='public'.operator ++
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='operator ++'.int operator ++();
:eSNPL.
:SMTICS.
The &fn. is the pre-increment operator for the class.
The hash element which follows the current item is set to be the
new current item.
If the previous current item was the last element in the hash,
the iterator is positioned after the end of the hash.
:P.
The &fn. has the same semantics as the call operator,
.MONO operator ().
:P.
The current item will be set to the first hash element
if the iterator was positioned before the first element in the hash.
If the hash is empty, the iterator will be positioned after the end of
the hash.
:P.
If the iterator is not associated with a hash or the iterator
position before the increment was past the last element the hash, the
.MONO undef_iter
.ix 'undef_iter' 'exception'
exception is thrown, if enabled.
:RSLTS
The &fn. returns a non-zero value if the iterator is positioned on a
hash item.
Zero(0) is returned when the iterator is incremented past the end of the hash.
:SALSO.
.if '&clfnm_cl1.' ne '' .th .do begin
:SAL typ='fun'.current
.do end
:SAL typ='fun'.operator~b()
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_iter
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.reset
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='reset'    .void reset();
:eSNPL.
:SMTICS.
The &fn. resets the iterator to the initial state, positioning the iterator
before the first element in the associated hash.
:RSLTS
The iterator is positioned before the first hash element.
:SALSO.
:SAL typ='fun'.&clfnm_cl2.
.if '&clfnm_cl1.' ne '' .th .do begin
:SAL typ='fun'.&clfnm_cl1.
.do end
:SAL typ='fun'.container
:eSALSO.
:eLIBF.
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.reset
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
.if '&clfnm_cl1.' eq '' .th .do begin
:SNPF index='reset'    .void reset( &clobj_cl2.&tmplat. & );
.do end
.el .do begin
:SNPF index='reset'    .void &clfnm_cl2.&tmplat.::reset( &clobj_cl2.&tmplat. & );
:SNPF index='reset'    .void &clfnm_cl1.&tmplat.::reset( &clobj_cl1.&tmplat. & );
.do end
:eSNPL.
:SMTICS.
The &fn. resets the iterator to operate on the specified hash.
The iterator is positioned before the first element in the hash.
:RSLTS
The iterator is positioned before the first element of the specified hash.
:SALSO.
:SAL typ='fun'.&clfnm_cl2.
.if '&clfnm_cl1.' ne '' .th .do begin
:SAL typ='fun'.&clfnm_cl1.
.do end
:SAL typ='fun'.container
:eSALSO.
:eLIBF.

.if '&clfnm_cl1.' eq '' .th .do begin
.if &lpref. eq Val .th .do begin
.se lret='Value '
.do end
.el .do begin
.se lret='Value *'
.do end
:CMT.========================================================================
:LIBF fmt='mfun' prot='public'.value
:SNPL.
:SNPFLF          .#include <wchiter.h>
:SNPFLF          .public:
:SNPF index='value'.&lret.value();
:eSNPL.
:SMTICS.
.if &lpref. eq Val .th .do begin
The &fn. returns the value of
.MONO Value
at the current iterator position.
.do end
.el .do begin
The &fn. returns a pointer to the
.MONO Value
the current iterator position.
.do end
:P.
.ITERATOR_UNDEFINED
In this case the
.MONO undef_item
.ix 'undef_item'
exception is thrown, if enabled.
:RSLTS.
.if &lpref. eq Val .th .do begin
The value of the
.MONO Value
at the current iterator element is returned.
If the current element is undefined, a default initialized object is returned.
.do end
.el .do begin
A pointer to the
.MONO Value
at the current iterator element is returned.
If the current element is undefined,
an undefined pointer is returned.
.do end
:SALSO.
:SAL typ='fun'.operator~b()
:SAL typ='fun'.operator~b++
:SAL typ='fun'.reset
:SAL typ='omtyp' ocls='WCIterExcept'.undef_item
:eSALSO.
:eLIBF.
.do end

.*
.DM ITERATOR_UNDEF DELETE
