#ifndef _WATCOM_EXCEPTION_DEFINED
#define _WATCOM_EXCEPTION_DEFINED
 #define _WATCOM_EXCEPTION_DEFINED_
 struct __WATCOM_exception {
:: unfortunately we use pack(1) so using this as a base class can throw
:: off alignment of ints so we use this filler for the AXP & PPC to prevent this
  #if defined(__AXP__) || defined(__PPC__)
     void *__filler;
  #endif
 };
#endif
