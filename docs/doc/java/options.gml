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
:cmt    :dir.                                   =<dir> req'd
:cmt    :path.                                  =<path> req'd
:cmt    :special. <fn> [<arg_usage_text>]       call <fn> to parse option
:cmt    :optional.                              value is optional
:cmt    :negate.                                for a simple switch turns off option
:cmt    :noequal.                               args can't have option '='
:cmt    :argequal. <char>                       args use <char> instead of '='
:cmt    :internal.                              option is undocumented
:cmt    :prefix.                                prefix of a :special. option
:cmt    :usage. <text>                          English usage text
:cmt    :jusage. <text>                         Japanese usage text
:cmt    :title.                                 English usage text
:cmt    :jtitle.                                Japanese usage text
:cmt    :timestamp.                             kludge to record "when" an option
:cmt                                            is set so that dependencies
:cmt                                            between options can be simulated

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx

:title. Usage: wjava [options] file [options]
:jtitle. 使用法: wjava [ｵﾌﾟｼｮﾝ] ﾌｧｲﾙ [ｵﾌﾟｼｮﾝ]
:target. any
:title. Options:
:jtitle. オプション:
:target. any
:title. \t    ( /option is also accepted )
:jtitle. \t    ( /optionも使用できます )
:target. any
:ntarget. qnx
:title. \t    ( '=' is always optional, i.e., -w4 )
:jtitle. \t    ( '='は常に省略可能です。つまり -w4 )
:target. any
:title. \t    ( a trailing '[-]' disables option where accepted )
:jtitle. \t    ( 終端の '[-]' はﾃﾞﾌｫﾙﾄｵﾌﾟｼｮﾝを使用します )
:target. any

:chain. m display message output
:jusage. m ﾒｯｾｰｼﾞ出力を表示する

:option. br
:target. any
:usage. generate browser information in classfile
:jusage.

:option. cp classpath
:target. any
:path.
:usage. set class path for compilation
:jusage. ｺﾝﾊﾟｲﾙのためにｸﾗｽ ﾊﾟｽを設定してください

:option. cp:i
:target. any
:usage. ignore CLASSPATH environment variable
:jusage. CLASSPATH環境変数を無視する

:option. cp:p
:target. any
:path.
:usage. prepend path to class path
:jusage. ｸﾗｽﾊﾟｽの前に指定されたﾊﾟｽを使用する

:option. cp:o
:target. any
:negate.
:usage. print classpath
:jusage. ｸﾗｽﾊﾟｽを表示する

:option. d
:target. any
:dir.
:usage. set root directory for class file output
:jusage. ｸﾗｽ ﾌｧｲﾙ出力のためのﾙｰﾄ ﾃﾞｨﾚｸﾄﾘを設定

:option. deprecation
:target. any
:usage. warn about use of a deprecated API
:jusage.  deprecated API使用時に警告を表示する

:option. ef
:target. any
:file.
:optional.
:usage. set error file name
:jusage. ｴﾗｰ ﾌｧｲﾙ名を設定する

:option. eq
:target. any
:negate.
:immediate. HandleOptionEQ
:usage. do not display error messages (but still write to .err file)
:jusage. ｴﾗｰ ﾒｯｾｰｼﾞを表示しない(.err ﾌｧｲﾙには出力)

:option. ew
:target. any
:negate.
:immediate. HandleOptionEW
:usage. alternate error message formatting
:jusage. 相互ｴﾗｰ ﾒｯｾｰｼﾞ ﾌｫｰﾏｯﾄ

:option. e
:target. any
:number. CheckErrorLimit
:usage. set limit on number of error messages
:jusage. ｴﾗｰ ﾒｯｾｰｼﾞの数に制限を付ける

:option. g
:target. any
:negate.
:usage. generate full debugging info (lines + variables)
:jusage. すべてのﾃﾞﾊﾞｯｸﾞ情報を生成する(行番号+変数)

:option. g:l
:target. any
:negate.
:usage. generate line number debugging info
:jusage. 行番号ﾃﾞﾊﾞｯｸﾞ情報を生成する

:option. g:t
:target. any
:negate.
:usage. generate debug tables i.e. local variables
:jusage. ﾃﾞﾊﾞｯｸﾞ表の生成をする 例 ﾛｰｶﾙ変数

:option. ic
:target. any
:negate.
:usage. ignore case differences in the file system
:jusage.  ﾌｧｲﾙ名の大文字、小文字を無視する

:option. javadoc
:target. any
:usage. temporary -- enable JAVADOC PROCESSING
:jusage.
:internal.

:option. jck
:target. any
:usage. compiling JCK source code
:jusage. JCK ｿｰｽ ｺｰﾄﾞをｺﾝﾊﾟｲﾙ中
:internal.

:option. k
:target. any
:negate.
:usage. continue processing files (ignore errors)
:jusage. ﾌｧｲﾙの処理を続行(ｴﾗｰを無視)

:option. kw
:target. any
:negate.
:usage. write out classfiles for correct source files
:jusage.

:option. m
:target. any
:usage. default = -mpst
:jusage. ﾃﾞﾌｫﾙﾄ = -mpst

:option. mi
:target. any
:usage. internal statistics
:jusage. 内部解析結果
:internal.

:option. mp
:target. any
:usage. progress messages
:jusage. ﾌﾟﾛｸﾞﾚｽ ﾒｯｾｰｼﾞ

:option. ms
:target. any
:usage. sizes
:jusage. ｻｲｽﾞ

:option. mt
:target. any
:usage. timings
:jusage. ﾀｲﾐﾝｸﾞ

:option. nowarn
:target. any
:usage. turn off warnings
:jusage. 警告を消す
:enumerate. warn_level

