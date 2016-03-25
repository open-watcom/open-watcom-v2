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
* Description:  GUI library sample program.
*
****************************************************************************/


#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <malloc.h>
#include "sample.h"
#include "dlgstat.h"
#include "dlgdynam.h"
#include "dlgtest.h"
#include "guirmenu.h"

char *program_invocation_name = "sample";

char            *OldValue       = NULL;
gui_window      *DialogWindow   = NULL;
gui_ctl_id      CurrPopupItem   = MENU_CHANGE;

static gui_window           *HelpWindow     = NULL;
static const char           *help_file;
static gui_help_instance    help_inst;
static const char           LongStr[] =
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ"
"0123456789abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
static  gui_window      *MainWnd        = NULL;
static  gui_window      *Child1Wnd      = NULL;
static  gui_window      *Child2Wnd      = NULL;
static  gui_window      *Child3Wnd      = NULL;
static  gui_window      *Child4Wnd      = NULL;
static  gui_window      *Child4_1Wnd    = NULL;
static  gui_window      *Child4_2Wnd    = NULL;
static  gui_window      *Child4_3Wnd    = NULL;
static  int             Percent         = 50;
static  bool            Highlight       = false;
static  int             PrevRow;
static  bool            KeyDown         = false;
static  bool            WndScaled       = false;
static  char            *FloatHelp      = "Float Toolbar";
static  char            *FixHelp        = "Fix Toolbar";
static  int             Child1HScrollRange      = 0;
static  gui_point       StartPoint      = { 0, 0 };
static  gui_point       EndPoint        = { 0, 0 };
static  gui_point       StartRectPoint  = { 0, 0 };
static  gui_rect        SFillRect       = { 0, 0, 0, 0 };
#if keys
static  gui_key         KeyDownKey;
#endif

static  gui_rect        ScreenRect      = { 500, 500, WIDTH, HEIGHT };
static  gui_rect        ScaleRect       = { 500, 500, WIDTH, HEIGHT };
static  gui_coord       Scale           = { WIDTH, HEIGHT };
static  gui_colour_set  StatusColour    = { GUI_BRIGHT_WHITE, GUI_CYAN };

static void HandlePopup( gui_window *gui, gui_rect *rect, gui_ctl_id id )
{
    switch( id ) {
    case MENU_CHANGE :
        GUISetMenuText( MainWnd, MENU_REDRAW, "&New", false );
        GUISetMenuText( MainWnd, MENU_RESTORE, "&Restore\talso tabbed", false );
        GUISetMenuText( MainWnd, MENU_RESIZE, "&Newer and very very very very very very very very very very very very very long\ttabbed", false );
        break;
    case MENU_RESIZE :
        GUIResizeWindow( gui, rect );
        break;
    case MENU_MINIMIZE :
        GUIMinimizeWindow( gui );
        break;
    case MENU_MAXIMIZE :
        GUIMaximizeWindow( gui );
        break;
    case MENU_RESTORE :
        GUIRestoreWindow( gui );
        GUIDrawStatusText( gui, "Restore has happened" );
        break;
    }
}

/*
 * MainEventWnd - call back routine for the main window
 */

static void SetFixedFloat( gui_window *gui )
{
    if( GUIHasToolBar( gui ) ) {
        if( GUIToolBarFixed( gui) ) {
            GUISetMenuText( gui, MENU_CHANGE_TOOLBAR, "&Float Toolbar", false);
            GUISetHintText( gui, MENU_CHANGE_TOOLBAR, FloatHelp );
        } else {
            GUISetMenuText( gui, MENU_CHANGE_TOOLBAR, "&Fix Toolbar", false );
            GUISetHintText( gui, MENU_CHANGE_TOOLBAR, FixHelp );
        }
    } else {
        GUIEnableMenuItem( gui, MENU_CHANGE_TOOLBAR, false, false );
    }
}

static void SetToolbarCreate( gui_window *gui )
{
    bool        has;

    has = GUIHasToolBar( gui );
    GUIEnableMenuItem( gui, MENU_FIXED_TOOLBAR, has == false, false );
    GUIEnableMenuItem( gui, MENU_FLOATING_TOOLBAR, has == false, false );
    GUIEnableMenuItem( gui, MENU_CHANGE_TOOLBAR, has, false );
    GUIEnableMenuItem( gui, MENU_CLOSE_TOOLBAR, has, false );
}

#if keys
static bool DisplayKey( gui_key key, char *Buffer )
{
    bool        ret;

    ret = false;
    switch( key ) {
    case GUI_KEY_PAGEUP :
        strcpy( Buffer, "Got key down and up page up" );
        break;
    case GUI_KEY_PAGEDOWN :
        strcpy( Buffer, "Got key down and up page down" );
        break;
    case GUI_KEY_CTRL_PAGEUP :
        strcpy( Buffer, "Got key down and up ctrl page up" );
        break;
    case GUI_KEY_CTRL_PAGEDOWN :
        strcpy( Buffer, "Got key down and up ctrl page down" );
        break;
    case GUI_KEY_ALT_EXT_PAGEUP :
        strcpy( Buffer, "Got key down and up alt page up" );
        break;
    case GUI_KEY_ALT_EXT_PAGEDOWN :
        strcpy( Buffer, "Got key down and up alt page down" );
        break;
    case GUI_KEY_TAB :
        strcpy( Buffer, "Got key down and up tab" );
        break;
    case GUI_KEY_SHIFT_TAB :
        strcpy( Buffer, "Got key down and up shift tab" );
        break;
    case GUI_KEY_ALT_TAB :
        strcpy( Buffer, "Got key down and up alt tab" );
        break;
    case GUI_KEY_CTRL_TAB :
        strcpy( Buffer, "Got key down and up ctrl tab" );
        break;
    case GUI_KEY_F10 :
        strcpy( Buffer, "Got key down and up pf-10" );
        break;
    case GUI_KEY_F11 :
        strcpy( Buffer, "Got key down and up pf-11" );
        break;
    case GUI_KEY_F12 :
        strcpy( Buffer, "Got key down and up pf-12" );
        break;
    case GUI_KEY_CTRL_F10 :
        strcpy( Buffer, "Got key down and up ctrl_pf-10" );
        break;
    case GUI_KEY_CTRL_F11 :
        strcpy( Buffer, "Got key down and up ctrl_pf-11" );
        break;
    case GUI_KEY_CTRL_F12 :
        strcpy( Buffer, "Got key down and up ctrl_pf-12" );
        break;
    case GUI_KEY_ALT_F10 :
        strcpy( Buffer, "Got key down and up alt_pf-10" );
        break;
    case GUI_KEY_ALT_F11 :
        strcpy( Buffer, "Got key down and up alt_pf-11" );
        break;
    case GUI_KEY_ALT_F12 :
        strcpy( Buffer, "Got key down and up alt_pf-12" );
        break;
    case GUI_KEY_SHIFT_F10 :
        strcpy( Buffer, "Got key down and up shift_pf-10" );
        break;
    case GUI_KEY_SHIFT_F11 :
        strcpy( Buffer, "Got key down and up shift_pf-11" );
        break;
    case GUI_KEY_SHIFT_F12 :
        strcpy( Buffer, "Got key down and up shift_pf-12" );
        break;
    default :
        if( ( key >= GUI_KEY_CTRL_A ) && ( key <= GUI_KEY_CTRL_Z ) ) {
            sprintf( Buffer, "Got key down and up ctrl-%c",
                    (char)( key - GUI_KEY_CTRL_A + GUI_KEY_a ) );
        } else if( ( key >= GUI_KEY_ALT_A ) && ( key <= GUI_KEY_ALT_Z ) ) {
            sprintf( Buffer, "Got key down and up alt-%c",
                     (char)( key - GUI_KEY_ALT_A + GUI_KEY_a ) );
        } else if( ( key >= GUI_KEY_F1 ) && ( key <= GUI_KEY_F9 ) ) {
            sprintf( Buffer, "Got key down and up pf-%c",
                     (char)( key - GUI_KEY_F1 + GUI_KEY_1 ) );
        } else if( ( key >= GUI_KEY_CTRL_F1 ) && ( key <= GUI_KEY_CTRL_F9 ) ) {
            sprintf( Buffer, "Got key down and up ctrl-pf-%c",
                     (char)( key - GUI_KEY_CTRL_F1 + GUI_KEY_1 ) );
        } else if( ( key >= GUI_KEY_ALT_F1 ) && ( key <= GUI_KEY_ALT_F9 ) ) {
            sprintf( Buffer, "Got key down and up alt-pf-%c",
                     (char)( key - GUI_KEY_ALT_F1 + GUI_KEY_1 ) );
        } else if( ( key >= GUI_KEY_SHIFT_F1 ) && ( key <= GUI_KEY_SHIFT_F9 ) ) {
            sprintf( Buffer, "Got key down and up shift-pf-%c",
                     (char)( key - GUI_KEY_SHIFT_F1 + GUI_KEY_1 ) );
        } else if( ( key >= GUI_KEY_ALT_BACKQUOTE ) && ( key <= GUI_KEY_ALT_SLASH ) ) {
            sprintf( Buffer, "Got key down and up alt-%c",
            (char)( key - GUI_KEY_ALT_BACKQUOTE + '`' ) );
        } else if( ( key >= GUI_KEY_ALT_1 ) && ( key <= GUI_KEY_ALT_9 ) ) {
            sprintf( Buffer, "Got key down and up alt-%c",
            (char)( key - GUI_KEY_ALT_1 + '1' ) );
        } else if(  key == GUI_KEY_ALT_0 ) {
            sprintf( Buffer, "Got key down and up alt-0" );
        } else {
            sprintf( Buffer, "Got key down and up %c", (char)key );
        }
    }
    GUIDrawStatusText( MainWnd, Buffer );
    return( ret );
}
#endif

