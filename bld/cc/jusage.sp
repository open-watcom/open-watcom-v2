::
:: Make sure to make changes in ALL the sections affected
::
:segment Taxp
使用方法: wccaxp [options] ファイル [options]
:elsesegment T386
使用方法: wcc386 [options] file [options]
:elsesegment
使用方法: wcc [options] file [options]
:endsegment
::
Options:
:segment !OS_qnx
	    ( /option is also accepted )
:endsegment
:segment T386
-3r	      386命令をレジスタ引数受渡し規則でコード生成
-3s	      386命令をスタック引数受渡し規則でコード生成
-4r	      486命令タイミングとレジスタ引数受渡し規則でコード生成
-4s	      486命令タイミングとスタック引数受渡し規則でコード生成
-5r	      Pentium命令タイミングとレジスタ引数受渡し規則でコード生成
-5s	      Pentium命令タイミングとスタック引数受渡し規則でコード生成
-6r	      Pentium Pro命令タイミングとレジスタ引数受渡し規則でコード生成
-6s	      Pentium Pro命令タイミングとスタック引数受渡し規則でコード生成
:elsesegment Ti86
-0	      8086命令
-1	      80186命令
-2	      80286命令
-3	      386用最適化
-4	      486用最適化
-5	      Pentium用最適化
-6	      Pentium Pro用最適化
-7	      インライン80x87命令
:endsegment
-aa	      ローカルの集合体または共用体に対する非定数の初期化を許します
-ai	      スタティックの初期化への型チェックを行いません
-aq	      const/volatileに対する修飾子不一致の警告を出しません
:segment Taxp
-as	      2バイト整数が整列されるものと仮定します
:endsegment
-bd	      作成ターゲットはダイナミック・リンク・ライブラリ(DLL)
-bm	      作成ターゲットはマルチ・スレッド環境
:segment T386 | Taxp
-br	      dll実行時ライブラリを使用して作成します
:endsegment
-bt=<id>      作成ターゲットはオペレーティング・システム<id>用
-bw	      作成ターゲットはデフォルト・ウィンドウ・アプリケーション
-d<id>[=text] ソースの先頭に#define <id> [text]があるものとして処理します。
-d0	      デバッグ情報なし
-d1{+}	      行番号デバッグ情報
-d2	      完全シンボリックデバッグ情報
-d3	      参照されていない型名を含む完全シンボリックデバッグ情報
-d+	      拡張した-dマクロ定義を許容します
-db	      ブラウザ用情報を生成します
-e=<num>      エラーメッセージ数の上限を設定します
:segment HIDDEN
-ec	      コード・カバレジ・ギアを生成します
:endsegment
:segment T386 | Ti86
-ee	      エピローグ・フック・ルーチンを呼びます
-ef	      エラーおよび警告メッセージで完全パス名を使用します
:endsegment
-ei	      列挙型を強制的に少なくとも'int'と同じ長さにします
-em	      列挙型のベース型として強制的に最小の整数型を使用します
:segment T386 | Ti86
-en	      コード・セグメントにルーチン名を出力します
-ep[=<num>]   <num>バイトのスタックを確保するプロローグ・フック・ルーチンをコールします
:endsegment
-eq	      エラーメッセージを表示しません(ファイルには書き込まれます)
:segment T386
-et	      P5プロファイリング
-ez	      PharLap EZ-OMFオブジェクト・ファイルを生成します
:endsegment
-fh[=<file>]  プリ・コンパイル・ヘッダーを使用します
-fhq[=<file>] プリ・コンパイル・ヘッダー使用メッセージを抑止します
-fi=<file>    <file>を強制的にインクルードします
-fo[=<file>]  オブジェクトまたはプリプロセッサ出力のファイル名を指定します
-fr[=<file>]  エラーファイル名を指定します
:segment T386 | Ti86
-fp2	      287浮動小数点コードを生成します
-fp3	      387浮動小数点コードを生成します
-fp5	      浮動小数点コードをPentium用に最適化します
-fp6	      浮動小数点コードをPentium Pro用に最適化します
-fpc	      浮動小数点ライブラリの呼出しを生成します
-fpi	      エミュレーションによるインライン80x87命令を生成します
-fpi87	      インライン80x87命令を生成します
-fpr	      旧バージョンのコンパイラと互換性のある80x87コードを生成します
-g=<id>       コードグループ名を設定します
:endsegment
:segment T386 | Ti86
-hc	      Codeviewデバッグ情報を生成します
-hd	      DWARFデバッグ情報を生成します
-hw	      Watcomデバッグ情報を生成します
:elsesegment
-hc	      Codeviewデバッグ情報を生成します
-hd	      DWARFデバッグ情報を生成します
:endsegment
-i=<path>     インクルード･パスを追加します
-j	      デフォルトのcharを符号なしから符号付きに変更します
:segment T386
-mc	      コンパクト･メモリ･モデル(スモール･コード/ラージ･データ)
-mf	      フラット･メモリ･モデル(ｽﾓｰﾙ･ｺｰﾄﾞ/CS=DS=SS=ESを仮定したｽﾓｰﾙ･ﾃﾞｰﾀ)
-ml	      ラージ･メモリ･モデル(ラージ･コード/ラージ･データ)
-mm	      ミディアム･メモリ･モデル(ラージ･コード/スモール･データ)
-ms	      スモール･メモリ･モデル(スモール･コード/スモール･データ)
:elsesegment Ti86
-mc	      コンパクト･メモリ･モデル(スモール･コード/ラージ･データ)
-mf	      フラット･メモリ･モデル(ｽﾓｰﾙ･ｺｰﾄﾞ/CS=DS=SS=ESを仮定したｽﾓｰﾙ･ﾃﾞｰﾀ)
-mh	      ヒュージ･メモリ･モデル(ラージ･コード/ヒュージ･データ)
-ml	      ラージ･メモリ･モデル(ラージ･コード/ラージ･データ)
-mm	      ミディアム･メモリ･モデル(ラージ･コード/スモール･データ)
-ms	      スモール･メモリ･モデル(スモール･コード/スモール･データ)
:endsegment
:segment T386 | Ti86
-nc=<id>      コード･クラスの名前を設定します
-nd=<id>      データ･セグメントの名前を設定します
:endsegment
-nm=<file>    モジュールの名前を設定します
:segment T386 | Ti86
-nt=<id>      テキスト･セグメントの名前を設定します
:endsegment
:segment Taxp
-o{a,b,d,e,h,i,k,l,n,o,r,s,t,u,x,z} 最適化を制御します
:elsesegment
-o{a,b,c,d,e,f[+],h.i,k,l,m,n,o,p,r,s,t,u,x,z} 最適化を制御します
:endsegment
  a	      -> エイリアスの制約を緩和します
  b	      -> 分岐予測にそったコードを生成します
