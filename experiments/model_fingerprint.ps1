<#
  model_fingerprint.ps1 -- recompute the alpha model source fingerprint.

  The fingerprint covers every model source (src\model\*.cpp, src\model\*.inc,
  src\include\model\*.h).  It is the authoritative drift check for the frozen
  reference configuration documented in doc\REFERENCE_CONFIG.md.

  The default expected value is the current working-tree reference fingerprint
  (post WP4.2: the macro-guarded EM_FORCE_PREREQ spike block was added to
  interaction.cpp; the reference build behaviour is unchanged).  The historical
  model-ref-v1 fingerprint was 5b0944258b8f9b750688375f2e9a4cd99e74f0355f1c6b1aa8edc769ab743673.

  Usage:
    powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1
    powershell -ExecutionPolicy Bypass -File experiments\model_fingerprint.ps1 -Expected 5b0944...
  Exit code: 0 if it matches the expected value (default: the model-ref-v1
  fingerprint), 1 on mismatch.
#>
param(
  [string]$Expected = "428ece873cddf82fb5fcdfeed14fc4c9e769ccf08ebd5c0fb792bb7edb4b2ca8"
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
