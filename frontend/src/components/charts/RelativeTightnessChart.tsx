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
  baseline: string;
  compareTo: string[];
}

export const RelativeTightnessChart: React.FC<RelativeTightnessChartProps> = ({
  results,
  baseline,
  compareTo,
}) => {
  // Calcular tightness relativo para cada bound a comparar
  const chartData = results.map(exp => {
    const data: { dataset: string; [key: string]: string | number } = {
      dataset: exp.dataset,
    };

    // Verificar que el baseline existe en los resultados
    const baselineData = exp.bounds[baseline];
    if (!baselineData?.tightness) {
      // Si no hay baseline, mostrar valores absolutos en lugar de ratios
      compareTo.forEach(bound => {
        const compareData = exp.bounds[bound];
        data[bound] = compareData?.tightness || 0;
      });
    } else {
      // Calcular ratios relativos al baseline
      compareTo.forEach(bound => {
        const compareData = exp.bounds[bound];

        if (compareData?.tightness) {
          // Ratio: si > 1.0, el bound comparado es más tight
          data[bound] = compareData.tightness / baselineData.tightness;
        } else {
          data[bound] = 0;
        }
      });
    }

    return data;
  });

  const colors = ['#8884d8', '#82ca9d', '#ffc658', '#ff7300', '#00ff00', '#0088fe'];

  return (
    <Paper sx={{ p: 3 }}>
      <Typography variant="h6" gutterBottom>
        Tightness Relativo (vs {baseline})
      </Typography>
      <Typography variant="body2" color="text.secondary" gutterBottom>
        Ratio &gt; 1.0 indica que el bound es más ajustado (tighter) que {baseline}
      </Typography>
      <Box sx={{ mt: 2 }}>
        <ResponsiveContainer width="100%" height={400}>
          <BarChart data={chartData}>
            <CartesianGrid strokeDasharray="3 3" />
            <XAxis dataKey="dataset" angle={-45} textAnchor="end" height={100} />
            <YAxis label={{ value: 'Tightness Ratio', angle: -90, position: 'insideLeft' }} />
            <Tooltip />
            <Legend />
            <ReferenceLine y={1.0} stroke="#666" strokeDasharray="3 3" label="Baseline" />
            {compareTo.map((bound, index) => (
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

