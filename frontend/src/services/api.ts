import axios from 'axios';
import type { ExperimentConfig, ExperimentResponse } from '@/types/experiment';

const API_BASE_URL = import.meta.env.VITE_API_URL || 'http://localhost:8081/api';

const api = axios.create({
  baseURL: API_BASE_URL,
  timeout: 300000, // 5 minutos para experimentos largos
});

export const experimentApi = {
  /**
   * Ejecuta un experimento
   */
  async runExperiment(config: ExperimentConfig): Promise<ExperimentResponse> {
    const response = await api.post<ExperimentResponse>('/experiment/run', config);
    return response.data;
  },

  /**
   * Obtiene el estado de un experimento en ejecución
   */
  async getExperimentStatus(experimentId: string): Promise<ExperimentResponse> {
    const response = await api.get<ExperimentResponse>(`/experiment/${experimentId}`);
    return response.data;
  },

  /**
   * Obtiene lista de datasets disponibles
   */
  async getAvailableDatasets(): Promise<string[]> {
    const response = await api.get<string[]>('/datasets');
    return response.data;
  },
};

export default api;

