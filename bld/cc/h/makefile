#pmake/50: build headers cpu_386 cpu_axp

!include cproj.mif

all: weights.gh keywords.gh .symbolic
        @%null

weights.gh keywords.gh : c.key pc.key seh.key dummy.key
        $(bld_bin)\findhash -q  c.key pc.key seh.key dummy.key

clean: .SYMBOLIC
        @if exist *.gh del *.gh
        @if exist cerror.gml del cerror.gml