static change_struct *MakeChangeStruct( char *str, int length,
                                        gui_window *gui )
{
    change_struct       *old;

    old = (change_struct *)GUIMemAlloc( sizeof( change_struct  ) );
    old->string = str;
    old->length = length;
    old->parent = 0;
    old->edit_contr = EDIT_CONTROL;
    old->wnd_to_update = gui;
    return( old );
}

bool MainEventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    bool                ret;
    gui_ctl_id          id;
    char                *new;
    int                 numrows;
    gui_text_metrics    text_metrics;
    gui_rect            client;
    gui_coord           scroll;
    char                *text;
    int                 num;
    gui_colour_set      *colours;
    gui_point           point;
    char                *fontinfo;
    char                back;
    gui_key             key;
    gui_colour          colour;
    gui_rgb             rgb;
#if parent_keys
    char                Buffer[80];
#endif


    ret = false;
    gui = gui;
    switch( gui_ev ) {
    case GUI_CLOSE :
        GUIHelpFini( help_inst, HelpWindow, help_file );
        ret = true;
        break;
    case GUI_PAINT :
        GUIGetPaintRect( gui, &client );
        break;
    case GUI_STATUS_CLEARED :
        GUIDrawStatusText( gui, "Ready!" );
        client.x = 700;
        client.y = 700;
        client.width = 100;
        client.height = 100;
        GUIWndDirtyRect( gui, &client );
        break;
    case GUI_INIT_WINDOW :
        if( MainWnd == NULL ) {
#if dynamic_menus
            int     i;
            for( i = 0; i < NUM_MAIN_MENUS; i++ ) {
                GUIAppendMenu( gui, &MainMenu[i], false );
            }
#endif
//          GUIEnableMenuItem( gui, MENU_REDRAW, false, false );
            GUIEnableMenuItem( gui, MENU_RESIZE, true, false );
            GUICheckMenuItem( gui, MENU_RESIZE, true, false );
            GUICheckMenuItem( gui, MENU_STATIC_DIALOG, false, false );
            back = '\xb0';
            GUISetBackgroundChar( gui, back );
#if dynamic_menus
            GUIAppendMenuByIdx( gui, 5, &ModifyColour );
            GUIAppendMenuToPopup( gui, MENU_MODIFY_COLOUR, &PopupMenu[0], false );
            //GUIAppendMenu( gui, &ChildMenu, false );
            GUIInsertMenuByIdx( gui, 0, ChildMenu, false );
            GUIInsertMenuByID( gui, MENU_SEP_QUIT, &ModifyColour );
#endif
        }
        GUIGetTextMetrics( gui, &text_metrics );
        GUIGetClientRect( gui, &client );
        GUISetHScrollRange( gui, 2 * client.width );
        GUIInitHScroll( gui, client.width - 1 );
        Percent = 50;
        GUISetVScrollThumb( gui, 50 );
        scroll.x = GUIGetHScroll( gui );
        scroll.y = GUIGetVScroll( gui );
        scroll.x = GUIGetHScrollCol( gui );
        scroll.y = GUIGetVScrollRow( gui );
        numrows = GUIGetNumRows( gui );
        ret = true;
        break;
    case GUI_FONT_CHANGED :
        GUIResizeStatusWindow( gui, 0, 0 );
        break;
    case GUI_CONTROL_RCLICKED :
#ifndef __OS2_PM__
        //WinExec( "g:\\lang\\binw\\viw.exe foobar", 1 );
        //WinExec( "f:\\src\\win\\wre.exe", 1 );
#endif
        GUI_GETID( param, id );
        text = GUIGetText( gui, id );
        GUIDisplayMessage( gui, text, text, GUI_ABORT_RETRY_IGNORE );
        GUIMemFree( text );
        break;
    case GUI_RBUTTONUP :
        if( gui != MainWnd ) {
            GUI_GET_POINT( param, point );
#if 0
            GUICreateFloatingPopup( gui, &point, NUM_POPUP_MENUS, &PopupMenu,
                                    GUI_TRACK_RIGHT, &CurrPopupItem );
#endif
        }
        break;
    case GUI_INITMENUPOPUP:
        {
            char        text[100];

            GUI_GETID( param, id );
            sprintf ( text,"GUI_INITMENUPOPUP: id = %d", id );
            GUIDrawStatusText( MainWnd, text );
        }
        //GUISetFocus( Child3Wnd, COMBOBOX_CONTROL );
        break;
    case GUI_SCROLL_UP :
        Percent--;
        GUISetVScrollThumb( gui, Percent );
        break;
    case GUI_SCROLL_DOWN :
        Percent++;
        GUISetVScrollThumb( gui, Percent );
        break;
    case GUI_DESTROY :
        GUIMemFree( OldValue );
        OldValue = NULL;
        text = GUIGetText( gui, EDITWINDOW_CONTROL );
        GUIMemFree( text );
        if( MainWnd == gui ) {
            MainWnd = NULL;
        }
        if( Child3Wnd == gui ) {
            Child3Wnd = NULL;
        }
        break;
    case GUI_TOOLBAR_FLOATING :
    case GUI_TOOLBAR_FIXED :
        SetFixedFloat( gui );
        break;
    case GUI_TOOLBAR_DESTROYED :
        SetFixedFloat( gui );
        SetToolbarCreate( gui );
        break;
    case GUI_CONTROL_CLICKED :
        StaticDialogEventWnd( gui, gui_ev, param );
        break;
    case GUI_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case MENU_RESIZE :
        case MENU_MINIMIZE :
        case MENU_MAXIMIZE :
        case MENU_RESTORE :
        case MENU_CHANGE :
            if( gui == MainWnd ) {
                HandlePopup( gui, &Parent.rect, id );
            } else {
                HandlePopup( gui, &Child3.rect, id );
            }
            break;
        case MENU_STATIC_DIALOG :
            GUIGetRestoredSize( Child2Wnd, &client );
            StaticDialogCreate( MainWnd );
            break;
        case MENU_DYNAMIC_DIALOG :
            DynamicDialogCreate( MainWnd );
            break;
        case MENU_TEST_DIALOG :
            TestDialogCreate( MainWnd );
            break;
        case MENU_RES_DIALOG :
            ResDialogCreate( MainWnd );
            {
                gui_menu_struct *menu;
                int             num;
                GUICreateMenuStructFromRes( GUI_MAKEINTRESOURCE( 100 ), &menu, &num );
                if( menu && num ) {
                    int     i;
                    for( i = 0; i < num; i++ ) {
                        GUIAppendMenuToPopup( MainWnd, MENU_MODIFY_COLOUR, &menu[i], false );
                    }
                    GUIFreeGUIMenuStruct( menu, num );
                }
            }
            break;
        case MENU_GETNEWVAL :
            if( GUIGetNewVal( "Please enter new value :", OldValue, &new ) ==
                GUI_RET_OK ) {
                GUIMemFree( OldValue );
                OldValue = new;
            }
            break;
        case MENU_QUIT :
            GUIDestroyWnd( gui );
            GUIDestroyWnd( NULL );
            break;
        case MENU_EXIT :
            GUIDestroyWnd( gui );
            GUIDestroyWnd( NULL );
            break;
        case MENU_SPAWN :
#ifndef __OS2_PM__
            //WinExec( "g:\\lang\\binw\\viw.exe foobar", 1 );
#endif
            break;
        case MENU_REPLACE_COLOURS :
            if( Child3Wnd != NULL ) {
                num = GUIGetNumWindowColours( Child3Wnd );
                colours = GUIGetWindowColours( Child3Wnd );
                GUIMemFree( colours );
#if !default_colours
                GUISetWindowColours( Child3Wnd, GUI_NUM_ATTRS, &ParentColours );
#endif
            }
            break;
        case MENU_SHOW_WINDOW :
            if( Child1Wnd != NULL ) {
                {
                    gui_ord x;
                    x = GUIGetExtentX( Child1Wnd, LongStr, strlen(LongStr) );
                }
                if( !GUIIsWindowVisible( Child1Wnd ) ) {
                    GUIShowWindowNA( Child1Wnd );
                } else {
                    GUIHideWindow( Child1Wnd );
                }
            }
            break;
        case MENU_REDRAW :
            GUIWndDirty( NULL );
            break;
        case MENU_FIXED_TOOLBAR :
        case MENU_FLOATING_TOOLBAR :
            if( GUICreateToolBar( gui, ( id == MENU_FIXED_TOOLBAR ), 0,
                                  NUM_TOOLBAR_BUTTONS, ToolBar, true,
                                  &ToolPlain, &ToolStandout ) ) {
                SetFixedFloat( gui );
                SetToolbarCreate( gui );
            }
            break;
        case MENU_CHANGE_TOOLBAR :
            GUIChangeToolBar( gui );
            SetFixedFloat( gui );
            break;
        case MENU_CLOSE_TOOLBAR :
            GUICloseToolBar( gui );
            break;
        case MENU_STATUS_WND :
            if( GUIHasStatus( gui ) ) {
                GUICloseStatusWindow( gui );
                GUICheckMenuItem( gui, MENU_STATUS_WND, false, false );
                GUIEnableMenuItem( gui, MENU_RESIZE_STATUS, false, false );
            } else {
                if( GUICreateStatusWindow( gui, 0, 0, &StatusColour ) ) {
                    GUIDrawStatusText( gui, "Hi Lisa" );
                    GUICheckMenuItem( gui, MENU_STATUS_WND, true, false );
                    GUIEnableMenuItem( gui, MENU_RESIZE_STATUS, true, false );
                }
            }
            break;
        case MENU_HELP_CONTENTS:
            GUIShowHelp( help_inst, HelpWindow, GUI_HELP_CONTENTS, help_file, NULL );
            break;
        case MENU_HELP_ON_HELP:
            GUIShowHelp( help_inst, HelpWindow, GUI_HELP_ON_HELP, help_file, NULL );
            break;
        case MENU_HELP_SEARCH:
            GUIShowHelp( help_inst, HelpWindow, GUI_HELP_SEARCH, help_file, NULL );
            break;
        case MENU_HELP_CONTEXT:
            GUIShowHelp( help_inst, HelpWindow, GUI_HELP_CONTEXT, help_file, (char *)30 );
            break;
        case MENU_HELP_KEY:
            GUIShowHelp( help_inst, HelpWindow, GUI_HELP_KEY, help_file, "Starting_the_Browser" );
            break;
        case MENU_HELP :
            GUIDisplayHelp( gui, "c:\\win31\\winhelp.hlp", "help" );
            break;
        case MENU_CASCADE1 :
            GUICascadeWindows();
            break;
        case MENU_SWITCH_FONT :
            if( Child1Wnd != NULL && Child2Wnd != NULL ) {
                fontinfo = GUIGetFontInfo( Child1Wnd );
                if( fontinfo != NULL ) {
                    fontinfo = GUIGetFontFromUser( NULL );
                    if( GUISetFontInfo( Child2Wnd, fontinfo ) ) {
                        GUIWndDirty( Child2Wnd );
                    }
                    GUIMemFree( fontinfo );
                }
            }
            break;
        case MENU_RESIZE_STATUS :
            GUIGetTextMetrics( gui, &text_metrics );
            GUIResizeStatusWindow( gui, 0, 2 * text_metrics.max.y );
            break;
        case MENU_FLIP_MOUSE_ON :
            GUIGMouseOn();
            break;
        case MENU_FLIP_MOUSE_OFF :
            GUIGMouseOff();
        case MENU_SET_MENU :
            GUIResetMenus( gui, NUM_NEW_MAIN_MENUS, &NewMainMenu );
            GUIResetMenus( Child1Wnd, NUM_NEW_MAIN_MENUS, &NewMainMenu );
        case MENU_TEST_RESIZE :
            if( GUIIsMaximized( Child1Wnd ) ) {
                GUIRestoreWindow( Child1Wnd );
            } else {
                GUIMaximizeWindow( Child1Wnd );
            }
            GUIIsMaximized( gui );
            GUIGetClientRect( gui, &client );
            client.x = 0;
            client.y = 0;
            client.width /= 2;
            GUISetRestoredSize( Child1Wnd, &client );
            break;
        case MENU_RESET_MENU :
            GUIResetMenus( gui, NUM_MAIN_MENUS, MainMenu );
            GUIResetMenus( Child1Wnd, NUM_CHILD_MENUS, ChildMenu );
            break;
        case MENU_MODIFY_COLOUR :
            if( GUIGetColourFromUser( "Choose colour to modify",
                                       NULL, &colour ) ) {
                GUIGetRGB( colour, &rgb );
                if( GUIGetRGBFromUser( rgb, &rgb ) ) {
                    GUISetRGB( colour, rgb );
                }
            }
            break;
        case COMBOOKAY_CONTROL :
            new = GUIGetText( gui, EDITWINDOW_CONTROL );
            GUIDisplayMessage( MainWnd, new, "Issue Command:", GUI_RETRY_CANCEL );
            GUIClearText( gui, EDITWINDOW_CONTROL );
            GUIMemFree( new );
            break;
        }
        break;
    case GUI_KEY_CONTROL :
        GUI_GET_KEY_CONTROL( param, id, key );
        if( key == GUI_KEY_ENTER ) {
            new = GUIGetText( gui, id );
            GUIDisplayMessage( MainWnd, new, "Issue Command:", GUI_YES_NO_CANCEL );
            GUIMemFree( new );
            GUIClearText( gui, id );
        }
        break;
    case GUI_KEYDOWN :
        KeyDown = true;
