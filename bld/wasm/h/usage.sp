:segment ENGLISH
Usage:   wasm [options] asm_file [options] [@env_var]
:segment qnx
Options:
:elsesegment
Options:		    ( /option is also supported )
:endsegment
-{0,1,2,3,4,5}{p}{r,s}
    -0		8086 instructions
    -1		80186 instructions
    -2		80286 instructions
    -3		80386 instructions
    -4		80486 instructions
    -5		pentium instructions
    add	p	protect mode
    add	r	register calling conventions
    add	s	stack calling conventions
		ie: /2	    /3s	    /4pr    /5p
:segment HIDDEN
    -3r_ or -3s_	just like /3r and /3s except assume the programmer
			put uscores in the assembly file correctly
			thus -3r_ doesn't add uscores, -3s_ removes uscores
:endsegment
-bt=<os>            set the build target to <os>
-c		    disable outputting of data in code records
-d<name>[=text]	    define text macro
-d1		    line number debugging support
-e		    stop reading ASM file at END directive
-e<number>	    set error limit number
-fr=<file_name>	    set error file name
-fo=<file_name>	    set object file name
-fi=<file_name>	    force <file_name> to be included
-fpc		    calls to floating-point library
-fpi		    inline 80x87 instructions with emulation
-fpi87		    inline 80x87 instructions
-fp0		    floating-point for 8087
-fp2		    floating-point for 287
-fp3		    floating-point for 387
-fp5		    floating-point for Pentium
-i=<directory>	    add directory to list of include directories
-j or -s	    force signed types to be used for signed values
-m{t,s,m,c,l,h,f}   memory model:
		    (Tiny, Small, Medium, Compact, Large, Huge, Flat)
:segment HIDDEN
-nc=<name>	    set code class name // NYI
:endsegment
-nd=<name>	    set data segment name
-nm=<name>	    set module name
-nt=<name>	    set name of text segment
-o		    allow C form of octal constants
-zq or -q	    operate quietly
-?		    print this message
-w<number>	    set warning level number
-we		    treat all warnings as errors

:elsesegment JAPANESE
使用方法: wasm [options] file [options] [@env_var]
:segment qnx
オプション:
:elsesegment
オプション:		    ( /ｵﾌﾟｼｮﾝ でも指定できます )
:endsegment
	-fo=<file_name>	    オブジェクトファイルを設定します
	-fe=<file_name>	    エラーファイル名を指定します
	-i=<directory>	    インクルード・ディレクトリのリストを追加します
	-e		    ENDディレクティブでASMファイルの読み込みを止めます
	-j or -s	    符号付き型を符号付き値のために使用するようにします
	-d1		    行番号デバッグ情報を出力します
	-d<name>[=text]	    テキストマクロを定義します
	-0		    8086 命令
	-1		    80186 命令
	-2		    80286 命令
	-3		    80386 命令
	-4		    80486 命令
	-m{t,s,m,c,l,h,f}   メモリ・モデル
			    (Tiny, Small, Medium, Compact, Large, Huge, Flat)
	-o		    allow C form of octal constants
	-3r		    386 レジスタ呼び出し規約
	-3s		    386 スタック呼び出し規約
	-4r		    486 レジスタ呼び出し規約
	-4s		    486 スタック呼び出し規約
	-zq or -q	    メッセージ等の出力をしません
	-?  		    このメッセージを表示します
:endsegment
:segment IS_RC
.
:endsegment
