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
  AppBar,
  Toolbar,
  Chip,
  Card,
  CardContent,
  Grid,
} from '@mui/material';
import ScienceIcon from '@mui/icons-material/Science';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import DownloadIcon from '@mui/icons-material/Download';
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
    <Box sx={{ minHeight: '100vh', bgcolor: 'background.default' }}>
      <AppBar 
        position="static" 
        elevation={0}
        sx={{ 
          background: 'linear-gradient(135deg, #6366f1 0%, #8b5cf6 100%)',
          borderBottom: '1px solid rgba(255, 255, 255, 0.1)',
        }}
      >
        <Toolbar>
          <ScienceIcon sx={{ mr: 2, fontSize: 32 }} />
          <Typography variant="h5" component="div" sx={{ flexGrow: 1, fontWeight: 700 }}>
            DTW Bounds Comparison
          </Typography>
          {results.length > 0 && (
            <Chip 
              label={`${results.length} resultados`} 
              color="secondary" 
              sx={{ bgcolor: 'rgba(255, 255, 255, 0.2)', color: 'white' }}
            />
          )}
        </Toolbar>
      </AppBar>

      <Container maxWidth="xl" sx={{ py: 4 }}>
        <Box sx={{ mb: 4 }}>
          <Typography variant="h4" component="h1" gutterBottom sx={{ fontWeight: 700, mb: 1 }}>
            Comparación de Lower Bounds DTW
          </Typography>
          <Typography variant="body1" color="text.secondary" sx={{ fontSize: '1.1rem' }}>
            Herramienta para comparar Lower Bounds de Dynamic Time Warping basada en el paper Webb2021.pdf
          </Typography>
        </Box>

        {error && (
          <Alert 
            severity="error" 
            sx={{ mb: 3, borderRadius: 2 }} 
            onClose={() => setError(null)}
          >
            {error}
          </Alert>
        )}

        <Card sx={{ mb: 3, borderRadius: 3 }}>
          <Box sx={{ borderBottom: 1, borderColor: 'divider' }}>
            <Tabs 
              value={tabValue} 
              onChange={(_, newValue) => setTabValue(newValue)}
              sx={{
                '& .MuiTab-root': {
                  textTransform: 'none',
                  fontWeight: 600,
                  fontSize: '1rem',
                  minHeight: 64,
                },
              }}
            >
              <Tab label="Configuración de Experimento" />
              <Tab 
                label="Resultados" 
                disabled={results.length === 0}
                sx={{ 
                  '&.Mui-disabled': { 
                    opacity: 0.5 
                  } 
                }}
              />
            </Tabs>
          </Box>

      <TabPanel value={tabValue} index={0}>
        {isLoading ? (
          <Box 
            sx={{ 
              display: 'flex', 
              justifyContent: 'center', 
              alignItems: 'center', 
              minHeight: 500,
              background: 'linear-gradient(135deg, #f8fafc 0%, #e0e7ff 100%)',
              borderRadius: 3,
              border: '2px dashed',
              borderColor: 'primary.main',
            }}
          >
            <Box sx={{ textAlign: 'center' }}>
              <CircularProgress 
                size={80} 
                thickness={4}
                sx={{ 
                  color: 'primary.main',
                  mb: 3,
                }} 
              />
              <Typography variant="h5" sx={{ mt: 2, fontWeight: 600, mb: 1 }}>
                Ejecutando experimento...
              </Typography>
              <Typography variant="body1" color="text.secondary" sx={{ mt: 1, maxWidth: 400 }}>
                Esto puede tomar varios minutos dependiendo de los datasets seleccionados.
                Por favor, no cierres esta ventana.
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
          <Box sx={{ display: 'flex', flexDirection: 'column', gap: 4 }}>
            <Card sx={{ p: 3, background: 'linear-gradient(135deg, #6366f1 0%, #8b5cf6 100%)', color: 'white' }}>
              <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
                <Box>
                  <Typography variant="h5" sx={{ fontWeight: 700, mb: 1, color: 'white' }}>
                    Resultados del Experimento
                  </Typography>
                  <Typography variant="body2" sx={{ color: 'rgba(255, 255, 255, 0.9)' }}>
                    {results.length} dataset{results.length > 1 ? 's' : ''} procesado{results.length > 1 ? 's' : ''}
                  </Typography>
                </Box>
                <Button 
                  variant="contained" 
                  onClick={handleExportCSV}
                  startIcon={<DownloadIcon />}
                  sx={{
                    bgcolor: 'white',
                    color: 'primary.main',
                    '&:hover': {
                      bgcolor: 'rgba(255, 255, 255, 0.9)',
                    },
                  }}
                >
                  Exportar CSVs
                </Button>
              </Box>
            </Card>

            <Grid container spacing={3}>
              {results.some(r => Object.values(r.bounds).some(b => b.tightness !== undefined)) && (
                <Grid item xs={12}>
                  <Card sx={{ p: 3 }}>
                    <RelativeTightnessChart
                      results={results}
                      baseline={baseline}
                      compareTo={compareToBounds}
                    />
                  </Card>
                </Grid>
              )}

              <Grid item xs={12}>
                <Card sx={{ p: 3 }}>
                  <TimeComparisonChart results={results} bounds={boundsInResults} />
                </Card>
              </Grid>

              <Grid item xs={12}>
                <Card sx={{ p: 3 }}>
                  <PrunedComparisonChart results={results} bounds={boundsInResults} />
                </Card>
              </Grid>

              <Grid item xs={12}>
                <Card sx={{ p: 3 }}>
                  <ResultsTable results={results} bounds={boundsInResults} />
                </Card>
              </Grid>
            </Grid>
          </Box>
        )}
      </TabPanel>
      </Card>
    </Container>
    </Box>
  );
};

