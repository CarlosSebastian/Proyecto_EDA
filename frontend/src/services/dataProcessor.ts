import type { ExperimentResult } from '@/types/experiment';

/**
 * Calcula tightness relativo entre dos bounds
 */
export interface TightnessRatio {
  dataset: string;
  ratio: number;
}

export function calculateRelativeTightness(
  results: ExperimentResult[],
  baseline: string,
  compareTo: string
): TightnessRatio[] {
  return results
    .map(exp => {
      const baselineData = exp.bounds[baseline];
      const compareData = exp.bounds[compareTo];

      if (!baselineData?.tightness || !compareData?.tightness) {
        return null;
      }

      // Ratio: si > 1.0, compareTo es más tight
      return {
        dataset: exp.dataset,
        ratio: compareData.tightness / baselineData.tightness,
      };
    })
    .filter((r): r is TightnessRatio => r !== null);
}

/**
 * Calcula mejora de tiempo porcentual
 */
export interface TimeImprovement {
  dataset: string;
  improvement: number; // Porcentaje: positivo = más rápido
}

export function calculateTimeImprovement(
  results: ExperimentResult[],
  baseline: string,
  compare: string
): TimeImprovement[] {
  return results
    .map(exp => {
      const baselineTime = exp.bounds[baseline]?.time || 0;
      const compareTime = exp.bounds[compare]?.time || 0;

      if (baselineTime === 0) {
        return { dataset: exp.dataset, improvement: 0 };
      }

      // Porcentaje de mejora: positivo = más rápido
      const improvement = ((baselineTime - compareTime) / baselineTime) * 100;

      return { dataset: exp.dataset, improvement };
    })
    .filter(r => r !== null);
}

/**
 * Encuentra el mejor bound para cada métrica
 */
export function findBestBound(
  results: ExperimentResult[],
  metric: 'time' | 'pruned' | 'accuracy' | 'tightness'
): { [dataset: string]: string } {
  const best: { [dataset: string]: string } = {};

  results.forEach(exp => {
    let bestBound = '';
    let bestValue = metric === 'time' ? Infinity : -Infinity;

    Object.entries(exp.bounds).forEach(([boundName, boundData]) => {
      const value = boundData[metric];
      if (value === undefined) return;

      const isBetter =
        metric === 'time'
          ? value < bestValue
          : metric === 'accuracy' || metric === 'tightness'
          ? value > bestValue
          : metric === 'pruned'
          ? value > bestValue
          : false;

      if (isBetter) {
        bestValue = value;
        bestBound = boundName;
      }
    });

    if (bestBound) {
      best[exp.dataset] = bestBound;
    }
  });

  return best;
}

