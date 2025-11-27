import React from 'react';
import {
  BarChart,
  Bar,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  Legend,
  ResponsiveContainer,
  ReferenceLine,
} from 'recharts';
import { Paper, Typography, Box } from '@mui/material';
import { calculateRelativeTightness } from '@/services/dataProcessor';
import type { ExperimentResult } from '@/types/experiment';

interface RelativeTightnessChartProps {
  results: ExperimentResult[];
  baseline?: string;
  compareTo?: string[];
}

export const RelativeTightnessChart: React.FC<RelativeTightnessChartProps> = ({
  results,
  baseline,
  compareTo,
}) => {
  // Obtener TODOS los bounds que tienen datos de tightness en los resultados
  // No importa qué bounds se pasaron como props, mostramos todos los disponibles
  const allBounds = results.length > 0
    ? Object.keys(results[0].bounds).filter(bound => {
        return results.some(exp => {
          const boundData = exp.bounds[bound];
          return boundData && boundData.tightness !== undefined && boundData.tightness > 0;
        });
      })
    : [];

  // Si no hay bounds válidos, no mostrar el gráfico
  if (allBounds.length === 0) {
    return (
      <Paper sx={{ p: 3 }}>
        <Typography variant="h6" gutterBottom>
          Tightness de Lower Bounds
        </Typography>
        <Typography variant="body2" color="text.secondary">
          No hay datos de tightness disponibles.
        </Typography>
      </Paper>
    );
  }

  // Mostrar valores absolutos de tightness para todos los bounds
  const chartData = results.map(exp => {
    const data: { dataset: string; [key: string]: string | number } = {
      dataset: exp.dataset,
    };

    // Agregar tightness absoluto para cada bound
    allBounds.forEach(bound => {
      const boundData = exp.bounds[bound];
      data[bound] = boundData?.tightness || 0;
    });

    return data;
  });

  const colors = ['#8884d8', '#82ca9d', '#ffc658', '#ff7300', '#00ff00', '#0088fe'];

  return (
    <Paper sx={{ p: 3 }}>
      <Typography variant="h6" gutterBottom>
        Tightness de Lower Bounds
      </Typography>
      <Typography variant="body2" color="text.secondary" gutterBottom>
        Valores absolutos de tightness. Valores más altos indican bounds más ajustados (tighter).
      </Typography>
      <Box sx={{ mt: 2 }}>
        <ResponsiveContainer width="100%" height={400}>
          <BarChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="dataset" angle={-45} textAnchor="end" height={100} />
            <YAxis label={{ value: 'Tightness', angle: -90, position: 'insideLeft' }} />
            <Tooltip />
            <Legend />
            {allBounds.map((bound, index) => (
              <Bar
                key={bound}
                dataKey={bound}
                fill={colors[index % colors.length]}
                name={bound}
              />
            ))}
          </BarChart>
        </ResponsiveContainer>
      </Box>
    </Paper>
  );
};

