pick( ERR_DISK_FULL,	    "重大なエラー:ディスクがいっぱいです",
			    "Fatal Error: Disk full." )
pick( ERR_SAMPLES_LOST,	    "エラー:サンプリング・データの一部が失われました",
			    "Error: some samples have been lost" )
pick( ERR_SAMPLE_TRUNCATE,  "エラー:ディスクの空き領域が不足です.サンプルファイルを切り詰めます ",
			    "Error: Out of disk space. Sample file truncated" )
pick( MSG_EXPECTING,	    "'=' または '#' があるべき箇所に次のコードがありました: ",
			    "Expecting either a '=' or a '#' instead found: " )
pick( MSG_INVALID_CHAR,	    "数字の後に無効な文字があります",
			    "Invalid character after number" )
pick( MSG_OUT_OF_RANGE,	    "数字が有効範囲内にありません",
			    "number is outside of its allowable range" )
pick( MSG_SAMPLE_BUFF,	    "サンプリング用バッファのメモリが確保できません",
			    "Can't allocate sample buffer" )
pick( MSG_CALLGRAPH_BUFF,   "コールグラフ用バッファのメモリを確保できません",
			    "Can't allocate callgraph buffer" )
pick( MSG_VERSION,	    "このバージョンのオペレーティング・システムの上では動作できません",
			    "Can't work with this version of the operating system" )
pick( MSG_SAMPLE_FILE,	    "サンプルファイルをオープンできません",
			    "Can't open sample file" )
pick( MSG_PROGRAM,	    "プログラムが見つかりません: ",
			    "Can't find program: " )
pick( MSG_BUFF_SMALL,	    "タイマー間隔に対してバッファが小さすぎます",
			    "Buffer size too small for timer rate" )
pick( MSG_INVALID_OPTION,   "無効なオプション: ",
			    "invalid option: " )

pick( MSG_USAGE_LN_1,	    "使用方法: wsample [options] program [program arguments]\n",
			    "Usage: wsample [options] program [program arguments]\n" )
pick( MSG_USAGE_LN_2,	    "オプション:\n",
			    "Options:\n" )
#ifndef WIN
pick( MSG_USAGE_LN_3,	    "    /b=<size> サンプリング用バッファサイズ (ﾃﾞﾌｫﾙﾄ: 63)\n",
			    "    /b=<size> specify the buffer size for samples (default: 63)\n" )
#endif
pick( MSG_USAGE_LN_4,	    "    /c        コールグラフ情報をサンプルファイルに含めます\n",
			    "    /c        callgraph information is to be included in the sample file\n" )
pick( MSG_USAGE_LN_5,	    "    /f=<file> サンプルファイルのファイル名\n",
			    "    /f=<file> specify the output filename for the sample file\n" )

pick( MSG_OPTIONS_1,	    "    /r=<rate> サンプリング間隔\n",
			    "    /r=<rate> specify the sampling rate\n" )
#ifdef DOS      /* messages in sampdos */
pick( MSG_OPTIONS_2,	    "    /i=<intr> サンプリングを行う割込み番号\n",
			    "    /i=<intr> specify an interrupt number for assigning samples\n" )
pick( MSG_OPTIONS_3,	    "    /d        DOS割込みの監視の禁止\n",
			    "    /d        disable assignment of DOS interrupts to application\n" )
#endif

#if defined(OS2) || defined(OS22)      /* messages in sampos2 and sampos22 */
pick( MSG_OPTIONS_2,	    "    /s        新しいセッションでのプログラムの開始\n",
			    "    /s        start the program in a new session\n" )
#endif

#ifndef WIN
pick( MSG_USAGE_LN_6,	    "\n    <size>    は 1 から 63 までの範囲です (ﾃﾞﾌｫﾙﾄ: 63 kb)\n",
			    "\n    <size>    is a number in the range 1 to 63 (default: 63 kb)\n" )
#endif

#ifdef DOS      /* messages in sampdos */
pick( MSG_EXPLAIN_1,	    "    <rate>    はサンプリング間のおよその時間間隔(ﾐﾘ秒単位)を\n",
			    "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,	    "              指定する数値(範囲:1から55まで)です (ﾃﾞﾌｫﾙﾄ: 55)\n",
			    "              time interval (in milliseconds) between samples (default: 55)\n" )
pick( MSG_EXPLAIN_3,	    "    <intr>    は16進数(範囲:20からffまで)で、以下の特別な方法で処理する\n",
			    "    <intr>    is a hex (base 16) number (range 20 to ff) specifying\n" )
pick( MSG_EXPLAIN_4,	    "              割込み番号です - \n",
			    "              an interrupt number to treat in a special manner - \n" )