#if parent_keys
        GUI_GET_KEY( param, KeyDownKey );
#endif
        break;
    case GUI_KEYUP :
#if parent_keys
        GUI_GET_KEY( param, key );
        if( ( KeyDown == true ) && ( KeyDownKey == key ) ) {
            ret = DisplayKey( key, Buffer );
            KeyDown = false;
        }
#endif
        ret = false;
        break;
    default :
        break;
    }
    return( ret );
}

static void DoOkay( gui_window *gui )
{
    change_struct       *change;
    char                *new;
    int                 act_length;
    int                 i;
    gui_rect            rect;

    GUIDisplayMessage( gui, "OK Button", "Got button clicked: ",
                       GUI_INFORMATION );
    change = GUIGetExtra( gui );
    new = GUIGetText( change->parent, change->edit_contr );
    if( new != NULL ) {
        act_length = strlen( new );
    } else {
        act_length = 0;
    }
    if( act_length < change->length ) {
        if( act_length > 0 ) {
            strncpy( change->string, new, act_length );
        }
        for( i = act_length; i < change->length; i++ ) {
            change->string[i] = ' ';
        }
    } else {
        strncpy( change->string, new, change->length );
    }
    GUIMemFree( new );
    GUIGetClientRect( change->wnd_to_update, &rect );
    GUIWndDirtyRect( change->wnd_to_update, &rect );
    GUIMemFree( change );
    GUISetExtra( gui, NULL );
}

