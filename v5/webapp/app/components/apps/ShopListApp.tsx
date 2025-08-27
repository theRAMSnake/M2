'use client';

import { useState, useEffect } from 'react';
import {
  Box,
  Container,
  Paper,
  Typography,
  TextField,
  Button,
  List,
  ListItem,
  ListItemText,
  ListItemSecondaryAction,
  IconButton,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  Accordion,
  AccordionSummary,
  AccordionDetails,
  Divider,
  Alert,
  Fab,
  Grid,
  Card,
  CardContent,
} from '@mui/material';
import {
  Add as AddIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  ExpandMore as ExpandMoreIcon,
  ShoppingCart as ShoppingCartIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';

interface ShopGroup {
  title: string;
}

interface ShopItem {
  title: string;
  group: string;
}

interface ShopListData {
  groups: ShopGroup[];
  items: ShopItem[];
}

export function ShopListApp() {
  const [data, setData] = useState<ShopListData>({ groups: [], items: [] });
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [groupDialogOpen, setGroupDialogOpen] = useState(false);
  const [bulkEditDialogOpen, setBulkEditDialogOpen] = useState(false);
  const [editingGroup, setEditingGroup] = useState<ShopGroup | null>(null);
  const [bulkEditingGroup, setBulkEditingGroup] = useState<string>('');
  const [groupFormData, setGroupFormData] = useState({ title: '' });
  const [bulkEditText, setBulkEditText] = useState('');

  useEffect(() => {
    loadShopList();
  }, []);

  const loadShopList = async () => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      const response = await fetch('/api/documents/shared/shop_list', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const result = await response.json();
        setData(result.data || { groups: [], items: [] });
      } else if (response.status === 404) {
        // Document doesn't exist yet, start with empty data
        setData({ groups: [], items: [] });
      } else {
        setError('Failed to load shop list');
      }
    } catch (error) {
      console.error('Failed to load shop list:', error);
      setError('Failed to load shop list');
    } finally {
      setLoading(false);
    }
  };

  const saveShopList = async (newData: ShopListData) => {
    try {
      setError(null);
      const token = getAuthToken();
              const response = await fetch('/api/documents/shared/shop_list', {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(newData)
      });

      if (response.ok) {
        setData(newData);
      } else {
        setError('Failed to save shop list');
      }
    } catch (error) {
      console.error('Failed to save shop list:', error);
      setError('Failed to save shop list');
    }
  };

  const handleAddGroup = () => {
    setEditingGroup(null);
    setGroupFormData({ title: '' });
    setGroupDialogOpen(true);
  };

  const handleEditGroup = (group: ShopGroup) => {
    setEditingGroup(group);
    setGroupFormData({ title: group.title });
    setGroupDialogOpen(true);
  };

  const handleDeleteGroup = async (groupTitle: string) => {
    const newData = {
      groups: data.groups.filter(g => g.title !== groupTitle),
      items: data.items.filter(item => item.group !== groupTitle)
    };
    await saveShopList(newData);
  };

  const handleSaveGroup = async () => {
    if (!groupFormData.title.trim()) return;

    let newData: ShopListData;
    if (editingGroup) {
      // Update existing group
      newData = {
        groups: data.groups.map(g => 
          g.title === editingGroup.title ? { title: groupFormData.title } : g
        ),
        items: data.items.map(item => 
          item.group === editingGroup.title ? { ...item, group: groupFormData.title } : item
        )
      };
    } else {
      // Add new group
      newData = {
        groups: [...data.groups, { title: groupFormData.title }],
        items: data.items
      };
    }

    await saveShopList(newData);
    setGroupDialogOpen(false);
  };



  const handleBulkEdit = (groupTitle: string) => {
    const groupItems = data.items.filter(item => item.group === groupTitle);
    const itemsText = groupItems.map(item => item.title).join('\n');
    setBulkEditingGroup(groupTitle);
    setBulkEditText(itemsText);
    setBulkEditDialogOpen(true);
  };

  const handleSaveBulkEdit = async () => {
    if (!bulkEditingGroup) return;

    // Parse the text into individual items (split by newlines and filter out empty lines)
    const itemTitles = bulkEditText
      .split('\n')
      .map(line => line.trim())
      .filter(line => line.length > 0);

    // Remove existing items for this group
    const itemsWithoutGroup = data.items.filter(item => item.group !== bulkEditingGroup);
    
    // Add new items
    const newItems = itemTitles.map(title => ({
      title,
      group: bulkEditingGroup
    }));

    const newData = {
      groups: data.groups,
      items: [...itemsWithoutGroup, ...newItems]
    };

    await saveShopList(newData);
    setBulkEditDialogOpen(false);
  };

  if (loading) {
    return (
      <Container maxWidth="lg" sx={{ py: 4 }}>
        <Typography>Loading...</Typography>
      </Container>
    );
  }

  return (
    <Container maxWidth="lg" sx={{ py: 4 }}>
      <Paper sx={{ p: 4, bgcolor: 'background.paper' }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
          <Box>
            <Typography variant="h4" component="h1" gutterBottom>
              Shop List
            </Typography>
            <Typography variant="subtitle1" color="text.secondary">
              Manage your shopping lists
            </Typography>
          </Box>
          <Button
            variant="contained"
            startIcon={<AddIcon />}
            onClick={handleAddGroup}
          >
            Add Group
          </Button>
        </Box>

        {error && (
          <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
            {error}
          </Alert>
        )}

        {data.groups.length === 0 ? (
          <Box sx={{ textAlign: 'center', py: 8 }}>
            <ShoppingCartIcon sx={{ fontSize: 64, color: 'text.secondary', mb: 2 }} />
            <Typography variant="h6" color="text.secondary" gutterBottom>
              No shopping groups yet
            </Typography>
            <Typography variant="body2" color="text.secondary" sx={{ mb: 3 }}>
              Create your first shopping group to get started
            </Typography>
            <Button
              variant="contained"
              startIcon={<AddIcon />}
              onClick={handleAddGroup}
            >
              Create First Group
            </Button>
          </Box>
        ) : (
          <Box>
            {data.groups.map((group) => {
              const groupItems = data.items.filter(item => item.group === group.title);
              return (
                <Accordion key={group.title} defaultExpanded sx={{ mb: 2 }}>
                  <AccordionSummary expandIcon={<ExpandMoreIcon />}>
                    <Box sx={{ display: 'flex', alignItems: 'center', width: '100%' }}>
                      <Typography variant="h6" sx={{ flexGrow: 1 }}>
                        {group.title}
                      </Typography>
                      <Box sx={{ display: 'flex', gap: 1 }}>
                        <IconButton
                          size="small"
                          onClick={(e) => {
                            e.stopPropagation();
                            handleEditGroup(group);
                          }}
                        >
                          <EditIcon />
                        </IconButton>
                        <IconButton
                          size="small"
                          color="error"
                          onClick={(e) => {
                            e.stopPropagation();
                            handleDeleteGroup(group.title);
                          }}
                        >
                          <DeleteIcon />
                        </IconButton>
                      </Box>
                    </Box>
                  </AccordionSummary>
                                     <AccordionDetails>
                     <Box sx={{ mb: 2 }}>
                       <Button
                         variant="outlined"
                         size="small"
                         startIcon={<EditIcon />}
                         onClick={() => handleBulkEdit(group.title)}
                       >
                         Edit Items
                       </Button>
                     </Box>
                    
                    {groupItems.length === 0 ? (
                      <Typography variant="body2" color="text.secondary" sx={{ fontStyle: 'italic' }}>
                        No items in this group yet
                      </Typography>
                                         ) : (
                       <List dense>
                         {groupItems.map((item, index) => (
                           <ListItem key={`${item.title}-${index}`} sx={{ pl: 0 }}>
                             <ListItemText primary={item.title} />
                           </ListItem>
                         ))}
                       </List>
                     )}
                  </AccordionDetails>
                </Accordion>
              );
            })}
          </Box>
        )}

        {/* Group Dialog */}
        <Dialog open={groupDialogOpen} onClose={() => setGroupDialogOpen(false)} maxWidth="sm" fullWidth>
          <DialogTitle>
            {editingGroup ? 'Edit Group' : 'Add Group'}
          </DialogTitle>
          <DialogContent>
            <TextField
              label="Group Title"
              value={groupFormData.title}
              onChange={(e) => setGroupFormData({ title: e.target.value })}
              fullWidth
              margin="normal"
              required
              autoFocus
            />
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setGroupDialogOpen(false)}>Cancel</Button>
            <Button 
              onClick={handleSaveGroup} 
              variant="contained"
              disabled={!groupFormData.title.trim()}
            >
              {editingGroup ? 'Update' : 'Add'}
            </Button>
          </DialogActions>
        </Dialog>

        

         {/* Bulk Edit Dialog */}
         <Dialog open={bulkEditDialogOpen} onClose={() => setBulkEditDialogOpen(false)} maxWidth="md" fullWidth>
           <DialogTitle>
             Bulk Edit Items - {bulkEditingGroup}
           </DialogTitle>
           <DialogContent>
             <Typography variant="body2" color="text.secondary" sx={{ mb: 2 }}>
               Edit all items in this group. Each line represents one item. Empty lines will be ignored.
             </Typography>
             <TextField
               label="Items (one per line)"
               value={bulkEditText}
               onChange={(e) => setBulkEditText(e.target.value)}
               fullWidth
               multiline
               rows={12}
               variant="outlined"
               autoFocus
             />
           </DialogContent>
           <DialogActions>
             <Button onClick={() => setBulkEditDialogOpen(false)}>Cancel</Button>
             <Button 
               onClick={handleSaveBulkEdit} 
               variant="contained"
             >
               Save Changes
             </Button>
           </DialogActions>
         </Dialog>
      </Paper>
    </Container>
  );
}
