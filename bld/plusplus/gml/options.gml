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
:cmt    :noequal.                               args can't have option '='
:cmt    :argequal. <char>                       args use <char> instead of '='
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

:cmt    where:
:cmt        <arch>:     i86, 386, axp, any, dbg, qnx, ppc, linux, sparc

:cmt    Translations are required for the :jtitle. and :jusage. tags
:cmt    if there is no text associated with the tag.

:title. Usage: wpp [options] file [options]
:jtitle. 使用方法: wpp [options] file [options]
:target. i86
:title. Usage: wpp386 [options] file [options]
:jtitle. 使用方法: wpp386 [options] file [options]
:target. 386
:title. Usage: wppaxp [options] file [options]
:jtitle. 使用方法: wppaxp [options] file [options]
:target. axp
:title. Usage: wppppc [options] file [options]
:jtitle. 使用方法: wppppc [options] file [options]
:target. ppc
:title. Options:
:jtitle. オプション:
:target. any
:title. \t    ( /option is also accepted )
:jtitle. \t    ( /optionも使用できます )
:target. i86 386 axp ppc
:ntarget. qnx linux
:title. \t    ( '=' is always optional, i.e., -w4 -zp4 )
:jtitle. \t    ( '='は常に省略可能です。つまり -w4 -zp4 )
:target. any

:page. (Press return to continue)
:jusage. (続行するために何かキーを押して下さい)

:chain. p preprocess source file
:jusage. p ソースファイルを前処理します
:chain. o optimization
:jusage. o 最適化

:option. 0
:target. i86
:enumerate. arch_i86
:usage. 8086 instructions
:jusage. 8086 命令

:option. 1
:target. i86
:enumerate. arch_i86
:usage. 186 instructions
:jusage. 186 命令

:option. 2
:target. i86
:enumerate. arch_i86
:usage. 286 instructions
:jusage. 286 命令

:option. 3
:target. i86
:enumerate. arch_i86
:usage. 386 instructions
:jusage. 386 命令

:option. 4
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for 486
:jusage. 386 命令, 486用最適化

:option. 5
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium
:jusage. 386 命令, Pentium用最適化

:option. 6
:target. i86
:enumerate. arch_i86
:usage. 386 instructions, optimize for Pentium Pro
:jusage. 386 命令, Pentium Pro用最適化

:option. 3r 3
:target. 386
:enumerate. arch_386
:usage. 386 register calling conventions
:jusage. 386 レジスタ呼び出し規約

:option. 3s
:target. 386
:enumerate. arch_386
:usage. 386 stack calling conventions
:jusage. 386 スタック呼び出し規約

:option. 4r 4
:target. 386
:enumerate. arch_386
:usage. 486 register calling conventions
:jusage. 486 レジスタ呼び出し規約

:option. 4s
:target. 386
:enumerate. arch_386
:usage. 486 stack calling conventions
:jusage. 486 スタック呼び出し規約

:option. 5r 5
:target. 386
:enumerate. arch_386
:usage. Pentium register calling conventions
:jusage. Pentium レジスタ呼び出し規約

:option. 5s
:target. 386
:enumerate. arch_386
:usage. Pentium stack calling conventions
:jusage. Pentium スタック呼び出し規約

:option. 6r 6
:target. 386
:enumerate. arch_386
:usage. Pentium Pro register calling conventions
:jusage. Pentium Pro レジスタ呼び出し規約

:option. 6s
:target. 386
:enumerate. arch_386
:usage. Pentium Pro stack calling conventions
:jusage. Pentium Pro スタック呼び出し規約

:option. as
:target. axp
:usage. assume short integers are aligned
:jusage. short 整数が整列していると仮定します

:option. bc
:target. any
:usage. build target is a console application
:jusage. 構築ターゲットはコンソール･アプリケーションです

:option. bd
:target. any
:usage. build target is a dynamic link library (DLL)
:jusage. 構築ターゲットはダイナミック･リンク･ライブラリです(DLL)

:option. bg
:target. any
:usage. build target is a GUI application
:jusage. 構築ターゲットはGUIアプリケーションです

:option. bm
:target. any
:usage. build target is a multi-thread environment
:jusage. 構築ターゲットはマルチスレッド環境です

