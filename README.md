# Proyecto EDA - Comparación de Lower Bounds DTW

Proyecto para comparar diferentes Lower Bounds de Dynamic Time Warping (DTW) basado en el paper "Tight lower bounds for Dynamic Time Warping" de Webb y Petitjean (2021).

## 🏗️ Estructura del Proyecto

```
Proyecto_EDA/
├── src/                    # Código C++ (TSTester, Bounds, DTW)
│   ├── bin/               # Ejecutables compilados
│   ├── TSTester/          # Código principal
│   ├── distance/          # Implementación de bounds y DTW
│   └── UCR_TS_Archive_2015/  # Datasets UCR
├── backend/               # API Server en C++
│   └── server.exe         # Servidor HTTP (compilar primero)
└── frontend/              # Interfaz web React + TypeScript
    └── src/               # Código fuente del frontend
```

## 🚀 Inicio Rápido

### 1. Compilar el Backend C++ (TSTester)

**IMPORTANTE:** El backend API ejecuta `TSTester.exe` automáticamente, pero primero debes compilarlo.

```bash
cd src
make
# o si no tienes make:
g++ -std=c++17 -O3 $(find . -name "*.cpp") -o bin/TSTester
```

Esto genera `src/bin/TSTester.exe` que será ejecutado automáticamente por el backend API.

### 2. Compilar el Backend API Server

**El Backend API se encarga de:**
- ✅ Ejecutar `TSTester.exe` automáticamente con los parámetros del frontend
- ✅ Generar CSVs automáticamente
- ✅ Leer los CSVs y convertirlos a JSON
- ✅ Devolver resultados al frontend

**NO necesitas ejecutar TSTester.exe manualmente** - el backend lo hace por ti.

```bash
cd backend

# Descargar httplib (si no existe)
.\download_httplib.ps1

# Compilar
make
# o manualmente:
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32
```

Esto genera `backend/server.exe`

### 3. Instalar Dependencias del Frontend

```bash
cd frontend
npm install
```

**Nota:** La primera vez puede tardar 3-5 minutos descargando ~310 paquetes. Esto es normal.

### 4. Ejecutar el Sistema Completo

#### Terminal 1: Backend API Server
```bash
cd backend
.\server.exe
```

Deberías ver:
```
Servidor iniciado en http://localhost:8081
Endpoints disponibles:
  POST /api/experiment/run
  GET  /api/datasets
  GET  /api/health
```

#### Terminal 2: Frontend
```bash
cd frontend
npm run dev
```

Deberías ver:
```
  VITE v5.x.x  ready in xxx ms

  ➜  Local:   http://localhost:3000/
  ➜  Network: use --host to expose
```

### 5. Abrir en el Navegador

Abre `http://localhost:3000` en tu navegador.

---

## 📋 Uso de la Aplicación

### Configurar un Experimento

1. **Tipos de Experimento:**
   - ☑️ Tightness Test (-t): Calcula tightness promedio
   - ☑️ 1-NN UNSORTED: Nearest Neighbor sin ordenar (10 iteraciones)
   - ☑️ 1-NN SORTED: Nearest Neighbor ordenado (1 iteración)

2. **Configuración de Ventana:**
   - **Óptima (-W)**: Usa ventana óptima para cada dataset (recomendado)
   - **Fija (-w)**: Tamaño fijo (ej: 10)
   - **Porcentaje (-g)**: Porcentaje de longitud (ej: 1%, 10%, 20%)

3. **Lower Bounds a Comparar:**
   - Selecciona múltiples bounds (checkboxes):
     - LB_Keogh
     - LB_Improved
     - LB_Enhanced (con parámetro k)
     - LB_Petitjean
     - LB_Webb
     - LB_EnhancedWebb (con parámetro k)

4. **Datasets:**
   - Usa el autocomplete para seleccionar datasets del UCR_TS_Archive
   - Puedes seleccionar múltiples (ej: 50words, Adiac, CBF)

5. **Ejecutar:**
   - Click en "Ejecutar Experimento"
   - El **Backend API ejecutará TSTester.exe automáticamente** con los parámetros configurados
   - TSTester.exe generará los CSVs automáticamente
   - El Backend leerá los CSVs y los convertirá a JSON
   - Los resultados aparecerán en la pestaña "Resultados"

