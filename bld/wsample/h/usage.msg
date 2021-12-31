/* UTF-8 encoding, ¥ */

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