:option. br
:target. 386 axp ppc
:usage. build target uses DLL version of C/C++ run-time library
:jusage. 構築ターゲットはDLL版のC/C++実行時ライブラリを使用します

:option. bw
:target. any
:usage. build target is a default windowing application
:jusage. 構築ターゲットはデフォルト･ウィンドウ･アプリケーションです

:option. bt
:target. any
:id.
:optional.
:usage. build target is operating system <id>
:jusage. 構築ターゲットはオペレーティング･システム <id>

:option. d0
:target. any
:enumerate. debug_info
:timestamp.
:usage. no debugging information
:jusage. デバッグ情報はありません

:option. d1
:target. any
:enumerate. debug_info
:timestamp.
:usage. line number debugging information
:jusage. 行番号デバッグ情報

:option. d2
:target. any
:enumerate. debug_info
:timestamp.
:usage. symbolic debugging information
:jusage. 完全シンボルデバッグ情報

:option. d2i
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 and debug inlines; emit inlines as COMDATs
:jusage. インライン関数の展開なしの-d2;インライン関数はCOMDATとして出力

:option. d2s
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 and debug inlines; emit inlines as statics
:jusage. インライン関数の展開なしの-d2;インライン関数はstaticとして出力

:option. d2t d2~
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d2 but without type names
:jusage. 型名なしの完全シンボルデバッグ情報

:option. d3
:target. any
:enumerate. debug_info
:timestamp.
:usage. symbolic debugging information with unreferenced type names
:jusage. 参照されていない型名を含む完全シンボルデバッグ情報

:option. d3i
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d3 and debug inlines; emit inlines as COMDATs
:jusage. インライン関数の展開なしの-d3;インライン関数はCOMDATとして出力

:option. d3s
:target. any
:enumerate. debug_info
:timestamp.
:usage. -d3 and debug inlines; emit inlines as statics
:jusage. インライン関数の展開なしの-d3;インライン関数はstaticとして出力

:option. d+
:target. any
:special. scanDefinePlus
:usage. allow extended -d macro definitions
:jusage. 拡張された -d マクロ定義を許可します

:option. db
:target. any
:prefix.
:usage. generate browsing information
:jusage. ブラウズ情報を生成します

:option. d
:target. any
:special. scanDefine <name>[=text]
:usage. same as #define name [text] before compilation
:jusage. コンパイル前の #define name [text] と同じ

:option. ecc
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __cdecl
:jusage.

:option. ecd
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __stdcall
:jusage.

:option. ecf
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __fastcall
:jusage.

:option. eco
:target. i86 386
:enumerate. intel_call_conv
:internal.
:usage. set default calling convention to _Optlink
:jusage.

:option. ecp
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __pascal
:jusage.

:option. ecr
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __fortran
:jusage.

:option. ecs
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __syscall
:jusage.

:option. ecw
:target. i86 386
:enumerate. intel_call_conv
:usage. set default calling convention to __watcall (default)
:jusage.

:option. ee
:target. any
:usage. call epilogue hook routine
:jusage. エピローグ･フックルーチンを呼び出します

:option. ef
:target. any
:usage. use full path names in error messages
:jusage. エラーメッセージに完全パス名を使用します

:option. ei
:target. any
:enumerate. enum_size
:usage. force enum base type to use at least an int
:jusage. enum型のベース型としてint型以上の大きさを使用します

:option. em
:target. any
:enumerate. enum_size
:usage. force enum base type to use minimum integral type
:jusage. enum型のベース型として最小の整数型を使用します

:option. en
:target. any
:usage. emit routine names in the code segment
:jusage. ルーチン名をコードセグメントに出力します

:option. ep
:target. any
:number. checkPrologSize 0
:usage. call prologue hook routine with <num> stack bytes available
:jusage. <num>バイトのスタックを使用するプロローグ･フック･ルーチンを呼び出します

:option. eq
:target. any
:immediate. handleOptionEQ
:usage. do not display error messages (but still write to .err file)
:jusage. エラーメッセージを表示しません(しかし.errファイルには書き込みます)

:option. er
:target. any
:usage. do not recover from undefined symbol errors
:jusage. 未定義シンボルエラーから回復しません

:option. et
:target. 386
:usage. emit Pentium profiling code
:jusage. Pentiumプロファイリング･コードを生成します

:option. et0
:target. 386
:usage. emit Pentium-CTR0 profiling code
:jusage. Pentium-CTR0プロファイリング･コードを生成します