### Ver Resultados

En la pestaña "Resultados" verás:

- **Gráfica de Tightness Relativo**: Compara tightness entre bounds (tipo paper)
- **Gráfica de Tiempos**: Compara tiempos de ejecución
- **Gráfica de Series Podadas**: Compara cuántas series se podaron
- **Tabla Interactiva**: Todas las métricas ordenables y filtrables

### Exportar Resultados

Click en "Exportar CSVs" para descargar todos los resultados en formato CSV.

---

## 🔧 Comandos Útiles

### Compilar TSTester
```bash
cd src
make
```

### Compilar Backend API
```bash
cd backend
make
```

### Ejecutar Backend API
```bash
cd backend
.\server.exe
```

### Ejecutar Frontend
```bash
cd frontend
npm run dev
```

### Ejecutar Experimento Manualmente (sin frontend)

**Nota:** Normalmente NO necesitas hacer esto, ya que el backend API ejecuta TSTester.exe automáticamente. Solo úsalo si quieres probar directamente:

```bash
cd src
.\bin\TSTester.exe -bkeogh,webb -W -d50words,Adiac,CBF -D UCR_TS_Archive_2015 -n resultados
```

---

## 📊 Ejemplo de Uso

### Comparar Webb y Petitjean vs Keogh

1. En el frontend, selecciona:
   - Bounds: Keogh, Webb, Petitjean
   - Datasets: 50words, Adiac, CBF, FaceAll, Two_Patterns
   - Tipos: Tightness + 1-NN UNSORTED
   - Ventana: Óptima

2. Click "Ejecutar Experimento"

3. El sistema:
   - **Backend API** recibe la configuración del frontend
   - **Backend API** ejecuta `TSTester.exe` con los parámetros correctos
   - **TSTester.exe** genera los CSVs automáticamente
   - **Backend API** lee los CSVs y los convierte a JSON
   - **Frontend** muestra los resultados en gráficas

4. Verás:
   - Tightness relativo (Webb/Keogh, Petitjean/Keogh)
   - Tiempos comparativos
   - Series podadas
   - Tabla completa

---

## 🐛 Solución de Problemas

### Error: "No se puede acceder a este sitio" en localhost:3000
- Asegúrate de que el frontend esté ejecutándose: `cd frontend && npm run dev`

### Error: "Connection refused" en el frontend
- Verifica que el backend esté ejecutándose: `cd backend && .\server.exe`
- Verifica que esté en el puerto 8081 (no 8080)

### Error al compilar backend
- Asegúrate de tener g++ instalado
- Descarga httplib: `.\download_httplib.ps1`
- Verifica que estés en el directorio `backend`

### Error al instalar dependencias del frontend
- Asegúrate de tener Node.js instalado (v16+)
- Intenta: `npm cache clean --force` y luego `npm install`

---

## 📝 Referencias

- **Paper Base**: Webb, G. I., & Petitjean, F. (2021). Tight lower bounds for Dynamic Time Warping. *Pattern Recognition*.
- **UCR Time Series Archive**: https://www.cs.ucr.edu/~eamonn/time_series_data_2018/

---

## 🎯 Características

- ✅ Comparación múltiple de Lower Bounds
- ✅ Gráficas de tightness relativo (tipo paper)
- ✅ Comparación de tiempos de ejecución
- ✅ Visualización de series podadas
- ✅ Tablas interactivas con ordenamiento
- ✅ Exportación a CSV
- ✅ Ejecución automática de experimentos
- ✅ Interfaz web moderna y responsive

---

## 📦 Dependencias

### Backend C++
- C++17 compatible compiler (g++, clang, MSVC)
- Windows 10+ (para el servidor API)

### Frontend
- Node.js 16+
- npm o yarn

### Backend API
- cpp-httplib (se descarga automáticamente)

---

## 🔗 Puertos

- **Frontend**: http://localhost:3000
- **Backend API**: http://localhost:8081

---

## 📄 Licencia

Este proyecto está basado en el paper Webb2021.pdf y utiliza el UCR Time Series Archive.
