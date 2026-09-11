<#
  analyze_wp2_scan.ps1 -- summarize a WP2 S-scan CSV.

  The realized throughput ratio calls/s2B is a resonance comb: many S values
  close the gate (ratio = inf).  The dial-free statistics are computed over the
  OPEN points (finite ratio) and the closure fraction is reported separately.

  Usage:
    powershell -ExecutionPolicy Bypass -File experiments\analyze_wp2_scan.ps1
    powershell ... -File experiments\analyze_wp2_scan.ps1 -Csv build\wp2\S_scan_L9.csv
#>
param([string]$Csv = "build\wp2\S_scan_L7.csv")

$Inv = [System.Globalization.CultureInfo]::InvariantCulture
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root
if (-not (Test-Path $Csv)) { throw "CSV not found: $Csv" }

$rows   = Import-Csv $Csv
$total  = $rows.Count
$open   = @($rows | Where-Object { $_.ratio -ne 'inf' } |
            ForEach-Object { [double]::Parse($_.ratio, $Inv) } | Sort-Object)
$closed = $total - $open.Count

Write-Output "S-scan: $Csv"
Write-Output ("points: {0}  open: {1}  closed(inf): {2}" -f $total, $open.Count, $closed)

if ($open.Count -gt 0) {
  $n      = $open.Count
  $median = if ($n % 2 -eq 1) { $open[[int][math]::Floor($n / 2)] }
            else { ($open[$n / 2 - 1] + $open[$n / 2]) / 2.0 }
  $mean   = ($open | Measure-Object -Average).Average
  Write-Output ("open ratios  min={0}  median={1}  mean={2}  max={3}" -f `
    $open[0].ToString($Inv), $median.ToString($Inv), $mean.ToString($Inv), $open[$n-1].ToString($Inv))
  Write-Output ("1/median = {0}   1/mean = {1}" -f (1.0/$median).ToString($Inv), (1.0/$mean).ToString($Inv))
} else {
  Write-Output "no open points: the gate is closed for every S in this scan."
}
