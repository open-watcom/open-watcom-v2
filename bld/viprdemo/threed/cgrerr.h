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


#ifndef CGR_ERROR_H
#define CGR_ERROR_H
/*
 Description:
 ============
     This file defines the error codes which can be returned by some
     of the cgr external routines. cgr_error can be called with
     this number to return an error message. Also, the cgr_err_check
     values are defined here (see cgr_chart_check).

*/

typedef enum {
    CGR_ERR_NONE,               // no error
    CGR_ERR_SYSTEM,             // fatal system error
    CGR_ERR_BAD_PRINT,          // could not spawn off print to printer
    CGR_ERR_NO_PRINTER,         // no printer to print to
    CGR_ERR_MEMORY,             // out of memory
    /**** load/save errors ****/
    CGR_ERR_EOF,                // premature end of file
    CGR_ERR_BAD_LINE,           // improper line format
    CGR_ERR_SYNTAX,             // unexpected text/option
    CGR_ERR_RANGE,              // value out of range
    CGR_ERR_BAD_OPTION,         // unrecognized option
    CGR_ERR_BAD_FILE,           // not a chart graphics file

    CGR_ERR_SCALE1,             // start before end value
    CGR_ERR_SCALE2,             // negative step
    CGR_ERR_SCALE3,             // too many intervals
    CGR_ERR_SCALE4,             // pro chart Y step in [0, 100]

    CGR_ERR_VIEWPORT_1,         // bad viewport: left > right
    CGR_ERR_VIEWPORT_2,         // bad viewport: bottom > top

    CGR_ERR_Y2SCALE1,           // start before end value, y2 scale
    CGR_ERR_Y2SCALE2,           // negative step, y2 scale
    CGR_ERR_Y2SCALE3,           // too many intervals, y2 scale
    CGR_ERR_Y2SCALE4,           // pro chart Y2 step in [0, 100]

    CGR_ERR_XSCALE1,            // start before end value, x scale
    CGR_ERR_XSCALE2,            // negative step, x scale
    CGR_ERR_XSCALE3,            // too many intervals, x scale

    /* ADD NEW ERRORS HERE. SEE ERROR.C */


    /**** internal errors: group these together LAST ****/
    CGR_ERR_INTERNAL_1          = 100,          // bad chart type
    CGR_ERR_INTERNAL_2                          // nothing to draw
} cgr_err;

#endif
