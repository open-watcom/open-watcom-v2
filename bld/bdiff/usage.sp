:segment BPATCH
:segment ENGLISH
:segment QNX
Usage: %C [-p] [-q] [-b] <file>
:elsesegment
Usage: %s {-p} {-q} {-b} <file>
:endsegment
Options:
	-p	Do not prompt for confirmation.
	-b	Do not create a .BAK file.
	-q	Print current patch level of file.
:elsesegment JAPANESE
:segment QNX
使用方法: %C [-p] [-q] [-b] <file>
:elsesegment
使用方法: %s {-p} {-q} {-b} <file>
:endsegment
	-p	確認の応答を禁止する
	-b	.BAK ファイルの作成を禁止する
	-q	ファイルのカレントパッチレベルを出力する
:endsegment
:elsesegment BDIFF
usage: %s <old_exe> <new_exe> <patch_file> [options]\n
options:  -p<file>         file to patch
          -c<file>         comment file
          -do<file>        old file's debug info
          -dn<file>        new file's debug info
          -l               don't write patch level into patch file
:endsegment
