
#ifndef EXCEPT_HPP
#define EXCEPT_HPP
#include <stddef.h>
#include <string.h>
#include <stdio.h>

//----------------------------------------------------------------
// WELCOME TO EHMIX Version 1.00, 5/12/94
//
// An EXPERIMENTAL MIXIN STYLE EXCEPTION CLASS HIERARCHY
//
// Purpose:
//
// This code is primarily intended to familiarise
// people with mixins and develop a useful exception class
// hierarchy with an aim to inclusion in the
// C++ Standard Library.
//
// The class set in the first section should be somewhat
// richer than what is required for the Standard Library.
// We need enough classes so some will be _excluded_.
//
// The test code is primarily intended to demonstrate
// how a mixin EH library might be used "as is" or extended
// by users or implementors.
//
// Arguments about convenience, ease of use, and extensibility
// make more sense with experience (however limited).
//
// The exact structure of the hierarchy including the names of
// the classes requires further consideration. Some of the names
// I have use are deliberately different from the current
// Working Paper names exactly to avoid a clash in this code.
//
// There is an intent to develop some templates to
// make mixins even easier to use, and to provide
// assertional and debugging tools corresponding
// to some of the exceptions. I have not done this
// yet because I do not yet know exactly how :-)
//
// Although it is not the primary purpose, the variety of
// techniques, including nested rethrows, may be useful
// for regression testing compilers. I may add some
// extra stress tests later.
//
// This code is Copyright (C) 1994 by Maxtal P/L, Sydney Australia,
// and is hereby released freely for any use which
// does not restrict such free public availability.
//
//----------------------------------------------------------------
//
// A PLEA:
//
// Unfortunately, some compilers support exceptions
// but do not provide RTTI or dynamic_cast. There are
// ugly pre-processor hacks here to allow these compilers
// to execute most of the code. Unfortunately, this obscures
// the code somewhat. Since this is supposed to be "hands on"
// stuff, please try to see past this hackery at the
// underlying structures.
//
// SUPPORTED COMPILERS (so far)
//
// BORLAND 4.0
// METAWARE HIGH C/C++
// WATCOM 10.0a (soon)
// EDG
//
// If your compiler is not on the list it is because
// I do not have it, and no one else has provided
// a configuration.
//
//
// John Max Skaller
// maxtal@suphys.physics.su.oz.au
//
//----------------------------------------------------------------
// VENDOR CONFIGURATION SECTION
//
// This section is for VENDORS to define their compiler attributes.
//
// I hope to just have a sequence of #if .. #elif ..
// switches here with the bits in the middle supplied
// by each vendor. This should include inner tests for
// supported memory models, etc.
//
// Vendors MUST support "set_unexpected()"
// Dynamic casts and Type_info are optional but prefered.
//
// Portable implementation of the bad_cast exception is not possible
// without explicitly qualified (non-deducible) template functions.
//
// define this switch if your compiler does not have dynamic_cast
#ifndef __WATCOM_RTTI__
#define NODYNCAST
#endif

// select how to delete a const pointer: using a const_cast,
// an old style cast, or no cast
#ifdef __WATCOM_RTTI__
#define DELCONSTCAST
#else
#define DELOLDCAST
#endif
//#define DELNOCAST

// define this switch if your compiler does not have Type_info
#ifndef __WATCOM_RTTI__
#define NORTTI
#endif

// define this switch to configure the malloc test
// for compilers with 64K segment limits

#if defined(__BORLANDC__)
#define SIZE64K
#endif

// VENDOR must configure this section to define
// Type_info and set_unexpected.
// If NORTTI is defined, a replacement mechanism is supplied
// automatically -- do nothing.
// Otherwise you must define the type Type_info here somehow.
//
#if defined(NORTTI)
typedef void (*pfv)();
pfv set_unexpected(pfv p);
#else
#include <typeinfo.h>
#endif

#if defined(__BORLANDC__)
#include <except.h>
#endif

void *getidx( void const *p );

//END VENDOR CONFIGURATION SECTION
//----------------------------------------------------------------

#if defined(NORTTI)
struct Type_info { char const *name()const { return "NO RTTI"; } };
Type_info dummy_type_info;
#define typeid(x) (x)
#endif

