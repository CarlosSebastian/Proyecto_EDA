# Script para ejecutar comparación de Lower Bounds en datasets UCR
# Basado en el paper Webb2021.pdf
# Compara LB_Webb vs LB_Improved (como en la Figura 2 del paper)

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Comparación de Lower Bounds DTW" -ForegroundColor Cyan
Write-Host "Basado en Webb2021.pdf - Figura 2" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Configuración
$exePath = "src\bin\TSTester.exe"
$datasetDir = "src\UCR_TS_Archive_2015"
$outputDir = "results_webb_vs_improved"
$bounds = "webb,improved"  # Comparación de la Figura 2 del paper

# Seleccionar 10+ datasets representativos con ventanas óptimas
$datasets = @(
    "50words",
    "Adiac",
    "BeetleFly",
    "BirdChicken",
    "CBF",
    "CinC_ECG_torso",
    "Coffee",
    "ECG200",
    "FaceFour",
    "Gun_Point",
    "ItalyPowerDemand",
    "Lighting2"
)

Write-Host "Datasets seleccionados:" -ForegroundColor Yellow
foreach ($ds in $datasets) {
    Write-Host "  - $ds" -ForegroundColor Gray
}
Write-Host ""

# Verificar que el ejecutable existe
if (-not (Test-Path $exePath)) {
    Write-Host "ERROR: No se encuentra el ejecutable en $exePath" -ForegroundColor Red
    Write-Host "Por favor, compila el proyecto primero con: make" -ForegroundColor Yellow
    exit 1
}

# Verificar que el directorio de datasets existe
if (-not (Test-Path $datasetDir)) {
    Write-Host "ERROR: No se encuentra el directorio de datasets en $datasetDir" -ForegroundColor Red
    exit 1
}

# Crear directorio de resultados
if (-not (Test-Path $outputDir)) {
    New-Item -ItemType Directory -Path $outputDir | Out-Null
    Write-Host "Directorio de resultados creado: $outputDir" -ForegroundColor Green
}

# Convertir array de datasets a string separado por comas
$datasetsStr = $datasets -join ","

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "Iniciando comparación..." -ForegroundColor Cyan
Write-Host "Bounds: $bounds" -ForegroundColor Yellow
Write-Host "Window: Óptima para cada dataset (-W)" -ForegroundColor Yellow
Write-Host "Experimento: Nearest Neighbor (UNSORTED)" -ForegroundColor Yellow
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Ejecutar el programa con los parámetros
$arguments = @(
    "-W",
    "-b$bounds",
    "-d$datasetsStr",
    "-D$datasetDir",
    "-n$outputDir"
)

Write-Host "Ejecutando: $exePath $($arguments -join ' ')" -ForegroundColor Gray
Write-Host ""

# Ejecutar
$process = Start-Process -FilePath $exePath -ArgumentList $arguments -Wait -NoNewWindow -PassThru

if ($process.ExitCode -eq 0) {
    Write-Host ""
    Write-Host "========================================" -ForegroundColor Green
    Write-Host "Comparación completada exitosamente!" -ForegroundColor Green
    Write-Host "========================================" -ForegroundColor Green
    Write-Host ""
    Write-Host "Resultados guardados en: $outputDir" -ForegroundColor Yellow
    
    # Listar archivos de resultados
    $resultFiles = Get-ChildItem -Path $outputDir -File
    if ($resultFiles) {
        Write-Host ""
        Write-Host "Archivos de resultados generados:" -ForegroundColor Cyan
        foreach ($file in $resultFiles) {
            Write-Host "  - $($file.Name)" -ForegroundColor Gray
        }
    }
} else {
    Write-Host ""
    Write-Host "ERROR: El programa terminó con código de error: $($process.ExitCode)" -ForegroundColor Red
    exit $process.ExitCode
}


