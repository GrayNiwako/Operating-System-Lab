#!/bin/sh

touch output.txt
echo "TextFile output.txt created successed"

find /bin -name 'b*' | grep '/bin/b*' | xargs ls -l | awk '{printf "%s %s %s\n", substr($9,6), $3, $1}' | sort -k1,1 -o output.txt
echo "Information written successed"

chmod 744 output.txt
echo "Authority set successed"