#if defined(NODYNCAST)
class exception_backtrace;
class source_reference;
class exception_message;
#endif


// HERE FOR USERS TO FIDDLE
// define this switch to get debug trace of EH
#define EHDEBUG

#if defined(EHDEBUG)
#define dprintf(x) printf x
#else
#define dprintf(x)
#endif



struct strng // dummy string class
{
  char data[512];
  strng(char const *p) { strcpy(data,p); }
};

//namespace MaxTal
//{
//----------------------------------------------------------------------
// ABSTRACTIONS
//
//----------------------------------------------------------------------
// ROOTS

  // exception
  // common root of all standard exceptions

#ifdef __WATCOM_RTTI__
#undef exception
#define exception max_exception
#endif

  struct exception
  {
    #if defined(NORTTI)
    virtual char const *name()const=0;
    #endif

    #if defined(NODYNCAST)
    virtual exception_backtrace const *
      downcast_backtrace() const { return 0; }
    virtual exception_message const *
      downcast_exception_message() const { return 0; }
    virtual source_reference const *
      downcast_source_reference() const { return 0; }
    #endif
    virtual ~exception(){} //=0 !! Too many compilers get this wrong
    virtual exception* clone()const =0; // polymorphic copy
  };

//  inline exception::~exception(){}

  // catastrophic system failure has compromised system
  // integrity. This exception should generally be thrown
  // only in debugging versions of programs

  struct catastrohpe {};

  // an exception has arisen during exception processing
  // for example, memory has been exhausted during copying the
  // object of a throw

  struct double_fault {};

//----------------------------------------------------------------------
// RESOURCE ACQUISITION

  // exception to signal an attempt by the library
  // to acquire a resource from
  // the operating system host has failed
  // this includes memory, file handles, files, etc

  struct resource_acquisition_failure :
    public virtual exception {};

    // a program, task, or function has exceeded its alloted time
    // possibly indicating an infinite loop, deadlock, or
    // other condition. Can also be used for deliberate timeouts.

    struct timeout :
      public virtual resource_acquisition_failure {};

    // exception to signal insufficient or fragmented memory
    // prevents satisfaction of an allocation request

    struct bad_alloc :
      public virtual resource_acquisition_failure
    {
      virtual size_t requested_store() const=0;
        // amount of request if not zero

      virtual size_t available_block() const=0;
        // amount that might have worked if not zero
    };

//----------------------------------------------------------------------
// PRECONDITIONS

  // exception to report that the functional precondition
  // on the arguments of a function has not been met
  //
  // replaces domain_error
  //
  // if a function has several arguments, a precondition
  // is any subset of the cartesian product of the possible valid values
  // of the explicit parameters of the function

  struct precondition_violation :
    public virtual exception {};

    // a pointer argument was null, when a non-null pointer was required

    struct null_pointer_error :
      public virtual precondition_violation {};

    // an arithmetic function or operator has detected
    // the combination of arguments given is invalid

    struct arithmetic_domain_error :
      public virtual precondition_violation {};

      // a single arithmetic argument has a value outside its domain

      struct arithmetic_argument_error :
        public virtual arithmetic_domain_error {};

        // an index is out of the valid domain of a container
        // or array. This class should be extended to
        // provide more (optional) information

        struct index_error :
          public virtual arithmetic_argument_error {};

    // the results of a calculation would not be representable
    // for example the conversion of a long to an int
    // overflow on division, etc.
    //
    // This is a domain error which is a consequence of implementation
    // restrictions rather than program logic -- although the
    // distinction is thin

    struct implementation_limit_exceeded :
      public virtual precondition_violation {};

      // the exponent of a floating number is too large to be
      // represented

      struct floating_overflow :
        public virtual implementation_limit_exceeded,
        public virtual arithmetic_domain_error {};

      // the exponent of a floating number is to small (negative) to be
      // represented

      struct floating_underflow :
        public virtual implementation_limit_exceeded,
        public virtual arithmetic_domain_error {};


      // results of a conversion or integral operation
      // would produce an integer out of the range
      // or a target or computation

      struct integral_overflow :
        public virtual implementation_limit_exceeded,
        public virtual arithmetic_domain_error {};



