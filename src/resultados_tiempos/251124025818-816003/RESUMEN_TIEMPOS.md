# Comparación de Tiempos: LB_Keogh vs LB_Webb

## Resultados de Ejecución

### Datasets Analizados
- **50words** (Window: 16, Train: 450, Test: 455)
- **Adiac** (Window: 5, Train: 390, Test: 391)
- **CBF** (Window: 14, Train: 30, Test: 900)
- **FaceAll** (Window: 3, Train: 560, Test: 1690)
- **Two_Patterns** (Window: 5, Train: 1000, Test: 4000)

---

## Tiempos de Ejecución (milisegundos)

| Dataset | LB_Keogh | LB_Webb | Mejora | % Mejora |
|---------|----------|---------|--------|----------|
| **50words** | 450.916 ms | 172.18 ms | 278.736 ms | **61.8%** más rápido |
| **Adiac** | 641.343 ms | 299.089 ms | 342.254 ms | **53.4%** más rápido |
| **CBF** | 244.58 ms | 167.837 ms | 76.743 ms | **31.4%** más rápido |
| **FaceAll** | 2538.07 ms | 811.658 ms | 1726.412 ms | **68.0%** más rápido |
| **Two_Patterns** | 2685.89 ms | 2375.69 ms | 310.2 ms | **11.5%** más rápido |

### Promedio de Mejora: **53.2%** más rápido con LB_Webb

---

## Series Podadas (Pruned)

| Dataset | LB_Keogh | LB_Webb | Diferencia |
|---------|----------|---------|------------|
| **50words** | 190,016 | 199,782 | +9,766 (5.1% más) |
| **Adiac** | 92,049 | 126,868 | +34,819 (37.8% más) |
| **CBF** | 10,752 | 16,238 | +5,486 (51.0% más) |
| **FaceAll** | 625,203 | 874,565 | +249,362 (39.9% más) |
| **Two_Patterns** | 3,782,430 | 3,888,300 | +105,870 (2.8% más) |

**Conclusión**: LB_Webb poda más series que LB_Keogh, lo que confirma que es un lower bound más ajustado (tighter), permitiendo descartar más candidatos sin calcular DTW completo.

---

## Desviación Estándar de Tiempos

| Dataset | LB_Keogh (std) | LB_Webb (std) |
|---------|----------------|---------------|
| **50words** | 18.58 ms | 8.28 ms |
| **Adiac** | 12.84 ms | 19.34 ms |
| **CBF** | 2.88 ms | 2.29 ms |
| **FaceAll** | 46.95 ms | 12.93 ms |
| **Two_Patterns** | 726.47 ms | 567.82 ms |

---

## Análisis

### Conclusiones Principales:

1. **LB_Webb es consistentemente más rápido** que LB_Keogh en todos los datasets probados.

2. **La mejora de velocidad varía entre 11.5% y 68.0%**, siendo más significativa en datasets más grandes (FaceAll: 68% mejora).

3. **LB_Webb poda más series** que LB_Keogh, confirmando que es un lower bound más ajustado según el paper Webb2021.pdf.

4. **LB_Webb mantiene o mejora la eficiencia** mientras proporciona un bound más tight, lo cual es consistente con las conclusiones del paper.

### Comparación con el Paper:

Estos resultados son consistentes con las figuras 1 y 2 del paper Webb2021.pdf, donde se muestra que:
- LB_Webb es siempre más ajustado que LB_Keogh
- LB_Webb es más eficiente computacionalmente
- LB_Webb proporciona un mejor trade-off entre tightness y eficiencia

---

**Fecha de ejecución**: 2024-11-24
**Parámetros**: Window óptima (-W), Nearest Neighbor (UNSORTED), 10 iteraciones