:segment T386 | Ti86
  c	      -> <call followed by return>から<jump>の最適化を無効にします
:endsegment
  d	      -> すべての最適化を無効にします
  e[=<num>]   -> ユーザ関数をインライン展開します(<num>は最大ｻｲｽﾞを制御します)
:segment T386 | Ti86
  f	      -> 必要に応じてトレース可能なスタック･フレームを生成します
  f+	      -> 常にトレース可能なスタック･フレームを生成します
:endsegment
  h	      -> 最適化を徹底します(コンパイル時間は長くなります)
  i	      -> 組込み関数をインライン展開します
  k	      -> レジスタ退避をフローグラフに入れます
  l	      -> ループ最適化を可能にします
  l+	      -> ループ・アンローリングでループ最適化を可能にします
:segment T386 | Ti86
  m	      -> 算術関数をインラインの80x87コードで展開して生成します
:endsegment
  n	      -> 数値的にやや不正確になるがより高速な最適化を可能にします
  o	      -> メモリが足りなくなってもコンパイルを継続します
  p	      -> 一貫した浮動小数点計算の結果を生成します
  r	      -> 最適なパイプラインを使用するために命令を並べ替えます
  s	      -> 実行時間よりコードサイズの最適化を優先します
  t	      -> コードサイズより実行時間の最適化を優先します
  u	      -> すべての関数はそれぞれ固有のアドレスを必ず持ちます
