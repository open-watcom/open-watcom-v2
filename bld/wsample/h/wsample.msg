pick( ERR_DISK_FULL,        "重大なエラー:ディスクがいっぱいです",
                "Fatal Error: Disk full." )
pick( ERR_SAMPLES_LOST,     "エラー:サンプリング・データの一部が失われました",
                "Error: some samples have been lost" )
pick( ERR_SAMPLE_TRUNCATE,  "エラー:ディスクの空き領域が不足です.サンプルファイルを切り詰めます ",
                "Error: Out of disk space. Sample file truncated" )
pick( MSG_EXPECTING,        "'=' または '#' があるべき箇所に次のコードがありました: ",
                "Expecting either a '=' or a '#' instead found: " )
pick( MSG_INVALID_CHAR,     "数字の後に無効な文字があります",
                "Invalid character after number" )
pick( MSG_OUT_OF_RANGE,     "数字が有効範囲内にありません",
                "number is outside of its allowable range" )
pick( MSG_SAMPLE_BUFF,      "サンプリング用バッファのメモリが確保できません",
                "Can't allocate sample buffer" )
pick( MSG_CALLGRAPH_BUFF,   "コールグラフ用バッファのメモリを確保できません",
                "Can't allocate callgraph buffer" )
pick( MSG_VERSION,      "このバージョンのオペレーティング・システムの上では動作できません",
                "Can't work with this version of the operating system" )
pick( MSG_SAMPLE_FILE,      "サンプルファイルをオープンできません",
                "Can't open sample file" )
pick( MSG_PROGRAM,      "プログラムが見つかりません: ",
                "Can't find program: " )
pick( MSG_BUFF_SMALL,       "タイマー間隔に対してバッファが小さすぎます",
                "Buffer size too small for timer rate" )
pick( MSG_INVALID_OPTION,   "無効なオプション: ",
                "invalid option: " )

pick( MSG_USAGE_LN_1,       "使用方法: wsample [options] program [program arguments]\n",
                "Usage: wsample [options] program [program arguments]\n" )
pick( MSG_USAGE_LN_2,       "オプション:\n",
                "Options:\n" )
#ifndef __WINDOWS__
pick( MSG_USAGE_LN_3,       "    -b=<size> サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63KB)\n",
                "    -b=<size> specify the buffer size for samples (default: 63KB)\n" )
#endif
pick( MSG_USAGE_LN_4,       "    -c        コールグラフ情報をサンプルファイルに含めます\n",
                "    -c        callgraph information is to be included in the sample file\n" )
pick( MSG_USAGE_LN_5,       "    -f=<file> サンプルファイルのファイル名\n",
                "    -f=<file> specify the output filename for the sample file\n" )
#if defined(__NETWARE__)
pick( MSG_USAGE_LN_6,
    "    -o=<tmct> Specify a count to be used to reset the timer at unload\n",
    "    -o=<tmct> Specify a count to be used to reset the timer at unload\n" )
pick( MSG_USAGE_LN_7,
    "    -e        Estimate the timer terminal count for -o= (see above)\n",
    "    -e        Estimate the timer terminal count for -o= (see above)\n" )
#endif

pick( MSG_OPTIONS_1,        "    -r=<rate> サンプリング間隔\n",
                "    -r=<rate> specify the sampling rate\n" )
#ifdef __DOS__      /* messages in sampdos */
pick( MSG_OPTIONS_2,        "    -i=<intr> サンプリングを行う割込み番号\n",
                "    -i=<intr> specify an interrupt number for assigning samples\n" )
pick( MSG_OPTIONS_3,        "    -d        DOS割込みの監視の禁止\n",
                "    -d        disable assignment of DOS interrupts to application\n" )
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_OPTIONS_2,        "    -s        新しいセッションでのプログラムの開始\n",
                "    -s        start the program in a new session\n" )
#endif

#ifndef __WINDOWS__
pick( MSG_USAGE_LN_8,       "\n    <size>    は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 KB)\n",
                "\n    <size>    is a number in the range 1 to 63 (default: 63 KB)\n" )
#endif

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_EXPLAIN_1,        "    <rate>    はサンプリング間のおよその時間間隔(ﾐﾘ秒単位)を\n",
                "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,        "              指定する数値(範囲:1から55まで)です (ﾃﾞﾌｫﾙﾄ: 55)\n",
                "              time interval (in milliseconds) between samples (default: 55)\n" )
pick( MSG_EXPLAIN_3,        "    <intr>    は16進数(範囲:20からffまで)で、以下の特別な方法で処理する\n",
                "    <intr>    is a hex (base 16) number (range 20 to ff) specifying\n" )
pick( MSG_EXPLAIN_4,        "              割込み番号です - \n",
                "              an interrupt number to treat in a special manner - \n" )
pick( MSG_EXPLAIN_5,        "              ソフトウェア割込み実行中のサンプリングは、割込みを発生した命令に\n",
                "              any samples during the execution of the software interrupt\n" )
pick( MSG_EXPLAIN_6,        "              割り当てます\n",
                "              are assigned to the instruction that invoked the interrupt\n" )
