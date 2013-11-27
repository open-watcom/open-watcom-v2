#ifndef _VA_LIST_DEFINED
#define _VA_LIST_DEFINED
 #if defined(__PPC__)
:segment DOS
  #if defined(__NT__)
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
:segment DOS | QNX
 #elif defined(__HUGE__) || defined(__SW_ZU)
:elsesegment
 #elif defined(__SW_ZU)
:endsegment
  typedef char _WCFAR *__va_list[1];
 #else
  typedef char    *__va_list[1];
 #endif
#endif
