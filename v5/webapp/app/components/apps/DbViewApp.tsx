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
  Alert,
  Grid,
  List,
  ListItem,
  ListItemText,
  ListItemButton,
  IconButton,
  useTheme,
  useMediaQuery,
  FormControl,
  InputLabel,
  Select,
  MenuItem,
} from '@mui/material';
import {
  Add as AddIcon,
  Delete as DeleteIcon,
  Search as SearchIcon,
  Save as SaveIcon,
  Cancel as CancelIcon,
  Refresh as RefreshIcon,
} from '@mui/icons-material';
import Editor from '@monaco-editor/react';
import { getAuthToken } from '../../utils/auth';

interface DocumentItem {
  path: string;
  data: any;
  updatedAt: string;
}

export function DbViewApp() {
  const theme = useTheme();
  const isMobile = useMediaQuery(theme.breakpoints.down('md'));
  
  const [documents, setDocuments] = useState<DocumentItem[]>([]);
  const [selectedDocument, setSelectedDocument] = useState<DocumentItem | null>(null);
  const [jsonValue, setJsonValue] = useState<string>('');
  const [loading, setLoading] = useState(false);
  const [error, setError] = useState<string | null>(null);
  const [addDialogOpen, setAddDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [documentToDelete, setDocumentToDelete] = useState<DocumentItem | null>(null);
  const [newPath, setNewPath] = useState('');
  const [isSaving, setIsSaving] = useState(false);
  const [queryPath, setQueryPath] = useState('');
  const [searchMode, setSearchMode] = useState<'prefix' | 'search'>('prefix');

  // Load documents on component mount
  useEffect(() => {
    loadDocuments('');
  }, []);

  const loadDocuments = async (path: string) => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      
      const url = path ? `/api/documents?prefix=${encodeURIComponent(path)}` : '/api/documents';
      const response = await fetch(url, {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        const docs = data.documents || [];
        setDocuments(docs);
      } else {
        const errorText = await response.text();
        setError(`Failed to load documents: ${response.status} ${errorText}`);
      }
    } catch (error) {
      setError(`Failed to load documents: ${error instanceof Error ? error.message : String(error)}`);
    } finally {
      setLoading(false);
    }
  };

  const searchDocuments = async (searchTerm: string) => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      
      const url = `/api/documents/search?q=${encodeURIComponent(searchTerm)}`;
      const response = await fetch(url, {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        const docs = data.documents || [];
        setDocuments(docs);
      } else {
        const errorText = await response.text();
        setError(`Failed to search documents: ${response.status} ${errorText}`);
      }
    } catch (error) {
      setError(`Failed to search documents: ${error instanceof Error ? error.message : String(error)}`);
    } finally {
      setLoading(false);
    }
  };

  const handleSearch = () => {
    if (searchMode === 'search') {
      searchDocuments(queryPath);
    } else {
      loadDocuments(queryPath);
    }
  };

  const handleClearSearch = () => {
    setQueryPath('');
    setSearchMode('prefix');
    loadDocuments('');
  };

  const handleDocumentClick = (doc: DocumentItem) => {
    setSelectedDocument(doc);
    setJsonValue(JSON.stringify(doc.data, null, 2));
  };

  const handleSaveDocument = async () => {
    if (!selectedDocument) return;

    try {
      setIsSaving(true);
      setError(null);

      let parsedData;
      try {
        parsedData = JSON.parse(jsonValue);
      } catch (e) {
        setError('Invalid JSON format');
        return;
      }

      const token = getAuthToken();
      const response = await fetch(`/api/documents/${encodeURIComponent(selectedDocument.path)}`, {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(parsedData)
      });

      if (response.ok) {
        // Update the document in the list
        setDocuments(prev => prev.map(doc => 
          doc.path === selectedDocument.path 
            ? { ...doc, data: parsedData, updatedAt: new Date().toISOString() }
            : doc
        ));
        setSelectedDocument({ ...selectedDocument, data: parsedData, updatedAt: new Date().toISOString() });
      } else {
        setError('Failed to save document');
      }
    } catch (error) {
      console.error('Failed to save document:', error);
      setError('Failed to save document');
    } finally {
      setIsSaving(false);
    }
  };

  const handleAddDocument = async () => {
    try {
      setError(null);
      
      if (!newPath.trim()) {
        setError('Path is required');
        return;
      }

      const initialData = {};

      const token = getAuthToken();
      const response = await fetch(`/api/documents/${encodeURIComponent(newPath)}`, {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(initialData)
      });

      if (response.ok) {
        // Refresh the list with current mode
        if (searchMode === 'search' && queryPath) {
          await searchDocuments(queryPath);
        } else {
          await loadDocuments(queryPath);
        }
        setAddDialogOpen(false);
        setNewPath('');
      } else {
        setError('Failed to create document');
      }
    } catch (error) {
      console.error('Failed to create document:', error);
      setError('Failed to create document');
    }
  };

  const handleDeleteDocument = async () => {
    if (!documentToDelete) return;

    try {
      setError(null);
      const token = getAuthToken();
      const response = await fetch(`/api/documents/${encodeURIComponent(documentToDelete.path)}`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        // Refresh the list with current mode
        if (searchMode === 'search' && queryPath) {
          await searchDocuments(queryPath);
        } else {
          await loadDocuments(queryPath);
        }
        if (selectedDocument?.path === documentToDelete.path) {
          setSelectedDocument(null);
          setJsonValue('');
        }
        setDeleteDialogOpen(false);
        setDocumentToDelete(null);
      } else {
        setError('Failed to delete document');
      }
    } catch (error) {
      console.error('Failed to delete document:', error);
      setError('Failed to delete document');
    }
  };

  const handleDeleteClick = (doc: DocumentItem) => {
    setDocumentToDelete(doc);
    setDeleteDialogOpen(true);
  };

  if (loading) {
    return (
      <Container maxWidth="lg" sx={{ py: 4 }}>
        <Typography>Loading documents...</Typography>
      </Container>
    );
  }

  return (
    <Container maxWidth="xl" sx={{ py: 4 }}>
      <Paper sx={{ p: 4, bgcolor: 'background.paper' }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
          <Box>
            <Typography variant="h4" component="h1" gutterBottom>
              Database Viewer
            </Typography>
            <Typography variant="subtitle1" color="text.secondary">
              Query and edit documents by path
            </Typography>
          </Box>
          <Box sx={{ display: 'flex', gap: 1 }}>
            <Button
              variant="contained"
              startIcon={<AddIcon />}
              onClick={() => setAddDialogOpen(true)}
            >
              Add Document
            </Button>
            <Button
              variant="outlined"
              startIcon={<RefreshIcon />}
              onClick={() => {
                if (searchMode === 'search' && queryPath) {
                  searchDocuments(queryPath);
                } else {
                  loadDocuments(queryPath);
                }
              }}
            >
              Refresh
            </Button>
          </Box>
        </Box>

        {error && (
          <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
            {error}
          </Alert>
        )}

        {/* Search Bar */}
        <Paper sx={{ p: 2, mb: 3, bgcolor: 'background.default' }}>
          <Box sx={{ display: 'flex', gap: 2, alignItems: 'center', flexWrap: 'wrap' }}>
            <FormControl size="small" sx={{ minWidth: 120 }}>
              <InputLabel>Mode</InputLabel>
              <Select
                value={searchMode}
                onChange={(e) => setSearchMode(e.target.value as 'prefix' | 'search')}
                label="Mode"
              >
                <MenuItem value="prefix">Prefix</MenuItem>
                <MenuItem value="search">Search</MenuItem>
              </Select>
            </FormControl>
            <TextField
              label={searchMode === 'search' ? 'Search Term' : 'Query Path'}
              value={queryPath}
              onChange={(e) => setQueryPath(e.target.value)}
                             placeholder={
                 searchMode === 'search' 
                   ? 'Enter search term to find documents by path or content (e.g., calendar, user, title, etc.)'
                   : 'Enter path to query (e.g., /snake, /calendar, or leave empty for root)'
               }
              fullWidth
              size="small"
              onKeyPress={(e) => e.key === 'Enter' && handleSearch()}
            />
            <Button
              variant="contained"
              startIcon={<SearchIcon />}
              onClick={handleSearch}
            >
              {searchMode === 'search' ? 'Search' : 'Query'}
            </Button>
            <Button
              variant="outlined"
              onClick={handleClearSearch}
            >
              Clear
            </Button>
          </Box>
                     <Typography variant="caption" color="text.secondary" sx={{ mt: 1, display: 'block' }}>
             {searchMode === 'search' 
               ? 'Search mode: Finds documents containing the search term in the path or document content'
               : 'Prefix mode: Lists documents under the specified path prefix'
             }
           </Typography>
        </Paper>

        <Grid container spacing={2} sx={{ height: '70vh' }}>
          {/* Document List */}
          <Grid item xs={12} md={4}>
            <Paper sx={{ height: '100%', overflow: 'auto' }}>
              <Box sx={{ p: 2, borderBottom: '1px solid', borderColor: 'divider' }}>
                <Typography variant="h6">
                  Documents ({documents.length})
                </Typography>
              </Box>
              <List dense>
                {documents.map((doc) => (
                  <ListItem
                    key={doc.path}
                    disablePadding
                    secondaryAction={
                      <IconButton
                        edge="end"
                        onClick={() => handleDeleteClick(doc)}
                        color="error"
                        size="small"
                      >
                        <DeleteIcon />
                      </IconButton>
                    }
                  >
                    <ListItemButton
                      onClick={() => handleDocumentClick(doc)}
                      selected={selectedDocument?.path === doc.path}
                      sx={{
                        '&.Mui-selected': {
                          bgcolor: 'primary.main',
                          color: 'primary.contrastText',
                          '&:hover': {
                            bgcolor: 'primary.dark',
                          },
                        },
                      }}
                    >
                      <ListItemText
                        primary={
                          <Typography variant="body2" noWrap>
                            {doc.path.split('/').pop() || doc.path}
                          </Typography>
                        }
                        secondary={
                          <Box>
                            <Typography variant="caption" color="text.secondary" display="block">
                              {doc.path}
                            </Typography>
                            <Typography variant="caption" color="text.secondary">
                              {new Date(doc.updatedAt).toLocaleString()}
                            </Typography>
                          </Box>
                        }
                      />
                    </ListItemButton>
                  </ListItem>
                ))}
                {documents.length === 0 && (
                  <ListItem>
                    <ListItemText
                      primary={
                        <Typography variant="body2" color="text.secondary" align="center">
                          No documents found
                        </Typography>
                      }
                    />
                  </ListItem>
                )}
              </List>
            </Paper>
          </Grid>

          {/* JSON Editor */}
          <Grid item xs={12} md={8}>
            <Paper sx={{ height: '100%', display: 'flex', flexDirection: 'column' }}>
              <Box sx={{ 
                p: 2, 
                borderBottom: '1px solid', 
                borderColor: 'divider',
                display: 'flex',
                justifyContent: 'space-between',
                alignItems: 'center'
              }}>
                <Typography variant="h6">
                  {selectedDocument ? `Editing: ${selectedDocument.path}` : 'JSON Editor'}
                </Typography>
                {selectedDocument && (
                  <Box sx={{ display: 'flex', gap: 1 }}>
                    <Button
                      size="small"
                      variant="outlined"
                      startIcon={<CancelIcon />}
                      onClick={() => {
                        setSelectedDocument(null);
                        setJsonValue('');
                      }}
                    >
                      Cancel
                    </Button>
                    <Button
                      size="small"
                      variant="contained"
                      startIcon={<SaveIcon />}
                      onClick={handleSaveDocument}
                      disabled={isSaving}
                    >
                      {isSaving ? 'Saving...' : 'Save'}
                    </Button>
                  </Box>
                )}
              </Box>
              <Box sx={{ flex: 1, minHeight: 0 }}>
                {selectedDocument ? (
                  <Editor
                    height="100%"
                    defaultLanguage="json"
                    value={jsonValue}
                    onChange={(value) => setJsonValue(value || '')}
                    options={{
                      minimap: { enabled: false },
                      fontSize: 14,
                      wordWrap: 'on',
                      automaticLayout: true,
                    }}
                    theme="vs-dark"
                  />
                ) : (
                  <Box sx={{ 
                    display: 'flex', 
                    alignItems: 'center', 
                    justifyContent: 'center', 
                    height: '100%',
                    color: 'text.secondary'
                  }}>
                    <Typography variant="body1">
                      Select a document from the list to edit
                    </Typography>
                  </Box>
                )}
              </Box>
            </Paper>
          </Grid>
        </Grid>

        {/* Add Document Dialog */}
        <Dialog open={addDialogOpen} onClose={() => setAddDialogOpen(false)} maxWidth="sm" fullWidth>
          <DialogTitle>Add New Document</DialogTitle>
          <DialogContent>
            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2, pt: 1 }}>
              <TextField
                label="Document Path"
                value={newPath}
                onChange={(e) => setNewPath(e.target.value)}
                fullWidth
                required
                placeholder="e.g., /my-document or /folder/document"
                helperText="Enter the full document path"
              />
            </Box>
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setAddDialogOpen(false)}>Cancel</Button>
            <Button onClick={handleAddDocument} variant="contained">
              Add Document
            </Button>
          </DialogActions>
        </Dialog>

        {/* Delete Confirmation Dialog */}
        <Dialog open={deleteDialogOpen} onClose={() => setDeleteDialogOpen(false)}>
          <DialogTitle>Confirm Deletion</DialogTitle>
          <DialogContent>
            <Typography>
              Are you sure you want to delete this document?
            </Typography>
            {documentToDelete && (
              <Box sx={{ 
                mt: 2, 
                p: 2, 
                bgcolor: 'background.paper', 
                borderRadius: 1, 
                border: '1px solid rgba(255,255,255,0.12)'
              }}>
                <Typography variant="body2">
                  <strong>Path:</strong> {documentToDelete.path}
                </Typography>
                <Typography variant="body2">
                  <strong>Last Updated:</strong> {new Date(documentToDelete.updatedAt).toLocaleString()}
                </Typography>
              </Box>
            )}
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setDeleteDialogOpen(false)}>Cancel</Button>
            <Button onClick={handleDeleteDocument} color="error" variant="contained">
              Delete
            </Button>
          </DialogActions>
        </Dialog>
      </Paper>
    </Container>
  );
}
