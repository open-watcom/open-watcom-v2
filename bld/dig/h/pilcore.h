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


#ifndef PILCORE_H

#define PIL_CORE_VERSION_MAJOR  1
#define PIL_CORE_VERSION_MINOR  0

#define PIL_CORE_SERVICE        0

#undef pilrdef
#define pilrdef( s, e ) REQ_##s##_##e,

enum pil_core_requests {
    #include "pcreq.h"
    REQ_CORE_LAST
};

enum pil_core_info_bits {
    PCIB_NONE           = 0x00,
    PCIB_CPU_CONFIG     = 0x01,
    PCIB_OS_CONFIG      = 0x02,
    PCIB_ADDR_ALIAS     = 0x04,
    PCIB_IMAGE          = 0x08,
    PCIB_SUPPLEMENTARY  = 0x10,
    PICB_STATE          = 0x20,
    PICB_MESSAGE        = 0x40
};

#undef pilrdef
#define pilrdef( s, e ) RES_##s##_##e,

enum pil_core_responses {
    #include "pcres.h"
    RES_CORE_LAST
};

enum pil_core_cpu_config_item {
    PCCI_OFFSET_SIZE,
    PCCI_SEGMENT_SIZE
};

enum pil_core_os_config_item {
    PCOI_SECTION_SIZE,
    PCOI_FHANDLE_SIZE,
    PCOI_DHANDLE_SIZE,
    PCOI_IHANDLE_SIZE,
    PCOI_PID_SIZE,
    PCOI_TID_SIZE,
    PCOI_PRETCODE_SIZE,
    PCOI_TRETCODE_SIZE,
    PCOI_FLAGS,
    PCOI_ARGV2RAW,
    PCOI_RAW2ARGV
};

enum pil_thread_state {
    PTS_RUNNING,
    PTS_STOPPED,
    PTS_EXEC,
    PTS_TRACE,
    PTS_EVENT,
    PTS_BREAK,
    PTS_EXCEPTION,
    PTS_USER,
    PTS_WARP,
    PTS_THREAD_TERM,
    PTS_PROC_TERM
};

#define PILCORE_H
#endif
