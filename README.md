# Proyecto EDA - Comparación de Lower Bounds DTW

Proyecto para comparar diferentes Lower Bounds de Dynamic Time Warping (DTW) basado en el paper "Tight lower bounds for Dynamic Time Warping" de Webb y Petitjean (2021).

## ⚡ Inicio Rápido (Resumen)

```powershell
# 1. Compilar TSTester
cd src
make

# 2. Compilar Backend API
cd backend
.\download_httplib.ps1
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32

# 3. Instalar Frontend
cd frontend
npm install

# 4. Ejecutar (en DOS terminales separadas)
# Terminal 1 - Backend:
cd backend
.\server.exe

# Terminal 2 - Frontend:
cd frontend
npm run dev

# 5. Abrir navegador: http://localhost:3000
```

📖 **Para instrucciones detalladas, ve a la sección [🚀 Cómo Ejecutar el Proyecto Completo](#-cómo-ejecutar-el-proyecto-completo)**

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

## 🚀 Cómo Ejecutar el Proyecto Completo

Sigue estos pasos en orden para ejecutar todo el sistema:

### 📋 Requisitos Previos

- **Compilador C++**: g++ (MinGW) o Visual Studio C++
- **Node.js**: Versión 16 o superior
- **PowerShell**: Para ejecutar scripts (Windows)
- **Git**: Para clonar el repositorio (opcional)

### 🔨 Paso 1: Compilar TSTester.exe

El backend API necesita `TSTester.exe` compilado para ejecutar los experimentos.

**En PowerShell:**
```powershell
# Navegar al directorio src
cd src

# Compilar TSTester
make

# Si no tienes make, compila manualmente:
# g++ -std=c++17 -O3 $(Get-ChildItem -Recurse -Filter "*.cpp") -o bin/TSTester.exe
```

**Verificar que se compiló correctamente:**
```powershell
# Debería existir el archivo
Test-Path bin\TSTester.exe
# Debe mostrar: True
```

✅ **Resultado esperado**: `src/bin/TSTester.exe` debe existir

---

### 🔨 Paso 2: Compilar el Backend API Server

El Backend API es el servidor HTTP que ejecuta `TSTester.exe` automáticamente.

**En PowerShell:**
```powershell
# Navegar al directorio backend
cd backend

# Descargar httplib.h (si no existe)
if (-not (Test-Path httplib\httplib.h)) {
    .\download_httplib.ps1
}

# Compilar el servidor
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32
```

**Verificar que se compiló correctamente:**
```powershell
# Debería existir el archivo
Test-Path server.exe
# Debe mostrar: True
```

✅ **Resultado esperado**: `backend/server.exe` debe existir

---

### 📦 Paso 3: Instalar Dependencias del Frontend

**En PowerShell:**
```powershell
# Navegar al directorio frontend
cd frontend

# Instalar dependencias (primera vez puede tardar 3-5 minutos)
npm install
```

**Nota:** La primera vez puede tardar 3-5 minutos descargando ~310 paquetes. Esto es normal.

✅ **Resultado esperado**: Debe crearse la carpeta `node_modules/` con todas las dependencias

---

### 🚀 Paso 4: Ejecutar el Sistema Completo

Necesitas **DOS terminales PowerShell** abiertas simultáneamente:

#### Terminal 1: Backend API Server

```powershell
# Desde la raíz del proyecto
cd backend
.\server.exe
```

**Deberías ver:**
```
Servidor iniciado en http://localhost:8081
Endpoints disponibles:
  POST /api/experiment/run
  GET  /api/datasets
  GET  /api/health
```

⚠️ **IMPORTANTE**: Deja esta terminal abierta. El servidor debe seguir corriendo.

#### Terminal 2: Frontend

```powershell
# Desde la raíz del proyecto (en una nueva terminal)
cd frontend
npm run dev
```

**Deberías ver:**
```
  VITE v5.x.x  ready in xxx ms

  ➜  Local:   http://localhost:3000/
  ➜  Network: use --host to expose
```

⚠️ **IMPORTANTE**: Deja esta terminal abierta también. El frontend debe seguir corriendo.

---

### 🌐 Paso 5: Abrir la Aplicación en el Navegador

1. Abre tu navegador web (Chrome, Firefox, Edge, etc.)
2. Ve a: **http://localhost:3000**
3. Deberías ver la interfaz de la aplicación

---

### ✅ Verificación Rápida

Para verificar que todo está funcionando:

**Verificar Backend:**
```powershell
# En una nueva terminal
Invoke-WebRequest -Uri "http://localhost:8081/api/health" -UseBasicParsing
# Debe devolver: {"status":"ok"}
```

**Verificar Frontend:**
- Abre http://localhost:3000 en tu navegador
- Deberías ver la página de configuración de experimentos

---

### 📝 Resumen de Comandos Rápidos

```powershell
# 1. Compilar TSTester
cd src
make

# 2. Compilar Backend API
cd backend
.\download_httplib.ps1  # Solo si no existe httplib.h
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32

# 3. Instalar Frontend
cd frontend
npm install

# 4. Ejecutar (en dos terminales separadas)
# Terminal 1:
cd backend
.\server.exe

# Terminal 2:
cd frontend
npm run dev

# 5. Abrir navegador
# http://localhost:3000
```

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
```powershell
cd src
make
```

### Compilar Backend API
```powershell
cd backend
# Asegúrate de tener httplib.h primero
.\download_httplib.ps1
# Compilar
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32
```

### Ejecutar Backend API
```powershell
cd backend
.\server.exe
```

### Ejecutar Frontend
```powershell
cd frontend
npm run dev
```

### Verificar que los Servicios Están Corriendo

**Verificar Backend:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8081/api/health" -UseBasicParsing
# Debe devolver: {"status":"ok"}
```

**Verificar Datasets Disponibles:**
```powershell
Invoke-WebRequest -Uri "http://localhost:8081/api/datasets" -UseBasicParsing
# Debe devolver una lista JSON con los datasets
```

**Verificar Procesos:**
```powershell
# Ver si el backend está corriendo
Get-Process | Where-Object {$_.ProcessName -like "*server*"}

# Ver si el frontend está corriendo
Get-Process | Where-Object {$_.ProcessName -eq "node"}
```

### Detener los Servicios

**Detener Backend:**
```powershell
# Encontrar y detener el proceso
Get-Process | Where-Object {$_.ProcessName -like "*server*"} | Stop-Process
```

**Detener Frontend:**
```powershell
# Presiona Ctrl+C en la terminal donde está corriendo npm run dev
# O detener todos los procesos node:
Get-Process | Where-Object {$_.ProcessName -eq "node"} | Stop-Process
```

### Ejecutar Experimento Manualmente (sin frontend)

**Nota:** Normalmente NO necesitas hacer esto, ya que el backend API ejecuta TSTester.exe automáticamente. Solo úsalo si quieres probar directamente:

```powershell
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

**Causa:** El frontend no está ejecutándose.

**Solución:**
```powershell
# Verificar que el frontend esté corriendo
cd frontend
npm run dev

# Verificar que no haya errores en la terminal
# Debe mostrar: "Local: http://localhost:3000/"
```

---

### Error: "Connection refused" o "Failed to fetch" en el frontend

**Causa:** El backend API no está ejecutándose o está en un puerto diferente.

**Solución:**
```powershell
# 1. Verificar que el backend esté corriendo
cd backend
.\server.exe

# 2. Verificar que responda
Invoke-WebRequest -Uri "http://localhost:8081/api/health" -UseBasicParsing

# 3. Verificar que no haya otro proceso usando el puerto 8081
netstat -ano | findstr :8081
```

---

### Error al compilar TSTester

**Causa:** Falta el compilador o hay problemas con el Makefile.

**Solución:**
```powershell
# Verificar que g++ esté instalado
g++ --version

# Si no está instalado, instala MinGW-w64 o Visual Studio C++
# Luego compila manualmente:
cd src
g++ -std=c++17 -O3 $(Get-ChildItem -Recurse -Filter "*.cpp") -o bin/TSTester.exe
```

---

### Error al compilar backend API

**Causa:** Falta httplib.h o problemas con las librerías de Windows.

**Solución:**
```powershell
cd backend

# 1. Descargar httplib si no existe
if (-not (Test-Path httplib\httplib.h)) {
    .\download_httplib.ps1
}

# 2. Compilar con las flags correctas
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32

# 3. Si hay errores de Windows API, verifica que tengas las headers correctas
```

---

### Error al instalar dependencias del frontend

**Causa:** Node.js no está instalado o hay problemas con npm.

**Solución:**
```powershell
# 1. Verificar Node.js
node --version
npm --version

# 2. Si Node.js no está instalado, descárgalo de nodejs.org (v16+)

# 3. Limpiar caché e instalar de nuevo
cd frontend
npm cache clean --force
npm install
```

---

### Error: "cannot open output file server.exe: Permission denied"

**Causa:** El archivo `server.exe` está siendo usado por otro proceso.

**Solución:**
```powershell
# Detener procesos que puedan estar usando server.exe
Get-Process | Where-Object {$_.ProcessName -like "*server*"} | Stop-Process

# Esperar un segundo y compilar de nuevo
Start-Sleep -Seconds 1
cd backend
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32
```

---

### El experimento no genera resultados

**Causa:** TSTester.exe no se ejecuta correctamente o los CSVs no se generan.

**Solución:**
```powershell
# 1. Verificar que TSTester.exe existe
Test-Path src\bin\TSTester.exe

# 2. Verificar que los datasets existen
Test-Path src\UCR_TS_Archive_2015

# 3. Revisar los logs del backend (en la terminal donde está corriendo)
# Debe mostrar: "Comando: ..." y "Output: ..."

# 4. Verificar que se crea el directorio de resultados
Test-Path src\resultados_api
```

---

### Error: "El término 'make' no se reconoce"

**Causa:** `make` no está instalado en Windows.

**Solución:**
```powershell
# En Windows, compila manualmente con g++
cd src
g++ -std=c++17 -O3 $(Get-ChildItem -Recurse -Filter "*.cpp") -o bin/TSTester.exe
```

---

### El frontend muestra errores de conexión

**Causa:** El backend no está en el puerto correcto o hay problemas de CORS.

**Solución:**
```powershell
# 1. Verificar que el backend esté en el puerto 8081
# Revisa backend/server.cpp línea 648: svr.listen("localhost", 8081);

# 2. Verificar que el frontend esté configurado para usar el puerto correcto
# Revisa frontend/vite.config.ts y frontend/src/services/api.ts

# 3. Reiniciar ambos servicios
```

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
