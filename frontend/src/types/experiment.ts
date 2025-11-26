export type ExperimentType = 'tightness' | 'nnUnsorted' | 'nnSorted';

export type WindowType = 'optimal' | 'fixed' | 'percentage';

export interface ExperimentConfig {
  experimentTypes: {
    tightness: boolean;
    nnUnsorted: boolean;
    nnSorted: boolean;
  };
  windowConfig: {
    type: WindowType;
    value?: number; // Para fixed o percentage
    multiple?: number[]; // Para múltiples porcentajes [1, 10, 20]
  };
  bounds: string[];
  datasets: string[];
}

export interface BoundResult {
  time: number;
  timeStdDev: number;
  pruned: number;
  accuracy: number;
  tightness?: number;
}

export interface ExperimentResult {
  dataset: string;
  bounds: {
    [boundName: string]: BoundResult;
  };
  window: number;
  experimentType: ExperimentType;
  timestamp: string;
}

export interface ExperimentResponse {
  experimentId: string;
  status: 'running' | 'completed' | 'error';
  results?: ExperimentResult[];
  error?: string;
  progress?: number;
}

