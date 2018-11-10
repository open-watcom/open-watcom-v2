# HDR Builder Control file
# ========================

set PROJNAME=hdr

set PROJDIR=<CWD>

[ INCLUDE <OWROOT>/build/prolog.ctl ]

[ INCLUDE <OWROOT>/build/defrule.ctl ]

[ BLOCK <BLDRULE> rel ]
#======================
    cdsay <PROJDIR>

[ BLOCK <BLDRULE> rel cprel ]
#============================
    <CCCMD> dos/*.h*           <OWRELROOT>/h/
    <CCCMD> dos/*.cnv          <OWRELROOT>/h/
    <CCCMD> dos/sys/*          <OWRELROOT>/h/sys/
    <CCCMD> dos/algorith       <OWRELROOT>/h/
    <CCCMD> dos/array          <OWRELROOT>/h/
    <CCCMD> dos/atomic         <OWRELROOT>/h/
    <CCCMD> dos/bitset         <OWRELROOT>/h/
    <CCCMD> dos/cassert        <OWRELROOT>/h/
    <CCCMD> dos/cctype         <OWRELROOT>/h/
    <CCCMD> dos/cerrno         <OWRELROOT>/h/
    <CCCMD> dos/cfloat         <OWRELROOT>/h/
    <CCCMD> dos/chrono         <OWRELROOT>/h/
    <CCCMD> dos/cinttype       <OWRELROOT>/h/
    <CCCMD> dos/ciso646        <OWRELROOT>/h/
    <CCCMD> dos/climits        <OWRELROOT>/h/
    <CCCMD> dos/clocale        <OWRELROOT>/h/
    <CCCMD> dos/cmath          <OWRELROOT>/h/
    <CCCMD> dos/codecvt        <OWRELROOT>/h/
    <CCCMD> dos/complex        <OWRELROOT>/h/
    <CCCMD> dos/conditio       <OWRELROOT>/h/
    <CCCMD> dos/csetjmp        <OWRELROOT>/h/
    <CCCMD> dos/csignal        <OWRELROOT>/h/
    <CCCMD> dos/cstdarg        <OWRELROOT>/h/
    <CCCMD> dos/cstdbool       <OWRELROOT>/h/
    <CCCMD> dos/cstddef        <OWRELROOT>/h/
    <CCCMD> dos/cstdint        <OWRELROOT>/h/
    <CCCMD> dos/cstdio         <OWRELROOT>/h/
    <CCCMD> dos/cstdlib        <OWRELROOT>/h/
    <CCCMD> dos/cstring        <OWRELROOT>/h/
    <CCCMD> dos/ctime          <OWRELROOT>/h/
    <CCCMD> dos/cwchar         <OWRELROOT>/h/
    <CCCMD> dos/cwctype        <OWRELROOT>/h/
    <CCCMD> dos/deque          <OWRELROOT>/h/
    <CCCMD> dos/exceptio       <OWRELROOT>/h/
    <CCCMD> dos/forward_       <OWRELROOT>/h/
    <CCCMD> dos/fstream        <OWRELROOT>/h/
    <CCCMD> dos/function       <OWRELROOT>/h/
    <CCCMD> dos/future         <OWRELROOT>/h/
    <CCCMD> dos/initiali       <OWRELROOT>/h/
    <CCCMD> dos/iomanip        <OWRELROOT>/h/
    <CCCMD> dos/ios            <OWRELROOT>/h/
    <CCCMD> dos/iosfwd         <OWRELROOT>/h/
    <CCCMD> dos/iostream       <OWRELROOT>/h/
    <CCCMD> dos/istream        <OWRELROOT>/h/
    <CCCMD> dos/iterator       <OWRELROOT>/h/
    <CCCMD> dos/limits         <OWRELROOT>/h/
    <CCCMD> dos/list           <OWRELROOT>/h/
    <CCCMD> dos/locale         <OWRELROOT>/h/
    <CCCMD> dos/map            <OWRELROOT>/h/
    <CCCMD> dos/memory         <OWRELROOT>/h/
    <CCCMD> dos/mutex          <OWRELROOT>/h/
    <CCCMD> dos/new            <OWRELROOT>/h/
    <CCCMD> dos/numeric        <OWRELROOT>/h/
    <CCCMD> dos/ostream        <OWRELROOT>/h/
    <CCCMD> dos/queue          <OWRELROOT>/h/
    <CCCMD> dos/random         <OWRELROOT>/h/
    <CCCMD> dos/ratio          <OWRELROOT>/h/
    <CCCMD> dos/regex          <OWRELROOT>/h/
    <CCCMD> dos/set            <OWRELROOT>/h/
    <CCCMD> dos/sstream        <OWRELROOT>/h/
    <CCCMD> dos/stack          <OWRELROOT>/h/
    <CCCMD> dos/stdexcep       <OWRELROOT>/h/
    <CCCMD> dos/streambu       <OWRELROOT>/h/
    <CCCMD> dos/string         <OWRELROOT>/h/
    <CCCMD> dos/strstrea       <OWRELROOT>/h/
    <CCCMD> dos/system_e       <OWRELROOT>/h/
    <CCCMD> dos/thread         <OWRELROOT>/h/
    <CCCMD> dos/tuple          <OWRELROOT>/h/
    <CCCMD> dos/typeinde       <OWRELROOT>/h/
    <CCCMD> dos/typeinfo       <OWRELROOT>/h/
    <CCCMD> dos/type_tra       <OWRELROOT>/h/
    <CCCMD> dos/unorderm       <OWRELROOT>/h/
    <CCCMD> dos/unorders       <OWRELROOT>/h/
    <CCCMD> dos/utility        <OWRELROOT>/h/
    <CCCMD> dos/valarray       <OWRELROOT>/h/
    <CCCMD> dos/vector         <OWRELROOT>/h/

    <CCCMD> rdos/*.h*          <OWRELROOT>/rh/
    <CCCMD> rdos/*.cnv         <OWRELROOT>/rh/
    <CCCMD> rdos/sys/*         <OWRELROOT>/rh/sys/
    <CCCMD> rdos/algorith      <OWRELROOT>/rh/
    <CCCMD> rdos/array         <OWRELROOT>/rh/
    <CCCMD> rdos/atomic        <OWRELROOT>/rh/
    <CCCMD> rdos/bitset        <OWRELROOT>/rh/
    <CCCMD> rdos/cassert       <OWRELROOT>/rh/
    <CCCMD> rdos/cctype        <OWRELROOT>/rh/
    <CCCMD> rdos/cerrno        <OWRELROOT>/rh/
    <CCCMD> rdos/cfloat        <OWRELROOT>/rh/
    <CCCMD> rdos/chrono        <OWRELROOT>/rh/
    <CCCMD> rdos/cinttype      <OWRELROOT>/rh/
    <CCCMD> rdos/ciso646       <OWRELROOT>/rh/
    <CCCMD> rdos/climits       <OWRELROOT>/rh/
    <CCCMD> rdos/clocale       <OWRELROOT>/rh/
    <CCCMD> rdos/cmath         <OWRELROOT>/rh/
    <CCCMD> rdos/codecvt       <OWRELROOT>/rh/
    <CCCMD> rdos/complex       <OWRELROOT>/rh/
    <CCCMD> rdos/conditio      <OWRELROOT>/rh/
    <CCCMD> rdos/csetjmp       <OWRELROOT>/rh/
    <CCCMD> rdos/csignal       <OWRELROOT>/rh/
    <CCCMD> rdos/cstdarg       <OWRELROOT>/rh/
    <CCCMD> rdos/cstdbool      <OWRELROOT>/rh/
    <CCCMD> rdos/cstddef       <OWRELROOT>/rh/
    <CCCMD> rdos/cstdint       <OWRELROOT>/rh/
    <CCCMD> rdos/cstdio        <OWRELROOT>/rh/
    <CCCMD> rdos/cstdlib       <OWRELROOT>/rh/
    <CCCMD> rdos/cstring       <OWRELROOT>/rh/
    <CCCMD> rdos/ctime         <OWRELROOT>/rh/
    <CCCMD> rdos/cwchar        <OWRELROOT>/rh/
    <CCCMD> rdos/cwctype       <OWRELROOT>/rh/
    <CCCMD> rdos/deque         <OWRELROOT>/rh/
    <CCCMD> rdos/exceptio      <OWRELROOT>/rh/
    <CCCMD> rdos/forward_      <OWRELROOT>/rh/
    <CCCMD> rdos/fstream       <OWRELROOT>/rh/
    <CCCMD> rdos/function      <OWRELROOT>/rh/
    <CCCMD> rdos/future        <OWRELROOT>/rh/
    <CCCMD> rdos/initiali      <OWRELROOT>/rh/
    <CCCMD> rdos/iomanip       <OWRELROOT>/rh/
    <CCCMD> rdos/ios           <OWRELROOT>/rh/
    <CCCMD> rdos/iosfwd        <OWRELROOT>/rh/
    <CCCMD> rdos/iostream      <OWRELROOT>/rh/
    <CCCMD> rdos/istream       <OWRELROOT>/rh/
    <CCCMD> rdos/iterator      <OWRELROOT>/rh/
    <CCCMD> rdos/limits        <OWRELROOT>/rh/
    <CCCMD> rdos/list          <OWRELROOT>/rh/
    <CCCMD> rdos/locale        <OWRELROOT>/rh/
    <CCCMD> rdos/map           <OWRELROOT>/rh/
    <CCCMD> rdos/memory        <OWRELROOT>/rh/
    <CCCMD> rdos/mutex         <OWRELROOT>/rh/
    <CCCMD> rdos/new           <OWRELROOT>/rh/
    <CCCMD> rdos/numeric       <OWRELROOT>/rh/
    <CCCMD> rdos/ostream       <OWRELROOT>/rh/
    <CCCMD> rdos/queue         <OWRELROOT>/rh/
    <CCCMD> rdos/random        <OWRELROOT>/rh/
    <CCCMD> rdos/ratio         <OWRELROOT>/rh/
    <CCCMD> rdos/regex         <OWRELROOT>/rh/
    <CCCMD> rdos/set           <OWRELROOT>/rh/
    <CCCMD> rdos/sstream       <OWRELROOT>/rh/
    <CCCMD> rdos/stack         <OWRELROOT>/rh/
    <CCCMD> rdos/stdexcep      <OWRELROOT>/rh/
    <CCCMD> rdos/streambu      <OWRELROOT>/rh/
    <CCCMD> rdos/string        <OWRELROOT>/rh/
    <CCCMD> rdos/strstrea      <OWRELROOT>/rh/
    <CCCMD> rdos/system_e      <OWRELROOT>/rh/
    <CCCMD> rdos/thread        <OWRELROOT>/rh/
    <CCCMD> rdos/tuple         <OWRELROOT>/rh/
    <CCCMD> rdos/typeinde      <OWRELROOT>/rh/
    <CCCMD> rdos/typeinfo      <OWRELROOT>/rh/
    <CCCMD> rdos/type_tra      <OWRELROOT>/rh/
    <CCCMD> rdos/unorderm      <OWRELROOT>/rh/
    <CCCMD> rdos/unorders      <OWRELROOT>/rh/
    <CCCMD> rdos/utility       <OWRELROOT>/rh/
    <CCCMD> rdos/valarray      <OWRELROOT>/rh/
    <CCCMD> rdos/vector        <OWRELROOT>/rh/

    <CCCMD> linux/*.h*         <OWRELROOT>/lh/
    <CCCMD> linux/arpa/*       <OWRELROOT>/lh/arpa/
    <CCCMD> linux/sys/*        <OWRELROOT>/lh/sys/
#    <CCCMD> linux/net/*        <OWRELROOT>/lh/net/
    <CCCMD> linux/netinet/*    <OWRELROOT>/lh/netinet/
    <CCCMD> linux/algorith     <OWRELROOT>/lh/
    <CCCMD> linux/array        <OWRELROOT>/lh/
    <CCCMD> linux/atomic       <OWRELROOT>/lh/
    <CCCMD> linux/bitset       <OWRELROOT>/lh/
    <CCCMD> linux/cassert      <OWRELROOT>/lh/
    <CCCMD> linux/cctype       <OWRELROOT>/lh/
    <CCCMD> linux/cerrno       <OWRELROOT>/lh/
    <CCCMD> linux/cfloat       <OWRELROOT>/lh/
    <CCCMD> linux/chrono       <OWRELROOT>/lh/
    <CCCMD> linux/cinttype     <OWRELROOT>/lh/
    <CCCMD> linux/ciso646      <OWRELROOT>/lh/
    <CCCMD> linux/climits      <OWRELROOT>/lh/
    <CCCMD> linux/clocale      <OWRELROOT>/lh/
    <CCCMD> linux/cmath        <OWRELROOT>/lh/
    <CCCMD> linux/codecvt      <OWRELROOT>/lh/
    <CCCMD> linux/complex      <OWRELROOT>/lh/
    <CCCMD> linux/conditio     <OWRELROOT>/lh/
    <CCCMD> linux/csetjmp      <OWRELROOT>/lh/
    <CCCMD> linux/csignal      <OWRELROOT>/lh/
    <CCCMD> linux/cstdarg      <OWRELROOT>/lh/
    <CCCMD> linux/cstdbool     <OWRELROOT>/lh/
    <CCCMD> linux/cstddef      <OWRELROOT>/lh/
    <CCCMD> linux/cstdint      <OWRELROOT>/lh/
    <CCCMD> linux/cstdio       <OWRELROOT>/lh/
    <CCCMD> linux/cstdlib      <OWRELROOT>/lh/
    <CCCMD> linux/cstring      <OWRELROOT>/lh/
    <CCCMD> linux/ctime        <OWRELROOT>/lh/
    <CCCMD> linux/cwchar       <OWRELROOT>/lh/
    <CCCMD> linux/cwctype      <OWRELROOT>/lh/
    <CCCMD> linux/deque        <OWRELROOT>/lh/
    <CCCMD> linux/exceptio     <OWRELROOT>/lh/
    <CCCMD> linux/forward_     <OWRELROOT>/lh/
    <CCCMD> linux/fstream      <OWRELROOT>/lh/
    <CCCMD> linux/function     <OWRELROOT>/lh/
    <CCCMD> linux/future       <OWRELROOT>/lh/
    <CCCMD> linux/initiali     <OWRELROOT>/lh/
    <CCCMD> linux/iomanip      <OWRELROOT>/lh/
    <CCCMD> linux/ios          <OWRELROOT>/lh/
    <CCCMD> linux/iosfwd       <OWRELROOT>/lh/
    <CCCMD> linux/iostream     <OWRELROOT>/lh/
    <CCCMD> linux/istream      <OWRELROOT>/lh/
    <CCCMD> linux/iterator     <OWRELROOT>/lh/
    <CCCMD> linux/limits       <OWRELROOT>/lh/
    <CCCMD> linux/list         <OWRELROOT>/lh/
    <CCCMD> linux/locale       <OWRELROOT>/lh/
    <CCCMD> linux/map          <OWRELROOT>/lh/
    <CCCMD> linux/memory       <OWRELROOT>/lh/
    <CCCMD> linux/mutex        <OWRELROOT>/lh/
    <CCCMD> linux/new          <OWRELROOT>/lh/
    <CCCMD> linux/numeric      <OWRELROOT>/lh/
    <CCCMD> linux/ostream      <OWRELROOT>/lh/
    <CCCMD> linux/queue        <OWRELROOT>/lh/
    <CCCMD> linux/random       <OWRELROOT>/lh/
    <CCCMD> linux/ratio        <OWRELROOT>/lh/
    <CCCMD> linux/regex        <OWRELROOT>/lh/
    <CCCMD> linux/set          <OWRELROOT>/lh/
    <CCCMD> linux/sstream      <OWRELROOT>/lh/
    <CCCMD> linux/stack        <OWRELROOT>/lh/
    <CCCMD> linux/stdexcep     <OWRELROOT>/lh/
    <CCCMD> linux/streambu     <OWRELROOT>/lh/
    <CCCMD> linux/string       <OWRELROOT>/lh/
    <CCCMD> linux/strstrea     <OWRELROOT>/lh/
    <CCCMD> linux/system_e     <OWRELROOT>/lh/
    <CCCMD> linux/thread       <OWRELROOT>/lh/
    <CCCMD> linux/tuple        <OWRELROOT>/lh/
    <CCCMD> linux/typeinde     <OWRELROOT>/lh/
    <CCCMD> linux/typeinfo     <OWRELROOT>/lh/
    <CCCMD> linux/type_tra     <OWRELROOT>/lh/
    <CCCMD> linux/unorderm     <OWRELROOT>/lh/
    <CCCMD> linux/unorders     <OWRELROOT>/lh/
    <CCCMD> linux/utility      <OWRELROOT>/lh/
    <CCCMD> linux/valarray     <OWRELROOT>/lh/
    <CCCMD> linux/vector       <OWRELROOT>/lh/

    <CCCMD> linux/arch/i386/*       <OWRELROOT>/lh/arch/i386/
    <CCCMD> linux/arch/i386/sys/*   <OWRELROOT>/lh/arch/i386/sys/
    <CCCMD> linux/arch/mips/*       <OWRELROOT>/lh/arch/mips/
    <CCCMD> linux/arch/mips/sys/*   <OWRELROOT>/lh/arch/mips/sys/

    <CCCMD> qnx/*.h*           <OWRELROOT>/qh/
    <CCCMD> qnx/sys/*          <OWRELROOT>/qh/sys/

[ BLOCK . . ]

[ INCLUDE <OWROOT>/build/epilog.ctl ]
