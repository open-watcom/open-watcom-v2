f16test.exe : caller.obj callee.obj
        wlink sys os2v2 d a option map name f16test.exe option stack=66k file caller file callee

caller.obj : caller.c
        wcc386d -oaxt -d2 -zq  $<

callee.obj : callee.c
        wcc -d2 -zq -zl -ml $<
