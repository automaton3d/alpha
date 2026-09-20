# anchor_semantic_check.ps1 -- semantic anchor check.
#
# attic/anchor_audit3.ps1 only proves that every `file:line` cited by the notes EXISTS and is inside
# the file.  It cannot see that the line has MOVED: when interaction.cpp gains lines (a port, a new
# candidate block, a longer comment), every anchor below the insertion point keeps resolving -- to the
# wrong construct.  That is exactly what happened on 2026-09-19, when a correction pass re-mapped
# eight anchors by meaning after a comment edit had shifted them.
#
# This script holds the anchors that matter and asserts that the CODE at the cited line still matches
# what the note claims.  It fails loudly when a construct moves, which is the signal to re-map the
# note AND this table in the same edit.
#
#   powershell -NoProfile -ExecutionPolicy Bypass -File experiments\anchor_semantic_check.ps1
#
# Exit code 0 = every anchor still says what it is cited for; 1 = drift (see the FAIL lines).

$ErrorActionPreference = 'Stop'
$root = Split-Path -Parent $PSScriptRoot
if (-not (Test-Path (Join-Path $root 'src\model\interaction.cpp'))) { $root = $PSScriptRoot }

$rows = @(
  @{ n='canFormPair predicate';              f='src/model/interaction.cpp'; l=226;  e='bool canFormPair' }
  @{ n='pair-formation branch';              f='src/model/interaction.cpp'; l=1893; e='samePos && sameT && currSrc.kind == SourceKind::S' }
  @{ n='chiefContact call site';             f='src/model/interaction.cpp'; l=1790; e='chiefContact\(currSrc' }
  @{ n='internal-contact early return';      f='src/model/interaction.cpp'; l=1839; e='const bool internal =' }
  @{ n='equal-charge D x D early return';    f='src/model/interaction.cpp'; l=1836; e='partnerSrc.kind==SourceKind::D && currSrc.ch==partnerSrc.ch' }
  @{ n='D-D cohesion loop';                  f='src/model/interaction.cpp'; l=666;  e='for \(const auto& \[a, b\] : internalContacts\)' }
  @{ n='escape timer threshold';             f='src/model/interaction.cpp'; l=772;  e='framesWithoutContact\[w\] < W_USED' }
  @{ n='parentRepel declaration';            f='src/model/interaction.cpp'; l=141;  e='equal-charge delegates of DIFFERENT dynamical islands' }
  @{ n='resolveParentRepulsion';             f='src/model/interaction.cpp'; l=865;  e='void resolveParentRepulsion' }
  @{ n='applyShellRelease';                  f='src/model/interaction.cpp'; l=829;  e='void applyShellRelease' }
  @{ n='chief_W global';                     f='src/model/interaction.cpp'; l=111;  e='int chief_W\[3\]' }
  @{ n='relocate()';                         f='src/model/interaction.cpp'; l=2287; e='void relocate\(' }
  @{ n='draft = north slide';                f='src/model/interaction.cpp'; l=2308; e='draft = north' }
  @{ n='homB consumed';                      f='src/model/interaction.cpp'; l=2352; e='draft.homB = false' }
  @{ n='encounter definition';               f='src/model/interaction.cpp'; l=1138; e='bool encounter\(' }
  @{ n='per-voxel contact partner';          f='src/model/simulation.cpp';  l=1000; e='getCell\(lattice_curr, x, y, z, partner.w\)' }
  @{ n='per-voxel encounter call';           f='src/model/simulation.cpp';  l=1001; e='encounter\(curr, draft, contactPartner\)' }
  @{ n='applyMomentum';                      f='src/model/simulation.cpp';  l=423;  e='static void applyMomentum' }
  @{ n='promotesDelegate (T2)';              f='src/include/model/chief_transition.h'; l=14; e='inline bool promotesDelegate' }
  @{ n='T2 disabled by the candidate';       f='src/include/model/chief_transition.h'; l=17; e='#ifdef PARENT_SELECTIVE_FSM' }
  @{ n='T2 invocation in chiefContact';      f='src/include/model/chief_transition.h'; l=73; e='if\(promotesDelegate\(main,mirror\)\)' }
  @{ n='demotesChief (T3)';                  f='src/include/model/chief_transition.h'; l=43; e='inline bool demotesChief' }
  @{ n='T3 invocation in chiefContact';      f='src/include/model/chief_transition.h'; l=74; e='if\(demotesChief\(main,mirror\)\)' }
  @{ n='S x S family minimum (DD_INTRA)';    f='src/include/model/chief_transition.h'; l=65; e='main.kind==SourceKind::S && mirror.kind==SourceKind::S' }
  @{ n='PHASE_DISTINCT block';               f='src/include/model/chief_transition.h'; l=52; e='#ifdef PHASE_DISTINCT_FSM' }
  @{ n='chiefContact definition';            f='src/include/model/chief_transition.h'; l=50; e='inline void chiefContact' }
  @{ n='length<=1 step invariant';           f='experiments/inertia_fixture.h'; l=95; e='assert\(length<=1\)' }
  @{ n='ISLAND_SIZE = W/(9L)';               f='src/include/model/simulation.h'; l=403; e='ISLAND_SIZE' }
)

$fail = 0
foreach ($r in $rows) {
  $path = Join-Path $root $r.f
  if (-not (Test-Path $path)) { Write-Output ("MISSING  {0,-34} {1}" -f $r.n, $r.f); $fail++; continue }
  $lines = [IO.File]::ReadAllLines($path)
  if ($r.l -lt 1 -or $r.l -gt $lines.Count) {
    Write-Output ("OUT-OF-RANGE {0,-30} {1}:{2}" -f $r.n, $r.f, $r.l); $fail++; continue
  }
  $text = $lines[$r.l - 1]
  if ($text -match $r.e) {
    Write-Output ("ok       {0,-34} {1}:{2}" -f $r.n, $r.f, $r.l)
  } else {
    Write-Output ("FAIL     {0,-34} {1}:{2}  expected /{3}/  found: {4}" -f $r.n, $r.f, $r.l, $r.e, $text.Trim())
    $fail++
  }
}
Write-Output ""
Write-Output ("checks={0}  failures={1}" -f $rows.Count, $fail)
if ($fail -gt 0) {
  Write-Output "DRIFT: a cited construct moved.  Re-map the note that cites it AND the row above in the same edit."
  exit 1
}
Write-Output "OK: every cited construct is where the notes say it is."
