'use client';

import { useState, useEffect } from 'react';
import {
  Box,
  Container,
  Paper,
  Typography,
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  MenuItem,
  IconButton,
  FormControlLabel,
  Checkbox,
  Alert,
  List,
  ListItem,
  ListItemText,
  ListItemButton,
  ListItemIcon,
  Chip,
  Divider,
  Accordion,
  AccordionSummary,
  AccordionDetails,
  FormControl,
  InputLabel,
  Select,
  useTheme,
} from '@mui/material';
import {
  Add as AddIcon,
  Delete as DeleteIcon,
  Edit as EditIcon,
  ExpandMore as ExpandMoreIcon,
  CheckCircle as CheckCircleIcon,
  RadioButtonUnchecked as RadioButtonUncheckedIcon,
  PriorityHigh as PriorityHighIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';
import { COLOR_PRESETS, getColorValue, getDefaultColor } from '../../utils/colorPresets';

interface Chore {
  id: string;
  title: string;
  is_urgent: boolean;
  color: string;
  is_done: boolean;
  reoccurance_period?: number;
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
}

export function ChoresApp() {
  const theme = useTheme();
  
  const [personalChores, setPersonalChores] = useState<Chore[]>([]);
  const [familyChores, setFamilyChores] = useState<Chore[]>([]);
  const [loading, setLoading] = useState(true);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [choreToDelete, setChoreToDelete] = useState<Chore | null>(null);
  const [editingChore, setEditingChore] = useState<Chore | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [hideDoneChores, setHideDoneChores] = useState(false);
  const [choreType, setChoreType] = useState<'personal' | 'family'>('personal');
  const [formData, setFormData] = useState({
    title: '',
    is_urgent: false,
    color: getDefaultColor(),
    is_done: false,
    reoccurance_period: undefined as number | undefined,
  });

  useEffect(() => {
    loadChores();
  }, []);

  const loadChores = async () => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      
      // Load personal chores
      const personalResponse = await fetch('/api/chores/personal', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (personalResponse.ok) {
        const personalData = await personalResponse.json();
        setPersonalChores(personalData.data?.chores || []);
      }

      // Load family chores
      const familyResponse = await fetch('/api/chores/family', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (familyResponse.ok) {
        const familyData = await familyResponse.json();
        setFamilyChores(familyData.data?.chores || []);
      }
    } catch (error) {
      console.error('Failed to load chores:', error);
      setError('Failed to load chores');
    } finally {
      setLoading(false);
    }
  };

  const handleAddChore = () => {
    setEditingChore(null);
    setFormData({
      title: '',
      is_urgent: false,
      color: getDefaultColor(),
      is_done: false,
      reoccurance_period: undefined,
    });
    setDialogOpen(true);
  };

  const handleEditChore = (chore: Chore) => {
    setEditingChore(chore);
    setChoreType(chore.createdBy === 'shared' ? 'family' : 'personal');
    setFormData({
      title: chore.title,
      is_urgent: chore.is_urgent,
      color: chore.color,
      is_done: chore.is_done,
      reoccurance_period: chore.reoccurance_period,
    });
    setDialogOpen(true);
  };

  const handleDeleteChore = (chore: Chore) => {
    setChoreToDelete(chore);
    setDeleteDialogOpen(true);
  };

  const handleSaveChore = async () => {
    try {
      setError(null);
      const token = getAuthToken();
      
      const choreData = {
        ...formData,
        id: editingChore?.id || Date.now().toString(),
      };

      const endpoint = choreType === 'personal' ? '/api/chores/personal' : '/api/chores/family';
      const method = editingChore ? 'PUT' : 'POST';
      const url = editingChore ? `${endpoint}/${editingChore.id}` : endpoint;

      const response = await fetch(url, {
        method,
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(choreData)
      });

      if (response.ok) {
        setDialogOpen(false);
        await loadChores();
      } else {
        const errorData = await response.json();
        setError(errorData.message || 'Failed to save chore');
      }
    } catch (error) {
      console.error('Failed to save chore:', error);
      setError('Failed to save chore');
    }
  };

  const handleDeleteConfirm = async () => {
    if (!choreToDelete) return;

    try {
      setError(null);
      const token = getAuthToken();
      
      const endpoint = choreToDelete.createdBy === 'shared' ? '/api/chores/family' : '/api/chores/personal';
      const response = await fetch(`${endpoint}/${choreToDelete.id}`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        setDeleteDialogOpen(false);
        setChoreToDelete(null);
        await loadChores();
      } else {
        const errorData = await response.json();
        setError(errorData.message || 'Failed to delete chore');
      }
    } catch (error) {
      console.error('Failed to delete chore:', error);
      setError('Failed to delete chore');
    }
  };

  const handleToggleChore = async (chore: Chore) => {
    try {
      setError(null);
      const token = getAuthToken();
      
      const updatedChore = {
        ...chore,
        is_done: !chore.is_done,
      };

      const endpoint = chore.createdBy === 'shared' ? '/api/chores/family' : '/api/chores/personal';
      const response = await fetch(`${endpoint}/${chore.id}`, {
        method: 'PUT',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(updatedChore)
      });

      if (response.ok) {
        await loadChores();
      } else {
        const errorData = await response.json();
        setError(errorData.message || 'Failed to update chore');
      }
    } catch (error) {
      console.error('Failed to update chore:', error);
      setError('Failed to update chore');
    }
  };

  const getFilteredChores = (chores: Chore[]) => {
    if (hideDoneChores) {
      return chores.filter(chore => !chore.is_done);
    }
    return chores;
  };

  const renderChoreList = (chores: Chore[], title: string, type: 'personal' | 'family') => {
    const filteredChores = getFilteredChores(chores);
    
    return (
      <Accordion defaultExpanded>
        <AccordionSummary expandIcon={<ExpandMoreIcon />}>
          <Typography variant="h6" sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
            {title}
            <Chip 
              label={filteredChores.length} 
              size="small" 
              color="primary" 
              variant="outlined"
            />
          </Typography>
        </AccordionSummary>
        <AccordionDetails>
          {filteredChores.length === 0 ? (
            <Typography variant="body2" color="text.secondary" sx={{ fontStyle: 'italic' }}>
              No {type} chores found
            </Typography>
          ) : (
            <List>
              {filteredChores.map((chore) => (
                <ListItem
                  key={chore.id}
                  disablePadding
                  sx={{
                    mb: 1,
                    border: `2px solid ${getColorValue(chore.color)}`,
                    borderRadius: 1,
                    backgroundColor: chore.is_done ? 'action.hover' : 'background.paper',
                  }}
                >
                  <ListItemButton
                    onClick={() => handleToggleChore(chore)}
                    sx={{
                      textDecoration: chore.is_done ? 'line-through' : 'none',
                      color: chore.is_urgent && !chore.is_done ? 'error.main' : 'inherit',
                    }}
                  >
                    <ListItemIcon>
                      {chore.is_done ? (
                        <CheckCircleIcon color="success" />
                      ) : (
                        <RadioButtonUncheckedIcon />
                      )}
                    </ListItemIcon>
                    <ListItemText
                      primary={
                        <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                          {chore.title}
                          {chore.is_urgent && !chore.is_done && (
                            <PriorityHighIcon color="error" fontSize="small" />
                          )}
                          {chore.reoccurance_period && (
                            <Chip 
                              label={`${chore.reoccurance_period}d`} 
                              size="small" 
                              variant="outlined"
                              color="secondary"
                            />
                          )}
                        </Box>
                      }
                      secondary={
                        <Typography variant="caption" color="text.secondary">
                          {new Date(chore.updatedAt).toLocaleDateString()}
                        </Typography>
                      }
                    />
                    <Box sx={{ display: 'flex', gap: 1 }}>
                      <IconButton
                        size="small"
                        onClick={(e) => {
                          e.stopPropagation();
                          handleEditChore(chore);
                        }}
                      >
                        <EditIcon fontSize="small" />
                      </IconButton>
                      <IconButton
                        size="small"
                        onClick={(e) => {
                          e.stopPropagation();
                          handleDeleteChore(chore);
                        }}
                        color="error"
                      >
                        <DeleteIcon fontSize="small" />
                      </IconButton>
                    </Box>
                  </ListItemButton>
                </ListItem>
              ))}
            </List>
          )}
        </AccordionDetails>
      </Accordion>
    );
  };

  if (loading) {
    return (
      <Container maxWidth="md">
        <Typography>Loading chores...</Typography>
      </Container>
    );
  }

  return (
    <Container maxWidth="md">
      <Box sx={{ mb: 3, display: 'flex', justifyContent: 'space-between', alignItems: 'center' }}>
        <Typography variant="h4" component="h1">
          Chores
        </Typography>
        <Button
          variant="contained"
          startIcon={<AddIcon />}
          onClick={handleAddChore}
        >
          Add Chore
        </Button>
      </Box>

      {error && (
        <Alert severity="error" sx={{ mb: 2 }}>
          {error}
        </Alert>
      )}

      <Box sx={{ mb: 2 }}>
        <FormControlLabel
          control={
            <Checkbox
              checked={hideDoneChores}
              onChange={(e) => setHideDoneChores(e.target.checked)}
            />
          }
          label="Hide done chores"
        />
      </Box>

      <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2 }}>
        {renderChoreList(personalChores, 'Personal Chores', 'personal')}
        {renderChoreList(familyChores, 'Family Chores', 'family')}
      </Box>

      {/* Add/Edit Chore Dialog */}
      <Dialog open={dialogOpen} onClose={() => setDialogOpen(false)} maxWidth="sm" fullWidth>
        <DialogTitle>
          {editingChore ? 'Edit Chore' : 'Add New Chore'}
        </DialogTitle>
        <DialogContent>
          <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2, mt: 1 }}>
            <TextField
              label="Title"
              value={formData.title}
              onChange={(e) => setFormData({ ...formData, title: e.target.value })}
              fullWidth
              required
            />
            
            <FormControl fullWidth>
              <InputLabel>Color</InputLabel>
              <Select
                value={formData.color}
                onChange={(e) => setFormData({ ...formData, color: e.target.value })}
                label="Color"
              >
                {COLOR_PRESETS.map((color) => (
                  <MenuItem key={color.value} value={color.value}>
                    <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                      <Box
                        sx={{
                          width: 20,
                          height: 20,
                          backgroundColor: color.color,
                          borderRadius: '50%',
                          border: '1px solid #ccc',
                        }}
                      />
                      {color.label}
                    </Box>
                  </MenuItem>
                ))}
              </Select>
            </FormControl>

            <TextField
              label="Recurrence (days)"
              type="number"
              value={formData.reoccurance_period || ''}
              onChange={(e) => {
                const value = e.target.value;
                setFormData({ 
                  ...formData, 
                  reoccurance_period: value ? Number(value) : undefined 
                });
              }}
              fullWidth
              placeholder="Leave empty for no recurrence"
              inputProps={{ min: 1 }}
              helperText="Enter number of days for recurrence (e.g., 7 for weekly)"
            />

            <FormControlLabel
              control={
                <Checkbox
                  checked={formData.is_urgent}
                  onChange={(e) => setFormData({ ...formData, is_urgent: e.target.checked })}
                />
              }
              label="Urgent"
            />

            <FormControlLabel
              control={
                <Checkbox
                  checked={formData.is_done}
                  onChange={(e) => setFormData({ ...formData, is_done: e.target.checked })}
                />
              }
              label="Done"
            />

            {!editingChore && (
              <FormControl fullWidth>
                <InputLabel>Chore Type</InputLabel>
                <Select
                  value={choreType}
                  onChange={(e) => setChoreType(e.target.value as 'personal' | 'family')}
                  label="Chore Type"
                >
                  <MenuItem value="personal">Personal</MenuItem>
                  <MenuItem value="family">Family</MenuItem>
                </Select>
              </FormControl>
            )}
          </Box>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleSaveChore} variant="contained">
            {editingChore ? 'Update' : 'Add'}
          </Button>
        </DialogActions>
      </Dialog>

      {/* Delete Confirmation Dialog */}
      <Dialog open={deleteDialogOpen} onClose={() => setDeleteDialogOpen(false)}>
        <DialogTitle>Delete Chore</DialogTitle>
        <DialogContent>
          <Typography>
            Are you sure you want to delete "{choreToDelete?.title}"? This action cannot be undone.
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDeleteDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleDeleteConfirm} color="error" variant="contained">
            Delete
          </Button>
        </DialogActions>
      </Dialog>
    </Container>
  );
}
