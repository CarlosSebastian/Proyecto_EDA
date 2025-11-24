import React, { useState } from 'react';
import {
  Paper,
  Typography,
  TextField,
  Autocomplete,
  Chip,
  Box,
} from '@mui/material';
import { AVAILABLE_DATASETS } from '@/types/bounds';

interface DatasetSelectorProps {
  selectedDatasets: string[];
  onDatasetsChange: (datasets: string[]) => void;
}

export const DatasetSelector: React.FC<DatasetSelectorProps> = ({
  selectedDatasets,
  onDatasetsChange,
}) => {
  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Datasets
      </Typography>
      <Autocomplete
        multiple
        options={AVAILABLE_DATASETS}
        value={selectedDatasets}
        onChange={(_, newValue) => onDatasetsChange(newValue)}
        renderInput={params => (
          <TextField
            {...params}
            label="Seleccionar datasets"
            placeholder="Buscar o seleccionar datasets"
          />
        )}
        renderTags={(value, getTagProps) =>
          value.map((option, index) => (
            <Chip
              label={option}
              {...getTagProps({ index })}
              key={option}
            />
          ))
        }
        sx={{ mt: 1 }}
      />
      <Box sx={{ mt: 2 }}>
        <Typography variant="body2" color="text.secondary">
          {selectedDatasets.length} dataset(s) seleccionado(s)
        </Typography>
      </Box>
    </Paper>
  );
};

