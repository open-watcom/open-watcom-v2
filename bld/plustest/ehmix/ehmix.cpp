// 95/07/20 -- J.W.Welch        -- return 0 in main


// BEGIN TEST CODE ----------------------------------------------------
#include <new.h>
#include <stdio.h>
#include <stdlib.h>

#include "except.h"
#include "getidx.h"

#if defined(SIZE64K)
// real mode -- 64k limits
const int inimemamt = 10000;
const int memmul = 1;
#else
const int inimemamt = 1000;
const int memmul = 10;
#endif

//using namespace MaxTal;

void *xmalloc(size_t amt)
{
    if( amt > 1000000 ) {
      throw bad_alloc_impl(amt,0);
    }
  void *p=malloc(amt);
  if(p) return p;
  else throw bad_alloc_impl(amt,0);
}

void xfree(void *p) { free(p); }


void tr1() {
  printf("tr1(): throw precondition violation\n");
  throw precondition_violation_impl();
}

void tr2() {
  printf("tr2(): throw null pointer error\n");
  throw null_pointer_error_impl();
}

void tr3() {
  printf("tr3(): throw floating overflow error\n");
  throw floating_overflow_impl();
}

typedef void *address;
address ptrs[10000];

void tr4() {
  printf("tr4(): exhaust memory using new[]\n");
  int count=0;
  try {
    for(int i=inimemamt;;i*=memmul)
    {
      printf("Allocating (new no %d) %d bytes .. ",count,i);
      ptrs[count]=new char[i];
      if( count >= 6 ) throw bad_alloc_impl(0,0);
      ++count;
      printf(" .. done %d\n",i);
    }
  }
  catch(...)
  {
    while(count>=0)
    {
      printf("Deleting no %d\n",count);
      delete [] (char *)ptrs[count];
      --count;
    }
    throw;
  }
};

void tr5()
{
  printf("tr5(): exhaust memory using malloc\n");
  int count=0;
  try
  {
    for(int i=inimemamt;;i*=memmul)
    {
      printf("Allocating (malloc no %d) %d bytes .. ",count,i);
      ptrs[count]=xmalloc(i);
      ++count;
      printf(" .. done %d\n",i);
    }
  }
  catch(...)
  {
    while(count>=0)
    {
      printf("Freeing no %d\n",count);
      xfree(ptrs[count]);
      --count;
    }
    throw;
  }
};

void tr6() throw( assertion_failure_impl )
{
  printf("tr6(): throw assertion failure\n");
  throw assertion_failure_impl();
}

void tr7()
{
  printf("tr7(): throw assertion failure with message\n");
  throw assertion_failure_msg("tr7 failure");
}


// MIVB test: intersected (MI) exception specifications

struct A { virtual void f() throw(implementation_limit_exceeded)=0; };
struct B { virtual void f() throw(resource_acquisition_failure)=0; };

struct resource_limit :
  public virtual implementation_limit_exceeded,
  public virtual resource_acquisition_failure {};

struct resource_limit_impl :
  public resource_limit,
  public exception_message_impl
{
  CLONE(resource_limit_impl)
  resource_limit_impl(char const *m) :
    exception_message_impl(m) {}
};

struct C : A, B
{
  virtual void f() throw(resource_limit)
  {
    throw resource_limit_impl("Implementation defined resource limit exceeded");
  }
};

void tr8()
{
  printf("tr8(): MI test\n");
  C c;
  c.f();
}

// pure translation
void tr9()
{
  printf("tr9(): simple exception translation test\n");
  try { throw assertion_failure_impl(); }
  catch (assertion_failure const &e)
  {
    throw "Translated Exception : tr9()";
  }
}

// mixin composition: attach a message to an exception
void tr10()
{
  printf("tr10(): test message attachment by cloning\n");
  try { throw bad_alloc_impl(100,200); }
  catch (bad_alloc_impl &e)
  {
    throw bad_alloc_msg(e,"Just fooling: there's plenty of RAM left!");
  }
}

// test backtracing!!
// we'll use an assertion failure with a message
struct backAssert :
  public assertion_failure_msg,
  public exception_backtrace_impl
{
  char const *name()const { return "exception_backtrace_impl"; }
  backAssert(char const *msg, exception const *prev) :
    assertion_failure_msg(msg),
    exception_backtrace_impl(prev)
  {}
  CLONE(backAssert)
};

void tr11_3()
{
  try { throw exception_message_impl("Root exception"); }
  catch (exception const&e)
  {
    printf("tr11_3: caught exception\n");
    throw backAssert("tr11_3 translator",&e);
  }
}

void tr11_2()
{
  try { tr11_3(); }
  catch (exception const&e)
  {
    printf("tr11_2: caught exception\n");
    throw backAssert("tr11_2 translator",&e);
  }
}

void tr11_1()
{
  try { tr11_2(); }
  catch (exception const&e)
  {
    printf("tr11_1: caught exception\n");
    throw backAssert("tr11_1 translator",&e);
  }
}

void tr11()
{
  printf("tr11(): backtrace chain test\n");
  try { tr11_1(); }
  catch (exception const&e)
  {
    printf("tr11: caught exception\n");
    throw backAssert("tr11 (0) translator",&e);
  }
}

// test source references
void tr12()
{
  printf("tr12(): throw source reference\n");
  throw assertion_failure_src(__FILE__,__LINE__);
}