:segment Taxp
  x	      -> -obiler -sと同等
:elsesegment
  x	      -> -obmiler -sと同等
:endsegment
  z	      -> NULLは、ターゲット環境内の有効なメモリを指します
-p{c,l,w=<num>} 前処理のみを実行します
  c	      -> コメントを含みます
  l	      -> #line擬似命令を挿入します
  w=<num>     -> 出力行を<num>桁で折り返します. 0は折り返しません.
:segment T386 | Ti86
-r	      関数呼び出しの前後でセグメントレジスタを退避/リストアします
-ri	      charおよびshortの戻り値をintと見なします
:endsegment
-s	      スタックオーバフロー・チェックを削除します
:segment T386 | Ti86
-sg	      スタックを増加する呼び出しを生成します
-st	      まず最初にSSを通してスタック・タッチします
:endsegment
:segment Taxp
-si	      スタック･フレーム･メモリをあるパターンで初期化します
:endsegment
-tp=<id>      #pragma on( <id> )を設定します
-u<id>	      定義済みマクロ<id>を未定義にします
-v	      .defファイルに関数宣言を出力します
:segment T386 | Ti86 | Taxp
-vcap	      VC++ 互換性: 引数リストの中でallocaを使用できます
:endsegment
:segment T386 & HIDDEN
-xgv	      インデックス付きグローバル変数にします
:endsegment
:segment Taxp & HIDDEN
-xd	      デフォルト例外ハンドラを使用します
:endsegment
-w=<num>      警告レベル番号を指定します
-wcd=<num>    警告制御: 警告メッセージ<num>を禁止します
-wce=<num>    警告制御: 警告メッセージ<num>を許可します
-we	      すべての警告をエラーとして扱います
:segment Ti86
-wo	      オーバーレイされたコードに関する問題を警告します
:endsegment
-za	      拡張機能を禁止します(つまり, ISO/ANSI C++飲み受け付けます)
-zc	      place const data into the code segment
:segment T386 | Ti86
-zdf	      DSを浮動にします. つまり, DGROUPに固定しません
-zdp	      DSをDGROUPに固定します
:endsegment
:segment T386
-zdl	      DGROUPから直接DSにロードします
:endsegment
-ze	      拡張機能を許可します(つまり, near, far, export, 等)
:segment T386 | Ti86
-zff	      FSを浮動にします. 1つのセグメントに固定しません
-zfp	      FSを1つのセグメントに固定します
-zgf	      GSを浮動にします. 1つのセグメントに固定しません
-zgp	      GSを1つのセグメントに固定します
:endsegment
-zg	      基本型を使用した関数プロトタイプを生成します
-zk0	      2バイト文字サポート: 日本語
-zk0u	      2バイト漢字をUnicodeに変換します
-zk1	      2バイト文字サポート: 中国語/台湾語
-zk2	      2バイト文字サポート: 韓国語
-zkl	      2バイト文字サポート: ローカル言語
-zku=<num>    指定したコードページのUNICODE変換テーブルをロードします
-zl	      デフォルト･ライブラリ情報を削除します
-zld	      ファイル依存情報を削除します
-zm	      各関数を別のセグメントに入れます
-zp=<num>     構造体メンバーを{1,2,4,8,16}に整列してパックします
-zpw	      構造体にパディングが追加されたときに警告します
:segment Taxp
-zps	      常に構造体をqword境界に整列します
:endsegment
-zq	      無メッセージモードで動作します
-zs	      構文チェックのみを行います
-zt[=<num>]   farデータ敷居値の設定(敷居値より大きいオブジェクトをfarメモリに)
:segment T386 | Ti86
-zu	      SS != DGROUP (スタックがデータセグメントにあると仮定しません)
-zw	      Microsoft Windows用のコードを生成します
:endsegment
:segment Ti86
-zW	      より効果的なMicrosoft Windowsエントリコード列を生成します
:endsegment