pick( MSG_EXPLAIN_5,	    "              ソフトウェア割込み実行中のサンプリングは、割込みを発生した命令に\n",
			    "              any samples during the execution of the software interrupt\n" )
pick( MSG_EXPLAIN_6,	    "              割り当てます\n",
			    "              are assigned to the instruction that invoked the interrupt\n" )
pick( MSG_EXPLAIN_7,	    "              (ﾃﾞﾌｫﾙﾄ: DOS 割込み 21 がﾓﾆﾀｰされています)\n",
			    "              (default: the DOS interrupt 21 is monitored)\n" )
#endif

#if defined(OS2) || defined(OS22)      /* messages in sampos2 and sampos22 */
pick( MSG_EXPLAIN_1,	    "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
			    "    <rate>    is a number (range 27 to 1000) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,	    "              指定する数値 (範囲:27 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
			    "              time interval (in milliseconds) between samples (default: 55)\n" )
#endif

#if defined(PLS) || defined(NLM)      /* messages in samppls and sampnlm */
pick( MSG_EXPLAIN_1,	    "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
			    "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,	    "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
			    "              time interval (in milliseconds) between samples (default: 55)\n" )
pick( MSG_EXPLAIN_3,	    "              注意: このスイッチの使用には注意してください.プログラム実行中の\n",
			    "              NB: use this switch with caution. It will cause the time\n" )
pick( MSG_EXPLAIN_4,	    "                  時間が正しくなくなります.また、値が小さすぎると、遅いシステム\n",
			    "                  to be incorrect during program execution. Also, rates\n" )
pick( MSG_EXPLAIN_5,	    "                  の場合、クラッシュする可能性があります.\n",
			    "                  which are too small, may crash slower systems.\n" )
#endif

#if defined(ECS) || defined(RSI)      /* messages in samprsi and sampecs */
pick( MSG_EXPLAIN_1,	    "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
			    "    <rate>    is a number (range 1 to 55) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,	    "              指定する数値 (範囲:1 から 55まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
			    "              time interval (in milliseconds) between samples (default: 55)\n" )
#endif

#ifdef WIN      /* messages in sampwin */
pick( MSG_EXPLAIN_1,	    "    <rate>    はサンプリング間のおよその時間間隔 (単位:ﾐﾘ秒) を\n",
			    "    <rate>    is a number (range 1 to 1000) specifying the approximate\n" )
pick( MSG_EXPLAIN_2,	    "              指定する数値 (範囲:1 から 1000まで) です (ﾃﾞﾌｫﾙﾄ: 55)\n",
			    "              time interval (in milliseconds) between samples (default: 55)\n" )
#endif

pick( MSG_USAGE_LN_7,	    "\n   オプションの中では '=' の代りに '#' を使ってください (例 /b#10)\n",
			    "\n   '#' may be used instead of '=' in options (eg. /b#10)\n" )
 
#ifdef DOS      /* messages in sampdos */
pick( MSG_SAMPLE_1,	    "プログラムの中のセクションが多すぎます",
			    "Too many sections in program" )
pick( MSG_SAMPLE_2,	    "割込み 34 から 3d は 80x87 エミュレーションのために予約されており、モニターできません",
			    "Interrupts 34 to 3d are reserved for 80x87 emulation and cannot be monitored" )
pick( MSG_SAMPLE_3,	    "9個以上の割込みはモニターできません",
			    "Cannot monitor more than eight interrupts" )
#endif

#ifdef OS2      /* messages in sampos2 */
pick( MSG_SAMPLE_1,	    "実行中: ",
			    "Executing: " )
pick( MSG_SAMPLE_2,	    "内部エラー: ",
			    "internal error: " )
pick( MSG_SAMPLE_3,	    "ファイルを実行できません",
			    "Cannot execute file" )
pick( MSG_SAMPLE_4,	    "新しいスレッドを作れません",
			    "Cannot create a new thread" )
pick( MSG_SAMPLE_5,	    "スリーパ・スレッドの優先順位を上げられません",
			    "Cannot boost sleeper thread priority" )
pick( MSG_SAMPLE_6,	    "記法が間違っています",
			    "Unexpected notification" )
pick( MSG_SAMPLE_7,	    "DosPTrace( GO ) が失敗しました",
			    "DosPTrace( GO ) failed" )
pick( MSG_SAMPLE_8,	    "OS/2 2.x 用サンプラが見つかりません",
			    "Cannot find OS/2 2.x sampler" )
pick( MSG_SAMPLE_9,	    "OS/2 2.x 用サンプラを開始できません",
			    "Cannot start OS/2 2.x sampler" )
#endif

#ifdef OS22     /* messages in sampos22 */
pick( MSG_SAMPLE_1,	    "実行中: ",
			    "Executing: " )
pick( MSG_SAMPLE_2,	    "内部エラー: ",
			    "internal error: " )
pick( MSG_SAMPLE_3,	    "ファイルを実行できません",
			    "Cannot execute file" )
pick( MSG_SAMPLE_4,	    "新しいスレッドを作れません",
			    "Cannot create a new thread" )
pick( MSG_SAMPLE_5,	    "スリーパ・スレッドの優先順位を上げられません",
			    "Cannot boost sleeper thread priority" )
pick( MSG_SAMPLE_6,	    "記法が間違っています",
			    "Unexpected notification" )
pick( MSG_SAMPLE_7,	    "DosDebug コールが失敗しました",
			    "DosDebug call failed" )
pick( MSG_SAMPLE_8,	    "DosDebug( STOP ) が失敗しました",
			    "DosDebug( STOP ) failed" )
pick( MSG_SAMPLE_9,	    "プロセスに接触できませんでした",
			    "Unable to attach to process" )
pick( MSG_SAMPLE_10,	    "CPU 例外が報告されています",
			    "CPU exception reported" )
#endif
        
#ifdef PLS      /* messages in samppls */
pick( MSG_SAMPLE_1,	    "内部エラー",
			    "Internal error" )
pick( MSG_SAMPLE_2,	    "ロードできません: ",
			    "Unable to load: " )
pick( MSG_SAMPLE_3,	    "タスク例外: ",
			    "Task Exception: " )
pick( MSG_SAMPLE_4,	    "予期しない例外: ",
			    "Unexpected Exception: " )
#endif

#ifdef NLM      /* messages in sampnlm */
pick( MSG_SAMPLE_1,	    "エラー! 実行できません: ",
			    "Error! Cannot execute: " )
#endif

#ifdef ECS      /* messages in sampecs */
pick( MSG_SAMPLE_1,	    "未知のタスク終了コード",
			    "Unknown task termination code" )
pick( MSG_SAMPLE_2,	    "タスク例外: ",
			    "Task Exception: " )
#endif

#ifdef RSI      /* messages in samprsi */
pick( MSG_SAMPLE_1,	    "タスク例外: ",
			    "Task Exception: " )
pick( MSG_SAMPLE_2,	    "ロードできません: ",
			    "Unable to load: " )
#endif

#ifdef WIN      /* messages in sampwin */
pick( MSG_SAMPLE_1,	    "モジュールが見つかりません!",
			    "Couldn't find any modules!" )
pick( MSG_SAMPLE_2,	    "割込みのフックに失敗しました",
			    "Failed to hook interrupts" )
pick( MSG_SAMPLE_3,	    "フックの通知を受けるのに失敗しました",
			    "Failed to get notify hook" )
pick( MSG_SAMPLE_4,	    "サンプリングを開始できません!",
			    "Could not start sampling!" )
pick( MSG_SAMPLE_5,	    "サンプルバッファをフラッシュしました\n",
			    "Sample buffer flushed\n" )
pick( MSG_SAMPLE_6,	    "内部エラー: %s\n",
			    "internal error: %s\n" )
pick( MSG_SAMPLE_7,	    "タイマ間隔を %d ﾐﾘ秒に設定しました\n",
			    "Timer rate set to %d ms\n" )
pick( MSG_SAMPLE_8,	    "他のどのようなモジュールがあるかを探索中...\n",
			    "Determining what other modules are present...\n" )
pick( MSG_SAMPLE_9,	    "ほかに %d 個のモジュールがあります\n",
			    "Found %d other modules\n" )
pick( MSG_SAMPLE_10,	    "WDEBUG.386を使用します\n",
			    "Will use WDEBUG.386\n" )
pick( MSG_SAMPLE_11,	    "実行終了, 全時間=%ld.%03d 秒\n",
			    "Execution finished, total time=%ld.%03d seconds\n" )
pick( MSG_SAMPLE_12,	    "サンプラ・ロード・エラー!",
			    "Error loading sampler!" )
pick( MSG_SAMPLE_13,	    "サンプラをクローズできません!\n",
			    "Cannot close sampler!\n" )
#endif

#if defined(RSI) || defined(ECS) || defined(PLS)
pick( MSG_EXCEPT_0,	    "", "" )
pick( MSG_EXCEPT_1,	    "除算オーバーフロー",
			    "divide overflow" )
pick( MSG_EXCEPT_2,	    "マスク不可能割込み(ﾉﾝﾏｽｶﾌﾞﾙ割込み,NMI)",
			    "non-maskable interrupt" )
pick( MSG_EXCEPT_3,	    "整数オーバーフロー",
			    "integer overflow" )
pick( MSG_EXCEPT_4,	    "境界検査",
			    "bounds check" )
pick( MSG_EXCEPT_5,	    "無効なオペコード",
			    "invalid opcode" )
pick( MSG_EXCEPT_6,	    "コプロセッサはありません",
			    "coprocessor not available" )
pick( MSG_EXCEPT_7,	    "二重フォールト",
			    "double fault" )
pick( MSG_EXCEPT_8,	    "コプロセッサ・セグメント・オーバーラン",
			    "coprocessor segment overrun" )
pick( MSG_EXCEPT_9,	    "無効なTSS",
			    "invalid TSS" )
pick( MSG_EXCEPT_10,	    "セグメントが(メモリ上に)ありません",
			    "segment not present" )
pick( MSG_EXCEPT_11,	    "スタック例外",
			    "stack exception" )
pick( MSG_EXCEPT_12,	    "一般保護フォールト",
			    "general protection fault" )
pick( MSG_EXCEPT_13,	    "ページ・フォールト",
			    "page fault" )
pick( MSG_EXCEPT_14,	    "コプロセッサ・エラー",
			    "coprocessor error" )
#endif

#ifdef ECS
pick( MSG_DBERROR_0,	    "", "" )
pick( MSG_DBERROR_1,	    "全て順調",
			    "Everything OK" )
pick( MSG_DBERROR_2,	    "間違った関数への引数",
			    "Bad argument to function" )
pick( MSG_DBERROR_3,	    "マシンは操作できない状態にあります",
			    "Machine in bad state for operation" )
pick( MSG_DBERROR_4,	    "無効なタスクID",
			    "Invalid task ID" )
pick( MSG_DBERROR_5,	    "無効な状態のタスク",
			    "Task in invalid state" )
pick( MSG_DBERROR_6,	    "拡張メモリが不足です",
			    "Not Enough Extended Memory" )
pick( MSG_DBERROR_7,	    "リアル(低位)メモリが不足です",
			    "Not Enough Real (low) Memory" )
pick( MSG_DBERROR_8,	    "一般的ブートの失敗",
			    "General Boot failure" )
pick( MSG_DBERROR_9,	    "カーネルを見つけられません",
			    "Can't find kernel" )
pick( MSG_DBERROR_10,	    "ハードウェア・エラー",
			    "Hardware failure" )
pick( MSG_DBERROR_11,	    "これ以上タスクはありません",
			    "No more tasks" )
pick( MSG_DBERROR_12,	    "実行可能モジュールが見つかりません",
			    "Can't Find Executable" )
pick( MSG_DBERROR_13,	    "実行可能モジュールをロードできません",
			    "Can't Load Executable" )
pick( MSG_DBERROR_14,	    "ファイル・ヘッダーの中に間違った記述があります",
			    "Bad signature in file header" )
pick( MSG_DBERROR_15,	    "タスクが終了しました",
			    "Task Terminated" )
pick( MSG_DBERROR_16,	    "タスクがデバッグ例外を受け取りました",
			    "Task got debug exception" )
pick( MSG_DBERROR_17,	    "タスクがブレイクポイントにぶつかりました",
			    "Task hit a breakpoint" )
pick( MSG_DBERROR_18,	    "タスクが一般保護フォールトを受け取りました",
			    "Task got general protection fault" )
pick( MSG_DBERROR_19,	    "タスクが例外を受け取りました",
			    "Task got an exception" )
pick( MSG_DBERROR_20,	    "タスクが設定されていない割込みを発行しました",
			    "Task issued an unclaimed interrupt" )
pick( MSG_DBERROR_21,	    "タスクがサポートされていないファンクションコールを発行しました",
			    "Task issued an unsupported function call" )
pick( MSG_DBERROR_22,	    "タスクが未定義のリアルモード・プロシジャ・コールを発行しました",
			    "Task issued an undefine real proc call" )
pick( MSG_DBERROR_23,	    "RCP バッファの長さを越えました",
			    "RCP buffer length exceeded" )
pick( MSG_DBERROR_24,	    "間違ったセグメント・セレクタ",
			    "Bad segment selector" )
pick( MSG_DBERROR_25,	    "オフセットが大きすぎます",
			    "Offset too big" )
pick( MSG_DBERROR_26,	    "OS/286/386 は 8088/86上では実行できません",
			    "can't run OS/286/386 on 8088/86" )
#endif
