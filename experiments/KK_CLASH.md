# Complementary chief transitions

An admissible equal-charge K x K clash demotes one K to D. The reference
tie-break preserves the smaller W address; the other source records that
address in parent and leader_w, preserving charge and affinity. Several
contacts select the smallest encountered eligible chief. D x D promotion
remains enabled, with equal charge as its only pair-specific eligibility
condition. Both rules retain the general active and positive-radius gates.

The legacy encounter uses chief_transition.h before its early returns. The
experimental FSM records a candidate per voxel, diffuses it through the six
spatial neighbors, and commits after diffusion. It does not write mirror or
remotely reassign the demoted chief's delegates. Consequently, unresolved
parents are measured explicitly; demotion alone does not prove membership
transfer or island stability.

Build with `experiments\build_coagulation_clash.bat`, then run
`build\coagulation_clash\coagulation_random.exe 1`. The L=9, W=243 experiment
uses 12 light frames, with inertia disabled and the same temporary overlap
perturbation as the previous runs. Results go to
`build/coagulation_clash/seed_1/`, preserving the older variants.

Audit with `python experiments/analyze_coagulation_random.py 1 --directory
build/coagulation_clash/seed_1`. The separate transition test covers all 64
charge words, reciprocal clashes, unequal-charge exclusion, and preservation
of charge and affinity. Its earlier K-persistence assertion has been replaced
by the explicitly requested K x K demotion.

Lower-W priority is an experimental deterministic choice. Neither an
address-independent outcome nor 9L stable islands follows from this choice.

## Completed seed-1 run

The 12-frame executable and independent forcing audit both exited with code
zero. The K counts from frames 2 through 12 were 8, 235, 16, 219, 30, 217,
31, 177, 59, 177, and 62. In frame 4, 219 chiefs were demoted. Every observed
K-to-D transition referenced a lower-W, equal-charge chief present in the
preceding encounter snapshot, and preserved affinity (clash_audit.json).

Only 20 artificial reemissions occurred, all in journey 5. The final sample
had 62 K, 181 D, 51 occupied centers, and 27 distinct chief centers. Eleven
delegates pointed to sources that were no longer K. No final group had three
constituents. Chief creation and removal are both active, but this short run
shows oscillation and unresolved membership rather than 81 stable islands.
Spatially occupied sites must not be counted as islands.

The remaining membership issue requires an explicit contact/message rule;
this change does not silently perform global parent-chain compression.
