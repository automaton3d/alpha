<#
  audit_sm_terms.ps1 -- audits the tracking of Standard-Model names in the manuscript.

  The Nomenclature states a naming convention: a Standard-Model name WITHOUT a
  model-side qualifier (-type, fragment, pair, pattern, cluster, channel, analogue)
  denotes the OBSERVED particle and must not be used for a model object.  This script
  reports every occurrence that carries no qualifier, so the convention can be checked
  mechanically instead of by reading.

  Exempt by construction (licence is explicit there):
    * the Nomenclature itself -- the convention paragraph and the decoder table;
    * the two taxonomy appendices, where the names denote the conjectured
      identifications (App. "Conjectured identifications with Standard-Model
      particles" and App. "Speculative particle spectrum and decay channels").

  Usage:  powershell -ExecutionPolicy Bypass -File experiments\audit_sm_terms.ps1
          powershell ... -File experiments\audit_sm_terms.ps1 -Tex doc\manuscript.tex
#>
param([string]$Tex = "$PSScriptRoot\..\doc\manuscript.tex")

$nouns = '\b(quark|gluon|photon|neutrino|antineutrino|lepton|electron|muon|proton|neutron|atom|hadron|graviton)s?\b'
$qual  = '\b(-type|type|fragment|pair|pattern|cluster|channel|analogue)s?\b'

$lines = Get-Content -LiteralPath $Tex
$exempt = New-Object 'System.Collections.Generic.List[int]'

# exemption 1: the Nomenclature (convention + decoder)
$n1 = ($lines | Select-String -Pattern '^\\section\*\{Nomenclature\}' -List).LineNumber
$n2 = ($lines | Select-String -Pattern '^%%%%%%%% FUNDING' -List).LineNumber
if ($n1 -and $n2) { $n1..($n2-1) | ForEach-Object { [void]$exempt.Add($_) } }

# exemption 2 and 3: the two taxonomy appendices
foreach ($pair in @(
    @('^\\section\{Conjectured identifications', '^\\section\{Cellular automata and quantum formalism'),
    @('^\\section\{Speculative particle spectrum', '^\\section\{'))) {
  $a = ($lines | Select-String -Pattern $pair[0] -List).LineNumber
  if (-not $a) { continue }
  $b = ($lines | Select-String -Pattern $pair[1] | Where-Object { $_.LineNumber -gt $a } | Select-Object -First 1).LineNumber
  if (-not $b) { $b = $lines.Count + 1 }
  $a..($b-1) | ForEach-Object { [void]$exempt.Add($_) }
}

$hits = 0
for ($i = 0; $i -lt $lines.Count; $i++) {
  $ln = $i + 1
  if ($exempt.Contains($ln)) { continue }
  $line = $lines[$i]
  if ($line.TrimStart().StartsWith('%')) { continue }
  foreach ($m in [regex]::Matches($line, $nouns)) {
    $tail = $line.Substring($m.Index, [Math]::Min($m.Length + 30, $line.Length - $m.Index))
    $head = $line.Substring([Math]::Max(0, $m.Index - 12), [Math]::Min(12, $m.Index))
    if ([regex]::IsMatch($tail, $qual) -or [regex]::IsMatch($head, '-type|type$')) { continue }
    $hits++
    $snip = $line.Substring([Math]::Max(0, $m.Index - 40), [Math]::Min(90, $line.Length - [Math]::Max(0, $m.Index - 40)))
    "L{0,-5} {1,-14} ...{2}..." -f $ln, $m.Value, $snip.Trim()
  }
}

"----"
if ($hits -eq 0) {
  'AUDIT CLEAN: no unqualified Standard-Model name outside the exempt taxonomy sections.'
} else {
  "AUDIT: $hits candidate(s) for review.  A bare name is CORRECT when it denotes the"
  'observed particle (cosmology bookkeeping, observer/relativity text, "not an'
  'electron assembled from the seed"); it is a defect when a MODEL object is meant.'
}
