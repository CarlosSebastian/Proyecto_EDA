import React, { useState } from 'react';
import {
  Card,
  Typography,
  TextField,
  Autocomplete,
  Chip,
  Box,
  Divider,
} from '@mui/material';
import StorageIcon from '@mui/icons-material/Storage';
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
    <Card sx={{ p: 3, borderRadius: 3 }}>
      <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
        <StorageIcon sx={{ mr: 1, color: 'primary.main' }} />
        <Typography variant="h6" sx={{ fontWeight: 600 }}>
          Datasets UCR
        </Typography>
        {selectedDatasets.length > 0 && (
          <Chip 
            label={`${selectedDatasets.length} seleccionado${selectedDatasets.length !== 1 ? 's' : ''}`} 
            size="small" 
            color="primary" 
            sx={{ ml: 2 }} 
          />
        )}
      </Box>
      <Divider sx={{ mb: 2 }} />
      <Autocomplete
        multiple
        options={AVAILABLE_DATASETS}
        value={selectedDatasets}
        onChange={(_, newValue) => onDatasetsChange(newValue)}
        renderInput={params => (
          <TextField
            {...params}
            label="Seleccionar datasets"
            placeholder="Buscar o seleccionar datasets del UCR Archive"
            sx={{
              '& .MuiOutlinedInput-root': {
                borderRadius: 2,
              },
            }}
          />
        )}
        renderTags={(value, getTagProps) =>
          value.map((option, index) => (
            <Chip
              label={option}
              {...getTagProps({ index })}
              key={option}
              color="primary"
              variant="outlined"
            />
          ))
        }
        sx={{ mt: 1 }}
      />
      <Box sx={{ mt: 2 }}>
        <Typography variant="body2" color="text.secondary">
          {selectedDatasets.length > 0 
            ? `${selectedDatasets.length} dataset${selectedDatasets.length !== 1 ? 's' : ''} seleccionado${selectedDatasets.length !== 1 ? 's' : ''} del UCR Time Series Archive`
            : 'Selecciona al menos un dataset para ejecutar el experimento'
          }
        </Typography>
      </Box>
    </Card>
  );
};

