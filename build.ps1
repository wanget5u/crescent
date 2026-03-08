if (!(Test-Path -Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }
if (!(Test-Path -Path "bin")) { New-Item -ItemType Directory -Path "bin" | Out-Null }

Write-Host "clang compile..." -ForegroundColor Cyan

clang src/*.c -I./include -D_CRT_SECURE_NO_WARNINGS -Wall -Wextra -o bin/crescent.exe

if ($LASTEXITCODE -eq 0) {
    Write-Host "Build successful! Running..." -ForegroundColor Green
    ./bin/crescent.exe
} else {
    Write-Host "Compilation error." -ForegroundColor Red
}