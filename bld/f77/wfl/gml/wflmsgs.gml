:cmt *****************************************************************************
:cmt *
:cmt *                            Open Watcom Project
:cmt *
:cmt * Copyright (c) 2002-2021 The Open Watcom Contributors. All Rights Reserved.
:cmt *    Portions Copyright (c) 1983-2002 Sybase, Inc. All Rights Reserved.
:cmt *
:cmt *  ========================================================================
:cmt *
:cmt *    This file contains Original Code and/or Modifications of Original
:cmt *    Code as defined in and that are subject to the Sybase Open Watcom
:cmt *    Public License version 1.0 (the 'License'). You may not use this file
:cmt *    except in compliance with the License. BY USING THIS FILE YOU AGREE TO
:cmt *    ALL TERMS AND CONDITIONS OF THE LICENSE. A copy of the License is
:cmt *    provided with the Original Code and Modifications, and is also
:cmt *    available at www.sybase.com/developer/opensource.
:cmt *
:cmt *    The Original Code and all software distributed under the License are
:cmt *    distributed on an 'AS IS' basis, WITHOUT WARRANTY OF ANY KIND, EITHER
:cmt *    EXPRESS OR IMPLIED, AND SYBASE AND ALL CONTRIBUTORS HEREBY DISCLAIM
:cmt *    ALL SUCH WARRANTIES, INCLUDING WITHOUT LIMITATION, ANY WARRANTIES OF
:cmt *    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, QUIET ENJOYMENT OR
:cmt *    NON-INFRINGEMENT. Please see the License for the specific language
:cmt *    governing rights and limitations under the License.
:cmt *
:cmt *  ========================================================================
:cmt *
:cmt * Description:  FORTRAN compile and link utility diagnostic messages.
:cmt *
:cmt *     UTF-8 encoding, ¥
:cmt *
:cmt *****************************************************************************
:cmt

:cmt    Word usage:
:cmt
:cmt    'parameter' is used for macro parms
:cmt    'argument' is used for function arguments

:cmt    GML Macros used (see MSGMACS.GML):

:cmt        :ansi <n>   warning if extensions enabled, error otherwise
:cmt        :ansierr    ignored if extensions enabled, error otherwise
:cmt        :ansiwarn <n>   ignored if extensions enabled, warn otherwise
:cmt        :warning <n>    message is a warning with a specific level
:cmt        :info       informational message
:cmt
:cmt        :msgsym <sym>    internal symbolic name for message
:cmt        :msgtxt <text>   text for message
:cmt        :msgattr <value> attribute value for message
:cmt
:cmt        :msglvl     start of header title for a group of messages
:cmt        :emsglvl    end of header title for a group of messages
:cmt        :errbad     start of an example that generates an error msg
:cmt        :eerrbad    end of an example that generates an error msg
:cmt        :errgood    start of an example that compiles clean
:cmt        :eerrgood   end of an example that compiles clean
:cmt
:cmt        .kw     highlight a keyword
:cmt        .id     highlight an identifier
:cmt        .ev     highlight an environment variable
:cmt        .us     italicize a phrase
:cmt        .np     start a new paragraph

:cmt    The following substitutions are made:
:cmt        &incvarup   environment variable for include path
:cmt        &wclname    Compile and Link utility name

:cmt    Note for translators:

:cmt    Japanese error messages are supported via the :MSGJTXT tag.
:cmt    If there is no :MSGJTXT. for a particular :MSGSYM. then the
:cmt    message will come out in English.  Translation may proceed
:cmt    by translating error messages that do not have the :MSGJTXT.
:cmt    tag present and adding the :MSGJTXT. tag with the Japanese
:cmt    text after the :MSGTXT. tag.  If the :MSGJTXT. has no text
:cmt    then the error message must also be translated.  This has
:cmt    been found to be easier when searching for messages that
:cmt    still need to be translated.

:cmt -------------------------------------------------------------------
:MSGGRP. CL
:MSGGRPTXT. Additional Messages
:cmt -------------------------------------------------------------------
:MSGSYM. CL_OUT_OF_MEMORY
:MSGTXT. Out of memory
:MSGJTXT. ﾒﾓﾘ不足
:MSGSYM. CL_ERROR_OPENING_TMP_FILE
:MSGTXT. Error: Unable to open temporary file
:MSGJTXT. ｴﾗｰ  : ﾃﾝﾎﾟﾗﾘﾌｧｲﾙがｵｰﾌﾟﾝできない
:MSGSYM. CL_UNABLE_TO_OPEN
:MSGTXT. Unable to open '%s'
:MSGJTXT. '%s'がｵｰﾌﾟﾝできない
:MSGSYM. CL_UNABLE_TO_INVOKE_COMPILER
:MSGTXT. Error: Unable to invoke compiler
:MSGJTXT. ｴﾗｰ  : ｺﾝﾊﾟｲﾗｰが起動できない
:MSGSYM. CL_BAD_COMPILE
:MSGTXT. Error: Compiler returned a bad status compiling '%s'
:MSGJTXT. ｴﾗｰ  : '%s'ｺﾝﾊﾟｲﾙ中にｺﾝﾊﾟｲﾗが不正なｽﾃｰﾀｽを返した
:MSGSYM. CL_UNABLE_TO_INVOKE_LINKER
:MSGTXT. Error: Unable to invoke linker
:MSGJTXT. ｴﾗｰ  : ﾘﾝｶｰが起動できない
:MSGSYM. CL_BAD_LINK
:MSGTXT. Error: Linker returned a bad status
:MSGJTXT. ｴﾗｰ  : ﾘﾝｶｰが不正なｽﾃｰﾀｽを返した
:MSGSYM. CL_UNABLE_TO_FIND
:MSGTXT. Error: Unable to find '%s'
:MSGJTXT. ｴﾗｰ  : '%s'が見つからない
:MSGSYM. CL_UNABLE_TO_INVOKE_CVPACK
:MSGTXT. Error: Unable to invoke CVPACK
:MSGJTXT. ｴﾗｰ: CVPACKを起動できません
:MSGSYM. CL_BAD_CVPACK
:MSGTXT. Error: CVPACK returned a bad status
:MSGJTXT. Error: CVPACKが不正なｽﾃｰﾀｽを返しました
:eMSGGRP. CL
