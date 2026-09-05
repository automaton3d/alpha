param(
    [string]$Csv = "$PSScriptRoot\pk_right.csv",
    [string]$Output = "$PSScriptRoot\pk_right.png"
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Windows.Forms.DataVisualization
$rows = @(Import-Csv -LiteralPath $Csv)
if ($rows.Count -lt 2) { throw 'The experiment CSV has no measured frames.' }

# Offscreen chart: no interactive window or external plotting dependency.
$chart = New-Object System.Windows.Forms.DataVisualization.Charting.Chart
try {
    $chart.Width = 1200
    $chart.Height = 800
    $chart.BackColor = [System.Drawing.Color]::White
    $chart.Titles.Add('Um propeller P e um cacique K') | Out-Null
    $chart.Titles[0].Font = New-Object System.Drawing.Font('Segoe UI', 18)

    foreach ($name in @('Position', 'Step')) {
        $area = New-Object System.Windows.Forms.DataVisualization.Charting.ChartArea $name
        $area.AxisX.Title = 'Quadro de luz'
        $area.AxisX.Minimum = 0
        $area.AxisX.Maximum = [double]$rows[-1].frame
        $area.AxisX.Interval = [Math]::Max(1, [Math]::Ceiling([double]$rows[-1].frame / 8))
        $area.AxisX.MajorGrid.LineColor = [System.Drawing.Color]::Gainsboro
        $area.AxisY.MajorGrid.LineColor = [System.Drawing.Color]::Gainsboro
        $chart.ChartAreas.Add($area)
    }
    $chart.ChartAreas['Position'].Position = New-Object System.Windows.Forms.DataVisualization.Charting.ElementPosition(4, 9, 91, 43)
    $chart.ChartAreas['Step'].Position = New-Object System.Windows.Forms.DataVisualization.Charting.ElementPosition(4, 55, 91, 39)
    $chart.ChartAreas['Position'].AxisY.Title = 'Posicao x (desenrolada no toro)'
    $chart.ChartAreas['Step'].AxisY.Title = 'Deslocamento por quadro'
    $chart.ChartAreas['Step'].AxisY.Minimum = -1
    $chart.ChartAreas['Step'].AxisY.Maximum = 1
    $chart.ChartAreas['Step'].AxisY.Interval = 1

    $legend = New-Object System.Windows.Forms.DataVisualization.Charting.Legend 'Objects'
    $legend.Docking = 'Bottom'
    $chart.Legends.Add($legend)
    $definitions = @(
        @{ Name='P (duas metades coincidentes)'; Field='p_unwrapped'; Area='Position'; Color='RoyalBlue' },
        @{ Name='K'; Field='k_unwrapped'; Area='Position'; Color='DarkOrange' },
        @{ Name='Passo P'; Field='dx_p'; Area='Step'; Color='RoyalBlue' },
        @{ Name='Passo K'; Field='dx_k'; Area='Step'; Color='DarkOrange' }
    )
    foreach ($definition in $definitions) {
        $series = New-Object System.Windows.Forms.DataVisualization.Charting.Series $definition.Name
        $series.ChartArea = $definition.Area
        $series.ChartType = 'Line'
        $series.BorderWidth = 3
        $series.Color = [System.Drawing.Color]::FromName($definition.Color)
        $series.IsVisibleInLegend = $definition.Area -eq 'Position'
        foreach ($row in $rows) {
            # Frame 0 is the initial state, not a failed zero-speed step.
            if ($definition.Area -eq 'Step' -and [int]$row.frame -eq 0) { continue }
            $series.Points.AddXY([double]$row.frame, [double]$row.($definition.Field)) | Out-Null
        }
        $chart.Series.Add($series)
    }
    $chart.SaveImage($Output, [System.Windows.Forms.DataVisualization.Charting.ChartImageFormat]::Png)
    Write-Output $Output
}
finally { $chart.Dispose() }
