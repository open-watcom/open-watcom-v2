echo "#########################################" >> c4.res
echo "Sun HyperSPARC, 100 MHz, SunOS 5.4" >> c4.res
echo "gcc 2.7.1, gcc -DMSC -DSMALL -msupersparc -O3" >> c4.res
make together
strip c4
c4 < input >> c4.res
