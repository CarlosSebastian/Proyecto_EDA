# Arquitectura Frontend - Comparación Múltiple de Lower Bounds

## 🎯 Objetivos

1. **Comparar múltiples Lower Bounds** (no solo 2)
2. **Mostrar gráficas de tightness relativo** (como Figuras 1 y 2 del paper)
3. **Generar CSVs desde el frontend** (ejecutar experimentos)
4. **Visualizar qué LB es más rápida** en cada dataset

---

## 📊 Tipos de Gráficas Necesarias

### 1. Gráfica de Tightness Relativo (Como Paper Figuras 1 y 2)

**Formato del Paper:**
- Eje X: Datasets
- Eje Y: Tightness relativo (ratio entre bounds)
- Múltiples líneas/barras para cada comparación

**Ejemplo:**
```
Tightness Relativo = LB_Webb / LB_Keogh
- Si > 1.0: LB_Webb es más tight
- Si < 1.0: LB_Keogh es más tight
```

**Visualización:**
- Gráfico de barras agrupadas o líneas
- Mostrar ratio para cada dataset
- Colores diferentes por bound

### 2. Gráfica de Tiempos Comparativos

**Formato:**
- Eje X: Datasets
- Eje Y: Tiempo (ms)
- Múltiples series (una por cada bound seleccionado)

**Visualización:**
- Gráfico de barras agrupadas
- O gráfico de líneas para ver tendencias

### 3. Gráfica de Series Podadas

**Formato:**
- Eje X: Datasets
- Eje Y: Número de series podadas
- Múltiples series por bound

### 4. Tabla Comparativa Interactiva

**Columnas:**
- Dataset
- LB_Keogh (tiempo, pruned, accuracy)
- LB_Webb (tiempo, pruned, accuracy)
- LB_Petitjean (tiempo, pruned, accuracy)
- ... (dinámico según selección)

**Funcionalidades:**
- Ordenamiento por cualquier columna
- Filtros
- Exportación a CSV/Excel

---

## 🏗️ Arquitectura de Datos

### Estructura de Datos Principal

```typescript
interface ExperimentResult {
  dataset: string;
  bounds: {
    [boundName: string]: {
      time: number;           // ms
      timeStdDev: number;     // desviación estándar
      pruned: number;         // series podadas
      accuracy: number;       // 0-1
      tightness?: number;     // tightness promedio (si se calcula)
    }
  };
  window: number;             // ventana usada
  timestamp: string;          // fecha/hora del experimento
}

interface ComparisonData {
  experiments: ExperimentResult[];
  selectedBounds: string[];   // bounds seleccionados para comparar
  selectedDatasets: string[]; // datasets seleccionados
}
```

### Cálculo de Tightness Relativo

```typescript
// Tightness relativo entre dos bounds
function calculateRelativeTightness(
  bound1: string,
  bound2: string,
  data: ExperimentResult[]
): Array<{dataset: string, ratio: number}> {
  return data.map(exp => {
    const b1 = exp.bounds[bound1];
    const b2 = exp.bounds[bound2];
    
    if (!b1 || !b2 || !b1.tightness || !b2.tightness) {
      return { dataset: exp.dataset, ratio: 1.0 };
    }
    
    // Ratio: si > 1.0, bound1 es más tight
    return {
      dataset: exp.dataset,
      ratio: b1.tightness / b2.tightness
    };
  });
}
```

---

## 🔧 Flujo de Trabajo

### Opción A: Generar CSV desde Frontend (Recomendado)

```
1. Usuario selecciona:
   - Bounds a comparar (múltiple selección)
   - Datasets a usar
   - Configuración de ventana

2. Frontend envía request a API:
   POST /api/experiment/run
   {
     bounds: ["keogh", "webb", "petitjean"],
     datasets: ["50words", "Adiac", "CBF"],
     window: -2,  // -2 = óptima, número = fija, porcentaje = g
     windowPercent: false
   }

3. Backend (C++ API):
   - Ejecuta TSTester.exe con parámetros
   - Genera CSVs en directorio temporal
   - Lee CSVs y convierte a JSON
   - Devuelve resultados

4. Frontend:
   - Recibe JSON con resultados
   - Genera CSV localmente (opcional)
   - Muestra visualizaciones
   - Guarda en historial
```

### Opción B: Cargar CSV Existente