:option. etp
:target. 386
:internal.
:usage. emit Timing for Profiler
:jusage. プロファイラのタイミングを出力します

:option. esp
:target. 386
:internal.
:usage. emit Statement counting for Profiler
:jusage. プロファイラ用ステートメント･カウンティングを出力します

:option. ew
:target. any
:immediate. handleOptionEW
:usage. alternate error message formatting
:jusage. 別のエラーメッセージ形式を使用します

:option. ez
:target. 386
:usage. generate PharLap EZ-OMF object files
:jusage. PharLap EZ-OMFオブジェクト･ファイルを生成します

:option. e
:target. any
:number. checkErrorLimit
:usage. set limit on number of error messages
:jusage. エラーメッセージ数の制限を設定します

:option. fbi
:target. any
:special. scanFBI
:internal.
:usage. generate browsing information
:jusage. ブラウズ情報を生成します

:option. fbx
:target. any
:special. scanFBX
:internal.
:usage. do not generate browsing information
:jusage. ブラウズ情報を生成しません

:option. fc
:target. any
:file.
:immediate. handleOptionFC
:usage. specify file of command lines to be batch processed
:jusage. バッチ処理するコマンドラインのファイルを指定します

:option. fh
:target. any
:file.
:optional.
:timestamp.
:usage. use pre-compiled header (PCH) file
:jusage. プリコンパイル･ヘッダー(PCH)を使用します

:option. fhd
:target. any
:usage. store debug info for PCH once (DWARF only)
:jusage. PCH用デバッグ情報を1度格納します(DWARFのみ)

:option. fhq
:target. any
:file.
:optional.
:timestamp.
:usage. do not display PCH activity warnings
:jusage. PCH使用メッセージを表示しません

:option. fhw
:target. any
:usage. force compiler to write PCH (will never read)
:jusage. PCHを書き込みに強制します (読みません)

:option. fhwe
:target. any
:usage. don't count PCH activity warnings (see -we option)
:jusage. PCH使用メッセージをエラーとして扱いません(-weオプション参照)

:option. fhr
:target. any
:usage. force compiler to read PCH (will never write)
:jusage. PCHを読み込みに強制します (書きません)

:option. fhr!
:target. any
:usage. compiler will read PCH without checking include files
:jusage. コンパイラはインクルードファイルをチェックすることなく、PCH読み込みます
:internal.

:option. fi
:target. any
:file.
:usage. force <file> to be included
:jusage. 強制的に<file>をインクルードします

:option. fo
:target. any
:file.
:optional.
:usage. set object or preprocessor output file name
:jusage. オブジェクトまたはプリプロセッサの出力ファイル名を設定します

:option. fr
:target. any
:file.
:optional.
:usage. set error file name
:jusage. エラー･ファイル名を設定します

:option. ft
:target. any
:enumerate. file_83
:usage. check for truncated versions of file names
:jusage. 切り詰めたファイル名をチェックします

:option. fx
:target. any
:enumerate. file_83
:usage. do not check for truncated versions of file names
:jusage. 切り詰めたファイル名をチェックしません

:option. fzh
:target. any
:usage. do not automatically postfix include file names
:jusage. do not automatically postfix include file names

:option. fzs
:target. any
:usage. do not automatically postfix source file names
:jusage. do not automatically postfix source file names

:option. 87d
:target. i86 386
:number. CmdX86CheckStack87 0
:enumerate. intel_fpu_model
:internal.
:usage. inline 80x87 instructions with specified depth
:jusage. 指定した深さのインライン80x87命令

:option. fpc
:target. i86 386
:enumerate. intel_fpu_model
:usage. calls to floating-point library
:jusage. 浮動小数点ライブラリを呼び出します

:option. fpi
:target. i86 386
:enumerate. intel_fpu_model
:usage. inline 80x87 instructions with emulation
:jusage. エミュレーション付きインライン80x87命令

:option. fpi87
:target. i86 386
:enumerate. intel_fpu_model
:usage. inline 80x87 instructions
:jusage. インライン80x87命令

:option. fp2 fp287
:target. i86 386
:enumerate. intel_fpu_level
:usage. generate 287 floating-point code
:jusage. 287浮動小数点コードを生成します