//----------------------------------------------------------------------
// PROTOCOL VIOLATIONS

  // exception to report that the environment state required for
  // successful establishment of the post conditions of a function
  // have not been met, probably due to a violation of a required
  // dynamic protocol, such as a specific sequence of calls,
  // or dynamic initialisation, deleting an auto variable,
  // deleting a function without using delete [] syntax
  //
  // Protocol violations are violations of _non-functional_
  // preconditions.


  struct protocol_violation :
    public virtual exception {};

    // A resource deadlock is typically a _translated_
    // exception. A timeout on resource acquisition
    // in certain contexts may indicate two tasks await
    // resources held by the other.

    struct resource_deadlock :
      public virtual protocol_violation,
      public virtual resource_acquisition_failure {};

    // to be thrown if a dynamic reference cast is attempted
    // to a type not derived from the source object
#ifdef __WATCOM_RTTI__
typedef type_info Type_info;
#define bad_cast max_bad_cast
#endif

    struct bad_cast :
      protocol_violation {};

      // following optional diagnostic structure may be supported

      struct bad_cast_diagnostic :
        public virtual bad_cast
      {
        virtual Type_info const &typeid_source() const=0;
        virtual Type_info const &typeid_destination() const=0;
      };

    // to be thrown if an implementation detects calls to
    // pure virtual functions (optional)

    struct pure_virtual_called :
      public virtual protocol_violation {};

    // an unhandled exception which violates a function's
    // exception specification has been detected
    //
    // usually this exception will be thrown by
    // a debugging version of the unhandled exception handler

    struct exception_specification_violation :
      public virtual protocol_violation {};

//----------------------------------------------------------------------
// DEBUGGING: INVARIANTS

  // the private state of an object cannot
  // be mapped into a valid public state
  // when such a mapping is required

  // For example: a float value is a NaN, a rational number
  // has a zero denominator

  // Represents either a client protocol violation such
  // as uninitialised value, or a library class programming
  // error

  struct representation_invariant_violation :
    public virtual exception {};

//----------------------------------------------------------------------
// DEBUGGING: ALGORITHMIC FAILURES (BUGS)

  // a user assertion has failed, usually indicating
  // a programming error

  struct assertion_failure :
    public virtual exception {};

    // an invariant of a loop has changed

    struct loop_invariant_violation :
      public virtual assertion_failure {};

    // a loop variant has not changed

    struct loop_variant_violation :
      public virtual assertion_failure {};

    // a user written check on an function's post-conditions
    // has revealed the procedure has not executed successfully

    struct postcondition_failure :
      public virtual assertion_failure {};

    // a point in the program which should not have been reachable
    // has been reached

    struct unreachable_code_reached :
      public virtual assertion_failure {};

//----------------------------------------------------------------------
// DEBUGGING: MESSAGES

  struct exception_message :
    public virtual exception
  {
    virtual char const *what()const=0;
  };

  struct source_reference :
    public virtual exception
  {
    virtual char const* file()const=0;
    virtual int line()const=0;
  };


  struct source_message :
    public virtual exception_message,
    public virtual source_reference {};


//----------------------------------------------------------------------
// DEBUGGING: TRACEBACK BY DELEGATION

  struct exception_backtrace :
    public virtual exception
  {
    virtual exception const *getBacktrace()const=0;
  };

//----------------------------------------------------------------------
// IMPLEMENTATION MIXINS
//
// ALL these classes are concrete implementations of abstractions
// They should be mixed into a derived class.
// Subsequent derivation is possible but must NOT mixin the same
// implementation mixins or duplicate concrete bases will exist
// It is simplest to mixin all implementations in the most
// derived class.

//  #define CLONE(T) T* clone()const { return new T(*this); }
// Metaware doesn't support covariant returns yet
  #define CLONE(T) exception * clone()const {\
    exception *x = new T(*this);\
    dprintf(("%p [%s] Cloned %p [%s]\n",getidx(this),typeid(*this).name(), getidx(x), typeid(*x).name()));\
    return x; }


