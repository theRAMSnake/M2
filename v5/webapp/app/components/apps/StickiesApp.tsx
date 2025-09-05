'use client';

import { useState, useEffect, useRef } from 'react';
import {
  Box,
  Container,
  Paper,
  Typography,
  Button,
  IconButton,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  Alert,
  Fab,
  Grid,
  Card,
  CardContent,
  Tooltip,
} from '@mui/material';
import {
  Add as AddIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  DragIndicator as DragIcon,
  OpenInFull as ResizeIcon,
  Save as SaveIcon,
  Close as CloseIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';
import { generateId } from '../../utils/idGenerator';

interface Sticky {
  id: string;
  title: string;
  content: string;
  position: { x: number; y: number };
  size: { width: number; height: number };
  color: string;
  createdAt: string;
  updatedAt: string;
}

interface StickiesData {
  stickies: Sticky[];
}

const COLORS = [
  '#FFE4E1', // Misty Rose
  '#E6E6FA', // Lavender
  '#F0F8FF', // Alice Blue
  '#F0FFF0', // Honeydew
  '#FFF8DC', // Cornsilk
  '#FDF5E6', // Old Lace
  '#F5F5DC', // Beige
  '#E0FFFF', // Light Cyan
];



export function StickiesApp() {
  const [data, setData] = useState<StickiesData>({ stickies: [] });
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [editingSticky, setEditingSticky] = useState<Sticky | null>(null);
  const [formData, setFormData] = useState({
    title: '',
    content: '',
    color: COLORS[0],
  });
  const [draggedSticky, setDraggedSticky] = useState<string | null>(null);
  const [resizingSticky, setResizingSticky] = useState<string | null>(null);
  const [dragOffset, setDragOffset] = useState({ x: 0, y: 0 });
  const [resizeStart, setResizeStart] = useState({ x: 0, y: 0, width: 0, height: 0 });

  useEffect(() => {
    loadStickies();
  }, []);

  const loadStickies = async () => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      const response = await fetch('/api/stickies', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const result = await response.json();
        setData(result.data || { stickies: [] });
      } else if (response.status === 404) {
        // Document doesn't exist yet, start with empty data
        setData({ stickies: [] });
      } else {
        setError('Failed to load stickies');
      }
    } catch (error) {
      console.error('Failed to load stickies:', error);
      setError('Failed to load stickies');
    } finally {
      setLoading(false);
    }
  };

  const saveStickies = async (newData: StickiesData) => {
    try {
      setError(null);
      const token = getAuthToken();
      const response = await fetch('/api/stickies', {
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
        setError('Failed to save stickies');
      }
    } catch (error) {
      console.error('Failed to save stickies:', error);
      setError('Failed to save stickies');
    }
  };

  const handleAddSticky = () => {
    setEditingSticky(null);
    setFormData({
      title: '',
      content: '',
      color: COLORS[Math.floor(Math.random() * COLORS.length)],
    });
    setDialogOpen(true);
  };

  const handleEditSticky = (sticky: Sticky) => {
    setEditingSticky(sticky);
    setFormData({
      title: sticky.title,
      content: sticky.content,
      color: sticky.color,
    });
    setDialogOpen(true);
  };

  const handleDeleteSticky = async (stickyId: string) => {
    const newData = {
      stickies: data.stickies.filter(s => s.id !== stickyId)
    };
    await saveStickies(newData);
  };

  const handleSaveSticky = async () => {
    if (!formData.title.trim()) return;

    const now = new Date().toISOString();
    let newData: StickiesData;

    if (editingSticky) {
      // Update existing sticky
      newData = {
        stickies: data.stickies.map(s => 
          s.id === editingSticky.id 
            ? { ...s, ...formData, updatedAt: now }
            : s
        )
      };
    } else {
      // Add new sticky
      const newSticky: Sticky = {
        id: generateId(),
        title: formData.title,
        content: formData.content,
        position: { x: 50, y: 50 },
        size: { width: 200, height: 150 },
        color: formData.color,
        createdAt: now,
        updatedAt: now,
      };
      newData = {
        stickies: [...data.stickies, newSticky]
      };
    }

    await saveStickies(newData);
    setDialogOpen(false);
  };

  const handleDragStart = (e: React.MouseEvent, stickyId: string) => {
    e.preventDefault();
    const sticky = data.stickies.find(s => s.id === stickyId);
    if (!sticky) return;

    setDraggedSticky(stickyId);
    setDragOffset({
      x: e.clientX - sticky.position.x,
      y: e.clientY - sticky.position.y,
    });
  };

  const handleDragMove = (e: MouseEvent) => {
    if (!draggedSticky) return;

    const newData = {
      stickies: data.stickies.map(s => 
        s.id === draggedSticky
          ? {
              ...s,
              position: {
                x: e.clientX - dragOffset.x,
                y: e.clientY - dragOffset.y,
              }
            }
          : s
      )
    };
    setData(newData);
  };

  const handleDragEnd = async () => {
    if (draggedSticky) {
      await saveStickies(data);
      setDraggedSticky(null);
    }
  };

  const handleResizeStart = (e: React.MouseEvent, stickyId: string) => {
    e.preventDefault();
    e.stopPropagation();
    const sticky = data.stickies.find(s => s.id === stickyId);
    if (!sticky) return;

    setResizingSticky(stickyId);
    setResizeStart({
      x: e.clientX,
      y: e.clientY,
      width: sticky.size.width,
      height: sticky.size.height,
    });
  };

  const handleResizeMove = (e: MouseEvent) => {
    if (!resizingSticky) return;

    const deltaX = e.clientX - resizeStart.x;
    const deltaY = e.clientY - resizeStart.y;
    const minSize = 100;

    const newData = {
      stickies: data.stickies.map(s => 
        s.id === resizingSticky
          ? {
              ...s,
              size: {
                width: Math.max(minSize, resizeStart.width + deltaX),
                height: Math.max(minSize, resizeStart.height + deltaY),
              }
            }
          : s
      )
    };
    setData(newData);
  };

  const handleResizeEnd = async () => {
    if (resizingSticky) {
      await saveStickies(data);
      setResizingSticky(null);
    }
  };

  useEffect(() => {
    if (draggedSticky) {
      document.addEventListener('mousemove', handleDragMove);
      document.addEventListener('mouseup', handleDragEnd);
      return () => {
        document.removeEventListener('mousemove', handleDragMove);
        document.removeEventListener('mouseup', handleDragEnd);
      };
    }
  }, [draggedSticky, dragOffset, data]);

  useEffect(() => {
    if (resizingSticky) {
      document.addEventListener('mousemove', handleResizeMove);
      document.addEventListener('mouseup', handleResizeEnd);
      return () => {
        document.removeEventListener('mousemove', handleResizeMove);
        document.removeEventListener('mouseup', handleResizeEnd);
      };
    }
  }, [resizingSticky, resizeStart, data]);

  if (loading) {
    return (
      <Container maxWidth="lg" sx={{ py: 4 }}>
        <Typography>Loading...</Typography>
      </Container>
    );
  }

     return (
     <Box sx={{ 
       position: 'fixed', 
       top: 64, // Account for the main navigation bar height
       left: 0, 
       right: 0, 
       bottom: 0, 
       bgcolor: 'background.default',
       display: 'flex',
       flexDirection: 'column',
       zIndex: 1
     }}>
      {/* Header */}
      <Box sx={{ 
        p: 2, 
        borderBottom: '1px solid', 
        borderColor: 'divider',
        bgcolor: 'background.paper',
        display: 'flex',
        justifyContent: 'space-between',
        alignItems: 'center'
      }}>
        <Box>
          <Typography variant="h4" component="h1" gutterBottom>
            Stickies
          </Typography>
          <Typography variant="subtitle1" color="text.secondary">
            Create and organize your sticky notes
          </Typography>
        </Box>
        <Button
          variant="contained"
          startIcon={<AddIcon />}
          onClick={handleAddSticky}
        >
          Add Sticky
        </Button>
      </Box>

      {error && (
        <Alert severity="error" sx={{ m: 2 }} onClose={() => setError(null)}>
          {error}
        </Alert>
      )}

      {/* Canvas Area */}
      <Box sx={{ 
        flex: 1, 
        position: 'relative', 
        bgcolor: 'background.default',
        overflow: 'hidden'
      }}>
                     {data.stickies.length === 0 ? (
             <Box sx={{ 
               textAlign: 'center', 
               py: 8,
               position: 'absolute',
               top: '50%',
               left: '50%',
               transform: 'translate(-50%, -50%)'
             }}>
               <Typography variant="h6" color="text.secondary" gutterBottom>
                 No stickies yet
               </Typography>
               <Typography variant="body2" color="text.secondary" sx={{ mb: 3 }}>
                 Create your first sticky note to get started
               </Typography>
               <Button
                 variant="contained"
                 startIcon={<AddIcon />}
                 onClick={handleAddSticky}
               >
                 Create First Sticky
               </Button>
             </Box>
           ) : (
             data.stickies.map((sticky) => (
               <Box
                 key={sticky.id}
                 sx={{
                   position: 'absolute',
                   left: sticky.position.x,
                   top: sticky.position.y,
                   width: sticky.size.width,
                   height: sticky.size.height,
                   bgcolor: sticky.color,
                   borderRadius: 1,
                   boxShadow: 2,
                   border: '1px solid',
                   borderColor: 'divider',
                   cursor: draggedSticky === sticky.id ? 'grabbing' : 'grab',
                   zIndex: draggedSticky === sticky.id ? 1000 : 1,
                   '&:hover': {
                     boxShadow: 4,
                   },
                 }}
                 onMouseDown={(e) => handleDragStart(e, sticky.id)}
               >
                                   {/* Header */}
                  <Box
                    sx={{
                      display: 'flex',
                      alignItems: 'center',
                      justifyContent: 'space-between',
                      p: 1,
                      borderBottom: '1px solid',
                      borderColor: 'rgba(0,0,0,0.2)',
                      bgcolor: 'rgba(0,0,0,0.1)',
                      borderRadius: '4px 4px 0 0',
                    }}
                  >
                    <Box sx={{ display: 'flex', alignItems: 'center', flex: 1 }}>
                      <DragIcon sx={{ fontSize: 16, mr: 0.5, color: 'rgba(0,0,0,0.7)' }} />
                      <Typography
                        variant="subtitle2"
                        sx={{
                          fontWeight: 'bold',
                          overflow: 'hidden',
                          textOverflow: 'ellipsis',
                          whiteSpace: 'nowrap',
                          flex: 1,
                          color: 'rgba(0,0,0,0.8)',
                        }}
                      >
                        {sticky.title}
                      </Typography>
                    </Box>
                    <Box sx={{ display: 'flex', gap: 0.5 }}>
                      <Tooltip title="Edit">
                        <IconButton
                          size="small"
                          sx={{ 
                            color: 'rgba(0,0,0,0.7)',
                            '&:hover': { 
                              bgcolor: 'rgba(0,0,0,0.1)',
                              color: 'rgba(0,0,0,0.9)'
                            }
                          }}
                          onClick={(e) => {
                            e.stopPropagation();
                            handleEditSticky(sticky);
                          }}
                        >
                          <EditIcon sx={{ fontSize: 16 }} />
                        </IconButton>
                      </Tooltip>
                      <Tooltip title="Delete">
                        <IconButton
                          size="small"
                          sx={{ 
                            color: 'rgba(220,0,0,0.8)',
                            '&:hover': { 
                              bgcolor: 'rgba(220,0,0,0.1)',
                              color: 'rgba(220,0,0,1)'
                            }
                          }}
                          onClick={(e) => {
                            e.stopPropagation();
                            handleDeleteSticky(sticky.id);
                          }}
                        >
                          <DeleteIcon sx={{ fontSize: 16 }} />
                        </IconButton>
                      </Tooltip>
                    </Box>
                  </Box>

                                   {/* Content */}
                  <Box
                    sx={{
                      p: 1,
                      height: `calc(100% - 40px)`,
                      overflow: 'auto',
                      '&::-webkit-scrollbar': {
                        width: '4px',
                      },
                      '&::-webkit-scrollbar-track': {
                        background: 'transparent',
                      },
                      '&::-webkit-scrollbar-thumb': {
                        background: 'rgba(0,0,0,0.2)',
                        borderRadius: '2px',
                      },
                    }}
                  >
                                         <textarea
                       value={sticky.content}
                       onChange={async (e) => {
                         const newContent = e.target.value;
                         if (newContent !== sticky.content) {
                           const newData = {
                             stickies: data.stickies.map(s =>
                               s.id === sticky.id
                                 ? { ...s, content: newContent, updatedAt: new Date().toISOString() }
                                 : s
                             )
                           };
                           await saveStickies(newData);
                         }
                       }}
                       style={{
                         width: '100%',
                         height: '100%',
                         border: 'none',
                         outline: 'none',
                         resize: 'none',
                         background: 'transparent',
                         color: 'rgba(0,0,0,0.8)',
                         fontSize: '14px',
                         lineHeight: '1.4',
                         fontFamily: 'inherit',
                         padding: '8px',
                       }}
                       placeholder="Start typing..."
                     />
                  </Box>

                                   {/* Resize Handle */}
                  <Box
                    sx={{
                      position: 'absolute',
                      bottom: 0,
                      right: 0,
                      width: 16,
                      height: 16,
                      cursor: 'nw-resize',
                      display: 'flex',
                      alignItems: 'center',
                      justifyContent: 'center',
                    }}
                    onMouseDown={(e) => handleResizeStart(e, sticky.id)}
                  >
                    <ResizeIcon sx={{ fontSize: 12, color: 'rgba(0,0,0,0.5)' }} />
                  </Box>
               </Box>
             ))
           )}
         </Box>

         {/* Add Sticky FAB */}
         <Fab
           color="primary"
           aria-label="add sticky"
           sx={{ position: 'fixed', bottom: 16, right: 16, zIndex: 1001 }}
           onClick={handleAddSticky}
         >
           <AddIcon />
         </Fab>

         {/* Sticky Dialog */}
         <Dialog open={dialogOpen} onClose={() => setDialogOpen(false)} maxWidth="sm" fullWidth>
           <DialogTitle>
             {editingSticky ? 'Edit Sticky' : 'Add Sticky'}
           </DialogTitle>
           <DialogContent>
             <TextField
               label="Title"
               value={formData.title}
               onChange={(e) => setFormData({ ...formData, title: e.target.value })}
               fullWidth
               margin="normal"
               required
               autoFocus
             />
             <TextField
               label="Content"
               value={formData.content}
               onChange={(e) => setFormData({ ...formData, content: e.target.value })}
               fullWidth
               margin="normal"
               multiline
               rows={6}
               placeholder="Enter your sticky note content..."
             />
             <Box sx={{ mt: 2 }}>
               <Typography variant="subtitle2" gutterBottom>
                 Color
               </Typography>
               <Box sx={{ display: 'flex', gap: 1, flexWrap: 'wrap' }}>
                 {COLORS.map((color) => (
                   <Box
                     key={color}
                     sx={{
                       width: 32,
                       height: 32,
                       bgcolor: color,
                       borderRadius: 1,
                       border: '2px solid',
                       borderColor: formData.color === color ? 'primary.main' : 'transparent',
                       cursor: 'pointer',
                       '&:hover': {
                         borderColor: 'primary.main',
                       },
                     }}
                     onClick={() => setFormData({ ...formData, color })}
                   />
                 ))}
               </Box>
             </Box>
           </DialogContent>
           <DialogActions>
             <Button onClick={() => setDialogOpen(false)}>Cancel</Button>
             <Button 
               onClick={handleSaveSticky} 
               variant="contained"
               disabled={!formData.title.trim()}
             >
               {editingSticky ? 'Update' : 'Add'}
             </Button>
           </DialogActions>
         </Dialog>
       </Box>
     );
   }
