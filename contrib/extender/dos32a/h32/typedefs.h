/*
 * Copyright (C) 1996-2002 Supernar Systems, Ltd. All rights reserved.
 *
 * Redistribution  and  use  in source and  binary  forms, with or without
 * modification,  are permitted provided that the following conditions are
 * met:
 *
 * 1.  Redistributions  of  source code  must  retain  the above copyright
 * notice, this list of conditions and the following disclaimer.
 *
 * 2.  Redistributions  in binary form  must reproduce the above copyright
 * notice,  this  list of conditions and  the  following disclaimer in the
 * documentation and/or other materials provided with the distribution.
 *
 * 3. The end-user documentation included with the redistribution, if any,
 * must include the following acknowledgment:
 *
 * "This product uses DOS/32 Advanced DOS Extender technology."
 *
 * Alternately,  this acknowledgment may appear in the software itself, if
 * and wherever such third-party acknowledgments normally appear.
 *
 * 4.  Products derived from this software  may not be called "DOS/32A" or
 * "DOS/32 Advanced".
 *
 * THIS  SOFTWARE AND DOCUMENTATION IS PROVIDED  "AS IS" AND ANY EXPRESSED
 * OR  IMPLIED  WARRANTIES,  INCLUDING, BUT  NOT  LIMITED  TO, THE IMPLIED
 * WARRANTIES  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED.  IN  NO  EVENT SHALL THE  AUTHORS  OR  COPYRIGHT HOLDERS BE
 * LIABLE  FOR  ANY DIRECT, INDIRECT,  INCIDENTAL,  SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL  DAMAGES  (INCLUDING, BUT NOT  LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE  GOODS  OR  SERVICES;  LOSS OF  USE,  DATA,  OR  PROFITS; OR
 * BUSINESS  INTERRUPTION) HOWEVER CAUSED AND  ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE)  ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */

/*==========================================================================*/
/*      C/C++ Run-Time Library: type definitions                            */
/*==========================================================================*/

#ifndef __TYPEDEFS__
#define __TYPEDEFS__

#ifdef __cplusplus
extern "C" {
#endif

#ifndef NULL
#define NULL    0
#endif
#define TRUE    1
#define FALSE   0
#define YES     1
#define NO      0
#define ON      1
#define OFF     0

/* typedefs for compatibility with previous versions */
typedef unsigned char     uchar;
typedef unsigned short    ushort;
typedef unsigned char     byte;
typedef unsigned short    word;
#if defined( __LP64__ ) || defined( _LP64 )
typedef unsigned int      ulong;
typedef unsigned int      dword;
#else
typedef unsigned long     ulong;
typedef unsigned long     dword;
#endif

/* the new typedefs are in upper case */
typedef unsigned char     UCHAR;
typedef unsigned short    USHORT;
typedef unsigned int      UINT;
typedef unsigned char     BYTE;
typedef unsigned short    WORD;
#if defined( __LP64__ ) || defined( _LP64 )
typedef unsigned int      ULONG;
typedef unsigned int      DWORD;
#else
typedef unsigned long     ULONG;
typedef unsigned long     DWORD;
#endif
/* WATCOM C v10.6 and below do not support __int64 */
#if ( defined( __WATCOMC__ ) && __WATCOMC__ > 1060 ) || defined( _MSC_VER )
typedef unsigned __int64  QWORD;
#elif defined( __GNUC__ ) || ( defined( __STDC_VERSION__ ) && __STDC_VERSION__ >= 199901L )
typedef unsigned long long QWORD;
#endif

#ifdef __cplusplus
}
#endif

#endif  // __TYPEDEFS__
