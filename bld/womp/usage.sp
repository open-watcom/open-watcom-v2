::
:: New Usage file for WOMP.
::
:segment ENGLISH
Usage: WOMP [options]* file [options|file]*
Options must precede the file(s) you wish them to affect (* is default)
/o=spec Output filename/directory specification
/fm    *Generate Microsoft 16- and 32-bit object files
/fm2    Generate Microsoft OS/2 2.0 32-bit object files
/fp     Generate PharLap Easy OMF-386 object files
/f-     Do not generate an object file (useful with /dx)
/dm     Generate Microsoft CodeView debugging information
/dp    *Generate PharLap/Metaware variant of CodeView debugging information
/dt     Generate Turbo debugging information
/dx     Generate human-readable text to console
/d-     Do not generate any debugging information
/pw    *Parse WATCOM debugging information
/p7     Parse WATCOM C 7.0 debugging information
/p-     Do not attempt to parse any debugging information
/q      Be quiet
:segment HIDDEN
/l      Move library COMENTs to beginning of file
:endsegment
/b      Leave temporary files and output batch file commands
These options apply to .WMP files
@file   Read file[.WMP] for options
#       Ignore from # to end of line (comment)
:elsesegment JAPANESE
使用方法: WOMP [options]* file [options|file]*
オプションはファイルの前に指定して下さい指定して下さい
/o=spec 出力ファイル名／ディレクトリの指定
/fm    *Microsoftの16ビットと32ビット・オブジェクト・ファイルの生成
/fm2    Microsoft OS/2 2.0の32ビット・オブジェクト・ファイルの生成
/fp     PharLap Easy OMF-386 オブジェクト・ファイルの生成
/f-     オブジェクト・ファイルの生成の禁止(/dxと共に使用すると便利
/dm     Microsoft CodeViewデバッグ情報の生成
/dp    *PharLap/Metawareデバッグ情報の生成
/dt     Turboデバッグ情報の生成
/dx     可読テキストのコンソールへの出力
/d-     デバッグ情報の生成の禁止
/pw    *WATCOMデバッグ情報の解析
/p7     WATCOM C7.0デバッグ情報の解析
/p-     デバッグ情報の解析の禁止
/q      無メッセージ・モード
:segment HIDDEN
/l      ライブラリCOMENTをファイルの先頭に移動
:endsegment
/b      テンポラリ・ファイルを残し、バッチファイル・コマンドを出力
以下のオプションは .WMP ファイルに適用できます
@file   オプションに関して[.WMP]ファイルを読みます
#       #から行末までを無視します(ｺﾒﾝﾄ)
:endsegment
:segment IS_RC
.
:endsegment
