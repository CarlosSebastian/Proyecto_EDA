# Backend API Server

Servidor HTTP en C++ que ejecuta TSTester.exe y devuelve resultados en JSON.

## 📋 Dependencias

- **cpp-httplib**: Librería HTTP ligera (se descarga automáticamente)
- **C++17**: Compilador compatible

## 🚀 Compilación

```bash
cd backend
make
```

Esto descargará automáticamente `httplib.h` si no existe.

## ▶️ Ejecución

```bash
make run
# o
./server
```

El servidor estará disponible en `http://localhost:8080`

## 🔌 Endpoints

### POST /api/experiment/run
Ejecuta un experimento y devuelve resultados.

**Request:**
```json
{
  "experimentTypes": {
    "tightness": true,
    "nnUnsorted": true,
    "nnSorted": false
  },
  "windowConfig": {
    "type": "optimal",
    "value": null,
    "multiple": null
  },
  "bounds": ["keogh", "webb"],
  "datasets": ["50words", "Adiac"]
}
```

**Response:**
```json
{
  "experimentId": "1234567890",
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
      "timestamp": "1234567890"
    }
  ]
}
```

### GET /api/datasets
Obtiene lista de datasets disponibles.

**Response:**
```json
["50words", "Adiac", "CBF", ...]
```

### GET /api/health
Health check del servidor.

**Response:**
```json
{"status": "ok"}
```

## 📝 Notas

- El servidor ejecuta `TSTester.exe` en `../src/bin/`
- Los resultados se guardan en `../src/resultados_api/`
- Los CSVs se generan automáticamente y se convierten a JSON

## 🔧 Configuración

Edita las constantes en `server.cpp`:
- `TSTESTER_PATH`: Ruta a TSTester.exe
- `DATASET_DIR`: Directorio de datasets UCR
- `RESULTS_DIR`: Directorio donde se guardan resultados

