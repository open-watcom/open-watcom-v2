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

:page.   (Press return to continue)
:jusage. (Press return to continue)

:title.  Usage: %s [options] input_file [output_file] [info_file]
:jtitle. 使用方法: %s [options] input_file [output_file] [info_file]
:target. any

:title.
:jtitle.
:target. any

:title.  .       input_file:  executable file
:jtitle. .       input_file:  実行可能ファイル
:target. any

:title.  .       output_file: optional output executable or '.'
:jtitle. .       output_file: オプションで出力実行可能ファイルまたは '.'を指定可能
:target. any

:title.  .       info_file:   optional output debugging or resource information file
:jtitle. .       info_file:   オプションでデバッグまたはリソース情報ファイルの出力又は
:target. any

:title.  .                    or input debugging or resource information file
:jtitle. .                    入力を指定可能
:target. any

:title.
:jtitle.
:target. any

:title.  Options:
:jtitle. オプション:
:target. any
:title.
:jtitle.
:target. bsd linux osx qnx haiku
:title.  .         ( /option is also accepted )
:jtitle. .         ( /optionも使用できます )
:target. any
:ntarget. bsd linux osx qnx haiku

:option. ? h
:target. any
:usage.  display this screen
:jusage. この画面を表示します

:option. a
:target. any
:usage.  add information rather than delete information
:jusage. 情報削除ではなく、情報付加をします

:option. n
:target. any
:usage.  don't print warning messages
:jusage. 警告メッセージを表示しません

:option. q
:target. any
:usage.  don't print informational messages
:jusage. メッセージを表示しません

:option. r
:target. any
:usage.  process resource information rather than debugging information
:jusage. デバッグ情報ではなく、リソース情報を処理します