:option. fp3 fp387
:target. i86 386
:enumerate. intel_fpu_level
:usage. generate 387 floating-point code
:jusage. 387浮動小数点コードを生成します

:option. fp5
:target. i86 386
:enumerate. intel_fpu_level
:usage. optimize floating-point for Pentium
:jusage. Pentium用浮動小数点最適化をします

:option. fp6
:target. i86 386
:enumerate. intel_fpu_level
:usage. optimize floating-point for Pentium Pro
:jusage. Pentium Pro用浮動小数点最適化をします

:option. fpr
:target. i86 386
:usage. generate backward compatible 80x87 code
:jusage. バージョン9.0以前と互換の80x87コードを生成します

:option. fpd
:target. i86 386
:usage. enable Pentium FDIV check
:jusage. Pentium FDIVチェックをします

:option. g
:target. i86 386
:id.
:usage. set code group name
:jusage. コード･グループ名を設定します

:option. hw
:target. i86 386 
:enumerate. dbg_output
:usage. generate Watcom debugging information
:jusage. Watcomデバッグ情報を生成します

:option. hd
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate DWARF debugging information
:jusage. DWARFデバッグ情報を生成します

:option. hda
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate DWARF debugging information
:jusage. DWARFデバッグ情報を生成します
:internal.

:option. hc
:target. i86 386 axp ppc
:enumerate. dbg_output
:usage. generate Codeview debugging information
:jusage. Codeviewデバッグ情報を生成します

:option. i
:target. any
:path.
:usage. add another include path
:jusage. インクルード･パスを追加します

:option. j
:target. any
:usage. change char default from unsigned to signed
:jusage. char型のデフォルトをunsignedからsignedに変更します

:option. jw
:target. any
:usage. warn about default char promotion to int
:jusage.
:internal.

:option. k
:target. any
:usage. continue processing files (ignore errors)
:jusage. ファイル処理を続行します(エラーを無視します)

:option. la
:target. axp
:usage. output Alpha assembly listing
:jusage. Alphaアセンブリ･リストを出力します
:internal.

:option. lo
:target. axp
:usage. output OWL listing
:jusage. OWLリストを出力します
:internal.

:option. mc
:target. i86 386
:enumerate. mem_model
:usage. compact memory model (small code/large data)
:jusage. コンパクト･メモリ･モデル(スモール･コード/ラージ･データ)

:option. mf
:target. 386
:enumerate. mem_model
:usage. flat memory model (small code/small data assuming CS=DS=SS=ES)
:jusage. フラット･メモリ･モデル(スモール･コード/CS=DS=SS=ESを仮定したスモール･データ)

:option. mfi
:target. 386
:enumerate. mem_model
:usage. flat memory model (interrupt functions will assume flat model)
:jusage. フラ;ト･メモリ･モ?ル(割り込み関数もフラ;トモ?ルであると仮定する)

:option. mh
:target. i86
:enumerate. mem_model
:usage. huge memory model (large code/huge data)
:jusage. ヒュージ･メモリ･モデル(ラージ･コード/ヒュージ･データ)

:option. ml
:target. i86 386
:enumerate. mem_model
:usage. large memory model (large code/large data)
:jusage. ラージ･メモリ･モデル(ラージ･コード/ラージ･データ)

:option. mm
:target. i86 386
:enumerate. mem_model
:usage. medium memory model (large code/small data)
:jusage. ミディアム･メモリ･モデル(ラージ･コード/スモール･データ)

:option. ms
:target. i86 386
:enumerate. mem_model
:usage. small memory model (small code/small data)
:jusage. スモール･メモリ･モデル(スモール･コード/スモール･データ)

:option. na
:target. any
:usage. disable automatic inclusion of _ialias.h
:jusage. スモール･メモリ･モデル(スモール･コード/スモール･データ)

:option. nc
:target. i86 386
:id.
:usage. set code class name
:jusage. コード･クラス名を設定します

:option. nd
:target. i86 386
:id.
:usage. set data segment name
:jusage. データ･セグメント名を設定します

:option. nm
:target. i86 386 axp ppc
:file.
:usage. set module name
:jusage. モジュール名を設定します

:option. nt
:target. i86 386
:id.
:usage. set name of text segment
:jusage. テキスト･セグメント名を設定します

:option. oa
:target. any
:usage. relax aliasing constraints
:jusage. エイリアスの制約を緩和します

