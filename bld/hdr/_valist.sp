#ifndef ___VA_LIST_DEFINED
#define ___VA_LIST_DEFINED
:segment !INTEL_ONLY
 #ifdef __PPC__
:segment DOS
  #ifdef __NT__
   typedef char  *__va_list;
  #else
:endsegment
   typedef struct {
       char  __gpr;
       char  __fpr;
       char  __reserved[2];
       char  *__input_arg_area;
       char  *__reg_save_area;
   } __va_list;
:segment DOS
  #endif
:endsegment
 #elif defined(__AXP__)
  typedef struct {
      char  *__base;
      int   __offset;
  } __va_list;
 #elif defined(__MIPS__)
  typedef struct {
      char  *__base;
      int   __offset;
  } __va_list;
 #elif defined(_M_IX86)
:endsegment
:segment BITS16
  #if defined(__HUGE__) || defined(__SW_ZU)
:elsesegment
  #if defined(__SW_ZU)
:endsegment
   typedef char _WCFAR *__va_list;
  #else
   typedef char    *__va_list;
  #endif
:segment !INTEL_ONLY
 #endif
:endsegment
#endif
