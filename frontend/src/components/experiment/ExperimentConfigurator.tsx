import React, { useState } from 'react';
import {
  Typography,
  FormGroup,
  FormControlLabel,
  Checkbox,
  Button,
  Box,
  Card,
  Divider,
  Stack,
} from '@mui/material';
import PlayArrowIcon from '@mui/icons-material/PlayArrow';
import ScienceIcon from '@mui/icons-material/Science';
import { BoundSelector } from './BoundSelector';
import { DatasetSelector } from './DatasetSelector';
import type { ExperimentConfig } from '@/types/experiment';

interface ExperimentConfiguratorProps {
  onRun: (config: ExperimentConfig) => void;
  onLoadCSV?: () => void;
  isLoading?: boolean;
}

export const ExperimentConfigurator: React.FC<ExperimentConfiguratorProps> = ({
  onRun,
  onLoadCSV,
  isLoading = false,
}) => {
  const [experimentTypes, setExperimentTypes] = useState({
    tightness: true,
    nnUnsorted: true,
    nnSorted: false,
  });

  const [selectedBounds, setSelectedBounds] = useState<string[]>(['keogh', 'webb']);
  const [boundParameters, setBoundParameters] = useState<{ [key: string]: number }>({
    enhanced: 5,
    enhancedwebb: 3,
  });

  const [selectedDatasets, setSelectedDatasets] = useState<string[]>([
    '50words',
    'Adiac',
    'CBF',
    'FaceAll',
    'Two_Patterns',
  ]);

  const handleRun = () => {
    if (selectedBounds.length === 0) {
      alert('Selecciona al menos un bound');
      return;
    }

    if (selectedDatasets.length === 0) {
      alert('Selecciona al menos un dataset');
      return;
    }

    if (!experimentTypes.tightness && !experimentTypes.nnUnsorted && !experimentTypes.nnSorted) {
      alert('Selecciona al menos un tipo de experimento');
      return;
    }

    const config: ExperimentConfig = {
      experimentTypes,
      windowConfig: {
        type: 'optimal',
      },
      bounds: selectedBounds.map(bound => {
        if (boundParameters[bound]) {
          return `${bound}${boundParameters[bound]}`;
        }
        return bound;
      }),
      datasets: selectedDatasets,
    };

    onRun(config);
  };

  return (
    <Box sx={{ display: 'flex', flexDirection: 'column', gap: 3 }}>
      <Card sx={{ p: 3, borderRadius: 3 }}>
        <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
          <ScienceIcon sx={{ mr: 1, color: 'primary.main' }} />
          <Typography variant="h6" sx={{ fontWeight: 600 }}>
            Tipos de Experimento
          </Typography>
        </Box>
        <Divider sx={{ mb: 2 }} />
        <FormGroup>
          <FormControlLabel
            control={
              <Checkbox
                checked={experimentTypes.tightness}
                onChange={e =>
                  setExperimentTypes({
                    ...experimentTypes,
                    tightness: e.target.checked,
                  })
                }
              />
            }
            label="Tightness Test (-t)"
          />
          <FormControlLabel
            control={
              <Checkbox
                checked={experimentTypes.nnUnsorted}
                onChange={e =>
                  setExperimentTypes({
                    ...experimentTypes,
                    nnUnsorted: e.target.checked,
                  })
                }
              />
            }
            label="1-NN UNSORTED (ventana óptima)"
          />
          <FormControlLabel
            control={
              <Checkbox
                checked={experimentTypes.nnSorted}
                onChange={e =>
                  setExperimentTypes({
                    ...experimentTypes,
                    nnSorted: e.target.checked,
                  })
                }
              />
            }
            label="1-NN SORTED (ventana óptima)"
          />
        </FormGroup>
      </Card>

      <BoundSelector
        selectedBounds={selectedBounds}
        onBoundsChange={setSelectedBounds}
        boundParameters={boundParameters}
        onParameterChange={(boundId, value) =>
          setBoundParameters({ ...boundParameters, [boundId]: value })
        }
      />

      <DatasetSelector
        selectedDatasets={selectedDatasets}
        onDatasetsChange={setSelectedDatasets}
      />

      <Card sx={{ p: 3, borderRadius: 3, bgcolor: 'primary.main', color: 'white' }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', flexWrap: 'wrap', gap: 2 }}>
          <Box>
            <Typography variant="h6" sx={{ fontWeight: 600, mb: 0.5, color: 'white' }}>
              Listo para ejecutar
            </Typography>
            <Typography variant="body2" sx={{ color: 'rgba(255, 255, 255, 0.9)' }}>
              {selectedBounds.length} bound{selectedBounds.length !== 1 ? 's' : ''} seleccionado{selectedBounds.length !== 1 ? 's' : ''} • {selectedDatasets.length} dataset{selectedDatasets.length !== 1 ? 's' : ''} seleccionado{selectedDatasets.length !== 1 ? 's' : ''}
            </Typography>
          </Box>
          <Stack direction="row" spacing={2}>
            {onLoadCSV && (
              <Button 
                variant="outlined" 
                onClick={onLoadCSV}
                sx={{
                  borderColor: 'white',
                  color: 'white',
                  '&:hover': {
                    borderColor: 'rgba(255, 255, 255, 0.8)',
                    bgcolor: 'rgba(255, 255, 255, 0.1)',
                  },
                }}
              >
                Cargar CSV
              </Button>
            )}
            <Button
              variant="contained"
              onClick={handleRun}
              disabled={isLoading}
              size="large"
              startIcon={<PlayArrowIcon />}
              sx={{
                bgcolor: '#4caf50',
                color: 'white',
                fontWeight: 700,
                px: 4,
                py: 1.5,
                fontSize: '1.1rem',
                boxShadow: '0 4px 12px rgba(76, 175, 80, 0.4)',
                textTransform: 'none',
                '&:hover': {
                  bgcolor: '#45a049',
                  boxShadow: '0 6px 16px rgba(76, 175, 80, 0.5)',
                  transform: 'translateY(-2px)',
                },
                '&:active': {
                  transform: 'translateY(0px)',
                },
                '&:disabled': {
                  bgcolor: 'rgba(76, 175, 80, 0.5)',
                  color: 'white',
                  boxShadow: 'none',
                },
                transition: 'all 0.2s ease-in-out',
              }}
            >
              {isLoading ? 'Ejecutando...' : 'Ejecutar Experimento'}
            </Button>
          </Stack>
        </Box>
      </Card>
    </Box>
  );
};