:option. ob
:target. any
:usage. enable branch prediction
:jusage. 分岐予測にそったコードを生成します

:option. oc
:target. i86 386 axp ppc
:usage. disable <call followed by return> to <jump> optimization
:jusage. <call followed by return>から<jump>の最適化を無効にします

:option. od
:target. any
:enumerate. opt_level
:timestamp.
:usage. disable all optimizations
:jusage. すべての最適化を無効にします

:option. oe
:target. any
:number. checkOENumber 100
:usage. expand user functions inline (<num> controls max size)
:jusage. ユーザ関数をインライン展開します(<num>は最大ｻｲｽﾞを制御します)

:option. of
:target. i86 386
:usage. generate traceable stack frames as needed
:jusage. 必要に応じてトレース可能なスタック･フレームを生成します

:option. of+
:target. i86 386
:usage. always generate traceable stack frames
:jusage. 常にトレース可能なスタック･フレームを生成します

:option. oh
:target. any
:usage. enable expensive optimizations (longer compiles)
:jusage. 最適化を繰り返します(コンパイルが長くなります)

:option. oi
:target. any
:usage. expand intrinsic functions inline
:jusage. 組込み関数をインライン展開します

:option. oi+
:target. any
:usage. enable maximum inlining depth
:jusage. インライン展開の深さを最大にします

:option. ok
:target. any
:usage. include prologue/epilogue in flow graph
:jusage. プロローグとエピローグをフロー制御可能にします

:option. ol
:target. any
:usage. enable loop optimizations
:jusage. ループ最適化を可能にします

:option. ol+
:target. any
:usage. enable loop unrolling optimizations
:jusage. ループ・アンローリングでループ最適化を可能にします

:option. om
:target. i86 386 axp ppc
:usage. generate inline code for math functions
:jusage. 算術関数をインラインの80x87コードで展開して生成します

:option. on
:target. any
:usage. allow numerically unstable optimizations
:jusage. 数値的にやや不正確になるがより高速な最適化を可能にします

:option. oo
:target. any
:usage. continue compilation if low on memory
:jusage. メモリが足りなくなってもコンパイルを継続します

:option. op
:target. any
:usage. generate consistent floating-point results
:jusage. 一貫した浮動小数点計算の結果を生成します

:option. or
:target. any
:usage. reorder instructions for best pipeline usage
:jusage. 最適なパイプラインを使用するために命令を並べ替えます

:option. os
:target. any
:enumerate. opt_size_time
:timestamp.
:usage. favor code size over execution time in optimizations
:jusage. 実行時間よりコードサイズの最適化を優先します

:option. ot
:target. any
:enumerate. opt_size_time
:timestamp.
:usage. favor execution time over code size in optimizations
:jusage. コードサイズより実行時間の最適化を優先します

:option. ou
:target. any
:usage. all functions must have unique addresses
:jusage. すべての関数はそれぞれ固有のアドレスを必ず持ちます

:option. ox
:target. any
:enumerate. opt_level
:timestamp.
:usage. equivalent to -obmiler -s
:jusage. -obmiler -sと同等

:option. oz
:target. any
:usage. NULL points to valid memory in the target environment
:jusage. NULLは、ターゲット環境内の有効なメモリを指します

:option. ad
:target. any
:file.
:optional.
:usage. generate make style automatic dependency file
:jusage. generate make style automatic dependency file

:option. adbs
:target. any
:usage. force path separators to '\\' in auto-depend file
:jusage. force path separators to '\\' in auto-depend file

:option. adfs
:target. any
:usage. force path separators to '/' in auto-depend file
:jusage. force path separators to '/' in auto-depend file

:option. add
:target. any
:file.
:optional.
:usage. specify first dependency in make style auto-depend file
:jusage. specify first dependency in make style auto-depend file

:option. adhp
:target. any
:file.
:optional.
:usage. specify default path for headers without one
:jusage. specify default path for headers without one

:option. adt
:target. any
:file.
:optional.
:usage. specify target in make style auto-depend file
:jusage. specify target in make style auto-depend file

:option. pil
:target. any
:nochain.
:usage. preprocessor ignores #line directives
:jusage. preprocessor ignores #line directives

:option. p
:target. any
:usage.
:jusage.