static void DoCancel( gui_window *gui )
{
    GUIDisplayMessage( gui, "Cancel Button", "Got button clicked: ",
                       GUI_INFORMATION );
    GUIDestroyWnd( gui );
}

/*
 * ControlEventWnd - call back routine for the Controls
 */

bool ControlEventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_ctl_id          id;
    change_struct       *change;

    switch( gui_ev ) {
    case GUI_INIT_WINDOW :
        change = (change_struct *)GUIGetExtra( gui );
        change->parent = gui;
        DialogWindow = gui;
        gui_ev = GUI_INIT_DIALOG;
        break;
    case GUI_CLOSE :
        return( true );
    case GUI_DESTROY :
        change = GUIGetExtra( gui );
        GUIMemFree( change );
        GUISetExtra( gui, NULL );
        DialogWindow = NULL;
        break;
    case GUI_CONTROL_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case CANCELBUTTON_CONTROL :
           DoCancel( gui );
           return( true );
           break;
        case OKBUTTON_CONTROL :
           DoOkay( gui );
           break;
        default :
            break;
        }
    default :
        break;
    }
    return( StaticDialogEventWnd( gui, gui_ev, param ) );
}

static void GetNewVal( char *str, int length, gui_window *gui )
{
    DialogWndControl.parent = MainWnd;
    if( !WndScaled ) {
        SetWidthHeight( &DialogWndControl.rect, DialogWndControl.parent != NULL );
        WndScaled = true;
    }
    DialogWndControl.extra = MakeChangeStruct( str, length, gui );
    GUICreateWindow( &DialogWndControl );
}

static void HScroll( gui_window *gui, int diff )
{
    int new;
    int old;
    int range;

    old = GUIGetHScrollCol( gui );
    new = old + diff;
    if( new < 0 ) {
        new = 0;
        diff = -old;
    }
    range = GUIGetHScrollRangeCols( gui );
    if( new > range ) {
        new = range;
        diff = new - old;
    }
    if( diff != 0 ) {
        GUISetHScrollCol( gui, new );
    }
}

static void VScroll( gui_window *gui, int diff )
{
    int new;
    int old;
    int range;

    old = GUIGetVScrollRow( gui );
    new = old + diff;
    if( new < 0 ) {
        new = 0;
        diff = -old;
    }
    range = GUIGetVScrollRangeRows( gui );
    if( new > range ) {
        new = range;
        diff = new - old;
    }
    if( diff != 0 ) {
        GUISetVScrollRow( gui, new );
    }
}

static void ProcessCursor( gui_window *gui, gui_key key )
{
    gui_point           point;
    gui_text_metrics    metrics;

    GUIGetCursorPos( gui, &point );
    GUIGetTextMetrics( gui, &metrics );
    switch( key ) {
    case GUI_KEY_LEFT :
        point.x -= metrics.avg.x;
        break;
    case GUI_KEY_RIGHT :
        point.x += metrics.avg.x;
        break;
    case GUI_KEY_UP :
        point.y -= metrics.avg.y;
        break;
    case GUI_KEY_DOWN :
        point.y += metrics.avg.y;
        break;
    default :
       return;
    }
    GUISetCursorPos( gui, &point );
}

static void InitIndent( gui_window *gui, int num_rows, out_info *out )
{
    int         i;
    gui_ord     max_length;
    gui_ord     extent;

    max_length = 0;
    for( i = 0; i < num_rows; i++ ) {
        extent = GUIGetExtentX( gui, out->display[i].data,
                        strlen( out->display[i].data ) );
        if( extent > max_length ) {
            max_length = extent;
        }
    }
    for( i = 0; i < num_rows; i++ ) {
       IndentData[i].indent = max_length;
    }
}

static int GetStringIndent( int *indent, gui_ord hscroll,
                            gui_text_metrics *metrics )
{
    int string_indent;

    if( ( hscroll * metrics->avg.x ) < *indent ) {
        *indent -= hscroll * metrics->avg.x;
        string_indent = 0;
    } else {
        string_indent = hscroll - ( *indent / metrics->avg.x );
        *indent = 0;
    }
    return( string_indent );
}

