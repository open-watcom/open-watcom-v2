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