```
1. Usuario sube archivos CSV o selecciona de historial
2. Frontend parsea CSVs
3. Convierte a estructura de datos interna
4. Muestra visualizaciones
```

---

## 🎨 Componentes React Necesarios

### 1. ExperimentConfigurator
```typescript
interface ExperimentConfiguratorProps {
  onRun: (config: ExperimentConfig) => void;
  onLoad: (files: File[]) => void;
}

// Permite:
// - Seleccionar múltiples bounds (checkboxes)
// - Seleccionar múltiples datasets (checkboxes)
// - Configurar ventana (óptima/fija/porcentaje)
// - Cargar CSVs existentes
```

### 2. BoundSelector (Multi-select)
```typescript
// Checkboxes para:
// - Keogh
// - Improved
// - Enhanced (con parámetro k)
// - Petitjean
// - Webb
// - EnhancedWebb (con parámetro k)
```

### 3. RelativeTightnessChart
```typescript
interface RelativeTightnessChartProps {
  data: ExperimentResult[];
  baseline: string;  // Bound base para comparación (ej: "Keogh")
  compareTo: string[]; // Bounds a comparar
}

// Muestra gráfica tipo paper (Figura 1 o 2)
// - Barras o líneas mostrando ratio
// - Línea de referencia en 1.0
```

### 4. TimeComparisonChart
```typescript
// Gráfico de barras/líneas comparando tiempos
// Múltiples series (una por bound)
```

### 5. ResultsTable
```typescript
// Tabla interactiva con:
// - Ordenamiento
// - Filtros
// - Exportación
// - Resaltado de mejor/worse
```

### 6. ExperimentRunner
```typescript
// Componente que:
// - Muestra progreso de ejecución
// - Llama a API
// - Maneja errores
// - Actualiza estado
```

---

## 🔌 API Backend (C++)

### Endpoint: POST /api/experiment/run

**Request:**
```json
{
  "bounds": ["keogh", "webb", "petitjean"],
  "datasets": ["50words", "Adiac", "CBF"],
  "window": -2,
  "windowPercent": false,
  "experimentType": "UNSORTED"  // UNSORTED, SORTED, SHUFFLED
}
```

**Response:**
```json
{
  "experimentId": "exp_123456",
  "status": "completed",
  "results": [
    {
      "dataset": "50words",
      "bounds": {
        "Keogh": {
          "time": 450.916,
          "timeStdDev": 18.5751,
          "pruned": 190016,
          "accuracy": 0.764835
        },
        "Webb": {
          "time": 172.18,
          "timeStdDev": 8.27684,
          "pruned": 199782,
          "accuracy": 0.764835
        }
      },
      "window": 16
    }
  ],
  "timestamp": "2024-11-24T10:30:00Z"
}
```

### Implementación Backend (cpp-httplib)

```cpp
// server.cpp (ejemplo básico)
#include "httplib.h"
#include <fstream>
#include <sstream>
#include <filesystem>

void runExperiment(const httplib::Request& req, httplib::Response& res) {
    // Parse JSON request
    // Construir comando para TSTester.exe
    // Ejecutar y capturar output
    // Leer CSVs generados
    // Convertir a JSON
    // Devolver respuesta
}
```

---

## 📁 Estructura de Archivos Frontend

```
frontend/
├── src/
│   ├── components/
│   │   ├── experiment/
│   │   │   ├── ExperimentConfigurator.tsx
│   │   │   ├── BoundSelector.tsx
│   │   │   ├── DatasetSelector.tsx
│   │   │   └── ExperimentRunner.tsx
│   │   ├── charts/
│   │   │   ├── RelativeTightnessChart.tsx  // ⭐ Gráfica tipo paper
│   │   │   ├── TimeComparisonChart.tsx
│   │   │   ├── PrunedComparisonChart.tsx
│   │   │   └── AccuracyChart.tsx
│   │   ├── tables/
│   │   │   ├── ResultsTable.tsx
│   │   │   └── ComparisonTable.tsx
│   │   └── layout/
│   │       ├── Dashboard.tsx
│   │       └── Sidebar.tsx
│   ├── services/
│   │   ├── api.ts              // Llamadas a backend
│   │   ├── csvParser.ts        // Parsear CSVs
│   │   ├── csvGenerator.ts     // Generar CSVs desde datos
│   │   └── dataProcessor.ts    // Calcular tightness, ratios, etc.
│   ├── hooks/
│   │   ├── useExperiment.ts
│   │   ├── useComparison.ts
│   │   └── useResults.ts
│   ├── types/
│   │   ├── experiment.ts
│   │   ├── bounds.ts
│   │   └── results.ts
│   └── utils/
│       ├── calculations.ts     // Cálculos de tightness relativo
│       └── formatters.ts
```