pick( MSG_EXPLAIN_7,        "              (ﾃﾞﾌｫﾙﾄ: DOS 割込み 21 がﾓﾆﾀｰされています)\n",
                "              (default: the DOS interrupt 21 is monitored)\n" )
#endif

#if defined(__OS2__)      /* messages in sampos2 and sampos22 */
pick( MSG_EXPLAIN_1,        "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
                "    <rate>    is a number (range 1 to 1000) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,        "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
                "              time interval (in milliseconds) between samples (default: 55)\n" )
#endif

#ifdef __LINUX__      /* messages in samplinux */
pick( MSG_EXPLAIN_1,        "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
                "    <rate>    is a number (range 1 to 1000) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,        "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 10)\n",
                "              time interval (in milliseconds) between samples (default: 10)\n" )
#endif

#if defined(__PLS__) || defined(__NETWARE__)      /* messages in samppls and sampnlm */
pick( MSG_EXPLAIN_1,        "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
                "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,        "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
                "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_3,        "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
                "              time interval (in milliseconds) between samples (default: 55)\n" )
pick( MSG_EXPLAIN_4,        "              注意: このスイッチの使用には注意してください.プログラム実行中の\n",
                "              NB: use this switch with caution. It will cause the time\n" )
pick( MSG_EXPLAIN_5,        "                  時間が正しくなくなります.また、値が小さすぎると、遅いシステム\n",
                "                  to be incorrect during program execution. Also, rates\n" )
pick( MSG_EXPLAIN_6,        "                  の場合、クラッシュする可能性があります.\n",
                "                  which are too small, may crash slower systems.\n" )
#endif

#if defined(__RSI__)      /* messages in samprsi */
pick( MSG_EXPLAIN_1,        "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
                "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,        "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
                "              time interval (in milliseconds) between samples (default: 55)\n" )
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_EXPLAIN_1,        "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
                "    <rate>    is a number (range 1 to 1000) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,        "              指定する数値 (範囲:1 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
                "              time interval (in milliseconds) between samples (default: 55)\n" )
#endif

pick( MSG_USAGE_LN_9,
    "\n   オプションの中では '=' の代りに '#' を使ってください (例 -b#10)\n",
    "\n   '#' may be used instead of '=' in options (eg. -b#10)\n" )

#ifdef __DOS__      /* messages in sampdos */
pick( MSG_SAMPLE_1,     "プログラムの中のセクションが多すぎます",
                "Too many sections in program" )
pick( MSG_SAMPLE_2,     "割込み 34 から 3d は 80x87 エミュレーションのために予約されており、モニターできません",
                "Interrupts 34 to 3d are reserved for 80x87 emulation and cannot be monitored" )
pick( MSG_SAMPLE_3,     "9個以上の割込みはモニターできません",
                "Cannot monitor more than eight interrupts" )
#endif

#if defined( __OS2__ ) && defined( _M_I86 )      /* messages in sampos2 */
pick( MSG_SAMPLE_1,     "実行中: ",
                "Executing: " )
pick( MSG_SAMPLE_2,     "内部エラー: ",
                "internal error: " )
pick( MSG_SAMPLE_3,     "ファイルを実行できません",
                "Cannot execute file" )
pick( MSG_SAMPLE_4,     "新しいスレッドを作れません",
                "Cannot create a new thread" )
pick( MSG_SAMPLE_5,     "スリーパ・スレッドの優先順位を上げられません",
                "Cannot boost sleeper thread priority" )
pick( MSG_SAMPLE_6,     "記法が間違っています",
                "Unexpected notification" )
pick( MSG_SAMPLE_7,     "DosPTrace( GO ) が失敗しました",
                "DosPTrace( GO ) failed" )
pick( MSG_SAMPLE_8,     "OS/2 2.x 用サンプラが見つかりません",
                "Cannot find OS/2 2.x sampler" )
pick( MSG_SAMPLE_9,     "OS/2 2.x 用サンプラを開始できません",
                "Cannot start OS/2 2.x sampler" )
#endif

#if defined( __OS2__ ) && !defined( _M_I86 )     /* messages in sampos22 */
pick( MSG_SAMPLE_1,     "実行中: ",
                "Executing: " )
pick( MSG_SAMPLE_2,     "内部エラー: ",
                "internal error: " )
pick( MSG_SAMPLE_3,     "ファイルを実行できません",
                "Cannot execute file" )
pick( MSG_SAMPLE_4,     "新しいスレッドを作れません",
                "Cannot create a new thread" )
pick( MSG_SAMPLE_5,     "スリーパ・スレッドの優先順位を上げられません",
                "Cannot boost sleeper thread priority" )
pick( MSG_SAMPLE_6,     "記法が間違っています",
                "Unexpected notification" )
pick( MSG_SAMPLE_7,     "DosDebug コールが失敗しました",
                "DosDebug call failed" )
pick( MSG_SAMPLE_8,     "DosDebug( STOP ) が失敗しました",
                "DosDebug( STOP ) failed" )
pick( MSG_SAMPLE_9,     "プロセスに接触できませんでした",
                "Unable to attach to process" )
