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
* Description:  Declaration of various document sections constants
*
****************************************************************************/


pick( "NONE",      DSECT_none,     no_ban       )    // nothing so far
pick( "GDOC",      DSECT_gdoc,     no_ban       )    // gdoc
pick( "FRONTM",    DSECT_frontm,   no_ban       )    // front matter
pick( "TITLEP",    DSECT_titlep,   no_ban       )    // title page
pick( "eTITLEP",   DSECT_etitlep,  no_ban       )    // end title page
pick( "ABSTRACT",  DSECT_abstract, abstract_ban )    // abstract
pick( "PREFACE",   DSECT_preface,  preface_ban  )    // preface
pick( "TOC",       DSECT_toc,      toc_ban      )    // table of contents
pick( "FIGLIST",   DSECT_figlist,  figlist_ban  )    // figure list
pick( "BODY",      DSECT_body,     body_ban     )    // body
pick( "APPENDIX",  DSECT_appendix, appendix_ban )    // appendix
pick( "BACKM",     DSECT_backm,    backm_ban    )    // back matter
pick( "INDEX",     DSECT_index,    index_ban    )    // index
pick( "TOC",       DSECT_toce,     toc_ban      )    // table of contents (at end of file)
pick( "FIGLIST",   DSECT_figliste, figlist_ban  )    // figure list (at end of file)
pick( "eGDOC",     DSECT_egdoc,    no_ban       )    // egdoc  has to be last