void PaintWindow( gui_window *gui, gui_ord row, gui_ord num, int vscroll,
                  int hscroll )
{
    out_info            *out;
    int                 numrows;
    int                 i;
    attr_entry          *currattr;
    gui_ord             extent;
    int                 indent;
    size_t              string_indent;
    gui_text_metrics    metrics;
    size_t              length;
    gui_rect            client;
    char                *data;
    gui_coord           pos;

    row += vscroll;
    GUIGetTextMetrics( gui, &metrics );
    out = GUIGetExtra( gui );
    if( (row + num) > out->numrows ) {
        numrows = out->numrows - row;
    } else {
        numrows = num;
    }
    for( i = 0; i < numrows; i++ ) {
        indent = out->display[row + i].indent;
        string_indent = GetStringIndent( &indent, hscroll, &metrics );
        length = strlen( out->display[row + i].data );
        if( string_indent < length ) {
            pos.x = indent;
            pos.y = ( row + i - vscroll ) * metrics.avg.y;
            GUIDrawTextPos( gui, out->display[row + i].data + string_indent,
                         length, &pos, GUI_MENU_PLAIN );
        }
        for( currattr = out->display[row + i].attr_list; currattr != NULL;
             currattr = currattr->next ) {
            indent = GUIGetExtentX( gui, out->display[row+i].data,
                                    currattr->start );
            string_indent = GetStringIndent( &indent, hscroll, &metrics ) + currattr->start;
            length = strlen( out->display[row+i].data );
            if( string_indent < length ) {
                length = currattr->end - currattr->start + 1;
                if( currattr->start < string_indent ) {
                    length -= ( string_indent - currattr->start );
                }
                if( length > 0 ) {
                    GUIDrawText( gui, out->display[row+i].data + string_indent,
                                 length, row + i - vscroll, indent,
                                 currattr->attr );
                }
            }
        }
    }
    GUIGetClientRect( gui, &client );
    for( i = 0; i < numrows; i++ ) {
        indent = IndentData[row + i].indent;
        string_indent = GetStringIndent( &indent, hscroll, &metrics );
        length = strlen( IndentData[row + i].data );
        if( string_indent >= length ) {
            data = NULL;
            length = 0;
        } else {
            data = IndentData[row+i].data + string_indent;
        }
        extent = client.width;
        if( hscroll == 0 ) {
            extent += GUIGetHScroll( gui );
        }
        GUIDrawTextExtent( gui, data, length, row + i - vscroll, indent,
                           GUI_MENU_ACTIVE, GUI_NO_COLUMN );
#if hot_spots
        GUIDrawHotSpot( gui, (i % NUM_HOT_SPOTS) + 1, row + i - vscroll, indent,
                       GUI_MENU_ACTIVE );
#endif
    }
}

/*
 * Child1EventWnd - call back routine for the first child window
 */

bool Child1EventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    bool                ret;
    int                 diff;
    gui_rect            client;
    gui_text_metrics    metrics;
    gui_ord             row;
    gui_ord             num;
    gui_point           point;
    gui_key             key;
    gui_ctl_id          id;
    gui_char_cursor     cursor;
    char                Buffer[80];

    ret = false;
    switch( gui_ev ) {
    case GUI_INIT_WINDOW :
        GUIGetClientRect( gui, &client );
        GUISetVScrollRangeRows( gui, NUM_CHILD1_ROWS );
        GUIInitVScrollRow( gui, 1 );
        GUIGetTextMetrics( gui, &metrics );
        Child1HScrollRange = client.width / metrics.avg.x;
        GUISetHScrollRangeCols( gui, Child1HScrollRange );
        GUIInitHScroll( gui, 0 );
        InitIndent( gui, NUM_CHILD1_ROWS, GUIGetExtra( gui ) );
        GUIAppendMenuToPopup( gui, MENU_MORE, &MenuMore, false );
        ret = true;
        break;
    case GUI_PAINT :
        GUI_GET_ROWS( param, row, num );
        PaintWindow( gui, row, num, GUIGetVScrollRow( gui ),
                     GUIGetHScrollCol( gui ) );
        break;
    case GUI_FONT_CHANGED :
    case GUI_RESIZE :
        GUISetVScrollRangeRows( gui, NUM_CHILD1_ROWS );
        GUISetHScrollRangeCols( gui, Child1HScrollRange );
        break;
    case GUI_SCROLL_UP :
        VScroll( gui, -1 );
        break;
    case GUI_SCROLL_DOWN :
        VScroll( gui, 1 );
        break;
    case GUI_SCROLL_LEFT :
        HScroll( gui, -1 );
        break;
    case GUI_SCROLL_RIGHT :
        HScroll( gui, 1 );
        break;
    case GUI_SCROLL_PAGE_UP :
        VScroll( gui, -GUIGetNumRows( gui ) );
        break;
    case GUI_SCROLL_PAGE_DOWN :
        VScroll( gui, GUIGetNumRows( gui ) );
        break;
    case GUI_SCROLL_PAGE_LEFT :
        GUIGetClientRect( gui, &client );
        GUIGetTextMetrics( gui, &metrics );
        HScroll( gui, -( client.width / metrics.avg.x ) );
        break;
    case GUI_SCROLL_PAGE_RIGHT :
        GUIGetClientRect( gui, &client );
        GUIGetTextMetrics( gui, &metrics );
        HScroll( gui, client.width / metrics.avg.x );
        break;
    case GUI_SCROLL_VERTICAL :
        GUI_GET_SCROLL( param, diff );
        VScroll( gui, diff );
        break;
    case GUI_SCROLL_HORIZONTAL :
        GUI_GET_SCROLL( param, diff );
        HScroll( gui, diff );
        break;
    case GUI_LBUTTONDOWN :
    case GUI_RBUTTONDOWN :
        GUI_GET_POINT( param, point );
        GUISetCursorPos( gui, &point );
        if( GUIGetCursorType( gui, &cursor ) ) {
            switch( cursor ) {
            case GUI_NO_CURSOR :
               cursor = GUI_NORMAL_CURSOR;
               break;
            case GUI_NORMAL_CURSOR :
               cursor = GUI_INSERT_CURSOR;
               break;
            case GUI_INSERT_CURSOR :
               cursor = GUI_NO_CURSOR;
               break;
            }
            GUISetCursorType( gui, cursor );
        }
        break;
    case GUI_KEYDOWN :
        GUI_GET_KEY( param, key );
        switch( key ) {
        case GUI_KEY_LEFT :
        case GUI_KEY_RIGHT :
        case GUI_KEY_UP :
        case GUI_KEY_DOWN :
            GUILoadString( 1041, Buffer, 80 );
            GUIDisplayMessage( gui, Buffer, "Load String", GUI_INFORMATION );
            ProcessCursor( gui, key );
            break;
        case GUI_KEY_PAGEUP :
                Child1EventWnd( gui, GUI_SCROLL_PAGE_UP, NULL );
                break;
        case GUI_KEY_PAGEDOWN :
                Child1EventWnd( gui, GUI_SCROLL_PAGE_DOWN, NULL );
                break;
        default :
            break;
        }
        break;
    case GUI_KEYUP :
#if keys
        GUI_GET_KEY( param, key );
        ret = DisplayKey( key, Buffer );
#endif
        ret = false;
     case GUI_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case MENU_RESET_MENU :
            GUIResetMenus( MainWnd, NUM_MAIN_MENUS, MainMenu );
            GUIResetMenus( Child1Wnd, NUM_CHILD_MENUS, ChildMenu );
        }
    default :
        return( ChildEventWnd( gui, gui_ev, param ) );
    }
    return( ret );
}

/*
 * Child3EventWnd - call back routine for the third child window
 */

