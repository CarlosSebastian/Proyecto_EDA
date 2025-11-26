# Cómo Funciona el Frontend - Explicación Detallada

## 🎯 Flujo General

```
Usuario → Configura Experimento → Frontend envía a API → Backend ejecuta TSTester.exe 
→ Genera CSVs → Backend lee CSVs → Devuelve JSON → Frontend muestra resultados
```

---

## 📋 Paso a Paso

### 1. **Configuración del Experimento** (Tab "Configuración")

El usuario configura qué quiere comparar:

#### A. Tipos de Experimento
- ☑️ **Tightness Test** (-t): Calcula tightness promedio
- ☑️ **1-NN UNSORTED**: Nearest Neighbor sin ordenar (10 iteraciones)
- ☑️ **1-NN SORTED**: Nearest Neighbor ordenado (1 iteración)

#### B. Configuración de Ventana
- **Óptima** (-W): Usa ventana óptima para cada dataset
- **Fija** (-w): Tamaño fijo (ej: 10)
- **Porcentaje** (-g): Porcentaje de longitud (ej: 1%, 10%, 20%)

#### C. Lower Bounds a Comparar
- Selecciona múltiples bounds (checkboxes):
  - Keogh
  - Improved
  - Enhanced (con parámetro k)
  - Petitjean
  - Webb
  - EnhancedWebb (con parámetro k)

#### D. Datasets
- Autocomplete para seleccionar datasets del UCR_TS_Archive
- Puedes seleccionar múltiples (ej: 50words, Adiac, CBF)

---

### 2. **Ejecución** (Click "Ejecutar Experimento")

Cuando el usuario hace click:

```typescript
// 1. Frontend valida la configuración
if (no bounds seleccionados) → Error
if (no datasets seleccionados) → Error
if (no tipos de experimento) → Error

// 2. Construye el objeto de configuración
const config = {
  experimentTypes: {
    tightness: true,
    nnUnsorted: true,
    nnSorted: false
  },
  windowConfig: {
    type: 'optimal',
    value: undefined
  },
  bounds: ['keogh', 'webb', 'petitjean'],
  datasets: ['50words', 'Adiac', 'CBF']
}

// 3. Envía a la API
POST /api/experiment/run
Body: config
```

---

### 3. **Backend Procesa** (API C++)

El backend recibe la configuración y:

```cpp
// 1. Construye comandos para TSTester.exe
// Si tightness está activo:
TSTester.exe -t -bkeogh,webb,petitjean -W -d50words,Adiac,CBF

// Si nnUnsorted está activo:
TSTester.exe -bkeogh,webb,petitjean -W -d50words,Adiac,CBF

// Si nnSorted está activo:
TSTester.exe -s -bkeogh,webb,petitjean -W -d50words,Adiac,CBF

// 2. Ejecuta cada comando
// 3. TSTester.exe genera CSVs automáticamente:
//    - tightness-W.csv
//    - times-W-nosort.csv
//    - pruned-W-nosort.csv
//    - accuracy-W-nosort.csv
//    - times-W-sort.csv
//    - etc.

// 4. Backend lee los CSVs generados
// 5. Convierte a JSON
// 6. Devuelve al frontend
```

---

### 4. **Frontend Recibe Resultados**

El frontend recibe JSON con esta estructura:

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
          "accuracy": 0.764835,
          "tightness": 0.85
        },
        "Webb": {
          "time": 172.18,
          "timeStdDev": 8.27684,
          "pruned": 199782,
          "accuracy": 0.764835,
          "tightness": 0.92
        }
      },
      "window": 16,
      "experimentType": "nnUnsorted",
      "timestamp": "2024-11-24T10:30:00Z"
    }
  ]
}
```

---

### 5. **Visualización** (Tab "Resultados")

El frontend muestra los resultados en diferentes visualizaciones:

#### A. Gráfica de Tightness Relativo ⭐ (Como Paper)

```typescript
// Calcula tightness relativo
// Para cada dataset:
ratio = Webb.tightness / Keogh.tightness

// Si ratio > 1.0 → Webb es más tight
// Si ratio < 1.0 → Keogh es más tight

