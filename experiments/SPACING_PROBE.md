# Spacing probe (item 1): is spacing emergent, or always imposed?

Question: the `81 x 3` candidate state exists only with a hard core keyed on the
seed family (`w/3`, `EXCLUSION_FSM`) -- a stand-in.  A mechanism that holds
same-charge copies apart beyond the contact range would do the same job with the
model's own ingredients and without any rule containing `L/3`.  This note measures
the **spacing threshold** and tests whether any model channel reaches it.

## Harness

`experiments/spacing_probe.cpp` (build `experiments/build_spacing_probe.bat`,
three arms: reference; `SPACING_POL` = polarization bootstrap live; and
`SPACING_POL + EM_FIRST_FSM`).  A three-copy family is planted in a `27x5x5` tube
(`RMAX = 2`, so the contact range is `2*RMAX = 4`):

    K at x=3,  D1 at x=6,  D2 at x=6+dD      (run: spacing_probe_<arm>.exe 40 dD)

`dD` is the D1-D2 gap -- the only quantity varied.  A promotion (a `D` becoming a
`K`) means `T2` fired and the family lost a member.

## Result: the threshold is `d >= 2*RMAX + 1 = 5`

| arm | `dD` | promotions | final roles | verdict |
|---|---|---|---|---|
| reference | 0, 1, 2, 3, **4** | 1 | 2 K + 1 D | T2 fired |
| reference | **5** | 0 | **1 K + 2 D** | roles kept |
| `SPACING_POL` | 0, 3 | 1 | 2 K + 1 D | T2 fired |
| `SPACING_POL + EM_FIRST_FSM` | 0 | 1 | 2 K + 1 D | T2 fired |

Three conclusions.

1. **A three-copy family is stable without any hard core -- but only beyond the
   contact range.**  At `dD >= 5 = 2*RMAX + 1` the reference rules keep `1K+2D`
   indefinitely (40 journeys here; the P7 control at 5 cells apart agrees).  No
   protective rule is needed *at that spacing*.
2. **Inside the range, `T2` always fires -- and no model channel prevents it.**
   Every separation `0 <= dD <= 4` promotes, in the reference arm and in both
   electric arms.  The electric channel was given its prerequisite (the
   polarization bootstrap plus the documented ordering fix) and still changes
   nothing: in the companion `K + 1D` run the `conv_repel` counter never moved.
   Together with `PHOTON_MEDIATION_FAR.md` (mediated R2/R1 indistinguishable) and
   `GEOMETRIC_QUANTUM.md` (the contact graph at the turnaround is complete), this
   closes every candidate for a *space-providing* mechanism.
3. **The candidates that do space copies read the partition.**  `ADDRESS_TARGET_FSM`
   walks island `i = w / ISLAND_SIZE` to a site computed from that index, and
   `ISLAND_ALIGNED_W_ROTATION` rotates inside `ISLAND_SIZE` blocks: both derive the
   destination from the `L/3` partition.  The `81 x 3` state is therefore
   *imposed* in every construction that produces it -- by blocking contacts
   (`EXCLUSION_FSM`) or by displacing copies according to the family index -- and
   never produced by a mechanism that does not know the partition.

## The design criterion this leaves for a genuine mechanism

Any future rule that wants to make the quantisation emergent must, in this
geometry, **hold the copies of one family at `d >= 2*RMAX + 1`** using only
cell-local data (its own address, coordinates and fields), with no reference to
`ISLAND_SIZE`, the family index, or `L`.  That is now a precise, falsifiable
target: plant a three-copy family inside the range and require `d12(t)` to grow
past `2*RMAX` and stay there, with `promotions = 0`, in the reference arm of this
probe.  Nothing in the current rule set does it.

## Reproduce

```bat
experiments\build_spacing_probe.bat
build\spacing_probe\spacing_probe_ref.exe 40 0     rem ... and dD = 1..5
build\spacing_probe\spacing_probe_pol.exe 40 3
build\spacing_probe\spacing_probe_polem.exe 40 0
```

## Limits

One geometry (`RMAX = 2`, long side 27), one charge word, three copies, 40
journeys; the threshold is measured for a *static* family (in the moving-body
configuration the encounter window samples different phases, so the effective
threshold there is an open question worth measuring with the `dD` argument
ported to `turnover_ablation.cpp`).