//----------------------------------------------------------------------
  // backtrace exception mixin maintains a pointer
  // chain of exceptions to enable historical reporting
  // of the source of an exception despite translation.
  // It can also be used to create and throw a list of exceptions.
  //
  // To translate an X to a Y, derive Y from the backtrace
  // implementation, and initialise it with X.
  // The backtrace object creates a clone of the X object.

  // We'd do better here by using a smart pointer.
  // But the rule is to have "very weak coupling". Oh well.

  struct exception_backtrace_impl :
    public virtual exception_backtrace
  {
    exception const * back;
    CLONE(exception_backtrace_impl)

    #if defined(NORTTI)
    char const *name()const { return "exception_backtrace_impl"; }
    #endif

    exception_backtrace_impl() :
      back(0) {dprintf(("WOOPS, constructed backtrace object with NULL attached\n")); }

    // user constructor to link clone of old exception into this one
    exception_backtrace_impl(exception const *b)
//    : back(b->clone())
      {
        if(b) back=b->clone();
        else back=0;
        dprintf(("ATTACH: the clone %p of %p to %p\n",getidx(back),getidx(b),getidx(this)));
      }

    // copy constructor: used by EH system NOT user!!
    exception_backtrace_impl(exception_backtrace_impl const& x)
//    : back(x.back->clone())
      {
      dprintf(("COPY->BEGIN: Copy backtrace object %p --> %p \n",getidx(&x),getidx(this)));
      dprintf(("Linked object of source is %p\n",getidx(x.back)));
      if(x.back)
      {
        back = x.back->clone();
        dprintf(("Linked object cloned to %p and attached to %p\n", getidx(back), getidx(this)));
      }
      else back=0;
      dprintf(("COPY->END:   Copy backtrace object %p --> %p done\n",getidx(&x),getidx(this)));
      }

    void operator=(exception_backtrace_impl const& b)
    {
      if(back != b.back)
      {
        #if defined(DELCONSTCAST)
        delete const_cast<exception*>(back); // ugly const cast!!
        #elif defined (DELOLDCAST)
        delete (exception*)(back); // uglier old style cast const cast!!
        #else
        delete back;
        #endif
        if(b.back)back = b.back->clone();
        else back=0;
      }
    }

    ~exception_backtrace_impl()
    {
      dprintf(("Destructor %p is Deleting backtrace %p\n",getidx(this),getidx(back)));
      #if defined(DELCONSTCAST)
      delete const_cast<exception*>(back); // ugly const cast!!
      #elif defined(DELOLDCAST)
      delete (exception*)(back); // uglier old style cast const cast!!
      #else
      delete back;
      #endif
    }

    exception const * getBacktrace() const { return back; }
    #if defined(NODYNCAST)
    exception_backtrace const * downcast_backtrace()const { return this; }
    #endif
  };

//----------------------------------------------------------------------

  // exception message

  struct exception_message_impl :
    public virtual exception_message
  {
    #if defined(NORTTI)
    char const *name()const { return "exception_message_impl"; }
    #endif
    #if defined(NODYNCAST)
    exception_message const * downcast_exception_message() const { return this; }
    #endif
    CLONE(exception_message_impl)
    strng message;
    exception_message_impl(char const *p) : message(p) {}
    char const *what()const { return message.data; }
  };

//----------------------------------------------------------------------

  // source reference

  struct source_reference_impl :
    public virtual source_reference
  {
    #if defined(NORTTI)
    char const *name()const { return "source_reference_impl"; }
    #endif
    #if defined(NODYNCAST)
    source_reference const * downcast_source_reference() const { return this; }
    #endif
    CLONE(source_reference_impl)

    char const* fileName;
    int lineNumber;

    source_reference_impl(char const *f, int l) :
      fileName(f), lineNumber(l) {}

    char const *file()const { return fileName; }
    int line()const { return lineNumber; }
  };

  struct source_message_impl :
    public virtual source_message,
    public source_reference_impl,
    public exception_message_impl
  {
    #if defined(NORTTI)
    char const *name()const { return "source_message_impl"; }
    #endif
    CLONE(source_message_impl)
    source_message_impl(char *fi, int li, char *msg) :
      source_reference_impl(fi,li),
      exception_message_impl(msg) {}
  };