:option. nowrite
:target. any
:usage. compile only - do not generate class files
:jusage. ｺﾝﾊﾟｲﾙのみ - ｸﾗｽ ﾌｧｲﾙを生成しない

:option. o
:target. any
:negate.
:usage.  full optimization (except for -o:rp)
:jusage. すべての最適化(-o:rpを除く)

:option. o:i
:target. any
:negate.
:usage.  optimize by inlining static, private, and final methods
:jusage. ｲﾝﾗｲﾝ static、private、と final ﾒｿｯﾄﾞでの最適化

:option. o:ib
:target. any
:number. CheckInlineLimit 32
:usage. restrict inlining to maximum <num> bytes of instructions
:jusage. ｲﾝｽﾄﾗｸｼｮﾝの最大ﾊﾞｲﾄ数を制限する

:option. o:o
:target. any
:negate.
:usage.  optimize bytecode jumps
:jusage. ﾊﾞｲﾄ ｺｰﾄﾞ ｼﾞｬﾝﾌﾟの最適化

:option. o:rp
:target. any
:negate.
:usage.  optimize classfile by removing unreferenced private members
:jusage. 参照されていないﾌﾟﾗｲﾍﾞｰﾄ ﾅﾝﾊﾞｰを除去してのｸﾗｽﾌｧｲﾙの最適化

:option. parse
:target. any
:file.
:usage. generate parser info for source files into <file>
:jusage.
:internal.

:option. pj
:target. any
:usage. enable preprocessed functionality
:jusage.
:internal.

:option. prof
:target. any
:usage. instrument methods with profiling code
:jusage. ﾌﾟﾛﾌｧｲﾙ ｺｰﾄﾞを生成
:internal.

:option. stream
:target. any
:usage. use stream i/o instead of file i/o
:jusage. ﾌｧｲﾙ入出力の代わりにstream入出力を使用する
:internal.

:option. tp
:target. dbg
:id.
:usage. set #pragma on( <id> )
:jusage. #pragma on( <id> ) を設定
:internal.

:option. tw
:target. any
:number.
:usage. set tab width to <num>
:jusage. <num>にﾀﾌﾞ幅を設定

:option. verbose
:target. any
:usage. same as -mpst
:jusage. -mpstと同様

:option. w
:target. any
:enumerate. warn_level
:number. CheckWarnLevel
:usage. set warning level number
:jusage. 警告ﾚﾍﾞﾙ番号を設定

:option. wcd
:target. any
:special. handleWCD =<group>-<num>
:usage. warning control: disable warning message <group>-<num>
:jusage. 警告ｺﾝﾄﾛｰﾙ: 警告ﾒｯｾｰｼﾞ<group>-<num>を使えなくする

:option. wce
:target. any
:special. handleWCE =<group>-<num>
:usage. warning control: enable warning message <group>-<num>
:jusage. 警告ｺﾝﾄﾛｰﾙ: 警告ﾒｯｾｰｼﾞ<group>-<num>を有効にする

:option. we
:target. any
:negate.
:usage. treat all warnings as errors
:jusage. 警告をすべてｴﾗｰとする

:option. ws
:target. any
:negate.
:usage. warn about Java style violations
:jusage. Java style violationsを警告する

:option. wx
:target. any
:enumerate. warn_level
:usage. set warning level to maximum setting
:jusage. 警告ﾚﾍﾞﾙを最大設定にする

:option. x
:target. any
:negate.
:usage. disable extensions - pedantic checking
:jusage.  拡張しない - ﾁｪｯｸおこなう

:option. xref
:target. any
:usage. return list of all class files referenced
:jusage.  すべてのｸﾗｽ ﾌｧｲﾙの参照ﾘｽﾄを作成する
:internal.

:option. xrefx
:target. any
:path.
:usage. during xref, exclude classes starting with given string
:jusage. xref, excludeｸﾗｽの間は与えられた文字列で始める
:internal.

:option. xrefxp
:target. any
:path.
:usage. during xref, exclude classes found in given path
:jusage.  xrefと同じ、ただし与えられたﾊﾟｽにあるｸﾗｽは除外する
:internal.

:option. xreffind
:target. any
:path.
:usage. regardless of xrefx, find classes starting with given string
:jusage.
:internal.

:option. zq
:target. any
:negate.
:usage. operate quietly (display only error messages)
:jusage. 冗長なﾒｯｾｰｼﾞを表示しない(ｴﾗｰ ﾒｯｾｰｼﾞのみ表示)

:option. zcm
:target. any
:negate.
:usage. create object for classes compatible with Microsoft VM
:jusage.  Microsoft VM互換のｸﾗｽ ｵﾌﾞｼﾞｪｸﾄを作成する
:internal.

:option. zcs
:target. any
:negate.
:usage. create object for classes compatible with Sparc VM
:jusage.  Sparc VM互換のｸﾗｽ ｵﾌﾞｼﾞｪｸﾄを作成する
:internal.

:option. zjm
:target. any
:negate.
:usage. create pre-compiled object compatible with Microsoft JIT
:jusage. Microsoft JIT互換のﾌﾟﾘ ｺﾝﾊﾟｲﾙｵﾌﾞｼﾞｪｸﾄを作成する
:internal.

:option. zjs
:target. any
:negate.
:usage. create pre-compiled object compatible with Sparc JIT
:jusage. Sparc JIT互換のﾌﾟﾘ ｺﾝﾊﾟｲﾙｵﾌﾞｼﾞｪｸﾄを作成する
:internal.

:option. zsd
:target. any
:negate.
:usage. feedback source dependencies
:jusage.
:internal.
