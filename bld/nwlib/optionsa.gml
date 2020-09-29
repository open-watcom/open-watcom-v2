:cmt *****************************************************************************
:cmt *
:cmt *                            Open Watcom Project
:cmt *
:cmt * Copyright (c) 2002-2020 The Open Watcom Contributors. All Rights Reserved.
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
:cmt * Description:  librarian ar command line options.
:cmt *
:cmt *     UTF-8 encoding, ¥
:cmt *
:cmt *****************************************************************************
:cmt

:cmt
:cmt GML Macros used:
:cmt
:cmt    :chain. <char> <usage>                  options that start with <char>
:cmt                                            can be chained together i.e.,
:cmt                                            -oa -ox -ot => -oaxt
:cmt    :option. <option> <synonym> ...         define an option
:cmt    :target. <arch1> <arch2> ...            valid for these architectures
:cmt    :ntarget. <arch1> <arch2> ...           not valid for these architectures
:cmt    :immediate. <fn>                        <fn> is called when option parsed
:cmt    :code. <source-code>                    <source-code> is executed when option parsed
:cmt    :enumerate. <field> [<value>]           option is one value in <name> enumeration
:cmt    :number. [<fn>] [<default>]             =<n> allowed; call <fn> to check
:cmt    :id. [<fn>]                             =<id> req'd; call <fn> to check
:cmt    :char.[<fn>]                            =<char> req'd; call <fn> to check
:cmt    :file.                                  =<file> req'd
:cmt    :path.                                  =<path> req'd
:cmt    :special. <fn> [<arg_usage_text>]       call <fn> to parse option
:cmt    :optional.                              value is optional
:cmt    :internal.                              option is undocumented
:cmt    :prefix.                                prefix of a :special. option
:cmt    :usagegrp. <option> <usage text>        group of options that start with <option>
:cmt                                            they are chained together in usage text only
:cmt    :usage. <text>                          English usage text
:cmt    :jusage. <text>                         Japanese usage text
:cmt    :title.                                 English title usage text
:cmt    :jtitle.                                Japanese title usage text
:cmt    :page.                                  text for paging usage message
:cmt    :nochain.                               option isn't chained with other options
:cmt    :timestamp.                             kludge to record "when" an option
:cmt                                            is set so that dependencies
:cmt                                            between options can be simulated
:cmt
:cmt Global macros
:cmt
:cmt    :noequal.                               args can't have option '='
:cmt    :argequal. <char>                       args use <char> instead of '='
:cmt

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx, ppc, linux, sparc

:cmt    Translations are required for the :jtitle. and :jusage. tags
:cmt    if there is no text associated with the tag.

:title.  Usage: %s <command> [<modifiers>] <archive> [modules]
:jtitle. 使用法： %s <command> [<modifiers>] <archive> [modules]
:titleu.  Usage: %C <command> [<modifiers>] <archive> [modules]
:jtitleu. 使用法： %C <command> [<modifiers>] <archive> [modules]
:target. any

:title.
:jtitle.
:target. any

:title.  Modifiers:
:jtitle. 修飾子：
:target. any

:title.  . -c     create archive
:jtitle. . -c     アーカイブを作成
:target. any

:title.  . -u     update (only with -r command)
:jtitle. . -u     更新（-rコマンドのみ）
:target. any

:title.  . -v     verbose output
:jtitle. . -v     詳細な出力
:target. any

:title.
:jtitle.
:target. any

:title.  Commands:
:jtitle. コマンド：
:target. any

:option. d
:target. any
:usage.  .     delete modules from archive
:jusage. .     アーカイブからモジュールを削除する

:option. r
:target. any
:usage.  .     insert or replace modules
:jusage. .     モジュールの挿入または交換

:option. t
:target. any
:usage.  .     display modules or content
:jusage. .     モジュールまたはコンテンツを表示する

:option. x
:target. any
:usage.  .     extract modules
:jusage. .     モジュールを抽出する
