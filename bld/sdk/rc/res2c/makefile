compiler = $(comp386)

.before
        set include=$(lang_root)\h;$(watcomdir)os220_h;$(watcomdir)h$(extra_include)
        @set path=$(os2_tools_root);$(%path)

.extensions:
.extensions: .exe .obj .c

c_opts = /d1/w4/zq/ox/mf
.c.obj:
        $(compiler) $(c_opts)$(app) $[@

res2c.exe : res2c.obj loadchar.obj loadtype.obj
        $(linker) sys os2v2 debug all f res2c, loadtype, loadchar

res2c.obj : res2c.c
        $(comp386) res2c.c /oax/d1+/w4/zq/bt=os2

loadchar.obj : loadchar.c
        $(comp386) loadchar.c /oax/d1+/w4/zq/bt=os2

loadtype.obj : loadtype.c
        $(comp386) loadtype.c /oax/d1+/w4/zq/bt=os2

