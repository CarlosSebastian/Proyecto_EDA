import React, { useState } from 'react';
import {
  Paper,
  Typography,
  FormGroup,
  FormControlLabel,
  Checkbox,
  Radio,
  RadioGroup,
  FormControl,
  FormLabel,
  TextField,
  Button,
  Box,
  Alert,
} from '@mui/material';
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

  const [windowType, setWindowType] = useState<'optimal' | 'fixed' | 'percentage'>('optimal');
  const [windowValue, setWindowValue] = useState<number>(10);
  const [windowPercentages, setWindowPercentages] = useState<number[]>([1, 10, 20]);

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
        type: windowType,
        value: windowType !== 'optimal' ? windowValue : undefined,
        multiple: windowType === 'percentage' ? windowPercentages : undefined,
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
    <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
      <Paper sx={{ p: 2 }}>
        <Typography variant="h6" gutterBottom>
          Tipos de Experimento
        </Typography>
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
      </Paper>

      <Paper sx={{ p: 2 }}>
        <Typography variant="h6" gutterBottom>
          Configuración de Ventana
        </Typography>
        <FormControl component="fieldset">
          <RadioGroup
            value={windowType}
            onChange={e =>
              setWindowType(e.target.value as 'optimal' | 'fixed' | 'percentage')
            }
          >
            <FormControlLabel
              value="optimal"
              control={<Radio />}
              label="Óptima (-W)"
            />
            <FormControlLabel
              value="fixed"
              control={<Radio />}
              label="Fija (-w)"
            />
            <FormControlLabel
              value="percentage"
              control={<Radio />}
              label="Porcentaje (-g)"
            />
          </RadioGroup>
        </FormControl>

        {windowType === 'fixed' && (
          <TextField
            type="number"
            label="Tamaño de ventana"
            value={windowValue}
            onChange={e => setWindowValue(parseInt(e.target.value) || 0)}
            sx={{ mt: 2, width: 200 }}
            inputProps={{ min: 1 }}
          />
        )}

        {windowType === 'percentage' && (
          <Box sx={{ mt: 2 }}>
            <Typography variant="body2" gutterBottom>
              Porcentajes (separados por comas):
            </Typography>
            <TextField
              fullWidth
              placeholder="1, 10, 20"
              value={windowPercentages.join(', ')}
              onChange={e => {
                const values = e.target.value
                  .split(',')
                  .map(v => parseInt(v.trim()))
                  .filter(v => !isNaN(v));
                setWindowPercentages(values);
              }}
            />
          </Box>
        )}
      </Paper>

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

      <Box sx={{ display: 'flex', gap: 2 }}>
        <Button
          variant="contained"
          color="primary"
          onClick={handleRun}
          disabled={isLoading}
          size="large"
        >
          {isLoading ? 'Ejecutando...' : 'Ejecutar Experimento'}
        </Button>
        {onLoadCSV && (
          <Button variant="outlined" onClick={onLoadCSV}>
            Cargar CSV Existente
          </Button>
        )}
      </Box>
    </Box>
  );
};

