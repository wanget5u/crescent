Clear-Host
if (!(Test-Path -Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }
if (!(Test-Path -Path "bin")) { New-Item -ItemType Directory -Path "bin" | Out-Null }
Write-Host "crescent - compiling..." -ForegroundColor Cyan
clang src/*.c -O3 -I./include -L./lib -lraylib -lwinmm -lgdi32 -luser32 -lshell32 -D_CRT_SECURE_NO_WARNINGS -Wall -Wextra -o bin/crescent.exe
if ($LASTEXITCODE -eq 0) {
    $width = $Host.UI.RawUI.WindowSize.Width
    Write-Host ("=" * $width) -ForegroundColor Green
    Write-Host "build successful" -ForegroundColor Black -BackgroundColor Green
    Write-Host ("=" * $width) -ForegroundColor Green
    ./bin/crescent.exe
} else {
    Write-Host "compilation failed" -ForegroundColor Red -BackgroundColor DarkRed
}