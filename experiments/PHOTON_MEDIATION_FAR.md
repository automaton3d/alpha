# Distinct-family ("far") mediated-duo probe (item C)

Closes the last confound of the Route-B line (`PHOTON_MEDIATION_DESIGN.md`
section 8, "Next" items 1 and 4).  Every earlier "photon holds the bodies
apart" positive was measured at `W = 4`, where the two bodies sit in the SAME
charge family (`w = 0` and `w = 1`), so the identity machinery elects a chief
between them and the island cohesion pulls them together; the mediator only
slowed that merge.  The `far` layout removes the confound: `W = 9`, bodies at
`w = 0` (island 0) and `w = 3` (island 1), free mediator pair at `w = 6/7`
(island 2), so the mediated channel is the only coupling left.

    experiments\alpha_probe_orphan.exe    EL 4  FRAMES 16384 120 no no 0 <pol|no> m 8 <bare|photon|grav> 0 far
    experiments\alpha_probe_em_orphan.exe EL 4  FRAMES <S>   120 no no 0 <pol|no> m 8 <bare|photon|grav> 0 far

`build_probe_orphan.bat` is pre-existing; **`build_probe_em_orphan.bat` is new**
(`EM_FIRST_FSM` + `ORPHAN_GUIDANCE_FSM` + the polarization bootstrap macros), so
the electroweak decision is taken BEFORE the identity merge while the
orphan-shell recruit/relay channel is live.  All runs: EL=11, SEP=4, 20 light
frames, `mag=0`, same charge `ch=0x08` on both bodies.

## 1. Orphan channel only, reference sieve (identity first) -- negative

`experiments\alpha_probe_orphan.exe` (rebuilt 13 Sep 2026; the archived binary
predated `interaction.cpp`), `pol no`, `S = 16384`:

| mediator | d(t) per frame (cells) | recruit_repel | recruit_attract |
|---|---|---|---|
| bare (W-matched control) | 4,4,2,**0**,0,...  | 0 | 0 |
| R2 photon | 4,4,2,**0**,0,... | 0 | 2 |
| R1 graviton | 4,4,2,**0**,0,... | 0 | -- |

The three merge at the same frame.  The R2 channel produces two attractive
recruits and no repulsion, because the identity merge runs first (the documented
`encounter()` ordering problem).

## 2. EM reorder + orphan channel (`alpha_probe_em_orphan.exe`)

| run | d(t) per frame (cells) | recruit events | repel | attract | annih. |
|---|---|---|---|---|---|
| photon, `S=16384`, pol | 4,4,2,2,**0**,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 | 500 | 0 | 2 | 0 |
| photon, `S=4096`, pol | 4,4,2,2,**0**,0,0,0,0,0,0,0,0,1,1,1,0,0,0,0 | 1714 | 0 | 3 | 0 |
| photon, `S=64`, pol | 4,5,5,3,4,5,5,5.4,5.1,5.1,4.1,2.5,3.5,0,3.0,6.7,5.4,3.5,3.0,3.7 | 506 | 0 | 32 | 26 |
| grav, `S=64`, pol | **identical to the photon row above** | 506 | 0 | 32 | 26 |
| bare, `S=64`, pol | 4,5,5,4,2,4,3,5,2.2,1,1,4,2.2,1,0,1,2,3.6,3.6,4.2 | 410 | 0 | 21 | 9 |
| photon, `S=64`, no pol | 4,4,2,0,0,0,0,0,0,2,3.5,2.5,2.2,7.1,5.7,5.7,6.6,6.0,5.9,6.9 | 704 | 2 | 22 | 6 |

Reading:

- **At the reference sieve the R2 channel does nothing in this layout**: the
  bodies reach contact at frame 5 and stay there for the remaining 15 frames
  (the `EM_FIRST_FSM` reorder alone does not help: `s2B` is shut at `S = 16384`).
- **Opening the gate (`S = 64`) produces motion, but not a mediator effect**:
  the photon and graviton trajectories are *bit-identical* (same d(t), same
  counters, same 26 annihilations), and the bare control shows the same
  qualitative oscillation.  The separation that appears is therefore
  charge-word blind: it is the opened electroweak branch acting on the shells,
  not the R2/R1 identity of the planted pair.  Even there the contact is not
  avoided (`d = 0` at frame 14 in the `pol` runs).
- **`S = 4096` is intermediate**: a 1-cell gap for three frames, then contact
  again.  No stable separation at any `S` tested.
- Consequence: the sign-selection ("equal charge -> repel") that the design
  assumed for the R2 channel is **not recovered with the identity merge out of
  the way**.  The earlier W=4 positives (min d = 1.00-3.00, "repel" signals)
  were the identity/cohesion background being partially counteracted, not two
  distinct islands being held apart.

## What this closes and what it does not

- Closes `PHOTON_MEDIATION_DESIGN.md` section 8 "Next" items (1) (distinct
  families) and (4) (re-run the quantization hunt in that layout): both are
  negative with the current candidate macros.
- The annihilation branch (item 3) is exercised here and counted
  (`annihilations=26` at `S=64`, 0 at the reference sieve).
- **Not** established: a mechanism that separates two equal-charge islands.
  With the identity hard core (`EXCLUSION_FSM`) the separation exists, but that
  rule is a stand-in (see `PBSB_ISLANDS.md`) and, as item A showed, the
  quantised population only appears in builds whose rules carry `L/3`.
- Caveats: one lattice size (`EL=11`), one separation (`SEP=4`), 20 frames,
  `mag=0`; the `pol` runs rely on the candidate polarization bootstrap
  (`POLAR_BOOTSTRAP_ADDRESS`), and the mediator is planted by the probe rather
  than formed by the dynamics.
