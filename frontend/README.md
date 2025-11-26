# DTW Bounds Comparison Frontend

Frontend para comparar Lower Bounds de Dynamic Time Warping basado en el paper Webb2021.pdf.

## 🚀 Inicio Rápido

### Instalación

```bash
cd frontend
npm install
```

### Desarrollo

```bash
npm run dev
```

Abre [http://localhost:3000](http://localhost:3000) en tu navegador.

### Build

```bash
npm run build
```

## 📋 Características

- ✅ Comparación múltiple de Lower Bounds
- ✅ Gráficas de tightness relativo (tipo paper)
- ✅ Comparación de tiempos de ejecución
- ✅ Visualización de series podadas
- ✅ Tablas interactivas con ordenamiento
- ✅ Exportación a CSV
- ✅ Ejecución automática de experimentos

## 🏗️ Estructura

```
frontend/
├── src/
│   ├── components/
│   │   ├── experiment/      # Configuración de experimentos
│   │   ├── charts/          # Gráficas de visualización
│   │   ├── tables/          # Tablas de resultados
│   │   └── layout/          # Layout principal
│   ├── services/            # API, CSV parser/generator
│   ├── types/               # Tipos TypeScript
│   └── App.tsx              # Componente principal
```

## 🔌 API Backend

El frontend espera una API backend en `http://localhost:8080/api` con los siguientes endpoints:

- `POST /api/experiment/run` - Ejecutar experimento
- `GET /api/experiment/:id` - Estado del experimento
- `GET /api/datasets` - Lista de datasets disponibles

## 📝 Notas

- El backend debe ejecutar `TSTester.exe` y generar CSVs automáticamente
- Los resultados se muestran en tiempo real cuando están disponibles
- Se pueden exportar todos los CSVs generados

