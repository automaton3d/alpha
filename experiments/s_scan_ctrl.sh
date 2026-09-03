#!/bin/bash
# s_scan_ctrl.sh - frame-count control for L=13: 200 frames at 4 S values
# that are on the L13 grid (128 + 1024k).
cd /e/alpha/experiments || exit 1
: > sscan_L13_ctrl.csv
for S in 128 5248 10368 15488; do
  out=$(timeout 90 ./alpha_probe.exe 13 4 200 "$S" 60 scatter 2>/dev/null \
        | grep 'realized overlap throughput' | head -1)
  a=$(echo "$out" | sed -n 's/.*= \([0-9][0-9]*\)\/\([0-9][0-9]*\) = .*/\1/p')
  b=$(echo "$out" | sed -n 's/.*= [0-9][0-9]*\/\([0-9][0-9]*\) = .*/\1/p')
  if [ -n "$a" ] && [ -n "$b" ]; then echo "$S,$b,$a" >> sscan_L13_ctrl.csv
  else echo "$S,-1,-1" >> sscan_L13_ctrl.csv; fi
done
echo "L13_CTRL_DONE"
