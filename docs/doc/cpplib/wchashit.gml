:P.
Hash iterators are used to step through a hash
one or more elements at a time.
Iterators which are
newly constructed or reset are
positioned before the first element in the hash.
The hash may be traversed one element at a time using the pre-increment
or call operator.
An increment operation causing the iterator to be positioned after the end
of the hash returns zero.
Further increments will cause the
.MONO undef_iter
.ix 'undex_iter' 'exception'
exception to be thrown, if it is enabled.
The
.MONO WCIterExcept
class provides the common exception handling
control interface for all of the iterators.
:P.
Since the iterator classes are all template classes, most
of the functionality was derived from common base classes.
In the listing of class member functions, those public member functions
which appear to be in the iterator class but are actually defined in
the common base class are identified as if they were explicitly specified
in the iterator class.
.se lpref=Ptr
.se lret='Value *'
.se tmplat='<Key,Value>'
.se clfnm_cl1=''
.se clobj_cl1=''
.se clfnm_cl2='WCPtrHashDictIter'
.se clobj_cl2='WCPtrHashDict'
:INCLUDE file='wchiter'
.se lpref=Val
.se lret='Value'
.se tmplat='<Key,Value>'
.se clfnm_cl1=''
.se clobj_cl1=''
.se clfnm_cl2='WCValHashDictIter'
.se clobj_cl2='WCValHashDict'
:INCLUDE file='wchiter'
.se lpref=Ptr
.se lret='Type *'
.se tmplat='<Type>'
.se clfnm_cl1='WCPtrHashTableIter'
.se clobj_cl1='WCPtrHashTable'
.se clfnm_cl2='WCPtrHashSetIter'
.se clobj_cl2='WCPtrHashSet'
:INCLUDE file='wchiter'
.se lpref=Val
.se lret='Type'
.se tmplat='<Type>'
.se clfnm_cl1='WCValHashTableIter'
.se clobj_cl1='WCValHashTable'
.se clfnm_cl2='WCValHashSetIter'
.se clobj_cl2='WCValHashSet'
:INCLUDE file='wchiter'
