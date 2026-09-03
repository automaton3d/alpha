#!/bin/bash
# s_scan2.sh - extend the S-averaged coupling scan to L=11 (full grid) and
# L=13 (coarser grid, fewer frames to fit the budget).
cd /e/alpha/experiments || exit 1

# L=11: 200 frames, S=128..16384 step 512 (32 points)
: > sscan_L11.csv
for S in $(seq 128 512 16384); do
  out=$(timeout 60 ./alpha_probe.exe 11 4 200 "$S" 30 scatter 2>/dev/null \
        | grep 'realized overlap throughput' | head -1)
  a=$(echo "$out" | sed -n 's/.*= \([0-9][0-9]*\)\/\([0-9][0-9]*\) = .*/\1/p')
  b=$(echo "$out" | sed -n 's/.*= [0-9][0-9]*\/\([0-9][0-9]*\) = .*/\1/p')
  if [ -n "$a" ] && [ -n "$b" ]; then echo "$S,$b,$a" >> sscan_L11.csv
  else echo "$S,-1,-1" >> sscan_L11.csv; fi
done
echo "L=11 done: $(wc -l < sscan_L11.csv) points"

# L=13: 100 frames, S=128..16384 step 1024 (16 points)
: > sscan_L13.csv
for S in $(seq 128 1024 16384); do
  out=$(timeout 60 ./alpha_probe.exe 13 4 100 "$S" 30 scatter 2>/dev/null \
        | grep 'realized overlap throughput' | head -1)
  a=$(echo "$out" | sed -n 's/.*= \([0-9][0-9]*\)\/\([0-9][0-9]*\) = .*/\1/p')
  b=$(echo "$out" | sed -n 's/.*= [0-9][0-9]*\/\([0-9][0-9]*\) = .*/\1/p')
  if [ -n "$a" ] && [ -n "$b" ]; then echo "$S,$b,$a" >> sscan_L13.csv
  else echo "$S,-1,-1" >> sscan_L13.csv; fi
done
echo "L=13 done: $(wc -l < sscan_L13.csv) points"
echo "S_SCAN2_DONE"