:option. pe
:target. any
:usage. encrypt identifiers
:jusage. 識別子を暗号化します

:option. pl
:target. any
:usage. insert #line directives
:jusage. #line擬似命令を挿入します

:option. pj
:target. any
:internal.
:usage. insert // #line comments
:jusage. // #line擬似命令を挿入します

:option. pc
:target. any
:usage. preserve comments
:jusage. コメントを残します

:option. pw
:target. any
:number. checkPPWidth
:usage. wrap output lines at <num> columns. Zero means no wrap.
:jusage. 出力行を<num>桁で折り返します. 0は折り返しません.

:option. p#
:target. any
:char.
:internal.
:usage. set preprocessor delimiter to something other than '#'
:jusage. プリプロセッサの区切り記号を'#'以外の何かに設定します

:option. q
:target. any
:usage. operate quietly (display only error messages)
:jusage. 無メッセージモードで動作します(エラーメッセージのみ表示されます)

:option. r
:target. i86 386
:usage. save/restore segment registers across calls
:jusage. 関数呼び出しの前後でセグメントレジスタを退避/リストアします

:option. re
:target. 386
:internal.
:usage. special Neutrino R/W data access code generation
:jusage.

:option. ri
:target. i86 386
:usage. return chars and shorts as ints
:jusage. 全ての関数の引数と戻り値をint型に変換します

:option. rod
:target. any
:path.
:internal.
:usage. specified <path> contains read-only files
:jusage. 指定された<path>には読み込み専用ファイルが含まれています

:option. s
:target. any
:usage. remove stack overflow checks
:jusage. スタックオーバフロー・チェックを削除します

:option. sg
:target. i86 386
:usage. generate calls to grow the stack
:jusage. スタックを増加する呼び出しを生成します

:option. si
:target. axp
:usage. generate calls to initialize local storage
:jusage. ローカル･メモリを初期化する呼び出しを生成します

:option. st
:target. i86 386
:usage. touch stack through SS first
:jusage. まず最初にSSを通してスタック・タッチします

:option. t
:target. any
:number. checkTabWidth
:usage. set number of spaces in a tab stop
:jusage. タブ･ストップに対応する空白の数を設定します

:option. tp
:target. dbg
:id.
:usage. set #pragma on( <id> )
:jusage. #pragma on( <id> )を設定します

:option. u
:target. any
:special. scanUndefine [=<name>]
:usage. undefine macro name
:jusage. マクロ名を未定義にします

:option. v
:target. any
:usage. output function declarations to .def file
:jusage. .defファイルに関数宣言を出力します

:option. vcap
:target. 386 axp
:usage. VC++ compatibility: alloca allowed in argument lists
:jusage. VC++ 互換性: 引数リストの中でallocaを使用できます

:option. w
:target. any
:enumerate. warn_level
:number. checkWarnLevel
:usage. set warning level number
:jusage. 警告レベル番号を設定します

:option. wcd
:target. any
:number.
:multiple.
:usage. warning control: disable warning message <num>
:jusage. 警告制御: 警告メッセージ<num>を禁止します

:option. wce
:target. any
:number.
:multiple.
:usage. warning control: enable warning message <num>
:jusage. 警告制御: 警告メッセージ <num> の表示をします

:option. we
:target. any
:usage. treat all warnings as errors
:jusage. すべての警告をエラーとして扱います

:option. wx
:target. any
:enumerate. warn_level
:usage. set warning level to maximum setting
:jusage. 警告レベルを最大設定にします

:option. x
:target. any
:usage. ignore INCLUDE environment variable
:jusage. ignore INCLUDE environment variable

:option. xbnm
:target. any
:internal.
:usage. use latest (incompatible) name mangling algorithms
:jusage. 

:option. xbsa
:target. any
:internal.
:usage. do not align segments if at all possible
:jusage. 

:option. xbov1
:target. any
:internal.
:usage. WP 13.3.3.2 change
:jusage. 

:option. xcmb
:target. any
:internal.
:usage. bind modifiers during template instantiation
:jusage. テンプレートをインスタンス化する間に修飾子をバインドします

:option. xcpi
:target. any
:internal.
:usage. instantiate template functions if a prototype is visible
:jusage. プロトタイプがあれば、テンプレート関数をインスタンス化します

