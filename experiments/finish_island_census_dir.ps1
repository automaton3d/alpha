param([string]$RunDir = "build\island_census_dir\run64")
# Waits for the directional island census to exit, then writes an independent
# audit next to the run directory: the standard analyzer output plus a compact
# trend table of the per-frame census, so the stabilisation question (does the
# count of distinct centres settle at 9L = 81?) can be read without re-running
# anything.  Usage:
#   powershell -ExecutionPolicy Bypass -File experiments\finish_island_census_dir.ps1 [rundir]
$ErrorActionPreference = "Continue"
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root
while (Get-Process island_census_dir -ErrorAction SilentlyContinue) { Start-Sleep -Seconds 20 }
$abs = Join-Path $root $RunDir
$out = Join-Path $abs "analysis.txt"
"# AUDIT of $RunDir" | Out-File -Encoding utf8 $out
"written: $(Get-Date -Format 'HH:mm:ss')" | Out-File -Append -Encoding utf8 $out
& python (Join-Path $root "experiments\analyze_island_census.py") $abs *>> $out
"" | Out-File -Append -Encoding utf8 $out
"TREND (frame, centres, K, D, groups, unresolved, maxpop, maxspan):" | Out-File -Append -Encoding utf8 $out
Import-Csv (Join-Path $abs "census.csv") |
    Select-Object frame, distinct_centers, K, D, groups, unresolved, max_population, max_span |
    Format-Table -AutoSize | Out-File -Append -Encoding utf8 $out
(Get-Content (Join-Path $abs "summary.txt") -ErrorAction SilentlyContinue | Select-String "SUMMARY") |
    Out-File -Append -Encoding utf8 $out
"ANALYSIS WRITTEN: $out" | Out-File -Append -Encoding utf8 $out
