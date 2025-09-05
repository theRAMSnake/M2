'use client';

import { useState, useEffect } from 'react';
import { 
  Box, 
  Typography, 
  Button, 
  Dialog, 
  DialogTitle, 
  DialogContent, 
  DialogActions, 
  TextField,
  Alert
} from '@mui/material';
import { Work as WorkIcon } from '@mui/icons-material';
import { getAuthToken } from '../utils/auth';

interface WorkBurdenData {
  workBurden: number;
  dailyWorkBurdenIncrement: number;
  workBurdenBaseline: number;
}

export function WorkBurdenCounter() {
  const [workBurden, setWorkBurden] = useState<number>(0);
  const [dailyIncrement, setDailyIncrement] = useState<number>(320);
  const [baseline, setBaseline] = useState<number | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [decreaseAmount, setDecreaseAmount] = useState<number>(0);

  useEffect(() => {
    loadWorkBurden();
  }, []);

  const loadWorkBurden = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/data', {
        headers: { 
          'Authorization': `Bearer ${token}`
        }
      });

      if (!response.ok) {
        throw new Error('Failed to load discipline data');
      }

      const result = await response.json();
      const disciplineData = result.data as WorkBurdenData;
      const currentWorkBurden = disciplineData.workBurden;
      const currentDailyIncrement = disciplineData.dailyWorkBurdenIncrement;
      const currentBaseline = disciplineData.workBurdenBaseline;
      
      setWorkBurden(currentWorkBurden);
      setDailyIncrement(currentDailyIncrement);
      
      // Use stored baseline, only show if current work burden is positive
      const calculatedBaseline = currentWorkBurden < 0 ? null : currentBaseline;
      setBaseline(calculatedBaseline);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load discipline data');
    } finally {
      setLoading(false);
    }
  };

  const handleDecrease = async () => {
    if (decreaseAmount <= 0) {
      setError('Please enter a positive amount to decrease');
      return;
    }

    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/workburden/decrease', {
        method: 'POST',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({ amount: decreaseAmount })
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to decrease work burden');
      }

      const result = await response.json();
      const newWorkBurden = result.data.newWorkBurden;
      setWorkBurden(newWorkBurden);
      
      // Update baseline (baseline doesn't change when decreasing, only when daily increment happens)
      const calculatedBaseline = newWorkBurden < 0 ? null : baseline;
      setBaseline(calculatedBaseline);
      
      setDialogOpen(false);
      setDecreaseAmount(0);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to decrease work burden');
    }
  };

  const getWorkBurdenColor = () => {
    if (workBurden <= 0) return 'success.main';
    if (workBurden <= 320) return 'warning.main';
    return 'error.main';
  };

  if (loading) {
    return (
      <Box display="flex" alignItems="center" gap={1}>
        <WorkIcon />
        <Typography variant="body2">...</Typography>
      </Box>
    );
  }

  return (
    <>
      <Box 
        display="flex" 
        alignItems="center" 
        gap={1}
        sx={{ 
          cursor: 'pointer',
          p: 1,
          borderRadius: 1,
          '&:hover': {
            bgcolor: 'action.hover'
          }
        }}
        onClick={() => setDialogOpen(true)}
      >
        <WorkIcon />
        <Box display="flex" flexDirection="column" alignItems="center">
          <Typography 
            variant="h6" 
            sx={{ 
              color: getWorkBurdenColor(),
              fontWeight: 'bold',
              fontSize: '1.2rem',
              lineHeight: 1
            }}
          >
            {workBurden}
          </Typography>
          {baseline !== null && (
            <Typography 
              variant="caption" 
              sx={{ 
                color: 'text.secondary',
                fontSize: '0.7rem',
                lineHeight: 1
              }}
            >
              {baseline}
            </Typography>
          )}
        </Box>
      </Box>

      <Dialog open={dialogOpen} onClose={() => setDialogOpen(false)} maxWidth="sm" fullWidth>
        <DialogTitle>Decrease Work Burden</DialogTitle>
        <DialogContent>
          <Box sx={{ pt: 2 }}>
            <Typography variant="body1" sx={{ mb: 1 }}>
              Current work burden: <strong>{workBurden}</strong>
            </Typography>
            {baseline !== null && (
              <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
                Target baseline: <strong>{baseline}</strong> (decrease by {workBurden - baseline} to reach it)
              </Typography>
            )}
            
            {error && (
              <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
                {error}
              </Alert>
            )}

            <TextField
              label="Amount to decrease"
              type="number"
              value={decreaseAmount || ''}
              onChange={(e) => setDecreaseAmount(parseInt(e.target.value) || 0)}
              fullWidth
              inputProps={{ min: 1 }}
              placeholder="Enter amount to decrease"
              sx={{ mb: 2 }}
            />

            <Typography variant="body2" color="text.secondary">
              New work burden will be: <strong>{workBurden - decreaseAmount}</strong>
            </Typography>
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleDecrease} variant="contained">
            Decrease
          </Button>
        </DialogActions>
      </Dialog>
    </>
  );
}
