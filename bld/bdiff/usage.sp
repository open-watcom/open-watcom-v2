::
:: BPATCH
::
:segment BPATCH
:segment ENGLISH
:segment QNX
Usage: %C [-p] [-q] [-b] <file>
:elsesegment
Usage: bpatch {-p} {-q} {-b} <file>
:endsegment
Options:
    -p      Do not prompt for confirmation.
    -b      Do not create a .BAK file.
    -q      Print current patch level of file.
:elsesegment JAPANESE
:segment QNX
使用方法: %C [-p] [-q] [-b] <file>
:elsesegment
使用方法: bpatch {-p} {-q} {-b} <file>
:endsegment
    -p      確認の応答を禁止する
    -b      .BAK ファイルの作成を禁止する
    -q      ファイルのカレントパッチレベルを出力する
:endsegment
::
:: BDUMP
::
:elsesegment BDUMP
:segment ENGLISH
:segment QNX
Usage: %C [-p] [-q] [-b] <file>
:elsesegment
Usage: bdump {-p} {-q} {-b} <file>
:endsegment
Options:
    -p      Do not prompt for confirmation.
    -b      Do not create a .BAK file.
    -q      Print current patch level of file.
:elsesegment JAPANESE
:segment QNX
使用方法: %C [-p] [-q] [-b] <file>
:elsesegment
使用方法: bdump {-p} {-q} {-b} <file>
:endsegment
    -p      確認の応答を禁止する
    -b      .BAK ファイルの作成を禁止する
    -q      ファイルのカレントパッチレベルを出力する
:endsegment
::
:: BDIFF
::
:elsesegment BDIFF
Usage: bdiff <old_exe> <new_exe> <patch_file> [options]\n
Options:
    -p<file>    file to patch
    -c<file>    comment file
    -do<file>   old file's debug info
    -dn<file>   new file's debug info
    -l          don't write patch level into patch file
:endsegment
