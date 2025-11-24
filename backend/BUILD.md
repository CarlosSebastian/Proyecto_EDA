# Guía de Compilación del Backend

## 📋 Requisitos

- **C++ Compilador**: g++ (MinGW en Windows) o cl (Visual Studio)
- **C++17**: Compatible con C++17
- **Windows**: Windows 10/11 (para las librerías de red)

## 🚀 Compilación

### Opción 1: Usando Make (Recomendado)

```bash
cd backend
make
```

Esto descargará automáticamente `httplib.h` si no existe.

### Opción 2: Descargar httplib manualmente

```powershell
# Ejecutar script de descarga
.\download_httplib.ps1

# O manualmente:
# Crear carpeta httplib
# Descargar https://raw.githubusercontent.com/yhirose/cpp-httplib/master/httplib.h
# Guardar en httplib/httplib.h
```

### Opción 3: Compilación manual

```bash
g++ -std=c++17 -O2 -I. -I./httplib server.cpp -o server.exe -lws2_32 -lwsock32
```

## ▶️ Ejecución

```bash
make run
# o
./server.exe
```

El servidor estará disponible en `http://localhost:8080`

## 🔧 Configuración

Edita las constantes en `server.cpp` si necesitas cambiar las rutas:

```cpp
const std::string TSTESTER_PATH = "../src/bin/TSTester.exe";
const std::string DATASET_DIR = "../src/UCR_TS_Archive_2015";
const std::string RESULTS_DIR = "../src/resultados_api";
```

## ⚠️ Notas

- Asegúrate de que `TSTester.exe` esté compilado antes de ejecutar el servidor
- El servidor necesita permisos para ejecutar `TSTester.exe`
- Los resultados se guardan en `../src/resultados_api/`