//----------------------------------------------------------------------

  // implementation of bad_alloc

  struct bad_alloc_impl :
    public virtual bad_alloc
  {
    #if defined(NORTTI)
    char const *name()const { return "bad_alloc_impl"; }
    #endif
    size_t requested;
    size_t available;
    bad_alloc_impl() : requested(0), available(0) {}
    bad_alloc_impl(size_t r, size_t a) : requested(r), available(a) {}
    size_t requested_store() const { return requested; }
    size_t available_block() const { return available; }
    CLONE(bad_alloc_impl)
  };

  struct bad_alloc_msg :
    public bad_alloc_impl,
    public exception_message_impl
  {
    #if defined(NORTTI)
    char const *name()const { return "bad_alloc_msg"; }
    #endif
    CLONE(bad_alloc_msg)
    bad_alloc_msg(bad_alloc_impl e, char const* p) :
      bad_alloc_impl(e),
      exception_message_impl(p) {}
  };

//----------------------------------------------------------------------

  // implementation of bad_cast_diagnostic

  struct bad_cast_diagnostic_impl : public virtual bad_cast_diagnostic
  {
    #if defined(NORTTI)
    char const *name()const { return "bad_cast_diagnostic_impl"; }
    #endif
    CLONE(bad_cast_diagnostic_impl)

    Type_info const &typeid_destination() const
      {
        return *dst;
      }

    Type_info const &typeid_source() const
      {
        return *src;
      }

  // implementor sets these pointers inside dynamic_cast
    Type_info *dst;
    Type_info *src;
  };

//----------------------------------------------------------------------
//
// Note that exception_message_impl is used as a _non-virtual_
// base of the T##_msg class. Thats because it requires nondefault
// initialisation.

#if defined(NORTTI)
  #define IMPLEMENT(T)                         \
  struct T##_impl : public virtual T           \
  {                                            \
    char const *name()const { return #T"_impl"; } \
    CLONE(T##_impl)                            \
  };                                           \
                                               \
  struct T##_msg :                             \
    public virtual T,                          \
    public exception_message_impl              \
  {                                            \
    char const *name()const { return #T"_msg"; }   \
    CLONE(T##_msg)                             \
    T##_msg(char const* p) :                   \
      exception_message_impl(p) {}             \
  };                                           \
                                               \
  struct T##_src:                              \
    public virtual T,                          \
    public source_reference_impl               \
  {                                            \
    char const *name()const { return #T"_src"; }   \
    CLONE(T##_src)                             \
    T##_src(char const* p, int l) :            \
      source_reference_impl(p,l) {}            \
  };

#else

  #define IMPLEMENT(T)                         \
  struct T##_impl : public virtual T           \
  {                                            \
    CLONE(T##_impl)                            \
  };                                           \
                                               \
  struct T##_msg :                             \
    public virtual T,                          \
    public exception_message_impl              \
  {                                            \
    CLONE(T##_msg)                             \
    T##_msg(char const* p) :                   \
      exception_message_impl(p) {}             \
  };                                           \
                                               \
  struct T##_src:                              \
    public virtual T,                          \
    public source_reference_impl               \
  {                                            \
    CLONE(T##_src)                             \
    T##_src(char const* p, int l) :            \
      source_reference_impl(p,l) {}            \
  };

#endif

  IMPLEMENT(exception)
  IMPLEMENT(resource_acquisition_failure)
  IMPLEMENT(timeout)
  IMPLEMENT(precondition_violation)
  IMPLEMENT(null_pointer_error)
  IMPLEMENT(arithmetic_domain_error)
  IMPLEMENT(arithmetic_argument_error)
  IMPLEMENT(index_error)
  IMPLEMENT(implementation_limit_exceeded)
  IMPLEMENT(floating_overflow)
  IMPLEMENT(floating_underflow)
  IMPLEMENT(integral_overflow)
  IMPLEMENT(protocol_violation)
  IMPLEMENT(pure_virtual_called)
  IMPLEMENT(exception_specification_violation)
  IMPLEMENT(representation_invariant_violation)
  IMPLEMENT(assertion_failure)
  IMPLEMENT(loop_invariant_violation)
  IMPLEMENT(loop_variant_violation)
  IMPLEMENT(postcondition_failure)
  IMPLEMENT(unreachable_code_reached)

  #undef IMPLEMENT
//  #undef CLONE
//}

// END LIBRARY HEADER ------------------------------------------------
#endif
