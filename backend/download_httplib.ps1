# Script para descargar cpp-httplib

Write-Host "Descargando cpp-httplib..." -ForegroundColor Cyan

if (-not (Test-Path "httplib")) {
    New-Item -ItemType Directory -Path "httplib" | Out-Null
}

$url = "https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h"
$output = "httplib/httplib.h"

try {
    Invoke-WebRequest -Uri $url -OutFile $output
    Write-Host "httplib.h descargado exitosamente!" -ForegroundColor Green
} catch {
    Write-Host "Error descargando httplib.h: $_" -ForegroundColor Red
    exit 1
}

