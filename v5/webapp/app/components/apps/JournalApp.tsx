'use client';

import { useState, useEffect, useCallback, useRef } from 'react';
import dynamic from 'next/dynamic';
// Type import causes propTypes mismatch in Next dynamic; avoid strict typing here
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
  Snackbar,
  Fab,
  Grid,
  Card,
  CardContent,
  Tooltip,
  Menu,
  MenuItem,
  ListItemIcon,
  ListItemText,
  CircularProgress,
} from '@mui/material';
import { useTheme } from '@mui/material/styles';
import {
  Add as AddIcon,
  Edit as EditIcon,
  Delete as DeleteIcon,
  Folder as FolderIcon,
  Description as PageIcon,
  FolderOpen as FolderOpenIcon,
  MoreVert as MoreVertIcon,
  Save as SaveIcon,
  Close as CloseIcon,
  ExpandMore as ExpandMoreIcon,
  ChevronRight as ChevronRightIcon,
} from '@mui/icons-material';
// Simple tree view implementation
// TinyMCE will be loaded dynamically
import { getAuthToken } from '../../utils/auth';
import { generateId } from '../../utils/idGenerator';

// Use Next.js dynamic import for TinyMCE Editor; cast to any to satisfy Next types
const Editor = dynamic<any>(
  () => import('@tinymce/tinymce-react').then(m => (m as any).Editor as any),
  {
  ssr: false,
  loading: () => (
    <Box sx={{ 
      display: 'flex', 
      alignItems: 'center', 
      justifyContent: 'center', 
      height: '100%',
      flexDirection: 'column',
      gap: 2,
      border: '1px solid #ccc',
      borderRadius: 1,
      p: 2
    }}>
      <CircularProgress />
      <Typography variant="body2" color="text.secondary">
        Loading editor...
      </Typography>
    </Box>
  )
}
);

interface JournalItem {
  id: string;
  name: string;
  type: 'folder' | 'page';
  path: string;
  parentPath?: string;
  createdBy: string;
  createdAt: string;
  updatedAt: string;
  content?: string;
}

interface JournalAppProps {
  userId: string;
}