bool Child3EventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    gui_point           point;
    gui_rgb             rgb, green;
    gui_text_metrics    text_metrics;

    switch( gui_ev ) {
    case GUI_LBUTTONDOWN :
        GUI_GET_POINT( param, StartRectPoint );
        break;
    case GUI_LBUTTONUP :
        GUI_GET_POINT( param, point );
        SFillRect.x = StartRectPoint.x;
        SFillRect.y = StartRectPoint.y;
        SFillRect.width = point.x - StartRectPoint.x;
        SFillRect.height = point.y - StartRectPoint.y;
        GUIWndDirty( gui );
        break;
    case GUI_RBUTTONDOWN :
        GUI_GET_POINT( param, StartPoint );
        break;
    case GUI_RBUTTONUP :
        GUI_GET_POINT( param, EndPoint );
        GUIWndDirty( gui );
        break;
    case GUI_PAINT :
        GUIGetTextMetrics( gui, &text_metrics );
//      GUIDrawLine( gui, &StartPoint, &EndPoint, GUI_PEN_SOLID,
//                  text_metrics.avg.x, GUI_FIRST_UNUSED );
        GUIGetRGB( GUI_BRIGHT_YELLOW, &rgb );
        GUIGetRGB( GUI_GREEN, &green );
        GUIDrawTextRGB( gui, "Line Draw!", 10, 1, 1, rgb, green );
        GUIDrawTextPosRGB( gui, "Line Draw!", 10, (gui_coord *)&StartPoint, green, rgb );
        GUIDrawLineRGB( gui, &StartPoint, &EndPoint, GUI_PEN_SOLID, 6,
                        rgb );
        GUIFillRect( gui, &SFillRect, GUI_BLUE );
        GUIDrawBar( gui,  6, 1, 20, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, false );
        GUIDrawTextRGB( gui, "Simple Bars", 11, 6, 75, rgb, green );
        GUIDrawBar( gui,  7, 1, 30, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, true );
        GUIDrawBar( gui,  8, 1, 70, GUI_BAR_SIMPLE, GUI_FRAME_ACTIVE, false );
        GUIDrawBar( gui,  9, 1, 20, GUI_BAR_SHADOW, GUI_FRAME_ACTIVE, false );
        GUIDrawTextRGB( gui, "Shadow Bars", 11, 9, 75, rgb, green );
        GUIDrawBar( gui, 10, 1, 30, GUI_BAR_SHADOW, GUI_FRAME_ACTIVE, true );
        GUIDrawBar( gui, 11, 1, 50, GUI_BAR_SHADOW, GUI_FRAME_ACTIVE, false );
        break;
    default :
        return( MainEventWnd( gui, gui_ev, param ) );
    }
    return( false );
}

#if swap_enable
static bool Enabled = true;
#endif

static void CreatePopup( gui_window *gui, int num_menus,
                         gui_menu_struct *menu, gui_ctl_id popup_id, bool sub )
{
    int                 i;
    gui_menu_struct     *sub_menu;
    int                 num_sub;

    for( i = 0; i < num_menus; i++ ) {
        if( sub ) {
            GUIAppendMenuToPopup( gui, popup_id, &menu[i], true );
        } else {
            sub_menu = menu[i].child;
            num_sub = menu[i].num_child_menus;
            menu[i].num_child_menus = 0;
            menu[i].child = NULL;
            GUIAppendMenu( gui, &menu[i], true );
            menu[i].num_child_menus = num_sub;
            menu[i].child = sub_menu;
        }
        if( menu[i].child != NULL ) {
            CreatePopup( gui, menu[i].num_child_menus, menu[i].child, menu[i].id, true );
        }
    }
}

/*
 * ChildEventWnd - call back routine for the child window(s)
 */

