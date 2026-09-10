# Graviton-mediated island repulsion through the orphan flux — design note

Status: **design** (no code yet).  Branch context: `polarization-broadcast-fix`
(one-shot bootstrap, first-arrival broadcast wave, `EM_FIRST_FSM`).  The EM
reordering experiment showed that direct body-shell contact does not produce a
directed equal-charge repulsion; this note designs the alternative that the
manuscript's speculative sections already name (manuscript.tex:794-796,
1229, 1374, 1382): a static coupling carried by **gravitons** guided by the
**orphan flux** between two particles.

## 1. Carrier consistency check (conclusion)

The recruited mediator must be the **R1 graviton**, not the R2 photon:

- R1 = all six bits complementary (`ca ^ cb == 0x3F`): operates in BOTH
  sectors (one Orbis, one Umbra half) and is **charge-neutral by construction**
  (manuscript.tex:491, 1229; canFormPair rule R1).
- R2 = same-sector complementary (`w1` equal): **sector-specific**; the pair
  carries two units of sector charge and only couples within one sector.
- Equal-charge islands (Q,Q) cannot synthesise an R2 photon (its halves are
  Q and the same-sector complement Q', which is absent), and charges are
  immutable in the reference dynamics.  The only recruitable carrier is a
  pre-existing vacuum pair that is globally neutral and sector-blind: the
  fully-complementary pair (g, g-bar) of any charge Q, i.e. R1.
- Orphan rule (manuscript.tex:1382): "Orphans (a=W) do not interact with
  photons but do interact with gravitons."  The static-interaction text
  (manuscript.tex:794) already starts the mechanism with "an orphan singleton
  from q1 aligns with a vacuum graviton".

Ressalvas to carry: graviton-ness is *necessary, not sufficient*.  Which half
(g or g-bar) engages the orphan and what sets the force sign (repel vs
attract) is not derivable from the charge algebra alone; it requires an
explicit orientation rule (pB/sB / m of the recruited drive pair).

## 2. Actors (prepared, production path, W >= 6)

- **Source S**: a bubble/body that reissues often enough to shed a
  **concentric orphan shell** (`a = W_USED`, ghost: affinity disabled,
  non-collapsing; manuscript.tex:616, 1374).  The shell is the "flux" that
  can engage vacuum pairs.
- **Mediator**: a free fully-complementary pair **G = (g, g-bar)** in the
  vacuum between S and T (a "vacuum graviton"), already present as a P source.
- **Target T**: a distant body of charge Q (equal to S's charge for the
  repulsion case under test).

## 3. Mechanism (to be defined as rules; not yet implemented)

1. **Orphan flux**: S's concentric orphan shells propagate outward after each
   reissue (existing orphan semantics, `a == W_USED`).
2. **Alignment**: when an active orphan cell of S's shell meets an active cell
   of the free pair G (same voxel, cross-layer), a *recruitment contact*
   occurs.  RULE TO DEFINE: gate (identity equal-charge? R1 complement pair?
   collapse?) and which half (g / g-bar) engages first.
3. **Guidance**: the pair is redirected along the flux toward T (needs a
   directed step; today free P pairs only expand and are consumed at radius).
4. **Affinity handover**: on reaching T, G acquires T's affinity and becomes a
   **drive pair of T** — this part exists: a free P pair that contacts a body
   island joins it and adopts its affinity/leader (interaction.cpp, P joins
   island path); subsequent internal P x K/D contacts transport T's
   constituents in G's m direction (drive-pair inertia, measured 0.25/0.75
   cells/frame; CoM conserved).
5. **Reciprocity**: the same happens from T toward S ("The same occurs in
   reverse", manuscript.tex:794).  With symmetric, CoM-conserving drive
   transport the net static interaction is symmetric in the mean.

## 4. Open design decisions (checklist)

- [ ] Recruitment gate for orphan x vacuum-pair (which rule fires first).
- [ ] Which half of R1 engages; is the choice orientation-dependent (pB/sB)?
- [ ] Force sign rule: repel for equal charge <=> the recruited drive pairs on
      S and T point away from each other (m antiparallel), so internal
      transport separates the bodies.
- [ ] Distance dependence: guidance acts while orphan flux overlaps the pair
      (no action at a distance beyond the shell reach; "flux" bounds the law).
- [ ] Charge-sign dependence: same charge repel, opposite attract?  (Opposite
      charges could instead *merge* the recruited pair into annihilation -
      R1 complement halves with opposite charges -> collapse, not repel.)

## 5. First experiment (prepared three-body, production path)

Setup: body S (equal charge 0x08, tube/cube W >= 6), body T (same 0x08) at
SEP, and a free fully-complementary P pair G (0x00/0x3F) centred between
them; S reissued periodically to shed orphan shells.  Boot macros
(POLAR_BOOTSTRAP_ADDRESS + POLAR_BROADCAST_WAVE) optional; EM_FIRST_FSM off
for the pure dressing test.

Measure: does T (and S) drift apart (d grows) via drive-pair transport
without core contact?  Per-frame: P affinity flips (pair joins T), source
kinds, d(t), CoM, enc_* counters, pair consumption events.

Controls: (a) no mediator G (merge baseline, d -> 0); (b) mediator but no
orphan flux (S without reissues); (c) orientations swapped.

Success: d grows monotonically while both bodies stay S/K+D (no identity
merge), CoM conserved in the mean, reproducible across seeds/axes.  If it
fails, the missing piece is the directed guidance/sign rule, not the carrier.

## 6. Code mapping (exists vs to add)

Exists: orphan cell semantics and orphan expansion (`a == W_USED`);
free P pairs (expand, consumed at max radius); P joins contacted island and
acquires affinity (interaction.cpp); drive-pair P x K/D transport with
CoM conservation (resolveInternalContacts / propeller).

To add (macro-guarded candidate: ORPHAN_GUIDANCE_FSM):
- orphan-shell x vacuum-pair recruitment contact rule;
- directed re-emission/guidance of the recruited pair along the flux;
- affinity handover on approach (may reuse the existing P-join on contact);
- symmetric reverse bookkeeping (pair may flip target);
- sign/orientation law (pB/sB / m).

References: manuscript.tex:491, 616, 794-796, 1229, 1374, 1382;
experiments/PBSB_ISLANDS.md (decision rule: EXCLUSION stays the stand-in
until a dressing-mediated repulsion is demonstrated).

