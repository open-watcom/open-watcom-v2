gcc -DUNIX -O2 -fomit-frame-pointer queens.c -o queens
strip queens
echo "--- done compiling and stripping ---"
queens -c 14 >> queens.res
