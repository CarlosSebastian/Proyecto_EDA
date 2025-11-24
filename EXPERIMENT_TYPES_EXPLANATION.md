# Tipos de Experimentos y Generación de CSVs

## 📋 Respuestas a tus Preguntas

### 1. ¿Tengo que crear manualmente cada CSV o lo creará la página?

**Respuesta: La página lo creará automáticamente** ✅

**Cómo funcionará:**

#### Opción A: Ejecución desde Frontend (Recomendado)
```
1. Usuario configura experimento en la UI
2. Frontend envía request a API backend
3. Backend ejecuta TSTester.exe con parámetros
4. TSTester.exe genera CSVs automáticamente
5. Backend lee CSVs y devuelve JSON al frontend
6. Frontend muestra resultados y puede generar CSV si lo necesita
```

**Ventajas:**
- ✅ No necesitas crear CSVs manualmente
- ✅ Todo automatizado
- ✅ Historial guardado automáticamente

#### Opción B: Cargar CSV Existente (Alternativa)
```
1. Si ya tienes CSVs generados, puedes cargarlos
2. Frontend los parsea y muestra
3. Útil para resultados previos
```

---

### 2. ¿Qué tipos de experimentos debería hacer?

**SÍ, debería hacer todos estos tests** según el paper:

## 🧪 Tipos de Experimentos del Paper

### A. Test de Tightness (-t)
**Qué hace:**
- Calcula tightness promedio de cada bound
- Tightness = bound_value / true_DTW_distance
- Muestra qué tan ajustado es cada bound

**Cuándo usar:**
- Para comparar tightness entre bounds
- Para generar gráficas tipo Figuras 1 y 2 del paper

**Comando:**
```bash
TSTester.exe -t -bkeogh,webb,petitjean -W -d50words,Adiac
```

**Output:**
- `tightness-W.csv` con valores de tightness promedio

---

### B. 1-NN con Ventana Óptima, UNSORTED (default)
**Qué hace:**
- Ejecuta Nearest Neighbor (1-NN)
- Sin ordenar las series de entrenamiento
- 10 iteraciones (promedio)
- Usa ventana óptima para cada dataset

**Cuándo usar:**
- Para comparar tiempos de ejecución
- Para ver cuántas series se podan
- Para medir precisión (accuracy)

**Comando:**
```bash
TSTester.exe -bkeogh,webb -W -d50words,Adiac
# o explícitamente:
TSTester.exe -bkeogh,webb -W -d50words,Adiac  # UNSORTED es default
```

**Output:**
- `times-W-nosort.csv`
- `pruned-W-nosort.csv`
- `accuracy-W-nosort.csv`
- `time-dev-W-nosort.csv`

---

### C. 1-NN con Ventana Óptima, SORTED (-s)
**Qué hace:**
- Ejecuta Nearest Neighbor (1-NN)
- Ordena las series de entrenamiento por lower bound
- 1 iteración (ya está ordenado)
- Usa ventana óptima

**Cuándo usar:**
- Para ver el efecto de ordenar por bound
- Comparar eficiencia con/sin ordenamiento

**Comando:**
```bash
TSTester.exe -s -bkeogh,webb -W -d50words,Adiac
```

**Output:**
- `times-W-sort.csv`
- `pruned-W-sort.csv`
- `accuracy-W-sort.csv`
- `time-dev-W-sort.csv`

---

### D. Ventanas Fijas (1%, 10%, 20%) (-g)
**Qué hace:**
- Ejecuta con ventana como porcentaje de la longitud
- Útil para ver cómo afecta el tamaño de ventana

**Cuándo usar:**
- Análisis de sensibilidad a tamaño de ventana
- Comparar diferentes configuraciones

**Comando:**
```bash
# Ventana 1%
TSTester.exe -g1 -bkeogh,webb -d50words,Adiac

# Ventana 10%
TSTester.exe -g10 -bkeogh,webb -d50words,Adiac

# Ventana 20%
TSTester.exe -g20 -bkeogh,webb -d50words,Adiac
```

**Output:**
- `times-g1-nosort.csv`
- `times-g10-nosort.csv`
- `times-g20-nosort.csv`
- etc.

---

## 🎯 Qué Debería Hacer el Frontend

### Configuración de Experimentos

El frontend debería permitir seleccionar:

