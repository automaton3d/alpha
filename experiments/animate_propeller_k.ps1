param(
    [string]$Csv = "$PSScriptRoot\pk_right.csv",
    [string]$Output = "$PSScriptRoot\pk_right.gif",
    [int]$LX = 51
)
$ErrorActionPreference = 'Stop'
Add-Type -AssemblyName System.Drawing
$rows = @(Import-Csv -LiteralPath $Csv)
if ($rows.Count -lt 2) { throw 'CSV sem quadros suficientes.' }
$framesDir = Join-Path $PSScriptRoot '..\build\propeller_k\gif_frames'
New-Item -ItemType Directory -Force -Path $framesDir | Out-Null
$font = New-Object System.Drawing.Font('Segoe UI', 14)
$small = New-Object System.Drawing.Font('Segoe UI', 11)
$title = New-Object System.Drawing.Font('Segoe UI', 20)
$grid = New-Object System.Drawing.Pen([System.Drawing.Color]::LightGray)
$ink = [System.Drawing.Brushes]::MidnightBlue
try {
    for ($i = 0; $i -lt $rows.Count; $i++) {
        $row = $rows[$i]
        $bitmap = New-Object System.Drawing.Bitmap(960, 460)
        $g = [System.Drawing.Graphics]::FromImage($bitmap)
        try {
            $g.SmoothingMode = 'AntiAlias'
            $g.Clear([System.Drawing.Color]::White)
            $g.DrawString('Um propeller e um K', $title, $ink, 25, 15)
            $g.DrawString("Quadro de luz: $($row.frame)   |   Distancia: $($row.distance) celulas", $font, $ink, 25, 62)
            $state = if ([int]$row.distance -eq 0) { 'Movimento conjunto: P e K na mesma posicao x' } else { 'Aproximacao: P alcanca K por contatos sucessivos' }
            $g.DrawString($state, $font, $ink, 25, 94)
            $lanes = @(
                @{Label='K'; Field='k_x'; Y=185; Brush=[System.Drawing.Brushes]::DarkOrange},
                @{Label='P1'; Field='p1_x'; Y=250; Brush=[System.Drawing.Brushes]::RoyalBlue},
                @{Label='P2'; Field='p2_x'; Y=315; Brush=[System.Drawing.Brushes]::RoyalBlue}
            )
            foreach ($lane in $lanes) {
                $y = [single]$lane.Y
                $g.DrawString($lane.Label, $font, $ink, 25, ($y-14))
                $g.DrawLine($grid, 100, $y, 925, $y)
                for ($x = 0; $x -lt $LX; $x++) {
                    $px = [single](100 + 825 * $x / ($LX-1))
                    $g.DrawLine($grid, $px, ($y-5), $px, ($y+5))
                }
                $px = [single](100 + 825 * [int]$row.($lane.Field) / ($LX-1))
                $g.FillEllipse($lane.Brush, ($px-10), ($y-10), 20, 20)
            }
            $g.DrawString('x = 0', $small, $ink, 85, 340)
            $g.DrawString("x = $($LX-1)", $small, $ink, 880, 340)
            $g.DrawString('Grade periodica: ao sair pela direita, retorna pela esquerda.', $small, $ink, 25, 376)
            $g.DrawString('P1 e P2 formam UM propeller. Faixas separam camadas, nao posicoes y.', $small, $ink, 25, 399)
            $g.DrawString('Dados do simulador; voo de P estipulado na variante experimental.', $small, $ink, 25, 422)
            $bitmap.Save((Join-Path $framesDir ('frame_{0:D4}.png' -f $i)), [System.Drawing.Imaging.ImageFormat]::Png)
        } finally { $g.Dispose(); $bitmap.Dispose() }
    }
} finally { $font.Dispose(); $small.Dispose(); $title.Dispose(); $grid.Dispose() }
$inputPattern = Join-Path $framesDir 'frame_%04d.png'
& ffmpeg -hide_banner -loglevel error -y -framerate 6 -i $inputPattern -filter_complex "[0:v]trim=end_frame=$($rows.Count),split[a][b];[a]palettegen[p];[b][p]paletteuse" -loop 0 -final_delay 200 $Output
if ($LASTEXITCODE -ne 0) { throw 'Falha ao gerar GIF.' }
Write-Output $Output
