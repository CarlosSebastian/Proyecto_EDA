# ✅ Verificación del Sistema - Estado Actual

## Estado de Componentes

### ✅ Backend API
- **Estado**: ✅ FUNCIONANDO
- **Puerto**: 8081
- **Proceso**: Corriendo (PID: 36032)
- **Endpoints verificados**:
  - ✅ `GET /api/health` - Responde correctamente
  - ✅ `GET /api/datasets` - Devuelve 85 datasets
  - ✅ `POST /api/experiment/run` - Acepta peticiones

### ✅ Frontend
- **Estado**: ✅ FUNCIONANDO
- **Puerto**: 3000
- **URL**: http://localhost:3000
- **Proceso**: Corriendo (PIDs: 1472, 27824)

### ✅ TSTester.exe
- **Estado**: ✅ EXISTE
- **Ubicación**: `src/bin/TSTester.exe`

### ✅ Datasets
- **Estado**: ✅ DISPONIBLES
- **Cantidad**: 85 datasets del UCR_TS_Archive_2015

## 📋 Instrucciones de Uso

### 1. Iniciar el Sistema

Si los procesos no están corriendo, ejecuta:

**Terminal 1 - Backend:**
```powershell
cd backend
.\server.exe
```

**Terminal 2 - Frontend:**
```powershell
cd frontend
npm run dev
```

### 2. Usar la Aplicación

1. Abre tu navegador en: **http://localhost:3000**

2. **Configurar Experimento:**
   - Selecciona los **Lower Bounds** a comparar (checkboxes)
   - Selecciona los **datasets** (autocomplete)
   - Configura el **tipo de experimento**:
     - ☑️ Tightness Test
     - ☑️ 1-NN UNSORTED
     - ☑️ 1-NN SORTED
   - Configura la **ventana**:
     - Óptima (recomendado)
     - Fija (ej: 10)
     - Porcentaje (ej: 1%, 10%, 20%)

3. **Ejecutar:**
   - Haz clic en **"Ejecutar Experimento"**
   - ⏳ Espera a que se generen los resultados (puede tardar varios minutos dependiendo del número de datasets)

4. **Ver Resultados:**
   - Los resultados aparecerán automáticamente en la pestaña "Resultados"
   - Verás gráficas de:
     - Tightness Relativo
     - Comparación de Tiempos
     - Series Podadas
   - Tabla interactiva con todas las métricas

5. **Exportar:**
   - Haz clic en **"Exportar CSVs"** para descargar todos los archivos CSV

## 🔍 Solución de Problemas

### El experimento no genera resultados

**Posibles causas:**
1. **TSTester tarda en ejecutarse**: Los experimentos pueden tardar varios minutos, especialmente con múltiples datasets
2. **Ruta con espacios**: El sistema maneja rutas con espacios automáticamente
3. **Permisos**: Asegúrate de tener permisos de escritura en `src/resultados_api/`

**Verificar:**
- Revisa la consola del backend para ver los logs de ejecución
- Verifica que `src/resultados_api/` se crea después de ejecutar un experimento
- Revisa que los CSVs se generan en subdirectorios con timestamps

### El frontend no se conecta al backend

**Verificar:**
- Backend está corriendo en puerto 8081
- Frontend está configurado para usar `http://localhost:8081/api`
- No hay firewall bloqueando las conexiones

### Error al compilar el backend

**Solución:**
```powershell
cd backend
# Asegúrate de tener httplib.h
if (-not (Test-Path httplib\httplib.h)) {
    .\download_httplib.ps1
}
# Compilar
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32
```

## 📊 Flujo de Datos

1. **Frontend** → Envía configuración JSON a `POST /api/experiment/run`
2. **Backend** → Parsea JSON y construye comando para `TSTester.exe`
3. **Backend** → Ejecuta `TSTester.exe` con los parámetros
4. **TSTester.exe** → Genera CSVs en `src/resultados_api/[timestamp]/`
5. **Backend** → Lee los CSVs y los convierte a JSON
6. **Backend** → Devuelve JSON al frontend
7. **Frontend** → Procesa resultados y muestra gráficas/tablas

## ✅ Pruebas Realizadas

- ✅ Backend responde en puerto 8081
- ✅ Endpoint `/api/health` funciona
- ✅ Endpoint `/api/datasets` devuelve 85 datasets
- ✅ Endpoint `/api/experiment/run` acepta peticiones
- ✅ Frontend se conecta al backend
- ✅ Procesos están corriendo correctamente

## 🎯 Próximos Pasos

1. Ejecuta un experimento pequeño desde el frontend (1-2 datasets)
2. Verifica que los resultados aparecen en la pestaña "Resultados"
3. Revisa las gráficas de comparación
4. Exporta los CSVs para verificar que se generan correctamente

---

**Fecha de verificación**: $(Get-Date -Format "yyyy-MM-dd HH:mm:ss")
**Sistema**: Windows 10
**Estado general**: ✅ FUNCIONAL

