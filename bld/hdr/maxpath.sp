#ifndef _MAX_PATH
:segment QNX
 #define _MAX_PATH      256     /*  maximum length of path name         */
:elsesegment
 #if defined(__OS2__) || defined(__NT__)
  #define _MAX_PATH   260 /* maximum length of full pathname */
 #else
  #define _MAX_PATH   144 /* maximum length of full pathname */
 #endif
:endsegment
#endif
