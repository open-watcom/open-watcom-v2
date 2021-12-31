/* UTF-8 encoding, ¥ */

pick( ERR_DISK_FULL
        , "Fatal Error: Disk full."
        , "重大なエラー:ディスクがいっぱいです"
)
pick( ERR_SAMPLES_LOST
        , "Error: some samples have been lost"
        , "エラー:サンプリング・データの一部が失われました"
)
pick( ERR_SAMPLE_TRUNCATE
        , "Error: Out of disk space. Sample file truncated"
        , "エラー:ディスクの空き領域が不足です.サンプルファイルを切り詰めます "
)
pick( MSG_EXPECTING
        , "Expecting either a '=' or a '#' instead found: "
        , "'=' または '#' があるべき箇所に次のコードがありました: "
)
pick( MSG_INVALID_CHAR
        , "Invalid character after number"
        , "数字の後に無効な文字があります"
)
pick( MSG_OUT_OF_RANGE
        , "number is outside of its allowable range"
        , "数字が有効範囲内にありません"
)
pick( MSG_SAMPLE_BUFF
        , "Can't allocate sample buffer"
        , "サンプリング用バッファのメモリが確保できません"
)
pick( MSG_CALLGRAPH_BUFF
        , "Can't allocate callgraph buffer"
        , "コールグラフ用バッファのメモリを確保できません"
)
pick( MSG_VERSION
        , "Can't work with this version of the operating system"
        , "このバージョンのオペレーティング・システムの上では動作できません"
)
pick( MSG_SAMPLE_FILE
        , "Can't open sample file"
        , "サンプルファイルをオープンできません"
)
pick( MSG_PROGRAM
        , "Can't find program: "
        , "プログラムが見つかりません: "
)
pick( MSG_BUFF_SMALL
        , "Buffer size too small for timer rate"
        , "タイマー間隔に対してバッファが小さすぎます"
)
pick( MSG_INVALID_OPTION
        , "invalid option: "
        , "無効なオプション: "
)

pick( MSG_USAGE_LN_1
        , "Usage: wsample [options] program [program arguments]"
        , "使用方法: wsample [options] program [program arguments]"
)
pick( MSG_USAGE_LN_2
        , "Options:"
        , "オプション:"
)
#ifndef __WINDOWS__
pick( MSG_USAGE_LN_3
        , "    -b=<size> specify the buffer size for samples (default: 63KB)"
        , "    -b=<size> サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63KB)"
)
#endif
pick( MSG_USAGE_LN_4
        , "    -c        callgraph information is to be included in the sample file"
        , "    -c        コールグラフ情報をサンプルファイルに含めます"
)
pick( MSG_USAGE_LN_5
        , "    -f=<file> specify the output filename for the sample file"
        , "    -f=<file> サンプルファイルのファイル名"
)
#if defined(__NETWARE__)
pick( MSG_USAGE_LN_6
        , "    -o=<tmct> Specify a count to be used to reset the timer at unload"
        , "    -o=<tmct> Specify a count to be used to reset the timer at unload"
)
pick( MSG_USAGE_LN_7
        , "    -e        Estimate the timer terminal count for -o= (see above)"
        , "    -e        Estimate the timer terminal count for -o= (see above)"
)
#endif

pick( MSG_OPTIONS_1
        , "    -r=<rate> specify the sampling rate"
        , "    -r=<rate> サンプリング間隔"
)
#ifdef __DOS__      /* messages in sampdos */
pick( MSG_OPTIONS_2
        , "    -i=<intr> specify an interrupt number for assigning samples"
        , "    -i=<intr> サンプリングを行う割込み番号"
)
pick( MSG_OPTIONS_3
        , "    -d        disable assignment of DOS interrupts to application"
        , "    -d        DOS割込みの監視の禁止"
)
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_OPTIONS_2
        , "    -s        start the program in a new session"
        , "    -s        新しいセッションでのプログラムの開始"
)
#endif

#ifndef __WINDOWS__
pick( MSG_USAGE_LN_8
        , "\n    <size>    is a number in the range 1 to 63 (default: 63 KB)"
        , "\n    <size>    は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 KB)"
)
#endif

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔(ﾐﾘ秒単位)を"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              指定する数値(範囲:1から55まで)です (ﾃﾞﾌｫﾙﾄ: 55)"
)
pick( MSG_EXPLAIN_3
        , "    <intr>    is a hex (base 16) number (range 20 to ff) specifying"
        , "    <intr>    は16進数(範囲:20からffまで)で、以下の特別な方法で処理する"
)
pick( MSG_EXPLAIN_4
        , "              an interrupt number to treat in a special manner - "
        , "              割込み番号です - "
)
pick( MSG_EXPLAIN_5
        , "              any samples during the execution of the software interrupt"
        , "              ソフトウェア割込み実行中のサンプリングは、割込みを発生した命令に"
)
pick( MSG_EXPLAIN_6
        , "              are assigned to the instruction that invoked the interrupt"
        , "              割り当てます"
)
pick( MSG_EXPLAIN_7
        , "              (default: the DOS interrupt 21 is monitored)"
        , "              (ﾃﾞﾌｫﾙﾄ: DOS 割込み 21 がﾓﾆﾀｰされています)"
)
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)"
)
#endif

