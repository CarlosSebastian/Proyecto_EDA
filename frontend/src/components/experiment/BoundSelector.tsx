import React from 'react';
import {
  FormGroup,
  FormControlLabel,
  Checkbox,
  Paper,
  Typography,
  TextField,
  Box,
} from '@mui/material';
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
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Lower Bounds a Comparar
      </Typography>
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
    </Paper>
  );
};

