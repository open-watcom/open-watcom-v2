gcc -O2 -DUNIX -DDP -DROLL clinpack.c -o clinpack_dpr
gcc -O2 -DUNIX -DDP -DUNROLL clinpack.c -o clinpack_dpu
gcc -O2 -DUNIX -DSP -DROLL clinpack.c -o clinpack_spr
gcc -O2 -DUNIX -DSP -DUNROLL clinpack.c -o clinpack_spu
strip clinpack_dpr
strip clinpack_dpu
strip clinpack_spr
strip clinpack_spu
clinpack_dpr >> clinpack.res
clinpack_dpu >> clinpack.res
clinpack_spr >> clinpack.res
clinpack_spu >> clinpack.res