const JournalApp: React.FC<JournalAppProps> = ({ userId }) => {
  const theme = useTheme();
  const editorRef = useRef<any>(null);
  const [items, setItems] = useState<JournalItem[]>([]);
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [editorLoaded, setEditorLoaded] = useState(false);
  const [selectedItem, setSelectedItem] = useState<JournalItem | null>(null);
  const [expandedNodes, setExpandedNodes] = useState<string[]>([]);
  
  // Dialog states
  const [createDialogOpen, setCreateDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [itemToDelete, setItemToDelete] = useState<JournalItem | null>(null);
  
  // Form states
  const [newItemName, setNewItemName] = useState('');
  const [newItemType, setNewItemType] = useState<'folder' | 'page'>('folder');
  const [newItemParent, setNewItemParent] = useState<string>('');
  const [editItemName, setEditItemName] = useState('');
  const [editItemContent, setEditItemContent] = useState('');
  const [hasUnsavedChanges, setHasUnsavedChanges] = useState(false);
  const [saveNotice, setSaveNotice] = useState<string | null>(null);

  // Build TinyMCE content CSS from MUI theme
  const editorContentStyle = `
    body { background: ${theme.palette.background.paper}; color: ${theme.palette.text.primary}; font-family: Helvetica,Arial,sans-serif; font-size: 14px; }
    a { color: ${theme.palette.primary.main}; }
    table, th, td { border-color: ${theme.palette.divider} !important; }
    hr { border-color: ${theme.palette.divider}; }
    blockquote { color: ${theme.palette.text.secondary}; border-left: 4px solid ${theme.palette.divider}; padding-left: 8px; }
    code { background: ${theme.palette.background.default}; color: ${theme.palette.warning.light}; padding: 2px 4px; border-radius: 3px; }
    pre { background: ${theme.palette.background.default}; color: ${theme.palette.text.primary}; padding: 8px; border-radius: 4px; }
  `;
  
  // No local preload: load TinyMCE from jsDelivr to avoid Tiny Cloud

  // Context menu states
  const [contextMenu, setContextMenu] = useState<{
    mouseX: number;
    mouseY: number;
    item: JournalItem | null;
  } | null>(null);

  // Use relative API base so it works in dev (Next rewrites to 3000) and prod (same-origin)
  const API_BASE = '/api';

  // fetch items for a given path (helper)
  const fetchItemsForPath = useCallback(async (path: string = ''): Promise<JournalItem[]> => {
    const token = getAuthToken();
    const response = await fetch(`${API_BASE}/journal/tree?path=${encodeURIComponent(path)}`, {
      headers: {
        'Authorization': `Bearer ${token}`,
      },
    });
    if (!response.ok) throw new Error(`HTTP error! status: ${response.status}`);
    const responseData = await response.json();
    return responseData.data?.items || [];
  }, [API_BASE]);

  const fetchItems = useCallback(async (path: string = '') => {
    try {
      setLoading(true);
      setError(null);
      const list = await fetchItemsForPath(path);
      
      if (path) {
        setItems(prev => {
          const others = prev.filter(i => i.parentPath !== path);
          return [...others, ...list];
        });
      } else {
        setItems(list);
      }
    } catch (err) {
      console.error('Error fetching journal items:', err);
      setError(err instanceof Error ? err.message : 'Failed to fetch journal items');
    } finally {
      setLoading(false);
    }
  }, [fetchItemsForPath]);

  const loadChildren = useCallback(async (parentPath: string) => {
    if (!parentPath) return;
    try {
      const children = await fetchItemsForPath(parentPath);
      if (children.length === 0) return;
      setItems(prev => {
        const others = prev.filter(i => i.parentPath !== parentPath);
        const existing = new Set(others.map(i => i.id));
        return [...others, ...children.filter(c => !existing.has(c.id))];
      });
    } catch (e) {
      console.error('Error lazy loading children:', e);
    }
  }, [fetchItemsForPath]);

  const fetchItemContent = useCallback(async (item: JournalItem) => {
    if (item.type !== 'page') return;
    try {
      const token = getAuthToken();
      const response = await fetch(`${API_BASE}/journal/pages/${item.id}`, {
        headers: {
          'Authorization': `Bearer ${token}`,
        },
      });

      if (response.ok) {
        const body = await response.json();
        const page = body?.data || body;
        const nextTitle = typeof page?.title === 'string' ? page.title : '';
        const nextContent = page?.content || '';
        setEditItemName(nextTitle);
        setEditItemContent(nextContent);
        setHasUnsavedChanges(false);
        // Update editor content directly and reset dirty
        if (editorRef.current) {
          editorRef.current.setContent(nextContent || '');
          editorRef.current.setDirty(false);
        }
      }
    } catch (err) {
      console.error('Error fetching page content:', err);
    }
  }, [API_BASE]);

  useEffect(() => {
    fetchItems();
  }, [fetchItems]);

  const handleCreateItem = async () => {
    try {
      setLoading(true);
      setError(null);
      
      const token = getAuthToken();
      const endpoint = newItemType === 'folder' ? 'folders' : 'pages';
      const payload = newItemType === 'folder' 
        ? { name: newItemName, parentPath: newItemParent }
        : { title: newItemName, content: '', parentPath: newItemParent };

      const response = await fetch(`${API_BASE}/journal/${endpoint}`, {
        method: 'POST',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`,
        },
        body: JSON.stringify(payload),
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }

      // Refresh tree: if created under a folder, lazy-load that folder and expand it; otherwise refresh root
      const parent = (newItemParent || '').toLowerCase();
      if (parent) {
        await loadChildren(parent);
        // expand the parent node in the UI
        const parentItem = items.find(i => (i.path || '').toLowerCase() === parent);
        if (parentItem) {
          setExpandedNodes(prev => prev.includes(parentItem.id) ? prev : [...prev, parentItem.id]);
        }
      } else {
        await fetchItems('');
      }

      setCreateDialogOpen(false);
      setNewItemName('');
      setNewItemParent('');
    } catch (err) {
      console.error('Error creating journal item:', err);
      setError(err instanceof Error ? err.message : 'Failed to create journal item');
    } finally {
      setLoading(false);
    }
  };

  const handleEditItem = async () => {
    if (!selectedItem) return;

    try {
      setLoading(true);
      setError(null);
      
      const token = getAuthToken();
      const endpoint = selectedItem.type === 'folder' ? 'folders' : 'pages';
      const payload = selectedItem.type === 'folder' 
        ? { name: editItemName }
        : { title: editItemName, content: editItemContent };

      const response = await fetch(`${API_BASE}/journal/${endpoint}/${selectedItem.id}`, {
        method: 'PUT',
        headers: {
          'Content-Type': 'application/json',
          'Authorization': `Bearer ${token}`,
        },
        body: JSON.stringify(payload),
      });

      if (!response.ok) {
        let message = `HTTP error! status: ${response.status}`;
        try {
          const body = await response.json();
          if (body && (body.message || body.error)) {
            message = body.message || body.error;
          }
        } catch {}
        throw new Error(message);
      }

      setHasUnsavedChanges(false);
      // Update local item list without collapsing the tree
      setItems(prev => prev.map(i => i.id === selectedItem.id
        ? (selectedItem.type === 'folder'
            ? { ...i, name: editItemName, updatedAt: new Date().toISOString() }
            : { ...i, name: editItemName, updatedAt: new Date().toISOString(), content: editItemContent })
        : i
      ));
      setSaveNotice('Changes saved');
    } catch (err) {
      console.error('Error updating journal item:', err);
      setError(err instanceof Error ? err.message : 'Failed to update journal item');
    } finally {
      setLoading(false);
    }
  };

  const handleAutoSave = async () => {
    if (hasUnsavedChanges && selectedItem) {
      await handleEditItem(); // Auto-save without closing
    }
  };

  const handleDeleteItem = async () => {
    if (!itemToDelete) return;

    try {
      setLoading(true);
      setError(null);
      
      const token = getAuthToken();
      const endpoint = itemToDelete.type === 'folder' ? 'folders' : 'pages';

      const response = await fetch(`${API_BASE}/journal/${endpoint}/${itemToDelete.id}`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`,
        },
      });

      if (!response.ok) {
        throw new Error(`HTTP error! status: ${response.status}`);
      }

      // If deleted item was selected, clear selection
      if (selectedItem && selectedItem.id === itemToDelete.id) {
        setSelectedItem(null);
        setEditItemName('');
        setEditItemContent('');
        setHasUnsavedChanges(false);
      }

      setDeleteDialogOpen(false);
      setItemToDelete(null);

      // Refresh root first
      await fetchItems('');

      // Re-load expanded folders to preserve their children
      for (const nodeId of expandedNodes) {
        const node = items.find(i => i.id === nodeId);
        if (node && node.type === 'folder' && node.path) {
          // eslint-disable-next-line no-await-in-loop
          await loadChildren(node.path);
        }
      }
    } catch (err) {
      console.error('Error deleting journal item:', err);
      setError(err instanceof Error ? err.message : 'Failed to delete journal item');
    } finally {
      setLoading(false);
    }
  };

  const handleItemClick = async (item: JournalItem) => {
    setSelectedItem(item);
    setHasUnsavedChanges(false);
    if (item.type === 'page') {
      // Reset to the item we have and push into editor without marking unsaved
      const initialContent = item.content || '';
      setEditItemName(item.name);
      setEditItemContent(initialContent);
      if (editorRef.current) {
        editorRef.current.setContent(initialContent);
        editorRef.current.setDirty(false);
      }
      await fetchItemContent(item);
    }
  };

  const handleContextMenu = (event: React.MouseEvent, item: JournalItem) => {
    event.preventDefault();
    setContextMenu({
      mouseX: event.clientX + 2,
      mouseY: event.clientY - 6,
      item,
    });
  };

  const handleContextMenuClose = () => {
    setContextMenu(null);
  };

  const handleEdit = (item: JournalItem) => {
    setSelectedItem(item);
    setEditItemName(item.name);
    setHasUnsavedChanges(false);
    if (item.type === 'page') {
      setEditItemContent(item.content || '');
    }
    handleContextMenuClose();
  };

  const handleDelete = (item: JournalItem) => {
    setItemToDelete(item);
    setDeleteDialogOpen(true);
    handleContextMenuClose();
  };

  const handleCreateInFolder = (parentPath: string) => {
    setNewItemParent(parentPath);
    setCreateDialogOpen(true);
    handleContextMenuClose();
  };

  const renderTreeItems = (items: JournalItem[], parentPath: string = '', level: number = 0): React.ReactNode => {
    const target = (parentPath || '').toLowerCase();
    const filteredItems = items.filter(item => (item.parentPath || '').toLowerCase() === target);
    
    return filteredItems.map((item) => (
      <Box key={item.id} sx={{ ml: level * 2 }}>
        <Box
          sx={{
            display: 'flex',
            alignItems: 'center',
            py: 0.5,
            px: 1,
            borderRadius: 1,
            cursor: 'pointer',
            backgroundColor: selectedItem?.id === item.id ? 'primary.light' : 'transparent',
            color: selectedItem?.id === item.id ? 'primary.contrastText' : 'inherit',
            '&:hover': {
              backgroundColor: selectedItem?.id === item.id ? 'primary.main' : 'action.hover',
            },
          }}
          onContextMenu={(e) => handleContextMenu(e, item)}
          onClick={async () => {
            // Always select the item first
            setSelectedItem(item);
            setHasUnsavedChanges(false);
            
            if (item.type === 'folder') {
              // For folders, also toggle expand/collapse
              const isOpen = expandedNodes.includes(item.id);
              const next = isOpen ? expandedNodes.filter(id => id !== item.id) : [...expandedNodes, item.id];
              setExpandedNodes(next);
              if (!isOpen) {
                await loadChildren(item.path);
              }
            } else {
              // For pages, open the editor
              await handleItemClick(item);
            }
          }}
        >
          {item.type === 'folder' ? (
            expandedNodes.includes(item.id) ? <FolderOpenIcon sx={{ mr: 1 }} /> : <FolderIcon sx={{ mr: 1 }} />
          ) : (
            <PageIcon sx={{ mr: 1 }} />
          )}
          <Typography variant="body2" sx={{ flexGrow: 1 }}>
            {item.name}
          </Typography>
          <IconButton
            size="small"
            onClick={(e) => {
              e.stopPropagation();
              handleContextMenu(e, item);
            }}
          >
            <MoreVertIcon fontSize="small" />
          </IconButton>
        </Box>
        {item.type === 'folder' && expandedNodes.includes(item.id) && 
          renderTreeItems(items, item.path, level + 1)
        }
      </Box>
    ));
  };

  return (
    <Container maxWidth={false} disableGutters sx={{ p: 2, height: 'calc(100vh - 120px)', boxSizing: 'border-box' }}>
      <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
        <Typography variant="h4" component="h1">
          Journal
        </Typography>
        <Button
          variant="contained"
          startIcon={<AddIcon />}
          onClick={() => {
            const parent = selectedItem
              ? (selectedItem.type === 'folder' ? (selectedItem.path || '') : (selectedItem.parentPath || ''))
              : '';
            setNewItemParent((parent || '').toLowerCase());
            setCreateDialogOpen(true);
          }}
        >
          New Item
        </Button>
      </Box>

      {error && (
        <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
          {error}
        </Alert>
      )}

      <Grid container spacing={2} sx={{ height: 'calc(100% - 64px)', minHeight: 0 }}>
        <Grid item xs={12} md={4} sx={{ height: '100%', minHeight: 0 }}>
          <Paper sx={{ p: 2, height: '100%', overflow: 'auto', display: 'flex', flexDirection: 'column' }}>
            <Typography variant="h6" sx={{ mb: 2 }}>
              Journal Structure
            </Typography>
            {loading ? (
              <Typography>Loading...</Typography>
            ) : (
              <Box sx={{ flex: 1, minHeight: 0, userSelect: 'none' }}>
                {renderTreeItems(items)}
              </Box>
            )}
          </Paper>
        </Grid>

        <Grid item xs={12} md={8} sx={{ height: '100%', minHeight: 0 }}>
          <Paper sx={{ p: 2, height: '100%', overflow: 'hidden', display: 'flex', flexDirection: 'column' }}>
            <Typography variant="h6" sx={{ mb: 2 }}>
              {selectedItem ? `Edit ${selectedItem.type}: ${selectedItem.name}` : 'Select an item to edit'}
            </Typography>
            {selectedItem && selectedItem.type === 'page' && (
              <Box sx={{ height: '100%', display: 'flex', flexDirection: 'column', minHeight: 0 }}>
                <TextField
                  fullWidth
                  label="Page Title"
                  value={editItemName}
                  onChange={(e) => {
                    const next = e.target.value;
                    setEditItemName(next);
                    setHasUnsavedChanges(true);
                  }}
                  onBlur={handleAutoSave}
                  sx={{ mb: 2 }}
                />
                <Box sx={{ flex: 1, minHeight: 0 }}>
                  <Editor
                    value={editItemContent}
                    onEditorChange={(content, editor) => {
                      // Only mark dirty if TinyMCE says content changed by user
                      setEditItemContent(content);
                      const isDirty = editor?.isDirty?.() || false;
                      setHasUnsavedChanges(isDirty);
                    }}
                    onInit={(_, editor) => {
                      editorRef.current = editor;
                      setEditorLoaded(true);
                    }}
                    onBlur={handleAutoSave}
                    tinymceScriptSrc="/tinymce/tinymce.min.js"
                    init={{
                      height: '100%',
                      menubar: false,
                      skin: 'oxide-dark',
                      content_css: 'dark',
                      plugins: [
                        'advlist', 'autolink', 'lists', 'link', 'charmap',
                        'anchor', 'searchreplace', 'visualblocks', 'code',
                        'insertdatetime', 'table', 'help', 'wordcount'
                      ],
                      toolbar: 'undo redo | blocks | ' +
                        'bold italic forecolor | alignleft aligncenter ' +
                        'alignright alignjustify | bullist numlist outdent indent | ' +
                        'table | link | code | removeformat | help',
                      content_style: editorContentStyle,
                      branding: false,
                      promotion: false,
                      // Self-hosted configuration
                      license_key: 'gpl',
                      base_url: '/tinymce',
                      suffix: '.min',
                      table_default_attributes: {
                        border: '1'
                      },
                      table_default_styles: {
                        'border-collapse': 'collapse',
                        'width': '100%'
                      },
                      table_toolbar: 'tableprops tabledelete | tableinsertrowbefore tableinsertrowafter tabledeleterow | tableinsertcolbefore tableinsertcolafter tabledeletecol',
                    }}
                  />
                </Box>
                <Box sx={{ mt: 2, display: 'flex', gap: 1, alignItems: 'center' }}>
                  <Button
                    variant="contained"
                    startIcon={<SaveIcon />}
                    onClick={async () => {
                      await handleEditItem();
                      if (editorRef.current) editorRef.current.setDirty(false);
                    }}
                    disabled={loading || !hasUnsavedChanges}
                  >
                    {hasUnsavedChanges ? 'Save Changes' : 'Saved'}
                  </Button>
                  {hasUnsavedChanges && (
                    <Typography variant="caption" color="warning.main">
                      • Unsaved changes
                    </Typography>
                  )}
                  <Button
                    variant="outlined"
                    startIcon={<CloseIcon />}
                    onClick={() => {
                      setSelectedItem(null);
                      setEditItemName('');
                      setEditItemContent('');
                      setHasUnsavedChanges(false);
                    }}
                  >
                    Cancel
                  </Button>
                </Box>
                <Snackbar
                  open={!!saveNotice}
                  autoHideDuration={2000}
                  onClose={() => setSaveNotice(null)}
                  message={saveNotice || ''}
                />
              </Box>
            )}
          </Paper>
        </Grid>
      </Grid>

      {/* Create Item Dialog */}
      <Dialog open={createDialogOpen} onClose={() => setCreateDialogOpen(false)} maxWidth="sm" fullWidth>
        <DialogTitle>Create New {newItemType === 'folder' ? 'Folder' : 'Page'}</DialogTitle>
        <DialogContent>
          <Box sx={{ display: 'flex', gap: 2, mb: 2 }}>
            <Button
              variant={newItemType === 'folder' ? 'contained' : 'outlined'}
              onClick={() => setNewItemType('folder')}
            >
              Folder
            </Button>
            <Button
              variant={newItemType === 'page' ? 'contained' : 'outlined'}
              onClick={() => setNewItemType('page')}
            >
              Page
            </Button>
          </Box>
          <TextField
            autoFocus
            margin="dense"
            label={newItemType === 'folder' ? 'Folder Name' : 'Page Title'}
            fullWidth
            variant="outlined"
            value={newItemName}
            onChange={(e) => setNewItemName(e.target.value)}
            sx={{ mb: 2 }}
          />
          {/* Parent path is derived from current selection */}
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setCreateDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleCreateItem} variant="contained" disabled={!newItemName || loading}>
            Create
          </Button>
        </DialogActions>
      </Dialog>


      {/* Delete Confirmation Dialog */}
      <Dialog open={deleteDialogOpen} onClose={() => setDeleteDialogOpen(false)}>
        <DialogTitle>Delete {itemToDelete?.type === 'folder' ? 'Folder' : 'Page'}</DialogTitle>
        <DialogContent>
          <Typography>
            Are you sure you want to delete "{itemToDelete?.name}"? This action cannot be undone.
          </Typography>
        </DialogContent>
        <DialogActions>
          <Button onClick={() => setDeleteDialogOpen(false)}>Cancel</Button>
          <Button onClick={handleDeleteItem} color="error" variant="contained" disabled={loading}>
            Delete
          </Button>
        </DialogActions>
      </Dialog>

      {/* Context Menu */}
      <Menu
        open={contextMenu !== null}
        onClose={handleContextMenuClose}
        anchorReference="anchorPosition"
        anchorPosition={
          contextMenu !== null
            ? { top: contextMenu.mouseY, left: contextMenu.mouseX }
            : undefined
        }
      >
        <MenuItem onClick={() => contextMenu?.item && handleEdit(contextMenu.item)}>
          <ListItemIcon>
            <EditIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Edit</ListItemText>
        </MenuItem>
        {contextMenu?.item?.type === 'folder' && (
          <MenuItem onClick={() => contextMenu?.item && handleCreateInFolder(contextMenu.item.path)}>
            <ListItemIcon>
              <AddIcon fontSize="small" />
            </ListItemIcon>
            <ListItemText>Create in folder</ListItemText>
          </MenuItem>
        )}
        <MenuItem onClick={() => contextMenu?.item && handleDelete(contextMenu.item)}>
          <ListItemIcon>
            <DeleteIcon fontSize="small" />
          </ListItemIcon>
          <ListItemText>Delete</ListItemText>
        </MenuItem>
      </Menu>
    </Container>
  );
};

export default JournalApp;
