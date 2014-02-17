#ifdef __NT__
    #include "pushpck2.h"
#else
    #include "pushpck1.h"
#endif

typedef struct {
#ifdef WDE
    long    dtStyle;
  #ifdef __NT__
    DWORD   dtExtendedStyle;
    WORD    dtItemCount;
  #else
    BYTE    dtItemCount;
  #endif
    short   dtX;
    short   dtY;
    short   dtCX;
    short   dtCY;
#else
    DWORD   dtStyle;
  #ifdef __NT__
    DWORD   dtExtendedStyle;
    WORD    dtItemCount;
  #else
    BYTE    dtItemCount;
  #endif
    WORD    dtX;
    WORD    dtY;
    WORD    dtCX;
    WORD    dtCY;
#endif
//  char    dtMenuName[];
//  char    dtClassName[];
//  char    dtCaptionText[];
} _DLGTEMPLATE;

typedef struct {
#ifdef WDE
    short   PointSize;
#else
    WORD    PointSize;
#endif
//  char    szTypeFace[];
} FONTINFO;

typedef struct {
#ifdef WDE
  #ifdef __NT__
    long    dtilStyle;
    DWORD   dtExtendedStyle;
  #endif
    short   dtilX;
    short   dtilY;
    short   dtilCX;
    short   dtilCY;
    short   dtilID;
  #ifndef __NT__
    long    dtilStyle;
  #endif
#else
  #ifdef __NT__
    DWORD   dtilStyle;
    DWORD   dtExtendedStyle;
  #endif
    WORD    dtilX;
    WORD    dtilY;
    WORD    dtilCX;
    WORD    dtilCY;
    WORD    dtilID;
  #ifdef __NT__
    WORD    crap;
  #else
    DWORD   dtilStyle;
  #endif
#endif
//  char    dtilClass[];
//  char    dtilText[];
//  BYTE    dtilInfo;
//  BYTE    dtilData;
} _DLGITEMTEMPLATE;

#include "poppck.h"
