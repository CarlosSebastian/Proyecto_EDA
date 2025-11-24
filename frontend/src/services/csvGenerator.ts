import Papa from 'papaparse';
import type { ExperimentResult } from '@/types/experiment';

/**
 * Genera un CSV desde los resultados del experimento
 */
export function generateResultsCSV(
  results: ExperimentResult[],
  metric: 'time' | 'pruned' | 'accuracy' | 'tightness' | 'timeStdDev'
): string {
  if (results.length === 0) return '';

  // Obtener todos los bounds únicos
  const allBounds = new Set<string>();
  results.forEach(r => {
    Object.keys(r.bounds).forEach(bound => allBounds.add(bound));
  });

  const boundsArray = Array.from(allBounds);

  // Crear filas
  const rows = results.map(result => {
    const row: { [key: string]: string | number } = {
      dataset: result.dataset,
    };

    boundsArray.forEach(bound => {
      const boundData = result.bounds[bound];
      if (boundData) {
        row[bound] = boundData[metric] ?? 0;
      } else {
        row[bound] = 0;
      }
    });

    return row;
  });

  // Generar CSV
  return Papa.unparse(rows);
}

/**
 * Genera múltiples CSVs para todas las métricas
 */
export function generateAllCSVs(results: ExperimentResult[]): {
  times: string;
  pruned: string;
  accuracy: string;
  tightness?: string;
  timeDev: string;
} {
  return {
    times: generateResultsCSV(results, 'time'),
    pruned: generateResultsCSV(results, 'pruned'),
    accuracy: generateResultsCSV(results, 'accuracy'),
    tightness: results.some(r => 
      Object.values(r.bounds).some(b => b.tightness !== undefined)
    ) ? generateResultsCSV(results, 'tightness') : undefined,
    timeDev: generateResultsCSV(results, 'timeStdDev'),
  };
}

/**
 * Descarga un CSV
 */
export function downloadCSV(content: string, filename: string): void {
  const blob = new Blob([content], { type: 'text/csv;charset=utf-8;' });
  const link = document.createElement('a');
  const url = URL.createObjectURL(blob);
  
  link.setAttribute('href', url);
  link.setAttribute('download', filename);
  link.style.visibility = 'hidden';
  
  document.body.appendChild(link);
  link.click();
  document.body.removeChild(link);
  
  URL.revokeObjectURL(url);
}

