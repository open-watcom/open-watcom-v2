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
:cmt * Description:  disassembler wdis command line options.
:cmt *
:cmt *    Source file uses UTF-8 character encoding
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
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx, ppc, linux, sparc, haiku

:cmt    Translations are required for the :jtitle. and :jusage. tags
:cmt    if there is no text associated with the tag.

:page. (Press return to continue)
:jusage. (Press return to continue)

:title. Usage:  wdis [options] obj_file [options]
:jtitle. 使用方法:  wdis [options] obj_file [options]
:target. any

:title. Options:
:jtitle. オプション:
:target. any
:title.  .         ( /option is also accepted )
:jtitle. .         ( /optionも使用できます )
:target. any
:ntarget. bsd linux osx qnx haiku

:option. ? h
:target. any
:usage. print this message
:jusage. このメッセージを表示します

:chain. f Instruction format
:jusage.

:option. a
:target. any
:usage. generate assembleable output
:jusage. ャAセンブルできる出力を生成します

:option. e
:target. any
:usage. generate lists of externs
:jusage. テxternのリストを生成します

:option. ff
:target. any
:usage. FPU emulator fixups as comment [80(x)86 only]
:jusage. FPU emulator fixups as comment [80(x)86 only]

:option. fi
:target. any
:usage. use alternate indexing format [80(x)86 only]
:jusage. 別のインデクシング形式を使用する[80(x)86のみ]

:option. fp
:target. any
:usage. do not use instruction name pseudonyms
:jusage. 命令名として擬似ニーモニックを使用しない

:option. fr
:target. any
:usage. do not use register name pseudonyms [RISC only]
:jusage. レジスタ名として擬似ニーモニックを使用しない[RISCのみ]

:option. fu
:target. any
:usage. instructions/registers in upper case
:jusage. 命令/レジスタを大文字にする

:option. i
:target. any
:char.
:usage. initial character of internal labels
:jusage. 内部ラベルの先頭文字

:option. l
:target. any
:file.
:optional.
:usage. generate listing file
:jusage. リスト･ファイルを生成します

:option. m
:target. any
:usage. leave C++ names mangled
:jusage. C++のマングルド名を残す

:option. p
:target. any
:usage. generate list of publics
:jusage. パブリックのリストを生成する

:option. s
:target. any
:file.
:optional.
:usage. include source lines
:jusage. ソース行を含めて表示する

