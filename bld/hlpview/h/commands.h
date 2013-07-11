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
* Description:  RTF commands that whc recognizes.
*
****************************************************************************/


// Keep these entries sorted alphabetically by their strings!
// The parser kinda depends on that.

_COMMAND( RC_ANSI,      "ansi"          ),
_COMMAND( RC_B,         "b"             ),
_COMMAND( RC_BOX,       "box"           ),
_COMMAND( RC_COLORTBL,  "colortbl"      ),
_COMMAND( RC_DEFF,      "deff"          ),
_COMMAND( RC_DN,        "dn"            ),
_COMMAND( RC_ENDNHERE,  "endnhere"      ),
_COMMAND( RC_F,         "f"             ),
_COMMAND( RC_FALT,      "falt"          ),
_COMMAND( RC_FBIDI,     "fbidi"         ),
_COMMAND( RC_FDECOR,    "fdecor"        ),
_COMMAND( RC_FI,        "fi"            ),
_COMMAND( RC_FMODERN,   "fmodern"       ),
_COMMAND( RC_FNIL,      "fnil"          ),
_COMMAND( RC_FONTEMB,   "fontemb"       ),
_COMMAND( RC_FONTTBL,   "fonttbl"       ),
_COMMAND( RC_FOOTNOTE,  "footnote"      ),
_COMMAND( RC_FROMAN,    "froman"        ),
_COMMAND( RC_FS,        "fs"            ),
_COMMAND( RC_FSCRIPT,   "fscript"       ),
_COMMAND( RC_FSWISS,    "fswiss"        ),
_COMMAND( RC_FTECH,     "ftech"         ),
_COMMAND( RC_FTNBJ,     "ftnbj"         ),
_COMMAND( RC_I,         "i"             ),
_COMMAND( RC_INFO,      "info"          ),
_COMMAND( RC_KEEP,      "keep"          ),
_COMMAND( RC_KEEPN,     "keepn"         ),
_COMMAND( RC_LI,        "li"            ),
_COMMAND( RC_LINE,      "line"          ),
_COMMAND( RC_LINEX,     "linex"         ),
_COMMAND( RC_PAGE,      "page"          ),
_COMMAND( RC_PAR,       "par"           ),
_COMMAND( RC_PARD,      "pard"          ),
_COMMAND( RC_PC,        "pc"            ),
_COMMAND( RC_PLAIN,     "plain"         ),
_COMMAND( RC_QC,        "qc"            ),
_COMMAND( RC_QJ,        "qj"            ),
_COMMAND( RC_QL,        "ql"            ),
_COMMAND( RC_QR,        "qr"            ),
_COMMAND( RC_RI,        "ri"            ),
_COMMAND( RC_RTF,       "rtf"           ),
_COMMAND( RC_SA,        "sa"            ),
_COMMAND( RC_SB,        "sb"            ),
_COMMAND( RC_SCAPS,     "scaps"         ),
_COMMAND( RC_SECT,      "sect"          ),
_COMMAND( RC_SECTD,     "sectd"         ),
_COMMAND( RC_SL,        "sl"            ),
_COMMAND( RC_STRIKE,    "strike"        ),
_COMMAND( RC_STYLESHEET,"stylesheet"    ),
_COMMAND( RC_TAB,       "tab"           ),
_COMMAND( RC_TQC,       "tqc"           ),
_COMMAND( RC_TQR,       "tqr"           ),
_COMMAND( RC_TX,        "tx"            ),
_COMMAND( RC_UL,        "ul"            ),
_COMMAND( RC_ULDB,      "uldb"          ),
_COMMAND( RC_ULW,       "ulw"           ),
_COMMAND( RC_UP,        "up"            ),
_COMMAND( RC_V,         "v"             ),
_COMMAND( RC_WINDOWS,   "windows"       ),

// The last entry is a terminator, please leave it in.
_COMMAND( RC_DUMMY, "zzzzz"  )