---

## 🎯 Flujo de Usuario

### Escenario 1: Comparar Webb y Petitjean vs Otros

1. **Seleccionar bounds:**
   - ✅ Webb
   - ✅ Petitjean
   - ✅ Keogh (como baseline)
   - ✅ Improved

2. **Seleccionar datasets:**
   - ✅ 50words
   - ✅ Adiac
   - ✅ CBF
   - ✅ FaceAll
   - ✅ Two_Patterns

3. **Configurar:**
   - Ventana: Óptima (-W)
   - Tipo: UNSORTED

4. **Ejecutar:**
   - Click "Run Experiment"
   - Ver progreso
   - Esperar resultados

5. **Visualizar:**
   - **Gráfica 1**: Tightness relativo (Webb/Keogh, Petitjean/Keogh)
   - **Gráfica 2**: Tiempos comparativos
   - **Tabla**: Todos los datos

6. **Exportar:**
   - Generar CSV con resultados
   - Descargar gráficas como PNG

---

## 💡 Cálculos Específicos

### Tightness Relativo

```typescript
// Para cada dataset, calcular ratio entre bounds
function calculateTightnessRatios(
  results: ExperimentResult[],
  baseline: string,
  compareTo: string[]
): TightnessRatioData[] {
  return results.map(exp => {
    const baselineData = exp.bounds[baseline];
    const ratios: Record<string, number> = {};
    
    compareTo.forEach(bound => {
      const compareData = exp.bounds[bound];
      if (baselineData?.tightness && compareData?.tightness) {
        // Ratio: > 1.0 significa que compareData es más tight
        ratios[bound] = compareData.tightness / baselineData.tightness;
      }
    });
    
    return {
      dataset: exp.dataset,
      ratios
    };
  });
}
```

### Mejora de Tiempo

```typescript
function calculateTimeImprovement(
  baseline: string,
  compare: string,
  results: ExperimentResult[]
): Array<{dataset: string, improvement: number}> {
  return results.map(exp => {
    const baselineTime = exp.bounds[baseline]?.time || 0;
    const compareTime = exp.bounds[compare]?.time || 0;
    
    if (baselineTime === 0) return { dataset: exp.dataset, improvement: 0 };
    
    // Porcentaje de mejora: positivo = más rápido
    const improvement = ((baselineTime - compareTime) / baselineTime) * 100;
    
    return { dataset: exp.dataset, improvement };
  });
}
```

---

## 🚀 Plan de Implementación

### Fase 1: MVP Básico
1. ✅ Cargar CSVs existentes
2. ✅ Parsear y mostrar tabla básica
3. ✅ Gráfico de tiempos simple (2 bounds)

### Fase 2: Múltiples Bounds
1. ✅ Selector multi-bound
2. ✅ Gráficos con múltiples series
3. ✅ Tabla comparativa completa

### Fase 3: Tightness Relativo
1. ✅ Calcular tightness desde datos
2. ✅ Gráfica de tightness relativo (tipo paper)
3. ✅ Comparaciones dinámicas

### Fase 4: Generación desde Frontend
1. ✅ API backend (C++)
2. ✅ Ejecutar experimentos desde UI
3. ✅ Generar CSVs en frontend
4. ✅ Historial de experimentos

---

## 📝 Notas Importantes

1. **Tightness**: Si no está en los CSVs, se puede calcular como `pruned / total_series` o usar el tightness del test (-t)

2. **CSV Generation**: El frontend puede generar CSVs desde los datos JSON usando `Papa.unparse()`

3. **API Backend**: Necesitará ejecutar `TSTester.exe` y leer los CSVs generados

4. **Visualización Paper**: Las Figuras 1 y 2 muestran tightness relativo, que es la métrica clave para comparar bounds

¿Quieres que empiece a crear la estructura base del frontend con estos componentes?

