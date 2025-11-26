import React from 'react';
import {
  FormGroup,
  FormControlLabel,
  Checkbox,
  Card,
  Typography,
  TextField,
  Box,
  Divider,
  Chip,
  Stack,
} from '@mui/material';
import CompareArrowsIcon from '@mui/icons-material/CompareArrows';
import { AVAILABLE_BOUNDS, type Bound } from '@/types/bounds';

interface BoundSelectorProps {
  selectedBounds: string[];
  onBoundsChange: (bounds: string[]) => void;
  boundParameters: { [boundId: string]: number };
  onParameterChange: (boundId: string, value: number) => void;
}

export const BoundSelector: React.FC<BoundSelectorProps> = ({
  selectedBounds,
  onBoundsChange,
  boundParameters,
  onParameterChange,
}) => {
  const handleBoundToggle = (boundId: string) => {
    if (selectedBounds.includes(boundId)) {
      onBoundsChange(selectedBounds.filter(b => b !== boundId));
    } else {
      onBoundsChange([...selectedBounds, boundId]);
    }
  };

  const getBoundDisplayName = (bound: Bound): string => {
    if (bound.hasParameter && boundParameters[bound.id]) {
      return `${bound.displayName}(${boundParameters[bound.id]})`;
    }
    return bound.displayName;
  };

  return (
    <Card sx={{ p: 3, borderRadius: 3 }}>
      <Box sx={{ display: 'flex', alignItems: 'center', mb: 2 }}>
        <CompareArrowsIcon sx={{ mr: 1, color: 'primary.main' }} />
        <Typography variant="h6" sx={{ fontWeight: 600 }}>
          Lower Bounds a Comparar
        </Typography>
        {selectedBounds.length > 0 && (
          <Chip 
            label={`${selectedBounds.length} seleccionado${selectedBounds.length !== 1 ? 's' : ''}`} 
            size="small" 
            color="primary" 
            sx={{ ml: 2 }} 
          />
        )}
      </Box>
      <Divider sx={{ mb: 2 }} />
      <FormGroup>
        {AVAILABLE_BOUNDS.map(bound => (
          <Box key={bound.id} sx={{ mb: 1 }}>
            <FormControlLabel
              control={
                <Checkbox
                  checked={selectedBounds.includes(bound.id)}
                  onChange={() => handleBoundToggle(bound.id)}
                />
              }
              label={getBoundDisplayName(bound)}
            />
            {bound.hasParameter && selectedBounds.includes(bound.id) && (
              <TextField
                size="small"
                type="number"
                label="Parámetro k"
                value={boundParameters[bound.id] || bound.defaultParameter || 0}
                onChange={e =>
                  onParameterChange(bound.id, parseInt(e.target.value) || 0)
                }
                sx={{ ml: 4, width: 120 }}
                inputProps={{ min: 1 }}
              />
            )}
          </Box>
        ))}
      </FormGroup>
    </Card>
  );
};

