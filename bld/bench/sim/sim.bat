gcc -DUNIX -O2 -fomit-frame-pointer sim.c -o sim
strip sim
echo "--- done with compiling and stripping ---"
echo "--- sim 8 tob.38-44 liv.42-48 ---"
sim 8 tob.38-44 liv.42-48

