echo (pass 2) processing %1
awkl -f special.awk -f inityp.awk -f pass2.awk %1 >%2
