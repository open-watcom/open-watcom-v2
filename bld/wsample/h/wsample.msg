pick( ERR_DISK_FULL
        , "Fatal Error: Disk full."
        , "重大なエラー:ディスクがいっぱいです"
)
pick( ERR_SAMPLES_LOST
        , "エラー:サンプリング・データの一部が失われました"
        , "Error: some samples have been lost"
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
        , "Usage: wsample [options] program [program arguments]\n"
        , "使用方法: wsample [options] program [program arguments]\n"
)
pick( MSG_USAGE_LN_2
        , "Options:\n"
        , "オプション:\n"
)
#ifndef __WINDOWS__
pick( MSG_USAGE_LN_3
        , "    -b=<size> specify the buffer size for samples (default: 63KB)\n"
        , "    -b=<size> サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63KB)\n"
)
#endif
pick( MSG_USAGE_LN_4
        , "    -c        callgraph information is to be included in the sample file\n"
        , "    -c        コールグラフ情報をサンプルファイルに含めます\n"
)
pick( MSG_USAGE_LN_5
        , "    -f=<file> specify the output filename for the sample file\n"
        , "    -f=<file> サンプルファイルのファイル名\n"
)
#if defined(__NETWARE__)
pick( MSG_USAGE_LN_6
        , "    -o=<tmct> Specify a count to be used to reset the timer at unload\n"
        , "    -o=<tmct> Specify a count to be used to reset the timer at unload\n"
)
pick( MSG_USAGE_LN_7
        , "    -e        Estimate the timer terminal count for -o= (see above)\n"
        , "    -e        Estimate the timer terminal count for -o= (see above)\n"
)
#endif

pick( MSG_OPTIONS_1
        , "    -r=<rate> specify the sampling rate\n"
        , "    -r=<rate> サンプリング間隔\n"
)
#ifdef __DOS__      /* messages in sampdos */
pick( MSG_OPTIONS_2
        , "    -i=<intr> specify an interrupt number for assigning samples\n"
        , "    -i=<intr> サンプリングを行う割込み番号\n"
)
pick( MSG_OPTIONS_3
        , "    -d        disable assignment of DOS interrupts to application\n"
        , "    -d        DOS割込みの監視の禁止\n"
)
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_OPTIONS_2
        , "    -s        start the program in a new session\n"
        , "    -s        新しいセッションでのプログラムの開始\n"
)
#endif

#ifndef __WINDOWS__
pick( MSG_USAGE_LN_8
        , "\n    <size>    is a number in the range 1 to 63 (default: 63 KB)\n"
        , "\n    <size>    は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 KB)\n"
)
#endif

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔(ﾐﾘ秒単位)を\n"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)\n"
        , "              指定する数値(範囲:1から55まで)です (ﾃﾞﾌｫﾙﾄ: 55)\n"
)
pick( MSG_EXPLAIN_3
        , "    <intr>    is a hex (base 16) number (range 20 to ff) specifying\n"
        , "    <intr>    は16進数(範囲:20からffまで)で、以下の特別な方法で処理する\n"
)
pick( MSG_EXPLAIN_4
        , "              an interrupt number to treat in a special manner - \n"
        , "              割込み番号です - \n"
)
pick( MSG_EXPLAIN_5
        , "              any samples during the execution of the software interrupt\n"
        , "              ソフトウェア割込み実行中のサンプリングは、割込みを発生した命令に\n"
)
pick( MSG_EXPLAIN_6
        , "              are assigned to the instruction that invoked the interrupt\n"
        , "              割り当てます\n"
)
pick( MSG_EXPLAIN_7
        , "              (default: the DOS interrupt 21 is monitored)\n"
        , "              (ﾃﾞﾌｫﾙﾄ: DOS 割込み 21 がﾓﾆﾀｰされています)\n"
)
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)\n"
        , "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n"
)
#endif

#ifdef __LINUX__      /* messages in samplinux */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 10)\n"
        , "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 10)\n"
)
#endif

#if defined(__PLS__) || defined(__NETWARE__)      /* messages in samppls and sampnlm */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n"
)
pick( MSG_EXPLAIN_2
        , "    <rate>    is a number (range 1 to 55) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n"
)
pick( MSG_EXPLAIN_3
        , "              time interval (in milliseconds) between samples (default: 55)\n"
        , "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n"
)
pick( MSG_EXPLAIN_4
        , "              NB: use this switch with caution. It will cause the time\n"
        , "              注意: このスイッチの使用には注意してください.プログラム実行中の\n"
)
pick( MSG_EXPLAIN_5
        , "                  to be incorrect during program execution. Also, rates\n"
        , "                  時間が正しくなくなります.また、値が小さすぎると、遅いシステム\n"
)
pick( MSG_EXPLAIN_6
        , "                  which are too small, may crash slower systems.\n"
        , "                  の場合、クラッシュする可能性があります.\n"
)
#endif

#if defined(__RSI__)      /* messages in samprsi */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 55) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)\n"
        , "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n"
)
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_EXPLAIN_1
        , "    <rate>    is a number (range 1 to 1000) specifying the approximate\n"
        , "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n"
)
pick( MSG_EXPLAIN_2
        , "              time interval (in milliseconds) between samples (default: 55)\n"
        , "              指定する数値 (範囲:1 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n"
)
#endif

pick( MSG_USAGE_LN_9
        , "\n   '#' may be used instead of '=' in options (eg. -b#10)\n"
        , "\n   オプションの中では '=' の代りに '#' を使ってください (例 -b#10)\n"
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

#ifdef __LINUX__      /* messages in samplnx */
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

#ifdef __PLS__      /* messages in samppls */
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

#ifdef __RSI__      /* messages in samprsi */
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
        , "Sample buffer flushed\n"
        , "サンプルバッファをフラッシュしました\n"
)
pick( MSG_SAMPLE_6
        , "internal error: %s\n"
        , "内部エラー: %s\n"
)
pick( MSG_SAMPLE_7
        , "Timer rate set to %d ms\n"
        , "タイマ間隔を %d ﾐﾘ秒に設定しました\n"
)
pick( MSG_SAMPLE_8
        , "Determining what other modules are present...\n"
        , "他のどのようなモジュールがあるかを探索中...\n"
)
pick( MSG_SAMPLE_9
        , "Found %d other modules\n"
        , "ほかに %d 個のモジュールがあります\n"
)
pick( MSG_SAMPLE_10
        , "Will use WDEBUG.386\n"
        , "WDEBUG.386を使用します\n"
)
pick( MSG_SAMPLE_11
        , "Execution finished, total time=%ld.%03d seconds\n"
        , "実行終了, 全時間=%ld.%03d 秒\n"
)
pick( MSG_SAMPLE_12
        , "Error loading sampler!"
        , "サンプラ・ロード・エラー!"
)
pick( MSG_SAMPLE_13
        , "Cannot close sampler!\n"
        , "サンプラをクローズできません!\n"
)
#endif

#if defined(__RSI__) || defined(__PLS__)
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
