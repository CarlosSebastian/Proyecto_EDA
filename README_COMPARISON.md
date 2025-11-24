# Comparación de Lower Bounds DTW - Basado en Webb2021.pdf

Este proyecto implementa y compara diferentes Lower Bounds para Dynamic Time Warping (DTW) según el paper "Tight lower bounds for Dynamic Time Warping" de Webb y Petitjean (2021).

## Scripts de Comparación

### 1. `run_comparison.ps1`
Compara **LB_Webb vs LB_Keogh** (como en la Figura 1 del paper)
- Ejecuta 12 datasets del UCR_TS_Archive
- Usa ventanas óptimas para cada dataset
- Genera resultados en `results_comparison/`

### 2. `run_comparison_webb_vs_improved.ps1`
Compara **LB_Webb vs LB_Improved** (como en la Figura 2 del paper)
- Ejecuta 12 datasets del UCR_TS_Archive
- Usa ventanas óptimas para cada dataset
- Genera resultados en `results_webb_vs_improved/`

## Datasets Seleccionados

Los scripts ejecutan los siguientes 12 datasets del UCR_TS_Archive:

1. **50words** - Window: 16
2. **Adiac** - Window: 5
3. **BeetleFly** - Window: 35
4. **BirdChicken** - Window: 30
5. **CBF** - Window: 14
6. **CinC_ECG_torso** - Window: 16
7. **Coffee** - Window: 0
8. **ECG200** - Window: 0
9. **FaceFour** - Window: 7
10. **Gun_Point** - Window: 0
11. **ItalyPowerDemand** - Window: 0
12. **Lighting2** - Window: 38

## Uso

### Paso 1: Compilar el proyecto
```bash
cd src
make
```

### Paso 2: Ejecutar comparación LB_Webb vs LB_Keogh
```powershell
.\run_comparison.ps1
```

### Paso 3: Ejecutar comparación LB_Webb vs LB_Improved
```powershell
.\run_comparison_webb_vs_improved.ps1
```

## Resultados

Los resultados se guardan en archivos CSV con las siguientes métricas:

- **times-*.csv**: Tiempo promedio de ejecución
- **time-dev-*.csv**: Desviación estándar del tiempo
- **accuracy-*.csv**: Precisión de clasificación
- **pruned-*.csv**: Número de series podadas (pruned)

## Lower Bounds Implementados

El código incluye las siguientes implementaciones de Lower Bounds:

- **LB_Keogh**: Lower bound clásico de Keogh
- **LB_Improved**: Lower bound mejorado
- **LB_Enhanced**: Lower bound mejorado con parámetro k
- **LB_Petitjean**: Lower bound de Petitjean (más ajustado)
- **LB_Webb**: Lower bound de Webb (nuevo, más eficiente)
- **LB_EnhancedWebb**: Variante parametrizada de LB_Webb

## Referencia

Webb, G. I., & Petitjean, F. (2021). Tight lower bounds for Dynamic Time Warping. *Pattern Recognition*.

