::
:: help page
:: For a page break, leave a line undefined.
::
:segment ENGLISH
Usage  :  wmake [options]* [macro=text]* [target]*
Options:  ('-' may be substituted for '/', and '#' for '=')
/a make all targs (ignoring time stamp)  /b block/ignore all implicit rules
/c do not check existence of files made  /d debug mode (echo progress of work)
/e erase files after error (no prompt)   /h do not print program header
/i ignore command return status          /k continue after an error
:segment HIDDEN
::	until we know this works properly for '::' rules
/j set time stamp of files made to the latest time stamp of their dependents
:endsegment
/m do not read MAKEINIT file             /ms Microsoft NMAKE mode
/n print commands without executing      /o take advantage of circular path  
/q query mode (commands not executed)    
/p print the dependency tree, macro definitions, and extension information
/r ignore built-in definitions           /s silent mode (do not print commands)
/t touch files (commands not executed)   /u UNIX compatibility mode
/v verbose listing of inline files       /z do not erase files (no prompt)
/l logfile	append output to logfile
/f filename	process filename instead of MAKEFILE ('/f -' means stdin)
macro=text	overrides the definition of 'macro' with the value 'text'
target		specifies a target to make (default is first target)
:elsesegment JAPANESE
使用方法:  wmake [options]* [macro=text]* [target]*
オプション:  ('/'は'-'で,'='は'#'で代用する事ができます)
/a 全ﾀｰｹﾞｯﾄを作成します(ﾀｲﾑ･ｽﾀﾝﾌﾟを無視)  /b すべての暗黙ﾙｰﾙを無視します
/c 作られたﾌｧｲﾙがあるどうかﾁｪｯｸしません   /d ﾃﾞﾊﾞｯｸﾞ･ﾓｰﾄﾞ(処理過程を出力します)
/e ｴﾗｰが起きた時、ﾌｧｲﾙを消します(確認しません)
/h ﾌﾟﾛｸﾞﾗﾑ･ﾍｯﾀﾞを出力しません		  /u UNIX互換ﾓｰﾄﾞ
/i ｺﾏﾝﾄﾞが返すｽﾃｰﾀｽを無視します           /k ｴﾗｰが起きても続行します
:segment HIDDEN
:: until we know this works properly for '::' rules
/j ﾌｧｲﾙのﾀｲﾑ･ｽﾀﾝﾌﾟを依存先の中で最新のﾀｲﾑ･ｽﾀﾝﾌﾟに設定します
:endsegment
/m MAKEINIT ﾌｧｲﾙを読みません              /ms microsoft nmake mode
/n 実行せずに、ｺﾏﾝﾄﾞを表示します          /o 循環ﾊﾟｽを利用します                    
/q 問い合せﾓｰﾄﾞ(ｺﾏﾝﾄﾞを実行しません)
/p 依存規制、ﾏｸﾛの定義、拡張ｲﾝﾌｫﾒｰｼｮﾝなどを出力します
/r ﾃﾞﾌｫﾙﾄ定義を無視します                 /s ｻｲﾚﾝﾄ･ﾓｰﾄﾞ(ｺﾏﾝﾄﾞを表示しません)
/t ﾌｧｲﾙのﾀｲﾑ･ｽﾀﾝﾌﾟを更新します(ｺﾏﾝﾄﾞを実行しません)
/v verbose listing of inline file
/z ﾌｧｲﾙを消しません(確認しません)
/l logfile    ﾛｸﾞ･ﾌｧｲﾙに出力を追加します
/f filename    MAKEFILEの代りにﾌｧｲﾙfilenameを処理します('/f -'は標準入力です)
macro=text    'macro' の値を 'text' で再定義します
target        作成したいﾀｰｹﾞｯﾄを指定します(ﾃﾞﾌｫﾙﾄは一番目のﾀｰｹﾞｯﾄです)
:endsegment
:segment IS_RC
.
:endsegment