// Muestra gráfica de barras con:
// - Eje X: Datasets
// - Eje Y: Ratio
// - Línea de referencia en 1.0
```

**Ejemplo visual:**
```
Dataset    | Keogh | Webb | Ratio
-----------|-------|------|-------
50words    | 0.85  | 0.92 | 1.08  ← Webb más tight
Adiac      | 0.80  | 0.88 | 1.10  ← Webb más tight
CBF        | 0.90  | 0.95 | 1.06  ← Webb más tight
```

#### B. Gráfica de Tiempos Comparativos

```typescript
// Muestra barras comparando tiempos
// Para cada dataset, una barra por cada bound
```

**Ejemplo visual:**
```
Dataset    | Keogh (ms) | Webb (ms) | Mejora
-----------|------------|-----------|-------
50words    | 450.92     | 172.18    | 61.8% más rápido
Adiac      | 641.34     | 299.09    | 53.4% más rápido
```

#### C. Gráfica de Series Podadas

```typescript
// Muestra cuántas series se podaron (pruned)
// Más podadas = bound más tight
```

#### D. Tabla Interactiva

```typescript
// Tabla con todas las métricas
// - Ordenamiento por cualquier columna
// - Filtros
// - Resaltado de mejor/worse
```

---

## 🔄 Flujo Completo con Ejemplo

### Ejemplo: Comparar Webb y Petitjean vs Keogh

**1. Usuario configura:**
```
Tipos: Tightness + 1-NN UNSORTED
Ventana: Óptima
Bounds: Keogh, Webb, Petitjean
Datasets: 50words, Adiac, CBF
```

**2. Frontend envía:**
```json
POST /api/experiment/run
{
  "experimentTypes": {
    "tightness": true,
    "nnUnsorted": true,
    "nnSorted": false
  },
  "windowConfig": {
    "type": "optimal"
  },
  "bounds": ["keogh", "webb", "petitjean"],
  "datasets": ["50words", "Adiac", "CBF"]
}
```

**3. Backend ejecuta:**
```bash
# Tightness test
TSTester.exe -t -bkeogh,webb,petitjean -W -d50words,Adiac,CBF

# 1-NN UNSORTED
TSTester.exe -bkeogh,webb,petitjean -W -d50words,Adiac,CBF
```

**4. TSTester.exe genera:**
```
resultados/
  ├── tightness-W.csv
  ├── times-W-nosort.csv
  ├── pruned-W-nosort.csv
  ├── accuracy-W-nosort.csv
  └── time-dev-W-nosort.csv
```

**5. Backend lee CSVs y convierte:**
```json
{
  "results": [
    {
      "dataset": "50words",
      "bounds": {
        "Keogh": { "time": 450.92, "pruned": 190016, "tightness": 0.85 },
        "Webb": { "time": 172.18, "pruned": 199782, "tightness": 0.92 },
        "Petitjean": { "time": 200.50, "pruned": 205000, "tightness": 0.95 }
      }
    }
  ]
}
```

**6. Frontend muestra:**

**Gráfica Tightness Relativo:**
- Baseline: Keogh
- Compara: Webb/Keogh = 1.08 (Webb más tight)
- Compara: Petitjean/Keogh = 1.12 (Petitjean más tight)

**Gráfica Tiempos:**
- Keogh: 450.92 ms
- Webb: 172.18 ms (61.8% más rápido)
- Petitjean: 200.50 ms (55.5% más rápido)

**Tabla:**
- Todas las métricas ordenables y filtrables

---

## 🎨 Componentes Clave

### ExperimentConfigurator
- **Función**: Permite configurar todo el experimento
- **Input**: Selecciones del usuario
- **Output**: Objeto `ExperimentConfig`

### RelativeTightnessChart
- **Función**: Muestra tightness relativo (tipo paper)
- **Input**: Resultados + baseline + bounds a comparar
- **Output**: Gráfica de barras con ratios

### TimeComparisonChart
- **Función**: Compara tiempos de ejecución
- **Input**: Resultados + bounds seleccionados
- **Output**: Gráfica de barras comparativa

### ResultsTable
- **Función**: Tabla interactiva con todas las métricas
- **Input**: Resultados
- **Output**: Tabla ordenable y filtrable

---

## 💾 Exportación de CSVs

Cuando el usuario hace click en "Exportar CSVs":

```typescript
// 1. Toma los resultados en memoria
// 2. Genera CSVs usando generateAllCSVs()
// 3. Descarga cada CSV:
downloadCSV(times, 'times.csv')
downloadCSV(pruned, 'pruned.csv')
downloadCSV(accuracy, 'accuracy.csv')
downloadCSV(tightness, 'tightness.csv')
```

---

## 🔌 Integración con Backend

### Endpoint Esperado

```
POST /api/experiment/run
Content-Type: application/json

Request:
{
  "experimentTypes": { ... },
  "windowConfig": { ... },
  "bounds": ["keogh", "webb"],
  "datasets": ["50words", "Adiac"]
}

Response:
{
  "experimentId": "exp_123",
  "status": "completed",
  "results": [ ... ]
}
```

### Qué debe hacer el Backend

1. Recibir configuración JSON
2. Construir comandos para `TSTester.exe`
3. Ejecutar `TSTester.exe` (puede tomar minutos)
4. Leer CSVs generados
5. Parsear CSVs a JSON
6. Devolver JSON al frontend

---

## 🎯 Ventajas del Sistema

1. **Automatización completa**: No necesitas crear CSVs manualmente
2. **Comparación múltiple**: Puedes comparar varios bounds a la vez
3. **Visualizaciones tipo paper**: Gráficas como las del paper Webb2021
4. **Interactividad**: Tablas ordenables, filtros, exportación
5. **Flexibilidad**: Configura cualquier combinación de experimentos

---

## 📝 Resumen en 3 Pasos

1. **Configuras**: Seleccionas bounds, datasets, tipos de experimento
2. **Ejecutas**: El sistema ejecuta TSTester.exe automáticamente
3. **Visualizas**: Ves gráficas y tablas comparativas como en el paper

¿Alguna parte específica que quieras que explique con más detalle?

