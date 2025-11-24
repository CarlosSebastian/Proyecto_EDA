# Guía de Configuración del Frontend

## 📦 Instalación

1. **Instalar dependencias:**
```bash
cd frontend
npm install
```

2. **Configurar variables de entorno (opcional):**
```bash
cp .env.example .env
# Editar .env si necesitas cambiar la URL de la API
```

## 🚀 Ejecutar

```bash
npm run dev
```

El frontend estará disponible en `http://localhost:3000`

## 🔧 Próximos Pasos

### 1. Crear API Backend

El frontend necesita una API backend que:
- Ejecute `TSTester.exe` con los parámetros
- Genere CSVs automáticamente
- Devuelva resultados en formato JSON

**Ejemplo de implementación con cpp-httplib:**

```cpp
// server.cpp
#include "httplib.h"
#include <fstream>
#include <sstream>
#include <filesystem>

void runExperiment(const httplib::Request& req, httplib::Response& res) {
    // Parse JSON request
    // Construir comando: TSTester.exe -bkeogh,webb -W -d50words,Adiac
    // Ejecutar y capturar output
    // Leer CSVs generados
    // Convertir a JSON
    // Devolver respuesta
}
```

### 2. Probar sin Backend (Modo Mock)

Puedes modificar `src/services/api.ts` para usar datos mock mientras desarrollas el backend:

```typescript
// Modo mock para desarrollo
const MOCK_MODE = true;

if (MOCK_MODE) {
  // Retornar datos de ejemplo
}
```

### 3. Cargar CSVs Existentes

Si ya tienes CSVs generados, puedes implementar la función de carga en `Dashboard.tsx`:

```typescript
const handleLoadCSV = async () => {
  // Usar input file para seleccionar CSVs
  // Parsear con parseMultipleCSVs
  // Actualizar estado de resultados
};
```

## 📝 Notas

- El frontend está listo para recibir datos del backend
- Todas las visualizaciones están implementadas
- La exportación de CSV funciona desde los datos en memoria
- Falta implementar el backend API (C++)

