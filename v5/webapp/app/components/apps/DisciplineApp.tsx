'use client';

import React, { useState, useEffect } from 'react';
import {
  Box,
  Card,
  CardContent,
  CardHeader,
  Typography,
  Button,
  TextField,
  FormControl,
  InputLabel,
  Select,
  MenuItem,
  Chip,
  LinearProgress,
  Alert,
  Divider,
  Grid,
  IconButton,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
} from '@mui/material';
import {
  Add as AddIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  Clear as ClearIcon,
  Psychology as PsychologyIcon,
  MonetizationOn as CoinsIcon,
  CardGiftcard as GiftIcon,
  Storage as DatabaseIcon,
  Notes as NotesIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';

interface CoinBalance {
  red: number;
  blue: number;
  purple: number;
  yellow: number;
  green: number;
}

interface Pool {
  id: string;
  name: string;
  currentAmount: number;
  maxAmount: number;
}

interface Modifier {
  id: string;
  name: string;
  color: string;
  value: number;
}

interface DisciplineData {
  coins: CoinBalance;
  pools: Pool[];
  nextCouponPrice: number;
  nextCouponColor: string;
  notebook: string;
}

const COIN_COLORS = ['red', 'blue', 'purple', 'yellow', 'green'] as const;

export function DisciplineApp() {
  const [data, setData] = useState<DisciplineData | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);

  // Form states
  const [addCoinColor, setAddCoinColor] = useState<string>('random');
  const [addCoinAmount, setAddCoinAmount] = useState<number>(1);
  const [newPoolName, setNewPoolName] = useState<string>('');
  const [newPoolMaxAmount, setNewPoolMaxAmount] = useState<number>(10);
  const [editingPool, setEditingPool] = useState<Pool | null>(null);
  const [editDialogOpen, setEditDialogOpen] = useState(false);
  const [addCoinsDialogOpen, setAddCoinsDialogOpen] = useState(false);
  const [createPoolDialogOpen, setCreatePoolDialogOpen] = useState(false);
  
  // Notebook state
  const [notebookContent, setNotebookContent] = useState<string>('');
  
  // Modifier states
  const [modifiers, setModifiers] = useState<Modifier[]>([]);
  const [modifiersDialogOpen, setModifiersDialogOpen] = useState(false);
  const [newModifierName, setNewModifierName] = useState<string>('');
  const [newModifierColor, setNewModifierColor] = useState<string>('red');
  const [newModifierValue, setNewModifierValue] = useState<number>(1);
  const [editingModifier, setEditingModifier] = useState<Modifier | null>(null);
  const [editModifierDialogOpen, setEditModifierDialogOpen] = useState(false);

  useEffect(() => {
    fetchData();
  }, []);

  const fetchData = async () => {
    try {
      setLoading(true);
      const token = getAuthToken();
      const response = await fetch('/api/discipline/data', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });
      if (!response.ok) {
        throw new Error('Failed to fetch discipline data');
      }
      const result = await response.json();
      setData(result.data);
      setNotebookContent(result.data.notebook || '');
    } catch (err) {
      setError(err instanceof Error ? err.message : 'An error occurred');
    } finally {
      setLoading(false);
    }
  };

  const addCoins = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/coins/add', {
        method: 'POST',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({ color: addCoinColor, amount: addCoinAmount })
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to add coins');
      }

      const result = await response.json();
      fetchData();
      
      // Reset form and close dialog
      setAddCoinAmount(1);
      setAddCoinsDialogOpen(false);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to add coins');
    }
  };

  const buyCoupon = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/coupons/buy', {
        method: 'POST',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        }
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to buy coupon');
      }

      const result = await response.json();
      fetchData();
         } catch (err) {
       setError(err instanceof Error ? err.message : 'Failed to buy coupon');
     }
  };



  const createPool = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/pools', {
        method: 'POST',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({ name: newPoolName, maxAmount: newPoolMaxAmount })
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to create pool');
      }

      const result = await response.json();
      fetchData();
      
      // Reset form and close dialog
      setNewPoolName('');
      setNewPoolMaxAmount(10);
      setCreatePoolDialogOpen(false);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to create pool');
    }
  };

  const updatePool = async (poolId: string, updates: Partial<Pool>) => {
    try {
      const token = getAuthToken();
      const response = await fetch(`/api/discipline/pools/${poolId}`, {
        method: 'PUT',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify(updates)
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to update pool');
      }

      const result = await response.json();
      fetchData();
      setEditDialogOpen(false);
      setEditingPool(null);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to update pool');
    }
  };

  const deletePool = async (poolId: string) => {
    if (!confirm('Are you sure you want to delete this pool?')) {
      return;
    }

    try {
      const token = getAuthToken();
      const response = await fetch(`/api/discipline/pools/${poolId}`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to delete pool');
      }

      const result = await response.json();
      fetchData();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to delete pool');
    }
  };

  const emptyPool = async (poolId: string) => {
    if (!confirm('Are you sure you want to empty this pool?')) {
      return;
    }

    try {
      const token = getAuthToken();
      const response = await fetch(`/api/discipline/pools/${poolId}/empty`, {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to empty pool');
      }

      const result = await response.json();
      fetchData();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to empty pool');
    }
  };

  const getCoinColorStyle = (color: string) => {
    const colorMap: Record<string, { backgroundColor: string; color: string }> = {
      red: { backgroundColor: '#ef4444', color: 'white' },
      blue: { backgroundColor: '#3b82f6', color: 'white' },
      purple: { backgroundColor: '#8b5cf6', color: 'white' },
      yellow: { backgroundColor: '#eab308', color: 'black' },
      green: { backgroundColor: '#22c55e', color: 'white' }
    };
    return colorMap[color] || { backgroundColor: '#6b7280', color: 'white' };
  };

  const updateNotebook = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/notebook', {
        method: 'PUT',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({ content: notebookContent })
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to update notebook');
      }

      fetchData();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to update notebook');
    }
  };

  const loadModifiers = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/modifiers', {
        headers: { 
          'Authorization': `Bearer ${token}`
        }
      });

      if (!response.ok) {
        throw new Error('Failed to load modifiers');
      }

      const result = await response.json();
      setModifiers(result.data.modifiers);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to load modifiers');
    }
  };

  const createModifier = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/discipline/modifiers', {
        method: 'POST',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({ 
          name: newModifierName, 
          color: newModifierColor, 
          value: newModifierValue 
        })
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to create modifier');
      }

      loadModifiers();
      setNewModifierName('');
      setNewModifierColor('red');
      setNewModifierValue(1);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to create modifier');
    }
  };

  const updateModifier = async () => {
    if (!editingModifier) return;
    
    try {
      const token = getAuthToken();
      const response = await fetch(`/api/discipline/modifiers/${editingModifier.id}`, {
        method: 'PUT',
        headers: { 
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`
        },
        body: JSON.stringify({ 
          name: newModifierName, 
          color: newModifierColor, 
          value: newModifierValue 
        })
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to update modifier');
      }

      loadModifiers();
      setNewModifierName('');
      setNewModifierColor('red');
      setNewModifierValue(1);
      setEditingModifier(null);
      setEditModifierDialogOpen(false);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to update modifier');
    }
  };

  const deleteModifier = async (modifierId: string) => {
    try {
      const token = getAuthToken();
      const response = await fetch(`/api/discipline/modifiers/${modifierId}`, {
        method: 'DELETE',
        headers: { 
          'Authorization': `Bearer ${token}`
        }
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to delete modifier');
      }

      loadModifiers();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to delete modifier');
    }
  };

  const canEarnCoupon = () => {
    if (!data) return false;
    return (data.coins as any)[data.nextCouponColor] >= data.nextCouponPrice;
  };

  if (loading) {
    return (
      <Box display="flex" justifyContent="center" alignItems="center" p={4}>
        <Typography>Loading discipline data...</Typography>
      </Box>
    );
  }

  if (error) {
    return (
      <Alert severity="error" sx={{ m: 2 }}>
        {error}
      </Alert>
    );
  }

  if (!data) {
    return (
      <Box p={4}>
        <Typography>No discipline data available.</Typography>
      </Box>
    );
  }

  return (
    <Box sx={{ p: 3, maxWidth: 1400, mx: 'auto' }}>
      <Box display="flex" alignItems="center" justifyContent="space-between" mb={3}>
        <Box display="flex" alignItems="center" gap={1}>
          <DatabaseIcon sx={{ fontSize: 28 }} />
          <Typography variant="h4" component="h1">
            Discipline App
          </Typography>
        </Box>
        <Button
          variant="outlined"
          size="small"
          onClick={() => {
            setModifiersDialogOpen(true);
            loadModifiers();
          }}
          sx={{ opacity: 0.3, '&:hover': { opacity: 0.7 } }}
        >
          Modifiers
        </Button>
      </Box>

      <Box display="flex" gap={3} sx={{ minHeight: '80vh' }}>
        {/* Main Content - 75% width */}
        <Box sx={{ flex: '0 0 75%', display: 'flex', flexDirection: 'column', overflow: 'auto' }}>
          {/* Coins Section */}
      <Card sx={{ mb: 3 }}>
        <CardHeader
          title={
            <Box display="flex" alignItems="center" gap={1}>
              <CoinsIcon />
              <Typography variant="h6">Coin Balance</Typography>
            </Box>
          }
        />
        <CardContent>
          <Grid container spacing={2} sx={{ mb: 3 }}>
            {COIN_COLORS.map(color => (
              <Grid item xs={12} sm={6} md={2.4} key={color}>
                <Box textAlign="center">
                  <Chip
                    label={data.coins[color]}
                    sx={{
                      ...getCoinColorStyle(color),
                      fontSize: '1.2rem',
                      height: 'auto',
                      padding: '8px 16px'
                    }}
                  />
                  <Typography variant="body2" color="text.secondary" sx={{ mt: 1, textTransform: 'capitalize' }}>
                    {color}
                  </Typography>
                </Box>
              </Grid>
            ))}
          </Grid>

          <Divider sx={{ my: 3 }} />

          <Box display="flex" justifyContent="center" gap={2}>
            <Button
              variant="outlined"
              onClick={() => setAddCoinsDialogOpen(true)}
              startIcon={<AddIcon />}
            >
              Add Coins
            </Button>
          </Box>
        </CardContent>
      </Card>

      {/* Coupons Section */}
      <Card sx={{ mb: 2 }}>
        <CardContent sx={{ py: 2 }}>
          <Box display="flex" justifyContent="space-between" alignItems="center">
            <Box display="flex" alignItems="center" gap={2}>
              <Typography variant="body1">
                Next coupon: {data.nextCouponPrice} coins
              </Typography>
              <Chip 
                label={data.nextCouponColor} 
                size="small" 
                sx={getCoinColorStyle(data.nextCouponColor)}
              />
            </Box>
            <Button
              variant="contained"
              size="small"
              onClick={buyCoupon}
              disabled={!canEarnCoupon()}
            >
              Buy Coupon
            </Button>
          </Box>
        </CardContent>
      </Card>

             {/* Pools Section */}
       <Card>
         <CardHeader title="Pools" />
         <CardContent>
           <Typography variant="h6" sx={{ mb: 2 }}>
             Existing Pools
           </Typography>
           {data.pools.length === 0 ? (
             <Typography color="text.secondary">No pools created yet</Typography>
           ) : (
             <Grid container spacing={2} sx={{ mb: 3 }}>
               {data.pools.map(pool => (
                 <Grid item xs={12} sm={6} md={4} lg={2.4} key={pool.id}>
                   <Card variant="outlined" sx={{ minHeight: '140px' }}>
                     <CardContent sx={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
                       {/* Title occupying full width */}
                       <Box sx={{ mb: 2 }}>
                         <Typography variant="subtitle1" sx={{ fontWeight: 'bold', wordBreak: 'break-word', lineHeight: 1.2, width: '100%' }}>{pool.name}</Typography>
                       </Box>
                       <Box sx={{ mb: 1 }}>
                         <Box display="flex" justifyContent="space-between" sx={{ mb: 1 }}>
                           <Typography variant="body2">Progress</Typography>
                           <Typography variant="body2">
                             {pool.currentAmount} / {pool.maxAmount}
                           </Typography>
                         </Box>
                         <LinearProgress
                           variant="determinate"
                           value={(pool.currentAmount / pool.maxAmount) * 100}
                           sx={{ height: 8, borderRadius: 4 }}
                         />
                       </Box>
                       <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
                         Pool for storing earned coupons
                       </Typography>
                       
                       {/* Buttons at the bottom */}
                       <Box sx={{ display: 'flex', justifyContent: 'center', gap: 1, mt: 'auto' }}>
                         <IconButton size="small" sx={{ padding: '4px' }}
                           onClick={() => {
                             setEditingPool(pool);
                             setEditDialogOpen(true);
                           }}
                         >
                           <EditIcon sx={{ fontSize: '16px' }} />
                         </IconButton>
                         <IconButton size="small" sx={{ padding: '4px' }}
                           onClick={() => emptyPool(pool.id)}
                           disabled={pool.currentAmount === 0}
                           title="Empty pool"
                         >
                           <ClearIcon sx={{ fontSize: '16px' }} />
                         </IconButton>
                         <IconButton size="small" sx={{ padding: '4px' }} onClick={() => deletePool(pool.id)}>
                           <DeleteIcon sx={{ fontSize: '16px' }} />
                         </IconButton>
                       </Box>
                     </CardContent>
                   </Card>
                 </Grid>
               ))}
             </Grid>
           )}

           <Divider sx={{ my: 3 }} />

           <Box display="flex" justifyContent="center">
             <Button
               variant="contained"
               onClick={() => setCreatePoolDialogOpen(true)}
               startIcon={<AddIcon />}
             >
               Create New Pool
             </Button>
           </Box>
         </CardContent>
       </Card>
        </Box>

        {/* Notebook Section - 25% width */}
        <Box sx={{ flex: '0 0 25%', minHeight: '400px' }}>
          <Card sx={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
            <CardHeader
              title={
                <Box display="flex" alignItems="center" gap={1}>
                  <NotesIcon />
                  <Typography variant="h6">Notebook</Typography>
                </Box>
              }
            />
            <CardContent sx={{ flex: 1, p: 0 }}>
              <TextField
                multiline
                fullWidth
                value={notebookContent}
                onChange={(e) => setNotebookContent(e.target.value)}
                onBlur={updateNotebook}
                placeholder="Write your notes here..."
                sx={{
                  height: '100%',
                  '& .MuiInputBase-root': {
                    height: '100%',
                    alignItems: 'flex-start'
                  },
                  '& .MuiInputBase-input': {
                    height: '100% !important',
                    overflow: 'auto !important'
                  }
                }}
                variant="outlined"
              />
            </CardContent>
          </Card>
        </Box>
      </Box>

       {/* Edit Pool Dialog */}
       <Dialog open={editDialogOpen} onClose={() => setEditDialogOpen(false)} maxWidth="sm" fullWidth>
         <DialogTitle>Edit Pool</DialogTitle>
         <DialogContent>
           <Grid container spacing={2} sx={{ mt: 1 }}>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Name"
                 value={editingPool?.name || ''}
                 onChange={(e) => setEditingPool(editingPool ? { ...editingPool, name: e.target.value } : null)}
               />
             </Grid>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Max Amount"
                 type="number"
                 value={editingPool?.maxAmount || 1}
                 onChange={(e) => setEditingPool(editingPool ? { ...editingPool, maxAmount: parseInt(e.target.value) || 1 } : null)}
                 inputProps={{ min: 1 }}
               />
             </Grid>
           </Grid>
         </DialogContent>
         <DialogActions>
           <Button onClick={() => setEditDialogOpen(false)}>Cancel</Button>
           <Button
             onClick={() => editingPool && updatePool(editingPool.id, editingPool)}
             variant="contained"
           >
             Save
           </Button>
         </DialogActions>
       </Dialog>

       {/* Add Coins Dialog */}
       <Dialog open={addCoinsDialogOpen} onClose={() => setAddCoinsDialogOpen(false)} maxWidth="sm" fullWidth>
         <DialogTitle>Add Coins</DialogTitle>
         <DialogContent>
           <Grid container spacing={2} sx={{ mt: 1 }}>
             <Grid item xs={12}>
               <FormControl fullWidth>
                 <InputLabel>Color</InputLabel>
                 <Select
                   value={addCoinColor}
                   label="Color"
                   onChange={(e) => setAddCoinColor(e.target.value)}
                 >
                   {COIN_COLORS.map(color => (
                     <MenuItem key={color} value={color}>
                       <Box display="flex" alignItems="center" gap={1}>
                         <Chip 
                           label={color} 
                           size="small" 
                           sx={getCoinColorStyle(color)}
                         />
                         <span style={{ textTransform: 'capitalize' }}>{color}</span>
                       </Box>
                     </MenuItem>
                   ))}
                   <MenuItem value="random">
                     <Box display="flex" alignItems="center" gap={1}>
                       <Chip 
                         label="?" 
                         size="small" 
                         sx={{ backgroundColor: '#6b7280', color: 'white' }}
                       />
                       <span>Random</span>
                     </Box>
                   </MenuItem>
                 </Select>
               </FormControl>
             </Grid>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Amount"
                 type="number"
                 value={addCoinAmount}
                 onChange={(e) => setAddCoinAmount(parseInt(e.target.value) || 1)}
                 inputProps={{ min: 1 }}
               />
             </Grid>
           </Grid>
         </DialogContent>
         <DialogActions>
           <Button onClick={() => setAddCoinsDialogOpen(false)}>Cancel</Button>
           <Button onClick={addCoins} variant="contained">
             Add Coins
           </Button>
         </DialogActions>
       </Dialog>

       {/* Create Pool Dialog */}
       <Dialog open={createPoolDialogOpen} onClose={() => setCreatePoolDialogOpen(false)} maxWidth="sm" fullWidth>
         <DialogTitle>Create New Pool</DialogTitle>
         <DialogContent>
           <Grid container spacing={2} sx={{ mt: 1 }}>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Name"
                 value={newPoolName}
                 onChange={(e) => setNewPoolName(e.target.value)}
                 placeholder="Pool name"
               />
             </Grid>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Max Amount"
                 type="number"
                 value={newPoolMaxAmount}
                 onChange={(e) => setNewPoolMaxAmount(parseInt(e.target.value) || 1)}
                 inputProps={{ min: 1 }}
               />
             </Grid>
           </Grid>
         </DialogContent>
         <DialogActions>
           <Button onClick={() => setCreatePoolDialogOpen(false)}>Cancel</Button>
           <Button
             onClick={createPool}
             disabled={!newPoolName.trim()}
             variant="contained"
           >
             Create Pool
           </Button>
         </DialogActions>
       </Dialog>

       {/* Modifiers Dialog */}
       <Dialog open={modifiersDialogOpen} onClose={() => setModifiersDialogOpen(false)} maxWidth="md" fullWidth>
         <DialogTitle>Modifiers</DialogTitle>
         <DialogContent>
           <Box sx={{ pt: 2 }}>
             <Box display="flex" justifyContent="space-between" alignItems="center" sx={{ mb: 2 }}>
               <Typography variant="h6">Daily Modifiers</Typography>
               <Button
                 variant="contained"
                 onClick={() => setEditModifierDialogOpen(true)}
                 startIcon={<AddIcon />}
               >
                 Add Modifier
               </Button>
             </Box>
             
             {modifiers.length === 0 ? (
               <Typography color="text.secondary">No modifiers configured</Typography>
             ) : (
               <Box>
                 {modifiers.map(modifier => (
                   <Card key={modifier.id} variant="outlined" sx={{ mb: 2 }}>
                     <CardContent>
                       <Box display="flex" justifyContent="space-between" alignItems="center">
                         <Box>
                           <Typography variant="h6">{modifier.name}</Typography>
                           <Typography variant="body2" color="text.secondary">
                             {modifier.color === 'random' ? 'Random color' : modifier.color} • {modifier.value > 0 ? '+' : ''}{modifier.value}
                           </Typography>
                         </Box>
                         <Box>
                           <IconButton
                             onClick={() => {
                               setEditingModifier(modifier);
                               setNewModifierName(modifier.name);
                               setNewModifierColor(modifier.color);
                               setNewModifierValue(modifier.value);
                               setEditModifierDialogOpen(true);
                             }}
                           >
                             <EditIcon />
                           </IconButton>
                           <IconButton onClick={() => deleteModifier(modifier.id)}>
                             <DeleteIcon />
                           </IconButton>
                         </Box>
                       </Box>
                     </CardContent>
                   </Card>
                 ))}
               </Box>
             )}
           </Box>
         </DialogContent>
         <DialogActions>
           <Button onClick={() => setModifiersDialogOpen(false)}>Close</Button>
         </DialogActions>
       </Dialog>

       {/* Create/Edit Modifier Dialog */}
       <Dialog open={editModifierDialogOpen} onClose={() => setEditModifierDialogOpen(false)} maxWidth="sm" fullWidth>
         <DialogTitle>{editingModifier ? 'Edit Modifier' : 'Add Modifier'}</DialogTitle>
         <DialogContent>
           <Grid container spacing={2} sx={{ mt: 1 }}>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Name"
                 value={newModifierName}
                 onChange={(e) => setNewModifierName(e.target.value)}
                 placeholder="Modifier name"
               />
             </Grid>
             <Grid item xs={12}>
               <FormControl fullWidth>
                 <InputLabel>Color</InputLabel>
                 <Select
                   value={newModifierColor}
                   label="Color"
                   onChange={(e) => setNewModifierColor(e.target.value)}
                 >
                   {COIN_COLORS.map(color => (
                     <MenuItem key={color} value={color}>
                       <Box display="flex" alignItems="center" gap={1}>
                         <Chip 
                           label={color} 
                           size="small" 
                           sx={getCoinColorStyle(color)}
                         />
                       </Box>
                     </MenuItem>
                   ))}
                   <MenuItem value="random">
                     <Box display="flex" alignItems="center" gap={1}>
                       <Chip label="Random" size="small" />
                     </Box>
                   </MenuItem>
                 </Select>
               </FormControl>
             </Grid>
             <Grid item xs={12}>
               <TextField
                 fullWidth
                 label="Value"
                 type="number"
                 value={newModifierValue}
                 onChange={(e) => setNewModifierValue(parseInt(e.target.value) || 0)}
                 helperText="Positive values produce coins, negative values consume coins"
               />
             </Grid>
           </Grid>
         </DialogContent>
         <DialogActions>
           <Button onClick={() => setEditModifierDialogOpen(false)}>Cancel</Button>
           <Button
             onClick={editingModifier ? updateModifier : createModifier}
             disabled={!newModifierName.trim()}
             variant="contained"
           >
             {editingModifier ? 'Update' : 'Create'}
           </Button>
         </DialogActions>
       </Dialog>

    </Box>
  );
}