bool ChildEventWnd( gui_window *gui, gui_event gui_ev, void *param )
{
    bool                ret;
    gui_point           point;
    gui_ord             row;
    gui_ord             col;
#if keys
    gui_key             key;
#endif
    gui_ctl_id          id;
    attr_entry          *new_attr;
    char                Buffer[80];
    int                 i;
    int                 num;
    gui_ord             prev_col;
    out_info            *out;
    char                *start;
    char                *end;
    char                *farend;
    int                 length;
    gui_rect            client;
    gui_coord           size;
    gui_text_metrics    metrics;
    attr_entry          *nextattr;
    attr_entry          *currattr;
    gui_create_styles   style;
    int                 vscroll;

    ret = false;
    vscroll = 0;
    switch( gui_ev ) {
    case GUI_CLOSE :
        ret = true;
        break;
    case GUI_INIT_WINDOW :
        GUIGetClientRect( gui, &client );
        GUIGetTextMetrics( gui, &metrics );
        GUISetHScrollRangeCols( gui, client.width / metrics.avg.x );
        GUISetVScrollRangeRows( gui, NUM_CHILD2_ROWS );
        InitIndent( gui, NUM_CHILD2_ROWS, GUIGetExtra( gui ) );
        ret = true;
        break;
    case GUI_LBUTTONDOWN :
        if( gui == Child2Wnd ) {
            GUISetMouseCursor( GUI_HOURGLASS_CURSOR );
#if swap_enable
            Enabled = !Enabled;
            GUIEnableMDIMenus( Enabled );
#endif
        }
        break;
    case GUI_LBUTTONUP :
        if( gui == Child2Wnd ) {
            GUISetMouseCursor( GUI_ARROW_CURSOR );
        }
        break;
    case GUI_ICONIFIED :
        GUIBringToFront( MainWnd );
        break;
    case GUI_RESIZE :
        GUI_GET_SIZE( param, size );
        break;
    case GUI_DESTROY :
        out = GUIGetExtra( gui );
        for( i = 0; i < out->numrows; i++ ) {
            for( currattr = out->display[i].attr_list; currattr != NULL;
                 currattr = nextattr ) {
                 nextattr = currattr->next;
                 GUIMemFree( currattr );
            }
        }
        if( gui == Child1Wnd ) {
            Child1Wnd = NULL;
        }
        if( gui == Child2Wnd ) {
            Child2Wnd = NULL;
        }
        break;
    case GUI_PAINT :
        GUI_GET_ROWS( param, row, num );
        PaintWindow( gui, row, num, 0, 0 );
        break;
    case GUI_MOUSEMOVE :
        if( Highlight ) {
            GUI_GET_POINT( param, point );
            row = GUIGetRow( gui, &point );
            style = GUIGetCreateStyle( gui );
            if( ( style & GUI_VSCROLL_EVENTS ) != 0 ) {
                vscroll = GUIGetVScrollRow( gui );
                row += vscroll;
            }
            out = GUIGetExtra( gui );
            if( row < out->numrows && row >= 0 ) {
                col = GUIGetCol( gui, out->display[row].data, &point );
                if( ( style & GUI_HSCROLL_EVENTS ) != 0 ) {
                    col += GUIGetHScrollCol( gui );
                }
                if( row == PrevRow ) {
                    if( ( col < strlen( out->display[row].data ) ) &&
                        ( out->display[row].attr_list->start < col ) &&
                        ( out->display[row].attr_list->end < col ) ) {
                        prev_col = out->display[row].attr_list->end;
                        out->display[row].attr_list->end = col;
                        if( style & GUI_VSCROLL_EVENTS ) {
                            row -= vscroll;
                        }
                        GUIWndDirtyRow( gui, row );
                    }
                }
            }
        }
        break;
    case GUI_RBUTTONUP :
        Highlight = false;
        GUI_GET_POINT( param, point );
        GUIGetMousePosn( gui, &point );
#if dynamic_menus
        CreatePopup( gui, NUM_POPUP_MENUS, PopupMenu, GUI_NO_EVENT, false );
//      CreatePopup( gui, 1, &PopupMenu[NUM_POPUP_MENUS-1], GUI_NO_EVENT, false );
        GUITrackFloatingPopup( gui, &point, GUI_TRACK_RIGHT, NULL); //&CurrPopupItem );
#else
        GUICreateFloatingPopup( gui, &point, NUM_POPUP_MENUS, &PopupMenu, true,
                                &CurrPopupItem );
#endif
        break;
    case GUI_RBUTTONDOWN :
        GUI_GET_POINT( param, point );
        Highlight = true;
        row = GUIGetRow( gui, &point );
        style = GUIGetCreateStyle( gui );
        if( ( style & GUI_VSCROLL_EVENTS ) != 0 ) {
            vscroll = GUIGetVScrollRow( gui );
            row += vscroll;
        }
        out = GUIGetExtra( gui );
        if( row < out->numrows ) {
            col = GUIGetStringPos( gui, IndentData[row].indent,
                                   IndentData[row].data, point.x );
            if( col != GUI_NO_COLUMN ) {
                sprintf( Buffer, "Mouse press - position %d", col );
                GUIDisplayMessage( gui, Buffer, "SAMPLE PROGRAM", GUI_INFORMATION );
            }
            col = GUIGetCol( gui, out->display[row].data, &point );
            if( ( style & GUI_HSCROLL_EVENTS ) != 0 ) {
                col += GUIGetHScrollCol( gui );
            }
            if( col < strlen( out->display[row].data ) ) {
                PrevRow = row;
                new_attr = (attr_entry *)GUIMemAlloc( sizeof( attr_entry ) );
                new_attr->start = col;
                new_attr->end = col;
#if default_colours
                new_attr->attr = GUI_MENU_STANDOUT;
#else
                new_attr->attr = GUI_FIRST_UNUSED;
#endif
                new_attr->next = out->display[row].attr_list;
                out->display[row].attr_list = new_attr;
                if( style & GUI_VSCROLL_EVENTS ) {
                    row -= vscroll;
                }
                GUIWndDirtyRow( gui, row );
            }
        }
        break;
    case GUI_LBUTTONDBLCLK :
        GUI_GET_POINT( param, point );
        row = GUIGetRow( gui, &point );
        style = GUIGetCreateStyle( gui );
        if( ( style & GUI_VSCROLL_EVENTS ) != 0 ) {
            vscroll = GUIGetVScrollRow( gui );
            row += vscroll;
        }
        out = GUIGetExtra( gui );
        if( row < out->numrows ) {
            col = GUIGetCol( gui, out->display[row].data, &point );
            length = strlen( out->display[row].data );
            if( col < length ) {
                farend = out->display[row].data + length - 1;
                start = out->display[row].data + col;
                end = start;
                while( ( *start != ' ' ) && ( start >= out->display[row].data ) )  start--;
                start++;
                while( ( *end != ' ' ) && ( end <= farend ) )  end++;
                end--;
                if( start <= end ) {
                    length = strlen( start ) - strlen( end ) +1;
                    GetNewVal( start, length, gui );
                }
            }
        }
        break;
    case GUI_CLICKED :
        GUI_GETID( param, id );
        switch( id ) {
        case MENU_RESIZE :
        case MENU_MINIMIZE :
        case MENU_MAXIMIZE :
        case MENU_RESTORE :
        case MENU_CHANGE :
            if( gui == Child2Wnd ) {
                HandlePopup( gui, &Child2.rect, id );
            } else {
                HandlePopup( gui, &Child1.rect, id );
            }
            break;
        case MENU_BRING_MAIN_TO_FRONT  :
            GUIBringToFront( MainWnd );
            break;
        case MENU_GET_CLIENT :
            GUIGetClientRect( gui, &client );
            sprintf( Buffer, "Got client : %d %d %d %d", client.x,
                     client.y, client.width, client.height );
            GUIDisplayMessage( gui, Buffer, "SAMPLE PROGRAM", GUI_INFORMATION );
            break;
        case MENU_GET_RECT :
            GUIGetRect( gui, &client );
            sprintf( Buffer, "Got Rect : %d %d %d %d", client.x,
                     client.y, client.width, client.height );
            GUIDisplayMessage( gui, Buffer, "SAMPLE PROGRAM", GUI_INFORMATION );
            GUIGetRestoredSize( gui, &client );
            sprintf( Buffer, "Got Rect : %d %d %d %d", client.x,
                     client.y, client.width, client.height );
            GUIDisplayMessage( gui, Buffer, "SAMPLE PROGRAM", GUI_INFORMATION );
            break;
        case MENU_GET_ABS_RECT :
            GUIGetAbsRect( gui, &client );
            sprintf( Buffer, "Got Rect : %d %d %d %d", client.x,
                     client.y, client.width, client.height );
            GUIDisplayMessage( gui, Buffer, "SAMPLE PROGRAM", GUI_INFORMATION );
            break;
        }
        break;
    case GUI_KEYDOWN :
        KeyDown = true;
#if keys
        GUI_GET_KEY( param, KeyDownKey );
        if( KeyDownKey == GUI_KEY_a  ) {
            GUISetHScrollCol( gui, GUIGetHScrollCol( gui ) + 1 );
        }
        if( KeyDownKey == GUI_KEY_s  ) {
            GUIGetMousePosn( gui, &point );
            GUICreateFloatingPopup( gui, &point, NUM_POPUP_MENUS, PopupMenu, true,
                                    &CurrPopupItem );

        }
#endif
        ret = true;
        break;
    case GUI_KEYUP :
#if keys
        GUI_GET_KEY( param, key );
        if( ( KeyDown == true ) && ( KeyDownKey == key ) ) {
            ret = DisplayKey( key, Buffer );
            KeyDown = false;
        }
#endif
        ret = false;
        break;
    default :
        break;
    }
    return( ret );
}

void SetWidthHeight( gui_rect *rect, bool has_parent )
{
    if( !has_parent ) {
        rect->x = (gui_ord)((long)(rect->x - ScreenRect.x ) *
                       (long)Scale.x / (long)ScreenRect.width ) + ScreenRect.x;
        rect->y = (gui_ord)((long)(rect->y - ScreenRect.y ) *
                       (long)Scale.y / (long)ScreenRect.height ) + ScreenRect.y;
    } else {
        rect->x = (gui_ord)((long)rect->x * (long)Scale.x /
                                 (long)ScreenRect.width );
        rect->y = (gui_ord)((long)rect->y * (long)Scale.y /
                                 (long)ScreenRect.height );
    }
    rect->width = (gui_ord)((float)rect->width * (float)Scale.x /
                                 (float)ScreenRect.width);
    rect->height = (gui_ord)((float)rect->height * (float)Scale.y /
                                  (float)ScreenRect.height);
}

/*
 * main - main function for application - called by GUI library
 */