:option. xd
:target. any
:enumerate. exc_level
:usage. disable exception handling (default) 
:jusage. 例外処理を使用不可にします(デフォルト) 

:option. xds
:target. any
:enumerate. exc_level
:usage. disable exception handling (table-driven destructors)
:jusage. 例外処理を使用不可にします(テーブル･ドリブンのデストラクタ)

:option. xdt
:target. any
:enumerate. exc_level
:usage. disable exception handling (same as -xd)
:jusage. 例外処理を使用不可にします(-xdと同じ)

:option. xr
:target. any
:usage. enable RTTI
:jusage. RTTIを使用可能にします

:option. xs
:target. any
:enumerate. exc_level
:usage. enable exception handling 
:jusage. 例外処理を使用可能にします

:option. xss
:target. any
:enumerate. exc_level
:usage. enable exception handling (table-driven destructors)
:jusage. 例外処理を使用可能にします(テーブル･ドリブンのデストラクタ)

:option. xst
:target. any
:enumerate. exc_level
:usage. enable exception handling (direct calls for destruction)
:jusage. 例外処理を使用可能にします(デストラクタへの直接呼出し)

:option. xgls
:target. i86
:internal.
:usage. only check seg of i86 far pointers when checking NULL equality
:jusage.

:option. xgv
:target. 386
:internal.
:usage. indexed global variables
:jusage. インデックス付きグローバル変数

:option. xto
:target. any
:internal.
:usage. obfuscate type signature names
:jusage.

:option. xx
:target. any
:nochain.
:usage. ignore default directories for file search (.,../h,../c,...)
:jusage. ignore default directories for file search (.,../h,../c,...)

:option. za
:target. any
:enumerate. iso
:usage. disable extensions (i.e., accept only ISO/ANSI C++)
:jusage. 拡張機能を使用不可にします(つまり, ISO/ANSI C++のみ受け付けます)

:option. za0x
:target. any
:usage. enable some features of the upcoming ISO C++0x standard
:jusage.

:option. zat
:target. any
:usage. disable alternative tokens (e.g. and, or, not)
:jusage.

:option. zc
:target. i86 386
:usage. place const data into the code segment
:jusage. リテラル文字列をコードセグメントに入れます

:option. zdf
:target. i86 386
:enumerate. ds_peg
:usage. DS floats (i.e. not fixed to DGROUP)
:jusage. DSを浮動にします(つまりDGROUPに固定しません)

:option. zdp
:target. i86 386
:enumerate. ds_peg
:usage. DS is pegged to DGROUP
:jusage. DSをDGROUPに固定します

:option. zdl
:target. 386
:usage. load DS directly from DGROUP
:jusage. DGROUPからDSに直接ロードします

:option. ze
:target. any
:enumerate. iso
:usage. enable extensions (i.e., near, far, export, etc.)
:jusage. 拡張機能を使用可能にします(つまり, near, far, export, 等.)

:option. zf
:target. any
:usage. scope of for loop initializer extends beyond loop
:jusage. FIX ME

:option. zfw
:target. i86
:usage. generate FWAIT instructions on 386 and later
:jusage.

:option. zfw
:target. 386
:usage. generate FWAIT instructions
:jusage.

:option. zff
:target. i86 386
:enumerate. fs_peg
:usage. FS floats (i.e. not fixed to a segment)
:jusage. FSを浮動にします(つまり, 1つのセグメントに固定しません)

:option. zfp
:target. i86 386
:enumerate. fs_peg
:usage. FS is pegged to a segment
:jusage. FSを1つのセグメントに固定します

:option. zg
:target. any
:usage. generate function prototypes using base types
:jusage. 基本型を使用した関数プロトタイプを生成します

:option. zgf
:target. i86 386
:enumerate. gs_peg
:usage. GS floats (i.e. not fixed to a segment)
:jusage. GSを浮動にします(つまり, 1つのセグメントに固定しません)

:option. zgp
:target. i86 386
:enumerate. gs_peg
:usage. GS is pegged to a segment
:jusage. GSを1つのセグメントに固定します

:option. zi
:target. dbg
:usage. dump informational statistics to stdout
:jusage. 情報として統計値をstdoutに出力します

:option. zk0 zk
:target. any
:enumerate. char_set
:usage. double-byte character support: Kanji
:jusage. 2バイト文字サポート: 日本語

