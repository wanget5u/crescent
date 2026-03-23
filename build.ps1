Clear-Host
if (!(Test-Path -Path "build")) { New-Item -ItemType Directory -Path "build" | Out-Null }
if (!(Test-Path -Path "bin")) { New-Item -ItemType Directory -Path "bin" | Out-Null }
Write-Host "crescent - compiling..."
$includes = @("-I./include", "-I./include/raylib")
$defines  = @("-D_CRT_SECURE_NO_WARNINGS", "-DIMGUI_HAS_DOCK")
$warnings = @("-Wall", "-Wextra")
$libs     = @("-L./lib", "-lraylib", "-lwinmm", "-lgdi32", "-luser32", "-lshell32")
$game_files = Get-ChildItem "src/*.c"
$header_files = Get-ChildItem -Path "src", "include" -Filter *.h -Recurse
$newest_header_time = $null
if ($header_files) {
    $newest_header_time = ($header_files | Measure-Object -Property LastWriteTime -Maximum).Maximum
}
$needs_linking = $false
foreach ($file in $game_files) {
    $objPath = "build/$($file.BaseName).o"
    $compile = $false
    if (!(Test-Path $objPath)) {
        $compile = $true
    } else {
        $obj_time = (Get-Item $objPath).LastWriteTime
        $c_time = (Get-Item $file.FullName).LastWriteTime
        if ($c_time -gt $obj_time -or ($null -ne $newest_header_time -and $newest_header_time -gt $obj_time)) {
            $compile = $true
        }
    }
    if ($compile) {
        Write-Host "[+] Compiling: $($file.Name)..."
        & clang -x c -c $file.FullName -O3 $includes $defines $warnings -o $objPath
        $needs_linking = $true
    }
}
if ($needs_linking -or !(Test-Path "bin/crescent.exe")) {
    Write-Host "[~] linking crescent.exe..."
    $all_objs = Get-ChildItem "build/*.o" | ForEach-Object { $_.FullName }
    & clang $all_objs $libs -o bin/crescent.exe
    if ($LASTEXITCODE -ne 0) {
        Write-Host "linking failed."
        exit
    }
} else {
    Write-Host "[v] code up to date, skipping compilation."
}
if (Test-Path "assets") {
    Write-Host "[~] syncing assets..."
    $null = robocopy "assets" "bin/assets" /MIR /NJH /NJS /NFL /NDL /NC /NS /NP
}
$width = $Host.UI.RawUI.WindowSize.Width
Write-Host ("=" * $width) -ForegroundColor Green
Write-Host "BUILD SUCCESSFUL" -ForegroundColor Black -BackgroundColor Green
Write-Host ("=" * $width) -ForegroundColor Green
./bin/crescent.exe