#ifdef __LINUX__      /* messages in samplinux */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 10)"
        , "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 10)"
)
#endif

#if defined( __PHARLAP__ ) || defined(__NETWARE__)      /* messages in samppls and sampnlm */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を"
)
pick( MSG_EXPLAIN_2
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を"
)
pick( MSG_EXPLAIN_3
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)"
)
pick( MSG_EXPLAIN_4
        , "              NB: use this switch with caution. It will cause the time"
        , "              注意: このスイッチの使用には注意してください.プログラム実行中の"
)
pick( MSG_EXPLAIN_5
        , "                  to be incorrect during program execution. Also, rates"
        , "                  時間が正しくなくなります.また、値が小さすぎると、遅いシステム"
)
pick( MSG_EXPLAIN_6
        , "                  which are too small, may crash slower systems."
        , "                  の場合、クラッシュする可能性があります."
)
#endif

#if defined( __DOS4G__ )    /* messages in samprsi */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)"
)
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)"
        , "              指定する数値 (範囲:1 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)"
)
#endif

pick( MSG_USAGE_LN_9
        , "\n   '#' may be used instead of '=' in options (eg. -b#10)"
        , "\n   オプションの中では '=' の代りに '#' を使ってください (例 -b#10)"
)

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_SAMPLE_1
        , "Too many sections in program"
        , "プログラムの中のセクションが多すぎます"
)
pick( MSG_SAMPLE_2
        , "Interrupts 34 to 3d are reserved for 80x87 emulation and cannot be monitored"
        , "割込み 34 から 3d は 80x87 エミュレーションのために予約されており、モニターできません"
)
pick( MSG_SAMPLE_3
        , "Cannot monitor more than eight interrupts"
        , "9個以上の割込みはモニターできません"
)
#endif

#if defined( __OS2__ ) && defined( _M_I86 )      /* messages in sampos2 */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "Cannot create a new thread"
        , "新しいスレッドを作れません"
)
pick( MSG_SAMPLE_5
        , "Cannot boost sleeper thread priority"
        , "スリーパ・スレッドの優先順位を上げられません"
)
pick( MSG_SAMPLE_6
        , "Unexpected notification"
        , "記法が間違っています"
)
pick( MSG_SAMPLE_7
        , "DosPTrace( GO ) failed"
        , "DosPTrace( GO ) が失敗しました"
)
pick( MSG_SAMPLE_8
        , "Cannot find OS/2 2.x sampler"
        , "OS/2 2.x 用サンプラが見つかりません"
)
pick( MSG_SAMPLE_9
        , "Cannot start OS/2 2.x sampler"
        , "OS/2 2.x 用サンプラを開始できません"
)
#endif

#if defined( __OS2__ ) && !defined( _M_I86 )     /* messages in sampos22 */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "Cannot create a new thread"
        , "新しいスレッドを作れません"
)
pick( MSG_SAMPLE_5
        , "Cannot boost sleeper thread priority"
        , "スリーパ・スレッドの優先順位を上げられません"
)
pick( MSG_SAMPLE_6
        , "Unexpected notification"
        , "記法が間違っています"
)
pick( MSG_SAMPLE_7
        , "DosDebug call failed"
        , "DosDebug コールが失敗しました"
)
pick( MSG_SAMPLE_8
        , "DosDebug( STOP ) failed"
        , "DosDebug( STOP ) が失敗しました"
)
pick( MSG_SAMPLE_9
        , "Unable to attach to process"
        , "プロセスに接触できませんでした"
)
pick( MSG_SAMPLE_10
        , "CPU exception reported"
        , "CPU 例外が報告されています"
)
#endif

#ifdef __NT__       /* messages in sampnt */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "Unexpected notification"
        , "記法が間違っています"
)
#endif

#ifdef __LINUX__    /* messages in samplnx */
pick( MSG_SAMPLE_1
        , "Executing: "
        , "実行中: "
)
pick( MSG_SAMPLE_2
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_3
        , "Cannot execute file"
        , "ファイルを実行できません"
)
pick( MSG_SAMPLE_4
        , "ptrace() failed"
        , "ptrace() が失敗しました"
)
pick( MSG_SAMPLE_5
        , "Unable to attach to process"
        , "プロセスに接触できませんでした"
)
pick( MSG_SAMPLE_6
        , "setitimer() failed"
        , "setitimer() が失敗しました"
)
#endif

