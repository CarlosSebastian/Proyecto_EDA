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
} from 'recharts';
import { Paper, Typography, Box } from '@mui/material';
import type { ExperimentResult } from '@/types/experiment';

interface TimeComparisonChartProps {
  results: ExperimentResult[];
  bounds: string[];
}

export const TimeComparisonChart: React.FC<TimeComparisonChartProps> = ({
  results,
  bounds,
}) => {
  const chartData = results.map(exp => {
    const data: { dataset: string; [key: string]: string | number } = {
      dataset: exp.dataset,
    };

    bounds.forEach(bound => {
      const boundData = exp.bounds[bound];
      data[bound] = boundData?.time || 0;
    });

    return data;
  });

  const colors = ['#8884d8', '#82ca9d', '#ffc658', '#ff7300', '#00ff00', '#0088fe'];

  return (
    <Paper sx={{ p: 3 }}>
      <Typography variant="h6" gutterBottom>
        Comparación de Tiempos de Ejecución
      </Typography>
      <Box sx={{ mt: 2 }}>
        <ResponsiveContainer width="100%" height={400}>
          <BarChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="dataset" angle={-45} textAnchor="end" height={100} />
            <YAxis label={{ value: 'Tiempo (ms)', angle: -90, position: 'insideLeft' }} />
            <Tooltip />
            <Legend />
            {bounds.map((bound, index) => (
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

