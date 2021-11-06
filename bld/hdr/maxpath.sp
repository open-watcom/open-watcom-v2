:: In this include file all file/path names length limit macros are defined
::
:: Path element MAX macros
::
:segment MAXELEM
::
/*
 * The following sizes are the maximum sizes of buffers used by the _fullpath()
 * _makepath() and _splitpath() functions.  They include space for the '\0'
 * terminator.
 */
:segment LINUX | QNX
#define _MAX_DRIVE  48          /*  maximum length of node name w/ '\0'  */
#define _MAX_NODE   _MAX_DRIVE  /*  maximum length of node name w/ '\0'  */
#define _MAX_DIR    256         /*  maximum length of subdirectory       */
#define _MAX_FNAME  48          /*  maximum length of a file name        */
#define _MAX_EXT    48          /*  maximum length of a file extension   */
:elsesegment RDOS
#define _MAX_DRIVE  3           /* maximum length of drive component     */
#define _MAX_DIR    256         /* maximum length of path component      */
#define _MAX_FNAME  256         /* maximum length of file name component */
#define _MAX_EXT    256         /* maximum length of extension component */
:elsesegment DOS
#if defined(__NT__) || defined(__OS2__) || defined(__WATCOM_LFN__) && defined(__DOS__)
#define _MAX_DRIVE  3           /* maximum length of drive component     */
#define _MAX_DIR    256         /* maximum length of path component      */
#define _MAX_FNAME  256         /* maximum length of file name component */
#define _MAX_EXT    256         /* maximum length of extension component */
#else
#define _MAX_DRIVE  3           /* maximum length of drive component     */
#define _MAX_DIR    130         /* maximum length of path component      */
#define _MAX_FNAME  9           /* maximum length of file name component */
#define _MAX_EXT    5           /* maximum length of extension component */
#endif
:endsegment
::
:elsesegment FILENAMEMAX
::
:: FILENAME_MAX macro
::
::#ifndef FILENAME_MAX
:segment LINUX | QNX
#define FILENAME_MAX    255
:elsesegment RDOS
#define FILENAME_MAX    260
:elsesegment DOS
#if defined(__OS2__) || defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__)
 #define FILENAME_MAX   260
#else
 #define FILENAME_MAX   144
#endif
:endsegment
::#endif
::
:elsesegment NAMEMAX
::
:: NAME_MAX macro
::
:segment DOS | RDOS
#ifndef NAME_MAX
:segment RDOS
 #define NAME_MAX       255     /* maximum filename for RDOS          */
:elsesegment DOS
 #if defined(__OS2__)
  #define NAME_MAX      255     /* maximum filename for HPFS          */
 #elif defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__)
  #define NAME_MAX      259     /* maximum filename for NTFS, FAT LFN and DOS LFN */
 #else
#define NAME_MAX        12      /* 8 chars + '.' +  3 chars */
 #endif
:endsegment
#endif
:endsegment
::
:elsesegment PATHMAX
::
:: PATH_MAX macro
::
:segment DOS | RDOS
:include ext.sp
::#ifndef PATH_MAX
:segment RDOS
#define PATH_MAX        259     /* maximum length of full pathname excl. '\0' */
:elsesegment DOS
#if defined(__OS2__) || defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__)
 #define PATH_MAX       259     /* maximum length of full pathname excl. '\0' */
#else
 #define PATH_MAX       143     /* maximum length of full pathname excl. '\0' */
#endif
:endsegment
::#endif
:include extepi.sp
:endsegment
::
:elsesegment MAXPATH
::
:: _MAX_PATH macro
::
#ifndef _MAX_PATH
:segment LINUX | QNX
 #define _MAX_PATH      256     /* maximum length of full pathname */
:elsesegment RDOS
 #define _MAX_PATH      260     /* maximum length of full pathname */
:elsesegment DOS
 #if defined(__OS2__) || defined(__NT__) || defined(__WATCOM_LFN__) && defined(__DOS__)
  #define _MAX_PATH     260     /* maximum length of full pathname */
 #else
  #define _MAX_PATH     144     /* maximum length of full pathname */
 #endif
:endsegment
#endif
::
:endsegment
