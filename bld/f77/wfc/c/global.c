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


#include "ftnstd.h"
#include "source.h"
#include "argument.h"
#include "symbol.h"
#include "stmtnoin.h"
#include "rememb.h"
#include "csdefn.h"
#include "fmtstg.h"
#include "itdefn.h"
#include "impl.h"
#include "lexinc.h"
#include "scaninc.h"
#include "token.h"
#include "ftnio.h"
#include "cioconst.h"
#include "fmtdef.h"

// The following files have been ordered so that variables are aligned on
// an even address.  Be careful when you change them!

#define gbl_defn
#include "fmtdefn.h"
#include "fcdata.h"
#include "prdefn.h"
#include "progsw.h"
#include "segsw.h"
#include "stmtsw.h"
#include "extnsw.h"
#include "gbldat.h"
