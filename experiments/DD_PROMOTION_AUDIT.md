# Missing D x D promotion: implementation audit

The results below describe the D-promotion-only variant. The subsequent
[K x K clash correction](KK_CLASH.md) restores deliberate demotion of one
equal-charge chief; it supersedes the old K-persistence test. Generic
minimum merging remains removed, but an admissible clash now can demote K.

## Restored transition after author clarification

The author confirmed that equal charge is the only pair-specific eligibility
condition. chief_transition.h now implements unilateral promotion: in D x D
of equal charge, the lower intrinsic W address becomes K, with parent cleared
and leader_w set to itself. The other remains D with its existing parent.
Address ordering is an explicit winner choice, not a family or affinity gate.
No same/different-parent, spin, or spatial-separation test is added. Active
and positive-radius gates are applied by encounter.

The legacy generic min-merge was replaced by role-specific contact handling,
so it no longer demotes an existing K. The coagulation DD variant carries
promotion notifications through spatial OR diffusion and applies them after
ordinary S election. It does not require mutual movement matching or nonzero
attraction, so coincident delegates are eligible too. With multiple contacts
in one frozen-snapshot journey, a D is promoted if it is the lower address
in at least one eligible D x D contact; several promotions can occur in
parallel. No target count or population is imposed.

Build the variant with experiments/build_coagulation_dd.bat and run
build/coagulation_dd/coagulation_random.exe 1. Outputs are separate from the
min-only baseline. The focused regression build_dd_promotion_test.bat passes
all 64 charge words, equal/different parents, unequal affinities and spins,
different-charge exclusion, and (at that stage) protection against subsequent
K demotion. That last assertion is superseded by the clash correction above.
Historical inertia probes have not been revalidated with this role transition.

The sections below preserve the audit that motivated the restoration.

The restored legacy path also rejects zero-radius contacts before handling
roles. Its equal-charge D x D transition returns before older interaction
branches can overwrite the new chief. The reciprocal main-side visit handles
the other delegate; no partner draft write is needed for promotion.

The restored legacy reduced seed test completed two frames with exit code
zero: first K=0, D=0, S=243; then K=235, D=8, S=0, at one center. There were
no unresolved parent references. The experimental frozen-journey variant
also reaches K=235, D=8, in its third frame. Thus the excessive chief count
is not explained solely by the frozen contact snapshot. Both implementations
promote many initially coincident delegates under the restored condition.
This does not establish 9L islands; promotion is active, but population
organization remains unresolved. In the current random intervention schedule,
the burst occurs before the first forced reemission (journey 5).

The restored random-reemission run (seed 1) completed all 12 journeys with
exit code zero. K remained 235 and D remained 8 after journey 3. There were
59 imposed reemissions (20 in journey 5, 38 in journey 7, one in journey 8).
The final sample contained 88 occupied centers, no unresolved parents, and
no chief group of three constituents. The independent analyzer passed with
`python experiments/analyze_coagulation_random.py 1 --directory build/coagulation_dd/seed_1`.
The test confirms active promotion and c-driven perturbations, not the
formation of 81 islands. No additional promotion eligibility condition was
introduced to force the desired count.

The model author clarified that a D x D interaction can promote one delegate
to K. The recent min-only election does not implement this transition. Its
failure to generate additional chiefs must not be interpreted as a limitation
of the intended model or evidence against the 9L island hypothesis.

## Confirmed code changes

Commit 381c1b4 (Inertia corrections) inserted a generic equal-charge election
ahead of the specific encounter branches in src/model/interaction.cpp.
It combines source addresses and previous leaders by minimum, assigns K/D,
then classifies same-island body contacts as internal and returns before
the explicit D x D branch. Subsequent working-tree changes remove the affinity
gate and use chief identity for internal contacts. Equal-charge body contacts
are therefore intercepted before the specific D x D handling.

The experimental color_fsm.inc does not call the legacy encounter branches.
It instead propagates colorChiefCandidate: a D contributes its parent and a
K its own address. Min diffusion cannot create a new chief identity after all
S are exhausted. The explicit D x D promotion was not carried into that path.

The current legacy D x D branch itself contains repulsion/momentum exchange
for different parents, or spin_target/leader synchronization for the same
parent. It contains no explicit assignment of SourceKind::K.

## What the history did and did not establish

The audit traversed all 56 revisions of src/model/interaction.cpp reachable
through git log --all, representing six distinct file contents. No promotion
was found in their explicit D x D branches. The oldest available such branch
examined in commit 222631f already lacks that assignment. The previously
attached manuscript and the selected manuscript revisions describe D x D
cohesion/repulsion, but did not recover the missing promotion condition.

Thus 381c1b4 is a confirmed introduction point for the generic election and
early interception, not a demonstrated deletion commit for the original
promotion. The original implementation may be outside the history examined.
The current semantic omission is confirmed; the exact historical predicate
and winner-selection rule have not yet been recovered.

## Consequences for the ongoing experiment

The random-reemission seed-1 run completed 12 journeys. There were 20 forced
reemissions in journey 5 and 223 in journey 6. The source-center count reached
205 and ended at 185, while K remained 8, D remained 235, and no group had
three constituents. The independent audit verified the forcing schedule,
random sequence, actual c-driven destinations, and preserved identities.
These are results of the incomplete election variant.

The harness no longer rejects new chief identities: that assertion would
have incorrectly encoded the omission as a required invariant. Restoring
promotion requires ensuring that generic leader minimization neither prevents
the D x D branch nor immediately demotes its newly elected K. The activation
condition (same/different parent, spin, geometry) and unilateral tie-break must
be recovered or confirmed rather than invented during the restoration.