1. **Tipo de Experimento:**
   - ☑️ Tightness Test
   - ☑️ 1-NN UNSORTED
   - ☑️ 1-NN SORTED
   - ☑️ Múltiples tipos (ejecutar todos)

2. **Configuración de Ventana:**
   - ☑️ Óptima (-W)
   - ☑️ Fija (-w número)
   - ☑️ Porcentaje (-g 1, 10, 20, etc.)
   - ☑️ Múltiples ventanas (ejecutar todos)

3. **Bounds a Comparar:**
   - ☑️ Multi-select (Keogh, Webb, Petitjean, Improved, etc.)

4. **Datasets:**
   - ☑️ Multi-select de datasets disponibles

---

## 📊 Flujo Completo Propuesto

### Escenario: Comparar Webb y Petitjean vs Otros

**Paso 1: Configurar Experimento**
```
Tipo: Tightness + 1-NN UNSORTED + 1-NN SORTED
Ventana: Óptima
Bounds: Keogh, Webb, Petitjean, Improved
Datasets: 50words, Adiac, CBF, FaceAll, Two_Patterns
```

**Paso 2: Ejecutar (Automático)**
```
Frontend → API → Ejecuta 3 experimentos:
1. Tightness test
2. 1-NN UNSORTED
3. 1-NN SORTED
```

**Paso 3: Resultados**
```
- tightness-W.csv
- times-W-nosort.csv
- times-W-sort.csv
- pruned-W-nosort.csv
- pruned-W-sort.csv
- accuracy-W-nosort.csv
- accuracy-W-sort.csv
```

**Paso 4: Visualizar**
```
- Gráfica de tightness relativo (desde tightness-W.csv)
- Gráfica de tiempos comparativos (desde times-*.csv)
- Tabla comparativa completa
```

---

## 🔧 Implementación en Frontend

### Componente: ExperimentConfigurator

```typescript
interface ExperimentConfig {
  // Tipo de experimento
  experimentTypes: {
    tightness: boolean;
    nnUnsorted: boolean;
    nnSorted: boolean;
  };
  
  // Ventana
  windowConfig: {
    type: 'optimal' | 'fixed' | 'percentage';
    value?: number;  // Para fixed o percentage
    multiple?: number[];  // Para múltiples porcentajes [1, 10, 20]
  };
  
  // Bounds
  bounds: string[];  // ['keogh', 'webb', 'petitjean']
  
  // Datasets
  datasets: string[];
}
```

### Ejemplo de Uso

```typescript
const config: ExperimentConfig = {
  experimentTypes: {
    tightness: true,
    nnUnsorted: true,
    nnSorted: true
  },
  windowConfig: {
    type: 'optimal'  // -W
  },
  bounds: ['keogh', 'webb', 'petitjean'],
  datasets: ['50words', 'Adiac', 'CBF']
};

// Frontend envía a API
POST /api/experiment/run
{
  ...config
}

// API ejecuta:
// 1. TSTester.exe -t -bkeogh,webb,petitjean -W -d50words,Adiac,CBF
// 2. TSTester.exe -bkeogh,webb,petitjean -W -d50words,Adiac,CBF
// 3. TSTester.exe -s -bkeogh,webb,petitjean -W -d50words,Adiac,CBF
```

---

## ✅ Resumen

### Pregunta 1: ¿Crear CSVs manualmente?
**NO** - El frontend ejecutará TSTester.exe automáticamente y generará los CSVs.

### Pregunta 2: ¿Qué tests hacer?
**SÍ, todos:**
- ✅ Tightness test
- ✅ 1-NN UNSORTED (ventana óptima)
- ✅ 1-NN SORTED (ventana óptima)
- ✅ Ventanas fijas (1%, 10%, 20%) - opcional pero útil

### Beneficios:
1. **Automatización completa** - No necesitas crear CSVs manualmente
2. **Comparaciones completas** - Todos los tipos de experimentos del paper
3. **Flexibilidad** - Puedes elegir qué experimentos ejecutar
4. **Historial** - Todo se guarda automáticamente

---

## 🚀 Próximos Pasos

1. Frontend permite seleccionar tipos de experimento
2. Frontend envía configuración a API
3. API ejecuta TSTester.exe con parámetros correctos
4. API lee CSVs generados y devuelve JSON
5. Frontend muestra resultados y puede generar CSV si necesita exportar

¿Te parece bien esta aproximación?

