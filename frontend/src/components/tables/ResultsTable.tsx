import React, { useState } from 'react';
import {
  Paper,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  TableSortLabel,
  Typography,
  Box,
  Chip,
} from '@mui/material';
import type { ExperimentResult } from '@/types/experiment';

interface ResultsTableProps {
  results: ExperimentResult[];
  bounds: string[];
}

type SortField = 'dataset' | string;
type SortDirection = 'asc' | 'desc';

export const ResultsTable: React.FC<ResultsTableProps> = ({ results, bounds }) => {
  const [sortField, setSortField] = useState<SortField>('dataset');
  const [sortDirection, setSortDirection] = useState<SortDirection>('asc');

  const handleSort = (field: SortField) => {
    if (sortField === field) {
      setSortDirection(sortDirection === 'asc' ? 'desc' : 'asc');
    } else {
      setSortField(field);
      setSortDirection('asc');
    }
  };

  const sortedResults = [...results].sort((a, b) => {
    let aValue: any;
    let bValue: any;

    if (sortField === 'dataset') {
      aValue = a.dataset;
      bValue = b.dataset;
    } else {
      // sortField es un bound.metric (ej: "keogh.time")
      const [bound, metric] = sortField.split('.');
      aValue = a.bounds[bound]?.[metric as keyof typeof a.bounds[string]] || 0;
      bValue = b.bounds[bound]?.[metric as keyof typeof b.bounds[string]] || 0;
    }

    if (aValue < bValue) return sortDirection === 'asc' ? -1 : 1;
    if (aValue > bValue) return sortDirection === 'asc' ? 1 : -1;
    return 0;
  });

  const formatValue = (value: number | undefined, type: 'time' | 'pruned' | 'accuracy' | 'tightness'): string => {
    if (value === undefined || isNaN(value)) return '-';
    
    switch (type) {
      case 'time':
        return `${value.toFixed(2)} ms`;
      case 'pruned':
        return value.toLocaleString();
      case 'accuracy':
        return `${(value * 100).toFixed(2)}%`;
      case 'tightness':
        return value.toFixed(4);
      default:
        return value.toString();
    }
  };

  return (
    <Paper sx={{ p: 2 }}>
      <Typography variant="h6" gutterBottom>
        Tabla de Resultados
      </Typography>
      <TableContainer>
        <Table size="small">
          <TableHead>
            <TableRow>
              <TableCell>
                <TableSortLabel
                  active={sortField === 'dataset'}
                  direction={sortField === 'dataset' ? sortDirection : 'asc'}
                  onClick={() => handleSort('dataset')}
                >
                  Dataset
                </TableSortLabel>
              </TableCell>
              {bounds.map(bound => (
                <React.Fragment key={bound}>
                  <TableCell align="right">
                    <TableSortLabel
                      active={sortField === `${bound}.time`}
                      direction={sortField === `${bound}.time` ? sortDirection : 'asc'}
                      onClick={() => handleSort(`${bound}.time`)}
                    >
                      {bound} - Tiempo
                    </TableSortLabel>
                  </TableCell>
                  <TableCell align="right">
                    <TableSortLabel
                      active={sortField === `${bound}.pruned`}
                      direction={sortField === `${bound}.pruned` ? sortDirection : 'asc'}
                      onClick={() => handleSort(`${bound}.pruned`)}
                    >
                      {bound} - Podadas
                    </TableSortLabel>
                  </TableCell>
                  <TableCell align="right">
                    <TableSortLabel
                      active={sortField === `${bound}.accuracy`}
                      direction={sortField === `${bound}.accuracy` ? sortDirection : 'asc'}
                      onClick={() => handleSort(`${bound}.accuracy`)}
                    >
                      {bound} - Accuracy
                    </TableSortLabel>
                  </TableCell>
                </React.Fragment>
              ))}
            </TableRow>
          </TableHead>
          <TableBody>
            {sortedResults.map((result) => (
              <TableRow key={result.dataset}>
                <TableCell>
                  <Chip label={result.dataset} size="small" />
                </TableCell>
                {bounds.map(bound => {
                  const boundData = result.bounds[bound];
                  return (
                    <React.Fragment key={bound}>
                      <TableCell align="right">
                        {formatValue(boundData?.time, 'time')}
                      </TableCell>
                      <TableCell align="right">
                        {formatValue(boundData?.pruned, 'pruned')}
                      </TableCell>
                      <TableCell align="right">
                        {formatValue(boundData?.accuracy, 'accuracy')}
                      </TableCell>
                    </React.Fragment>
                  );
                })}
              </TableRow>
            ))}
          </TableBody>
        </Table>
      </TableContainer>
    </Paper>
  );
};

