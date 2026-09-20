<#
  model_fingerprint.ps1 -- recompute the alpha model source fingerprint.

  The fingerprint covers every model source (src\model\*.cpp, src\model\*.inc,
  src\include\model\*.h).  It is the authoritative drift check for the frozen
  reference configuration documented in doc\REFERENCE_CONFIG.md.

  The default expected value is the current working-tree reference fingerprint
  (re-measured 19 Sep 2026, second time: the D x D correction pass rewrote four
  comments in interaction.cpp and chief_transition.h -- the false "D-D cohesion"
  claim -- and comments are part of this source-text hash.  The three earlier
  moves were the WINDING_GATED_FSM port (21 lines, inert with the macro OFF), two
  stale comment anchors, and the PARENT_* candidate blocks; a fourth move was the
  read-only A2 instrumentation (CASCADE_LOG: a ledger block appended at the END of
  chief_transition.h plus a 9-line hook after the chiefContact call, removed by the
  preprocessor when the macro is undefined -- the reference preprocessed output is
  byte-identical, verified).  In every case the
  reference BUILD behaviour is unchanged, which is proven separately by
  preprocessing the file with all candidate macros OFF and comparing: see the
  WORK_PLAN entries of 19 Sep 2026.  NOTE: because this fingerprint hashes source
  TEXT, any comment-only edit moves it -- that is expected, not drift; the
  behaviour check is the preprocessor comparison.  The historical model-ref-v1
  fingerprint was 5b0944258b8f9b750688375f2e9a4cd99e74f0355f1c6b1aa8edc769ab743673.

  Usage:
    powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1
    powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1 -Expected 5b0944...
  Exit code: 0 if it matches the expected value (default: the current reference
  fingerprint), 1 on mismatch.
#>
param(
  [string]$Expected = "bacade53e4d0c0a7ae1601992579d0066a603e4ccf9020051399f8cc510a5d82"
)

$ErrorActionPreference = "Stop"
$root = Split-Path -Parent $PSScriptRoot
if (-not (Test-Path (Join-Path $root "src\model"))) { $root = $PSScriptRoot | Split-Path -Parent }

$files = Get-ChildItem `
  (Join-Path $root "src\model\*.cpp"), `
  (Join-Path $root "src\model\*.inc"), `
  (Join-Path $root "src\include\model\*.h") -File | Sort-Object Name

$lines = foreach ($f in $files) {
  "$($f.Name) $((Get-FileHash $f.FullName -Algorithm SHA256).Hash)"
}
$lines | ForEach-Object { Write-Output $_ }

$joined  = ($lines -join "`n")
$bytes   = [System.Text.Encoding]::ASCII.GetBytes($joined)
$combined = ([System.BitConverter]::ToString(
  [System.Security.Cryptography.SHA256]::Create().ComputeHash($bytes)) -replace '-','').ToLower()

Write-Output ""
Write-Output "COMBINED MODEL FINGERPRINT: $combined"

if ($Expected) {
  if ($combined -eq $Expected.ToLower()) {
    Write-Output "MATCH: model matches the reference fingerprint."
    exit 0
  } else {
    Write-Output "MISMATCH: expected $($Expected.ToLower())."
    Write-Output "The model sources changed -- regenerate any affected manuscript table."
    exit 1
  }
}
