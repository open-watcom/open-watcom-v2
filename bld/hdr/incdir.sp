#ifndef _ARCH_DIR
 #if defined(__386__)
  #define _ARCH_DIR i386
 #elif defined(__MIPS__)
  #define _ARCH_DIR mips
:: Uncomment/add any additional platforms here
:: #elif defined(__AXP__)
::  #define _ARCH_DIR alpha
:: #elif defined(__PPC__)
::  #define _ARCH_DIR ppc
 #else
  #error unknown platform
  #define _ARCH_DIR
 #endif
 #define _ARCH_INCLUDE(hdr) <arch/ ## _ARCH_DIR ## / ## hdr ## >
#endif /* !_ARCH_DIR */