:option. zk1
:target. any
:enumerate. char_set
:usage. double-byte character support: Chinese/Taiwanese
:jusage. 2バイト文字サポート: 中国語/台湾語

:option. zk2
:target. any
:enumerate. char_set
:usage. double-byte character support: Korean
:jusage. 2バイト文字サポート: 韓国語

:option. zk0u
:target. any
:enumerate. char_set
:usage. translate double-byte Kanji to Unicode
:jusage. 2バイト漢字をUnicodeに変換します

:option. zkl
:target. any
:enumerate. char_set
:usage. double-byte character support: local installed language
:jusage. 2バイト文字サポート: ローカルにインストールされた言語

:option. zku
:target. any
:enumerate. char_set
:number.
:usage. load Unicode translate table for specified code page
:jusage. 指定したコードページのUnicode変換テーブルをロードします

:option. zl
:target. any
:usage. remove default library information
:jusage. デフォルト･ライブラリ情報を削除します

:option. zld
:target. any
:usage. remove file dependency information
:jusage. ファイル依存情報を削除します

:option. zlf
:target. any
:usage. always generate default library information
:jusage. デフォルト･ライブラリ情報を常に生成します

:option. zls
:target. any
:usage. remove automatically inserted symbols
:jusage. remove automatically inserted symbols

:option. zm
:target. i86 386
:usage. emit functions in separate segments
:jusage. 各関数を別のセグメントに入れます

:option. zm
:target. axp
:usage. emit functions in separate sections
:jusage. 各関数を別のセグメントに入れます

:option. zmf
:target. i86 386
:usage. emit functions in separate segments (near functions allowed)
:jusage. 各関数を別のセグメントに入れます(near関数が可能です)

:option. zo
:target. 386
:usage. use exception-handling for a specific operating system
:jusage. 指定されたオペレーティング･システム用の例外処理を使用します

:option. zp
:target. any
:number. checkPacking
:usage. pack structure members with alignment {1,2,4,8,16}
:jusage. 構造体メンバーを{1,2,4,8,16}に整列してパックします

:option. zpw
:target. any
:usage. output warning when padding is added in a class
:jusage. クラスにパディングが追加されたときに警告します

:option. zq
:target. any
:usage. operate quietly (display only error messages)
:jusage. 無メッセージモードで動作します(エラーメッセージのみ表示されます)

:option. zro
:target. any
:usage. omit floating point rounding calls (non ANSI)
:jusage. omit floating point rounding calls (non ANSI)

:option. zri
:target. 386
:usage. inline floating point rounding calls
:jusage. inline floating point rounding calls

:option. zs
:target. any
:usage. syntax check only
:jusage. 構文チェックのみを行います

:option. zt
:target. i86 386
:number. CmdX86CheckThreshold 256
:usage. far data threshold (i.e., larger objects go in far memory)
:jusage. farデータ敷居値(つまり, 敷居値より大きいオブジェクトをfarメモリに置きます)

:option. zu
:target. i86 386
:usage. SS != DGROUP (i.e., do not assume stack is in data segment)
:jusage. SS != DGROUP (つまり, スタックがデータセグメントにあると仮定しません)

:option. zv
:target. any
:usage. enable virtual function removal optimization
:jusage. 仮想関数を削除する最適化を行います

:option. zw
:target. i86 386
:enumerate. win
:usage. generate code for Microsoft Windows
:jusage. Microsoft Windows用のコードを生成します

:option. zws
:target. i86
:enumerate. win
:usage. generate code for Microsoft Windows with smart callbacks
:jusage. スマート･コールバックをするMicrosoft Windows用コードを生成します

:option. z\W
:target. i86
:enumerate. win
:usage. more efficient Microsoft Windows entry sequences
:jusage. より効果的なMicrosoft Windowsエントリコード列を生成します

:option. z\Ws
:target. i86
:enumerate. win
:usage. generate code for Microsoft Windows with smart callbacks
:jusage. スマート･コールバックをするMicrosoft Windows用コードを生成します

:option. zx
:target. i86 386
:usage. assume functions will modify FPU registers
:jusage. 関数がFPUレジスタを変更すると仮定します
:internal.

:option. zz
:target. 386
:usage. remove "@size" from __stdcall function names (10.0 compatible)
:jusage. "@size"を__stdcall関数名から削除します(10.0との互換性)
