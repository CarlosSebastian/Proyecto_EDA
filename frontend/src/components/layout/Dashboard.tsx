import React, { useState } from 'react';
import {
  Container,
  Box,
  Typography,
  Tabs,
  Tab,
  Alert,
  CircularProgress,
  Button,
} from '@mui/material';
import { ExperimentConfigurator } from '../experiment/ExperimentConfigurator';
import { RelativeTightnessChart } from '../charts/RelativeTightnessChart';
import { TimeComparisonChart } from '../charts/TimeComparisonChart';
import { PrunedComparisonChart } from '../charts/PrunedComparisonChart';
import { ResultsTable } from '../tables/ResultsTable';
import { experimentApi } from '@/services/api';
import { generateAllCSVs, downloadCSV } from '@/services/csvGenerator';
import type { ExperimentConfig, ExperimentResult } from '@/types/experiment';

interface TabPanelProps {
  children?: React.ReactNode;
  index: number;
  value: number;
}

function TabPanel(props: TabPanelProps) {
  const { children, value, index, ...other } = props;

  return (
    <div
      role="tabpanel"
      hidden={value !== index}
      id={`tabpanel-${index}`}
      aria-labelledby={`tab-${index}`}
      {...other}
    >
      {value === index && <Box sx={{ py: 3 }}>{children}</Box>}
    </div>
  );
}

export const Dashboard: React.FC = () => {
  const [tabValue, setTabValue] = useState(0);
  const [results, setResults] = useState<ExperimentResult[]>([]);
  const [isLoading, setIsLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [selectedBounds, setSelectedBounds] = useState<string[]>([]);
  const [baseline, setBaseline] = useState<string>('keogh');

  const handleRunExperiment = async (config: ExperimentConfig) => {
    setIsLoading(true);
    setError(null);
    setSelectedBounds(config.bounds);

    try {
      console.log('Enviando configuración:', config);
      const response = await experimentApi.runExperiment(config);
      console.log('Respuesta recibida:', response);
      
      if (response.status === 'error') {
        setError(response.error || 'Error ejecutando experimento');
        setIsLoading(false);
        return;
      }

      if (response.results && response.results.length > 0) {
        setResults(response.results);
        setTabValue(1); // Cambiar a tab de resultados
      } else {
        setError('No se recibieron resultados del servidor');
      }
    } catch (err: any) {
      const errorMessage = err.response?.data?.error || err.message || 'Error al ejecutar experimento';
      setError(errorMessage);
      console.error('Error completo:', err);
      if (err.response) {
        console.error('Respuesta del servidor:', err.response.data);
      }
    } finally {
      setIsLoading(false);
    }
  };

  const handleExportCSV = () => {
    if (results.length === 0) {
      alert('No hay resultados para exportar');
      return;
    }

    const csvs = generateAllCSVs(results);
    
    downloadCSV(csvs.times, 'times.csv');
    downloadCSV(csvs.pruned, 'pruned.csv');
    downloadCSV(csvs.accuracy, 'accuracy.csv');
    if (csvs.tightness) {
      downloadCSV(csvs.tightness, 'tightness.csv');
    }
    downloadCSV(csvs.timeDev, 'time-dev.csv');
  };

  const handleLoadCSV = () => {
    // TODO: Implementar carga de CSV
    alert('Funcionalidad de carga de CSV próximamente');
  };

  const boundsInResults = results.length > 0 
    ? Object.keys(results[0].bounds)
    : selectedBounds;

  const compareToBounds = boundsInResults.filter(b => b !== baseline);

  return (
    <Container maxWidth="xl" sx={{ py: 4 }}>
      <Typography variant="h4" component="h1" gutterBottom>
        DTW Bounds Comparison Tool
      </Typography>
      <Typography variant="body1" color="text.secondary" paragraph>
        Herramienta para comparar Lower Bounds de Dynamic Time Warping basada en Webb2021.pdf
      </Typography>

      {error && (
        <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
          {error}
        </Alert>
      )}

      <Box sx={{ borderBottom: 1, borderColor: 'divider', mb: 3 }}>
        <Tabs value={tabValue} onChange={(_, newValue) => setTabValue(newValue)}>
          <Tab label="Configuración" />
          <Tab label="Resultados" disabled={results.length === 0} />
        </Tabs>
      </Box>

      <TabPanel value={tabValue} index={0}>
        {isLoading ? (
          <Box sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', minHeight: 400 }}>
            <Box sx={{ textAlign: 'center' }}>
              <CircularProgress size={60} />
              <Typography variant="h6" sx={{ mt: 2 }}>
                Ejecutando experimento...
              </Typography>
              <Typography variant="body2" color="text.secondary" sx={{ mt: 1 }}>
                Esto puede tomar varios minutos dependiendo de los datasets seleccionados
              </Typography>
            </Box>
          </Box>
        ) : (
          <ExperimentConfigurator
            onRun={handleRunExperiment}
            onLoadCSV={handleLoadCSV}
            isLoading={isLoading}
          />
        )}
      </TabPanel>

      <TabPanel value={tabValue} index={1}>
        {results.length > 0 && (
          <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
            <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
              <Typography variant="h5">Resultados del Experimento</Typography>
              <Button variant="outlined" onClick={handleExportCSV}>
                Exportar CSVs
              </Button>
            </Box>

            {results.some(r => Object.values(r.bounds).some(b => b.tightness !== undefined)) && (
              <RelativeTightnessChart
                results={results}
                baseline={baseline}
                compareTo={compareToBounds}
              />
            )}

            <TimeComparisonChart results={results} bounds={boundsInResults} />
            <PrunedComparisonChart results={results} bounds={boundsInResults} />
            <ResultsTable results={results} bounds={boundsInResults} />
          </Box>
        )}
      </TabPanel>
    </Container>
  );
};

