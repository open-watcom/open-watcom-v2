:segment ENGLISH
Usage: wstrip [options] input_file [output_file] [info_file]
:segment !QNX
options: (/option is also accepted)
:elsesegment
options:
:endsegment
        -q     don't print informational messages
        -n     don't print warning messages
        -r     process resource information rather than debugging information
        -a     add information rather than delete information

        input_file:  executable file
        output_file: optional output executable or '.'
        info_file:   optional output debugging or resource information file
                     or input debugging or resource information file
:elsesegment JAPANESE
使用方法: wstrip [options] input_file [output_file] [info_file]
:segment !QNX
オプション: (/ｵﾌﾟｼｮﾝ も受け付けます)
:elsesegment
オプション:
:endsegment
        -q     メッセージを表示しません
        -n     警告メッセージを表示しません
        -r     デバッグ情報ではなく、リソース情報を処理します
        -a     情報削除ではなく、情報付加をします

        input_file:  実行可能ファイル
        output_file: オプションで出力実行可能ファイルまたは '.'を指定可能
        info_file:   オプションでデバッグまたはリソース情報ファイルの出力又は
                     入力を指定可能
:endsegment
:segment IS_RC
.
:endsegment