#ifdef __PHARLAP__  /* messages in samppls */
pick( MSG_SAMPLE_1
        , "Internal error"
        , "内部エラー"
)
pick( MSG_SAMPLE_2
        , "Unable to load: "
        , "ロードできません: "
)
pick( MSG_SAMPLE_3
        , "Task Exception: "
        , "タスク例外: "
)
pick( MSG_SAMPLE_4
        , "Unexpected Exception: "
        , "予期しない例外: "
)
#endif

#ifdef __NETWARE__      /* messages in sampnlm */
pick( MSG_SAMPLE_1
        , "Error! Cannot execute: "
        , "エラー! 実行できません: "
)
#endif

#ifdef __DOS4G__        /* messages in samprsi */
pick( MSG_SAMPLE_1
        , "Task Exception: "
        , "タスク例外: "
)
pick( MSG_SAMPLE_2
        , "Unable to load: "
        , "ロードできません: "
)
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_SAMPLE_1
        , "Couldn't find any modules!"
        , "モジュールが見つかりません!"
)
pick( MSG_SAMPLE_2
        , "Failed to hook interrupts"
        , "割込みのフックに失敗しました"
)
pick( MSG_SAMPLE_3
        , "Failed to get notify hook"
        , "フックの通知を受けるのに失敗しました"
)
pick( MSG_SAMPLE_4
        , "Could not start sampling!"
        , "サンプリングを開始できません!"
)
pick( MSG_SAMPLE_5
        , "Sample buffer flushed"
        , "サンプルバッファをフラッシュしました"
)
pick( MSG_SAMPLE_6
        , "internal error: "
        , "内部エラー: "
)
pick( MSG_SAMPLE_7
        , "Timer rate set to "
        , "タイマ間隔を "
)
pick( MSG_SAMPLE_8
        , " ms"
        , " ms"
)
pick( MSG_SAMPLE_9
        , "Determining what other modules are present..."
        , "他のどのようなモジュールがあるかを探索中..."
)
pick( MSG_SAMPLE_10
        , "Found "
        , "ほかに "
)
pick( MSG_SAMPLE_11
        , " other modules"
        , " 個のモジュールがあります"
)
pick( MSG_SAMPLE_12
        , "Will use WDEBUG.386"
        , "WDEBUG.386を使用します"
)
pick( MSG_SAMPLE_13
        , "Execution finished, total time="
        , "実行終了, 全時間="
)
pick( MSG_SAMPLE_14
        , " seconds"
        , " 秒"
)
pick( MSG_SAMPLE_15
        , "Error loading sampler!"
        , "サンプラ・ロード・エラー!"
)
pick( MSG_SAMPLE_16
        , "Cannot close sampler!"
        , "サンプラをクローズできません!"
)
#endif

#if defined( __DOS4G__ ) || defined( __PHARLAP__ )
pick( MSG_EXCEPT_0
        , ""
        , ""
)
pick( MSG_EXCEPT_1
        , "divide overflow"
        , "除算オーバーフロー"
)
pick( MSG_EXCEPT_2
        , "non-maskable interrupt"
        , "マスク不可能割込み(ﾉﾝﾏｽｶﾌﾞﾙ割込み,NMI)"
)
pick( MSG_EXCEPT_3
        , "integer overflow"
        , "整数オーバーフロー"
)
pick( MSG_EXCEPT_4
        , "bounds check"
        , "境界検査"
)
pick( MSG_EXCEPT_5
        , "invalid opcode"
        , "無効なオペコード"
)
pick( MSG_EXCEPT_6
        , "coprocessor not available"
        , "コプロセッサはありません"
)
pick( MSG_EXCEPT_7
        , "double fault"
        , "二重フォールト"
)
pick( MSG_EXCEPT_8
        , "coprocessor segment overrun"
        , "コプロセッサ・セグメント・オーバーラン"
)
pick( MSG_EXCEPT_9
        , "invalid TSS"
        , "無効なTSS"
)
pick( MSG_EXCEPT_10
        , "segment not present"
        , "セグメントが(メモリ上に)ありません"
)
pick( MSG_EXCEPT_11
        , "stack exception"
        , "スタック例外"
)
pick( MSG_EXCEPT_12
        , "general protection fault"
        , "一般保護フォールト"
)
pick( MSG_EXCEPT_13
        , "page fault"
        , "ページ・フォールト"
)
pick( MSG_EXCEPT_14
        , "coprocessor error"
        , "コプロセッサ・エラー"
)
#endif
