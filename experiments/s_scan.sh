#!/bin/bash
# s_scan.sh - S-scan for the S-averaged coupling (bypasses the S=16384 dial).
# For each L in {7,9} and each modulus S in the grid, records
#   S, conv_calls, conv_s2b   ->  realized r(S) = s2b/calls.
cd /e/alpha/experiments || exit 1

for L in 7 9; do
  : > sscan_L${L}.csv
  for S in $(seq 128 512 16384); do
    out=$(timeout 15 ./alpha_probe.exe "$L" 4 200 "$S" 12 scatter 2>/dev/null \
          | grep 'realized overlap throughput' | head -1)
    a=$(echo "$out" | sed -n 's/.*= \([0-9][0-9]*\)\/\([0-9][0-9]*\) = .*/\1/p')
    b=$(echo "$out" | sed -n 's/.*= [0-9][0-9]*\/\([0-9][0-9]*\) = .*/\1/p')
    if [ -n "$a" ] && [ -n "$b" ]; then
      echo "$S,$b,$a" >> sscan_L${L}.csv
    else
      echo "$S,-1,-1" >> sscan_L${L}.csv
    fi
  done
  echo "L=$L done: $(wc -l < sscan_L${L}.csv) points"
done
echo "S_SCAN_DONE"