void tr13()
{
  printf("tr13(): throw source reference with message\n");
  throw source_message_impl(__FILE__,__LINE__,"Test 13 message");
}

void decode_exception(exception const* pe)
{
  printf("  ..  Exact type %s\n", typeid(*pe).name());

  // note: cross cast via common base!
  // is an upcast and a downcast: no problem
  // Borland does not support declarations in conditionals yet

  for(int level = 0; pe; ++level)
  {
    // print exception type
    printf("  ..  Level %d Exact type %s\n", level, typeid(*pe).name());

    {
      arithmetic_domain_error const *a;
      #if !defined(NODYNCAST)
      a=dynamic_cast<arithmetic_domain_error const*>(pe);
      #else
      a = 0; // sorry, not implemented!
      #endif
      if(a) printf (
        "  ..  We've got an arithmetic_domain_error = %s\n",
          typeid(*a).name());
    }
    // is there a message?
    {
      exception_message const *m;
      #if !defined(NODYNCAST)
      m = dynamic_cast<exception_message const*>(pe);
      #else
      m = pe->downcast_exception_message();
      #endif

      if(m) printf("  .. MESSAGE %s\n",m->what());
    }

    // is there a source reference?
    {
      source_reference const *s;
      #if !defined(NODYNCAST)
      s = dynamic_cast<source_reference const*>(pe);
      #else
      s = pe->downcast_source_reference();
      #endif

      if(s) printf("  .. FILE %s LINE %d\n",s->file(), s->line());
    }

    // is there a backtrace?
    {
      dprintf(("  ..  Dynamic cast [exact type is %s] to exception_backtrace?\n",
        typeid(*pe).name()));

      exception_backtrace const *b;
      #if !defined(NODYNCAST)
      b  = dynamic_cast<exception_backtrace const*>(pe);
      #else
      b = pe->downcast_backtrace();
      #endif

      if(b) printf("  ..  backtrace to %p detected\n",getidx(b->getBacktrace()));
      else dprintf(("  ..  CAST FAILED!\n"));
      pe = b ? b->getBacktrace() : 0;
    }
  }
}

void test(int i)
{
  printf("---- Test %d ----\n",i);
  try
  {
    switch(i)
    {
      case 1: tr1(); break;
      case 2: tr2(); break;
      case 3: tr3(); break;
      case 4: tr4(); break;
      case 5: tr5(); break;
      case 6: tr6(); break;
      case 7: tr7(); break;
      case 8: tr8(); break;
      case 9: tr9(); break;
      case 10: tr10(); break;
      case 11: tr11(); break;
      case 12: tr12(); break;
      case 13: tr13(); break;
      default: throw unreachable_code_reached_impl();
    }
  }

  catch(resource_acquisition_failure const &e)
  {
    printf("Caught resource acquisition failure\n");
    printf("  ..  Exact type %s\n", typeid(e).name());
    printf("  ..  rethrowing (nested) internally ..\n");
    try { throw; }
    catch(bad_alloc const &e)
    {
      printf("  ..  Sensed bad_alloc using nested rethrown exception\n");
      printf("  ..  requested %d, available=%d\n",
        e.requested_store(),
        e.available_block()
      );
      decode_exception(&e);
    }
    catch(...){ printf("  ..  No more information\n"); }
    printf("  ..  Internal rethrow handled\n");
  }

  catch (precondition_violation const &e)
  {
    printf("Caught precondition Violation\n");
    decode_exception(&e);
  }

  catch (protocol_violation const &e)
  {
    printf("Caught protocol Violation\n");
    decode_exception(&e);
  }

  catch (assertion_failure const &e)
  {
    printf("Caught assertion failure\n");
    decode_exception(&e);
  }

  catch (exception const &e)
  {
    printf("Caught exception\n");
    decode_exception(&e);
  }

  catch (char *s)
  {
    printf("Exception string \"%s\" caught\n",s);
  }

  printf("Test %d complete, exceptions handled\n\n",i);
}

void my_new_handler()
{
  throw bad_alloc_impl();
}

void my_unexpected_handler()
{
  printf("Unexpected exception trapped in unexpected handler\n");
  throw exception_specification_violation_impl();
}

main(int argc, char **argv)
{
  set_new_handler(my_new_handler);
  set_unexpected(my_unexpected_handler);

  try
  {
    if(argc==1)
    {
      test(-1);
      test(1);
      test(2);
      test(3);
      test(4);
      test(5);
      test(6);
      test(7);
      #if !defined(__HIGHC__)
      test(8);
      #endif
      test(9);
      #if !defined(__HIGHC__)
      test(10);
      #endif
      #if !defined(__BORLANDC__)
      test(11);
      #endif
      test(12);
      test(13);
    }
    else for(int i=1; i<argc; ++i)
    {
      int val=-1;
      sscanf(argv[i],"%d", &val);
      test(val);
    }
    return 0;
  }

  catch (exception const &e)
  {
    printf
    (
      "An exception of type [%s] has unexpectedly leaked into main\n",
      typeid(e).name()
    );
    return 1;
  }

  catch(...)
  {
    printf("An unknown exception has unexpectedly leaked into main\n");
    return 1;
  }
  printf( "FATAL ERROR!\n" );
  return 1;
}