pick( MSG_SAMPLE_10,        "CPU 例外が報告されています",
                "CPU exception reported" )
#endif

#ifdef __LINUX__      /* messages in samplnx */
pick( MSG_SAMPLE_1,     "実行中: ",
                "Executing: " )
pick( MSG_SAMPLE_2,     "内部エラー: ",
                "internal error: " )
pick( MSG_SAMPLE_3,     "ファイルを実行できません",
                "Cannot execute file" )
pick( MSG_SAMPLE_4,     "ptrace() が失敗しました",
                "ptrace() failed" )
pick( MSG_SAMPLE_5,     "プロセスに接触できませんでした",
                "Unable to attach to process" )
pick( MSG_SAMPLE_6,     "setitimer() が失敗しました",
                "ptrace() failed" )
#endif

#ifdef __PLS__      /* messages in samppls */
pick( MSG_SAMPLE_1,     "内部エラー",
                "Internal error" )
pick( MSG_SAMPLE_2,     "ロードできません: ",
                "Unable to load: " )
pick( MSG_SAMPLE_3,     "タスク例外: ",
                "Task Exception: " )
pick( MSG_SAMPLE_4,     "予期しない例外: ",
                "Unexpected Exception: " )
#endif

#ifdef __NETWARE__      /* messages in sampnlm */
pick( MSG_SAMPLE_1,     "エラー! 実行できません: ",
                "Error! Cannot execute: " )
#endif

#ifdef __RSI__      /* messages in samprsi */
pick( MSG_SAMPLE_1,     "タスク例外: ",
                "Task Exception: " )
pick( MSG_SAMPLE_2,     "ロードできません: ",
                "Unable to load: " )
#endif

#ifdef __WINDOWS__      /* messages in sampwin */
pick( MSG_SAMPLE_1,     "モジュールが見つかりません!",
                "Couldn't find any modules!" )
pick( MSG_SAMPLE_2,     "割込みのフックに失敗しました",
                "Failed to hook interrupts" )
pick( MSG_SAMPLE_3,     "フックの通知を受けるのに失敗しました",
                "Failed to get notify hook" )
pick( MSG_SAMPLE_4,     "サンプリングを開始できません!",
                "Could not start sampling!" )
pick( MSG_SAMPLE_5,     "サンプルバッファをフラッシュしました\n",
                "Sample buffer flushed\n" )
pick( MSG_SAMPLE_6,     "内部エラー: %s\n",
                "internal error: %s\n" )
pick( MSG_SAMPLE_7,     "タイマ間隔を %d ﾐﾘ秒に設定しました\n",
                "Timer rate set to %d ms\n" )
pick( MSG_SAMPLE_8,     "他のどのようなモジュールがあるかを探索中...\n",
                "Determining what other modules are present...\n" )
pick( MSG_SAMPLE_9,     "ほかに %d 個のモジュールがあります\n",
                "Found %d other modules\n" )
pick( MSG_SAMPLE_10,        "WDEBUG.386を使用します\n",
                "Will use WDEBUG.386\n" )
pick( MSG_SAMPLE_11,        "実行終了, 全時間=%ld.%03d 秒\n",
                "Execution finished, total time=%ld.%03d seconds\n" )
pick( MSG_SAMPLE_12,        "サンプラ・ロード・エラー!",
                "Error loading sampler!" )
pick( MSG_SAMPLE_13,        "サンプラをクローズできません!\n",
                "Cannot close sampler!\n" )
#endif

#if defined(__RSI__) || defined(__PLS__)
pick( MSG_EXCEPT_0,     "", "" )
pick( MSG_EXCEPT_1,     "除算オーバーフロー",
                "divide overflow" )
pick( MSG_EXCEPT_2,     "マスク不可能割込み(ﾉﾝﾏｽｶﾌﾞﾙ割込み,NMI)",
                "non-maskable interrupt" )
pick( MSG_EXCEPT_3,     "整数オーバーフロー",
                "integer overflow" )
pick( MSG_EXCEPT_4,     "境界検査",
                "bounds check" )
pick( MSG_EXCEPT_5,     "無効なオペコード",
                "invalid opcode" )
pick( MSG_EXCEPT_6,     "コプロセッサはありません",
                "coprocessor not available" )
pick( MSG_EXCEPT_7,     "二重フォールト",
                "double fault" )
pick( MSG_EXCEPT_8,     "コプロセッサ・セグメント・オーバーラン",
                "coprocessor segment overrun" )
pick( MSG_EXCEPT_9,     "無効なTSS",
                "invalid TSS" )
pick( MSG_EXCEPT_10,        "セグメントが(メモリ上に)ありません",
                "segment not present" )
pick( MSG_EXCEPT_11,        "スタック例外",
                "stack exception" )
pick( MSG_EXCEPT_12,        "一般保護フォールト",
                "general protection fault" )
pick( MSG_EXCEPT_13,        "ページ・フォールト",
                "page fault" )
pick( MSG_EXCEPT_14,        "コプロセッサ・エラー",
                "coprocessor error" )
#endif
