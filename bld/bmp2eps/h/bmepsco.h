/*
 * libbmeps - Bitmap to EPS conversion library
 * Copyright (C) 2000 - Dirk Krause
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 * In this package the copy of the GNU Library General Public License
 * is placed in file COPYING.
 */

#ifndef BMEPSCO_INC
#define BMEPSCO_INC 1

#define VERSNUMB "0.9.0"

#if HAVE_CONFIG_H
#include <config.h>
#else
#ifndef HAVE_ZLIB
#define HAVE_ZLIB               0
#endif
#ifndef HAVE_LIBPNG
#define HAVE_LIBPNG             0
#endif
#ifndef HAVE_JPEGLIB_H
#define HAVE_JPEGLIB_H          0
#endif
#ifndef HAVE_TIFF_H
#if LZW_PATENT_EXPIRED
#define HAVE_TIFF_H             1
#else
#define HAVE_TIFF_H             0
#endif
#endif
#ifndef HAVE_PNM_H
#define HAVE_PNM_H              0
#endif
#if _MSC_VER
/* + MSVC */
#ifndef HAVE_STRICMP
#define HAVE_STRICMP            0
#endif
#ifndef HAVE__STRICMP
#define HAVE__STRICMP           1
#endif
#ifndef HAVE_UNLINK
#define HAVE_UNLINK             0
#endif
#ifndef HAVE__UNLINK
#define HAVE__UNLINK            1
#endif
#ifndef HAVE_GETPID
#define HAVE_GETPID             0
#endif
#ifndef HAVE__GETPID
#define HAVE__GETPID            1
#endif
#ifndef HAVE_PROCESS_H
#define HAVE_PROCESS_H          1
#endif
#ifndef HAVE__SETMODE
#define HAVE__SETMODE           1
#endif
#ifndef HAVE_SETMODE
#define HAVE_SETMODE            0
#endif
#ifndef HAVE__FILENO
#define HAVE__FILENO            1
#endif
#ifndef HAVE_FILENO
#define HAVE_FILENO             0
#endif
#ifndef HAVE_FCNTL_H
#define HAVE_FCNTL_H            1
#endif
#ifndef HAVE_IO_H
#define HAVE_IO_H               1
#endif
/* - MSVC */
#else
/* + !MSVC */
#if __BORLANDC__
/* + BC */
#ifndef HAVE_STRICMP
#define HAVE_STRICMP            1
#endif
/* - BC */
#else
/* + !MSVC && !BC */
#ifndef HAVE_STRCASECMP
#define HAVE_STRCASECMP         1
#endif
/* - !MSVC && !BC */
#endif
/* - !MSVC */
#endif
#ifndef HAVE_UNLINK
#define HAVE_UNLINK             1
#endif
#ifndef HAVE_GETPID
#define HAVE_GETPID             1
#endif
#ifndef HAVE_SYS_TYPES_H
#define HAVE_SYS_TYPES_H        1
#endif
#ifndef HAVE_STRING_H
#define HAVE_STRING_H           1
#endif
#ifndef HAVE_STDLIB_H
#define HAVE_STDLIB_HA
#endif
#ifndef HAVE_MALLOC_H1
#define HAVE_MALLOC_H           1
#endif
#ifndef HAVE_SETJMP_H
#define HAVE_SETJMP_H           1
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <process.h>

#endif
/* ifndef BMEPSCO_INC */

