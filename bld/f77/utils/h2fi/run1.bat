echo (pass 1) processing %1
echo # types from %1 >>%3
awkl -f special.awk -f %2 -f pass1.awk %1 >>%3
