import Papa from 'papaparse';
import type { ExperimentResult, BoundResult, ExperimentType } from '@/types/experiment';

interface CSVRow {
  [key: string]: string;
}

/**
 * Parsea un CSV de resultados y lo convierte a estructura de datos
 */
export function parseResultsCSV(
  csvContent: string,
  dataset: string,
  experimentType: ExperimentType,
  window: number
): ExperimentResult | null {
  const parsed = Papa.parse<CSVRow>(csvContent, {
    header: true,
    skipEmptyLines: true,
  });

  if (parsed.errors.length > 0) {
    console.error('Error parsing CSV:', parsed.errors);
    return null;
  }

  if (parsed.data.length === 0) {
    return null;
  }

  // El CSV tiene formato: dataset,bound1,bound2,bound3,...
  const row = parsed.data.find(r => r[Object.keys(r)[0]] === dataset);
  if (!row) {
    return null;
  }

  const bounds: { [key: string]: BoundResult } = {};
  const keys = Object.keys(row);
  
  // Primera columna es el dataset, el resto son bounds
  for (let i = 1; i < keys.length; i++) {
    const boundName = keys[i];
    const value = parseFloat(row[boundName]);
    
    if (!isNaN(value)) {
      // Determinar qué tipo de métrica es según el nombre del archivo
      bounds[boundName] = {
        time: 0,
        timeStdDev: 0,
        pruned: 0,
        accuracy: 0,
        tightness: 0,
      };
      
      // Esto se completará cuando se parseen todos los CSVs
      // Por ahora solo guardamos el valor
    }
  }

  return {
    dataset,
    bounds,
    window,
    experimentType,
    timestamp: new Date().toISOString(),
  };
}

/**
 * Parsea múltiples CSVs y los combina en un solo resultado
 */
export function parseMultipleCSVs(
  csvFiles: { content: string; type: 'times' | 'pruned' | 'accuracy' | 'tightness' | 'timeDev' }[],
  datasets: string[],
  experimentType: ExperimentType,
  window: number
): ExperimentResult[] {
  const results: { [dataset: string]: ExperimentResult } = {};

  datasets.forEach(dataset => {
    results[dataset] = {
      dataset,
      bounds: {},
      window,
      experimentType,
      timestamp: new Date().toISOString(),
    };
  });

  csvFiles.forEach(file => {
    const parsed = Papa.parse<CSVRow>(file.content, {
      header: true,
      skipEmptyLines: true,
    });

    parsed.data.forEach(row => {
      const dataset = row[Object.keys(row)[0]];
      if (!results[dataset]) return;

      const keys = Object.keys(row);
      for (let i = 1; i < keys.length; i++) {
        const boundName = keys[i];
        const value = parseFloat(row[boundName]);
        
        if (!isNaN(value)) {
          if (!results[dataset].bounds[boundName]) {
            results[dataset].bounds[boundName] = {
              time: 0,
              timeStdDev: 0,
              pruned: 0,
              accuracy: 0,
            };
          }

          switch (file.type) {
            case 'times':
              results[dataset].bounds[boundName].time = value;
              break;
            case 'timeDev':
              results[dataset].bounds[boundName].timeStdDev = value;
              break;
            case 'pruned':
              results[dataset].bounds[boundName].pruned = value;
              break;
            case 'accuracy':
              results[dataset].bounds[boundName].accuracy = value;
              break;
            case 'tightness':
              results[dataset].bounds[boundName].tightness = value;
              break;
          }
        }
      }
    });
  });

  return Object.values(results);
}

