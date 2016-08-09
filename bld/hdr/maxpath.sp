:: In this include file all file/path names length limit macros are defined
::
:: FILENAME_MAX macro
::
:segment FILENAME
::#ifndef FILENAME_MAX
:segment QNX | LINUX
#define FILENAME_MAX    255
:elsesegment
#if defined(__OS2__) || defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__) || defined(__OSI__)
 #define FILENAME_MAX   260
#else
 #define FILENAME_MAX   144
#endif
:endsegment
::#endif
:elsesegment NAMEMAX
::
:: NAME_MAX macro
::
:segment DOS
#ifndef NAME_MAX
 #if defined(__OS2__) || defined(__RDOS__)
  #define NAME_MAX    255     /* maximum filename for HPFS and RDOS */
 #elif defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__) || defined(__OSI__)
  #define NAME_MAX    259     /* maximum filename for NTFS, FAT LFN, DOS LFN and OSI */
 #else
  #define NAME_MAX    12      /* 8 chars + '.' +  3 chars */
 #endif
#endif
:endsegment
:elsesegment PATHMAX
::
:: PATH_MAX macro
::
:segment DOS
:include ext.sp
::#ifndef PATH_MAX
 #if defined(__OS2__) || defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__) || defined(__OSI__)
  #define PATH_MAX      259 /* maximum length of full pathname excl. '\0' */
 #else
  #define PATH_MAX      143 /* maximum length of full pathname excl. '\0' */
 #endif
::#endif
:include extepi.sp
:endsegment
:elsesegment
::
:: _MAX_PATH macro
::
#ifndef _MAX_PATH
:segment QNX | LINUX
 #define _MAX_PATH    256 /* maximum length of full pathname */
:elsesegment
 #if defined(__OS2__) || defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__) || defined(__OSI__)
  #define _MAX_PATH   260 /* maximum length of full pathname */
 #else
  #define _MAX_PATH   144 /* maximum length of full pathname */
 #endif
:endsegment
#endif
:endsegment