void GUImain( void )
{
    gui_window          *parent_wnd;
    gui_system_metrics  metrics;
#if hot_spots
    gui_coord           size;
#endif
#ifdef HELL_FREEZES_OVER
    gui_colour_set      colour_set;
#endif

    //GUIMemOpen();

    GUIWndInit( 300, GUI_GMOUSE );
    GUI3DDialogInit();
#if mdi
    GUIMDIInitMenuOnly();
    //GUIMDIInit();
#endif
#if !default_colours
//  GUISetBackgroundColour( &BackgroundColours );
//    GUISetDialogColours( &DialColours, &DialFrameColours );
#endif
    GUIGetRoundScale( &Scale );
    if( !GUIIsGUI() ) {
        WndScaled = true;
        DialogScaled = true;
        ControlsScaled = true;
    }

    StaticDialogInit();

    ScaleRect.width = Scale.x;
    ScaleRect.height = Scale.y;
    GUISetScale( &ScaleRect );
#if hot_spots
    GUIInitHotSpots( NUM_HOT_SPOTS, &HotSpots );
    if( GUIGetNumHotSpots() == NUM_HOT_SPOTS ) {
        GUIGetHotSpotSize( 1, &size );
        GUIGetHotSpotSize( NUM_HOT_SPOTS, &size );
    }
#endif
    GUIGetSystemMetrics( &metrics );
    SetWidthHeight( &Parent.rect, Parent.parent != NULL );
    MainWnd = GUICreateWindow( &Parent );
    parent_wnd = MainWnd;

    if( MainWnd == NULL ) {
        return;
    }

#ifdef __OS2_PM__
    //help_file = "d:\\toolkt21\\os2help\\dlgedit.hlp";
    help_file = "r:\\dlang\\binp\\help\\wfcopts.hlp";
#elif defined( __UNIX__ )
    help_file = "/usr/watcom/hlp/sample.hlp";
#else
    //help_file = "d:\\windev\\bin\\win31wh.hlp";
    help_file = "f:\\src\\win\\browser.hlp";
#endif
    if( !GUIIsGUI() ) {
        help_file = "f:\\wdbg.ihp";
    }
    HelpWindow = MainWnd;
    help_inst = GUIHelpInit( HelpWindow, help_file, "GUI Sample" );
#if controls_on_main
    {
        int     i;
        int     act_num;

        act_num = 0;
        for( i = 0; i < NUM_CONTROLS; i++ ) {
            Controls[i].parent = MainWnd;
            if( !ControlsScaled ) {
                SetWidthHeight( &Controls[i].rect, Controls[i].parent != NULL );
            }
            if( GUIAddControl( &Controls[i], &ToolPlain, &ToolStandout ) ) {
                act_num++;
            }
        }
        ControlsScaled = true;
        GUISetExtra( MainWnd, &act_num );
        StaticDialogEventWnd( MainWnd, GUI_INIT_DIALOG, NULL );
    }
#endif
#if controls_on_parent
    ComboBoxControl.parent = parent_wnd;
    SetWidthHeight( &ComboBoxControl.rect, ComboBoxControl.parent != NULL );
    GUIAddControl( &ComboBoxControl, &ToolPlain, &ToolStandout );
    GUISetText( MainWnd, EDITWINDOW_CONTROL, "default" );

    ComboBoxControl2.parent = parent_wnd;
    SetWidthHeight( &ComboBoxControl2.rect, ComboBoxControl2.parent != NULL );
    GUIAddControl( &ComboBoxControl2, &ToolPlain, &ToolStandout  );

    ComboOkayControl.parent = parent_wnd;
    SetWidthHeight( &ComboOkayControl.rect, ComboOkayControl.parent != NULL );
    GUIAddControl( &ComboOkayControl, &ToolPlain, &ToolStandout  );
#endif // controls_on_parent

    Child1.parent = parent_wnd;
    SetWidthHeight( &Child1.rect, Child1.parent != NULL );
    Child1Wnd = GUICreateWindow( &Child1 );

#if 1
    Child4.parent = parent_wnd;
    SetWidthHeight( &Child4.rect, Child4.parent != NULL );
    Child4Wnd = GUICreateWindow( &Child4 );

    Child4.parent = Child4Wnd;
    SetWidthHeight( &Child4.rect, Child4.parent != NULL );
    Child4_1Wnd = GUICreateWindow( &Child4 );

    Child4.parent = Child4_1Wnd;
    SetWidthHeight( &Child4.rect, Child4.parent != NULL );
    Child4_2Wnd = GUICreateWindow( &Child4 );

    Child4.parent = Child4_2Wnd;
    Child4.title = NULL;
    SetWidthHeight( &Child4.rect, Child4.parent != NULL );
    Child4_3Wnd = GUICreateWindow( &Child4 );
#endif

    Child2.parent = parent_wnd;
    SetWidthHeight( &Child2.rect, Child2.parent != NULL );
    Child2Wnd = GUICreateWindow( &Child2 );

#if test_colours
    GUIGetWndColour( Child2Wnd, GUI_FIRST_UNUSED, &colour_set );
    if( GUIGetColourFromUser( "Choose highlight background colour",
                               &colour_set.back, &colour_set.back ) ) {
        GUISetWndColour( Child2Wnd, GUI_FIRST_UNUSED, &colour_set );
    }
    GUIGetWndColour( Child2Wnd, GUI_BACKGROUND, &colour_set );
    if( GUIGetColourFromUser( "Choose window background colour",
                               &colour_set.back, &colour_set.back ) ) {
        GUISetWndColour( Child2Wnd, GUI_BACKGROUND, &colour_set );
    }
#endif

    Child3.parent = parent_wnd;
    SetWidthHeight( &Child3.rect, Child3.parent != NULL );
    Child3Wnd = GUICreateWindow( &Child3 );

#if controls_on_child3
    if( Child3Wnd != NULL ) {
        int i;
        ListBoxControl.parent = Child3Wnd;
        SetWidthHeight( &ListBoxControl.rect, ListBoxControl.parent != NULL );
        GUIAddControl( &ListBoxControl, &ToolPlain, &ToolStandout  );
        GUISetText( Child3Wnd, COMBOBOX_CONTROL, "default" );
        GUISetText( Child3Wnd, COMBOBOX_CONTROL, "default" );
        i = GUIGetExtentX( Child3Wnd, "default", 7 );
        i = GUIGetExtentY( Child3Wnd, "default" );
        i = GUIGetControlExtentX( Child3Wnd, COMBOBOX_CONTROL, "default", 7 );
        i = GUIGetControlExtentY( Child3Wnd, COMBOBOX_CONTROL, "default" );
        //GUISetFocus( Child3Wnd, COMBOBOX_CONTROL );
        //GUISelectAll( Child3Wnd, COMBOBOX_CONTROL, true );
        //for( i = 0; i < NUM_LIST_BOX_DATA; i++ ) {
            //GUIAddText( Child3Wnd, COMBOBOX_CONTROL, ListBoxData[i] );
        //}
#if combo
        for( i = 0; i< NUM_COMBOBOX_STRINGS; i++ ) {
             GUIAddText( Child1Wnd, COMBOBOX_CONTROL, ComboBoxStrings[i] );
        }
#endif
    }
#endif // controls_on_child3

#ifdef HELL_FREEZES_OVER
    GUIGetColourFromUser( "Choose status window foreground colour",
                               &colour_set.fore, &colour_set.fore );
    GUIGetColourFromUser( "Choose status window background colour",
                               &colour_set.back, &colour_set.back );
    GUICreateStatusWindow( MainWnd, 0, 0, &colour_set );
#endif
    GUISetMenuText( MainWnd, MENU_DIALOGS, "Wes' &Dialogs", false );
    GUIEnableMenuItem( MainWnd, MENU_QUIT, false, false );
    GUIEnableMenuItem( MainWnd, MENU_CASCADE, false, false );
    GUICheckMenuItem( MainWnd, MENU_CASCADE, true, false );
    GUISetMenuText( MainWnd, MENU_CASCADE, "Wes' Cascade", false );
    GUIDeleteMenuItem( MainWnd, MENU_CASCADE, false );
    //GUIInsertMenuToPopup( MainWnd, MENU_MODIFY_COLOUR, 0, &ChildMenu, false );
}

bool GUIDeath ( void )
{
    return( true );
}

char *GUIGetWindowClassName( void )
{
    return( "foobar" );
}

