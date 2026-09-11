<#
  run_wp2_sweep.ps1 -- WP2 driver for the alpha_A..F falsification campaign.

  Two modes (can be combined):
    * L-sweep : alpha_probe at the reference sieve for a list of L, overwriting
                build\wp2\alpha_A_sweep.csv.
    * S-scan  : alpha_probe at a fixed L (default 7) over a list of sieve moduli
                S, APPENDING each row to build\wp2\S_scan_L<ScanL>.csv, so the
                whole comb can be run in batches (it exceeds one shell window).

  The reference model is fully deterministic (no RNG on the reference path), so
  repeated runs give identical dynamics; the only run-to-run difference is the
  printed wall-clock line.  Error bars therefore come from the SWEPT parameter S,
  not from seeds.

  Prereq: experiments\alpha_probe.exe (run experiments\build_probe.bat first).

  Usage:
    powershell ... -File experiments\run_wp2_sweep.ps1 -L 7,9
    powershell ... -File experiments\run_wp2_sweep.ps1 -L "" -Fresh -SList 128,640,1152,1664,2176,2688,3200,3712
    powershell ... -File experiments\run_wp2_sweep.ps1 -L ""
#>
param(
  [string]$L     = "7,9,11,13",
  [string]$SList = "",
  [int]$ScanL    = 7,
  [int]$Sieve    = 16384,
  [int]$Budget   = 256,
  [switch]$Fresh,
  [switch]$SkipL
)

$ErrorActionPreference = "Continue"
$Inv = [System.Globalization.CultureInfo]::InvariantCulture

$root = Split-Path -Parent $PSScriptRoot
Set-Location $root
$out  = Join-Path $root "build\wp2"
New-Item -ItemType Directory -Force $out | Out-Null
$probe = Join-Path $root "experiments\alpha_probe.exe"
if (-not (Test-Path $probe)) { throw "alpha_probe.exe not found; run experiments\build_probe.bat first" }

function Get-Field([string]$txt, [string]$pat) { [regex]::Match($txt, $pat).Groups[1].Value }
function Fmt([double]$v) { $v.ToString($Inv) }

$Ls      = @($L.Split(',')     | Where-Object { $_.Trim().Length } | ForEach-Object { [int]$_.Trim() })
$Svalues = @($SList.Split(',') | Where-Object { $_.Trim().Length } | ForEach-Object { [int]$_.Trim() })
if ($SkipL) { $Ls = @() }

# --- L-sweep at the reference sieve -----------------------------------------
if ($Ls.Count -gt 0) {
  $rows = @()
  foreach ($l in $Ls) {
    $frames = 200
    if ([int]$l -ge 13) { $frames = 100 }
    $log = Join-Path $out "L$l.log"
    cmd /c "`"$probe`" $l 4 $frames $Sieve $Budget > `"$log`" 2>&1" | Out-Null
    $txt = Get-Content $log -Raw
    $rows += [pscustomobject]@{
      L             = $l
      frames        = $frames
      sieve         = $Sieve
      active_passes = [int](Get-Field $txt 'active-passes=(\d+)')
      s2B_passes    = [int](Get-Field $txt 's2B-passes=(\d+)')
      pairs_formed  = [int](Get-Field $txt 'pairs-formed=(\d+)')
      alpha_A       = Fmt ([double](Get-Field $txt 'frame-averaged = ([0-9.]+)'))
      inv_alpha_A   = Fmt ([double](Get-Field $txt '1/alpha_A = ([0-9.]+)'))
    }
  }
  Write-Output ""
  Write-Output "== WP2 alpha_A L-sweep (sieve=$Sieve) =="
  $rows | Format-Table -AutoSize
  $rows | Export-Csv (Join-Path $out "alpha_A_sweep.csv") -NoTypeInformation
}

# --- S-scan (appends, so it can be run in batches) --------------------------
if ($Svalues.Count -gt 0) {
  $scanCsv = Join-Path $out "S_scan_L$ScanL.csv"
  if ($Fresh -and (Test-Path $scanCsv)) { Remove-Item $scanCsv -Force }
  if (-not (Test-Path $scanCsv)) { "L,sieve,calls,s2B,ratio" | Set-Content $scanCsv }
  Write-Output ""
  Write-Output "== WP2 S-scan (L=$ScanL) =="
  foreach ($s in $Svalues) {
    $log = Join-Path $out "S${ScanL}_$s.log"
    cmd /c "`"$probe`" $ScanL 4 200 $s $Budget > `"$log`" 2>&1" | Out-Null
    $txt   = Get-Content $log -Raw
    $calls = [double](Get-Field $txt 'active-passes=(\d+)')
    $s2b   = [double](Get-Field $txt 's2B-passes=(\d+)')
    $ratio = if ($s2b -gt 0) { Fmt ($calls / $s2b) } else { "inf" }
    Add-Content $scanCsv ("{0},{1},{2},{3},{4}" -f $ScanL, $s, $calls, $s2b, $ratio)
    Write-Output ("  S={0,-6} calls={1,-7} s2B={2,-7} ratio={3}" -f $s, $calls, $s2b, $ratio)
  }
  Write-Output "appended to $scanCsv"
}

Set-Content (Join-Path $out "SWEEP_DONE.txt") (Get-Date).ToString("s")
Write-Output "outputs written to build\wp2\"
