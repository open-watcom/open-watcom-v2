/****************************************************************************
*
*                            Open Watcom Project
*
*    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
*
*  ========================================================================
*
*    This file contains Original Code and/or Modifications of Original
*    Code as defined in and that are subject to the Sybase Open Watcom
*    Public License version 1.0 (the 'License'). You may not use this file
*    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
*    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
*    provided with the Original Code and Modifications, and is also
*    available at www.sybase.com/developer/opensource.
*
*    The Original Code and all software distributed under the License are
*    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
*    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
*    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
*    NON-INFRINGEMENT. Please see the License for the specific language
*    governing rights and limitations under the License.
*
*  ========================================================================
*
* Description:  WHEN YOU FIGURE OUT WHAT THIS FILE DOES, PLEASE
*               DESCRIBE IT HERE!
*
****************************************************************************/


#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <process.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <limits.h>
#include <errno.h>
#include <conio.h>
#include <malloc.h>
#include <sys/stat.h>

#include "trmemcvr.h"
#include "orl.h"
#include "ar.h"
#include "lib.h"
#include "watcom.h"
#include "bool.h"
#include "wressetr.h"
#include "wreslang.h"
#include "demangle.h"

#include "libio.h"
#include "types.h"
#include "optdef.h"
#include "ops.h"
#include "memfuncs.h"
#include "objfile.h"
#include "inlib.h"

#include "exeelf.h"
#include "convert.h"
#include "util.h"
#include "libwalk.h"
#include "liblist.h"
#include "cmdline.h"
#include "orlrtns.h"
#include "error.h"
#include "msg.gh"
#include "ext.h"
#include "proclib.h"
#include "filetab.h"
#include "implib.h"
#include "symlist.h"
#include "writelib.h"
#include "coff.h"
#include "coffwrt.h"
#include "pcobj.h"
#include "banner.h"
#include "omfutil.h"
#include "exedos.h"
#include "exeos2.h"
#include "exeflat.h"
#include "exepe.h"
#include "exenov.h"
#include "main.h"
#include "idedll.h"
