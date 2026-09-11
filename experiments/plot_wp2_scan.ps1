<#
  plot_wp2_scan.ps1 -- dependency-free SVG plot of a WP2 S-scan CSV.

  Plots the realized throughput ratio calls/s2B (== 1/alpha) against the sieve
  modulus S on a log10 y-axis.  Open points are circles; closed points
  (ratio = inf) are red x marks on the baseline.  A dashed line marks the target
  ratio 1/137.036 for reference.

  Usage:
    powershell -ExecutionPolicy Bypass -File experiments\plot_wp2_scan.ps1
    powershell ... -File experiments\plot_wp2_scan.ps1 -Csv build\wp2\S_scan_L9.csv
#>
param(
  [string]$Csv = "build\wp2\S_scan_L7.csv",
  [string]$Out = ""
)

$Inv = [System.Globalization.CultureInfo]::InvariantCulture
$root = Split-Path -Parent $PSScriptRoot
Set-Location $root
if (-not (Test-Path $Csv)) { throw "CSV not found: $Csv" }
if (-not $Out) { $Out = [System.IO.Path]::ChangeExtension((Resolve-Path $Csv), ".svg") }

$rows = Import-Csv $Csv
$pts = foreach ($r in $rows) {
  [pscustomobject]@{
    S     = [double]::Parse($r.sieve, $Inv)
    calls = [double]::Parse($r.calls, $Inv)
    s2B   = [double]::Parse($r.s2B,   $Inv)
    ratio = if ($r.ratio -eq 'inf') { $null } else { [double]::Parse($r.ratio, $Inv) }
  }
}
$open   = @($pts | Where-Object { $_.ratio -ne $null })
$closed = @($pts | Where-Object { $_.ratio -eq $null })

$W = 880; $H = 440; $L = 74; $R = 26; $T = 34; $B = 58
$sMin = ($pts | Measure-Object S -Minimum).Minimum
$sMax = ($pts | Measure-Object S -Maximum).Maximum
$rMin = ($open | Measure-Object ratio -Minimum).Minimum
$rMax = ($open | Measure-Object ratio -Maximum).Maximum
$lmin = [math]::Floor([math]::Log10($rMin))
$lmax = [math]::Ceiling([math]::Log10($rMax))

function X([double]$s) { $L + ($s - $sMin) / ($sMax - $sMin) * ($W - $L - $R) }
function Y([double]$r) { $T + ($lmax - [math]::Log10($r)) / ($lmax - $lmin) * ($H - $T - $B) }
function N([double]$v) { $v.ToString("0.###", $Inv) }

$sb = New-Object System.Text.StringBuilder
[void]$sb.AppendLine("<svg xmlns='http://www.w3.org/2000/svg' width='$W' height='$H' viewBox='0 0 $W $H'>")
[void]$sb.AppendLine("<rect width='$W' height='$H' fill='white'/>")
[void]$sb.AppendLine("<text x='$L' y='22' font-family='sans-serif' font-size='15' font-weight='bold'>WP2 S-scan -- realized coupling 1/alpha = calls/s2B vs sieve modulus S (L=7, 200 frames)</text>")

# y grid (decades) + labels
for ($d = $lmin; $d -le $lmax; $d++) {
  $yy = Y ([math]::Pow(10, $d))
  [void]$sb.AppendLine("<line x1='$L' y1='$(N $yy)' x2='$($W-$R)' y2='$(N $yy)' stroke='#e0e0e0'/>")
  [void]$sb.AppendLine("<text x='$($L-8)' y='$(N ($yy+4))' text-anchor='end' font-family='sans-serif' font-size='11'>1e$d</text>")
}
# axes
[void]$sb.AppendLine("<line x1='$L' y1='$T' x2='$L' y2='$($H-$B)' stroke='#333'/>")
[void]$sb.AppendLine("<line x1='$L' y1='$($H-$B)' x2='$($W-$R)' y2='$($H-$B)' stroke='#333'/>")
# x ticks
foreach ($s in @(128, 2048, 4096, 8192, 12000, 16000)) {
  if ($s -lt $sMin -or $s -gt $sMax) { continue }
  $xx = X $s
  [void]$sb.AppendLine("<line x1='$(N $xx)' y1='$($H-$B)' x2='$(N $xx)' y2='$($H-$B+5)' stroke='#333'/>")
  [void]$sb.AppendLine("<text x='$(N $xx)' y='$($H-$B+19)' text-anchor='middle' font-family='sans-serif' font-size='11'>$s</text>")
}
[void]$sb.AppendLine("<text x='$(($W-$R+$L)/2)' y='$($H-16)' text-anchor='middle' font-family='sans-serif' font-size='12'>sieve modulus S</text>")

# target 1/137.036
$tt = Y 137.036
[void]$sb.AppendLine("<line x1='$L' y1='$(N $tt)' x2='$($W-$R)' y2='$(N $tt)' stroke='#c00' stroke-dasharray='6,4'/>")
[void]$sb.AppendLine("<text x='$($W-$R-4)' y='$(N ($tt-5))' text-anchor='end' font-family='sans-serif' font-size='11' fill='#c00'>target 1/137.036</text>")

# closed points (red x on baseline)
foreach ($p in $closed) {
  $xx = X $p.S; $yy = $H - $B
  [void]$sb.AppendLine("<path d='M$(N ($xx-4)) $(N ($yy-8)) L$(N ($xx+4)) $(N ($yy-0)) M$(N ($xx-4)) $(N ($yy-0)) L$(N ($xx+4)) $(N ($yy-8))' stroke='#c00' stroke-width='1.5' fill='none'/>")
}
# open points
foreach ($p in $open) {
  $xx = X $p.S; $yy = Y $p.ratio
  [void]$sb.AppendLine("<circle cx='$(N $xx)' cy='$(N $yy)' r='3.2' fill='#1a6'/>")
}
[void]$sb.AppendLine("<text x='$L' y='$($H-$B+40)' font-family='sans-serif' font-size='11' fill='#1a6'>green = open gate (finite 1/alpha)</text>")
[void]$sb.AppendLine("<text x='$(($W+$L)/2)' y='$($H-$B+40)' font-family='sans-serif' font-size='11' fill='#c00'>red x = closed gate (0 passes)</text>")
[void]$sb.AppendLine("</svg>")

Set-Content -Path $Out -Value $sb.ToString() -Encoding UTF8
Write-Output ("wrote {0}  ({1} open, {2} closed)" -f $Out, $open.Count, $closed.Count)
