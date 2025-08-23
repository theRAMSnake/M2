'use client';

import { useState, useEffect } from 'react';
import {
  Box,
  AppBar,
  Toolbar,
  Typography,
  Button,
  Container,
  Paper,
  CircularProgress,
  Drawer,
  Grid,
  Card,
  CardContent,
  CardActionArea,
  IconButton,
} from '@mui/material';
import { 
  LogoutOutlined, 
  Menu as MenuIcon,
  Dashboard as DashboardIcon,
  Face as MakeUpIcon,
  Event as CalendarIcon,
  AdminPanelSettings as AdminIcon,
} from '@mui/icons-material';
import { MakeUpApp, DashboardApp, CalendarApp, AdminApp } from './apps';
import { getAuthToken } from '../utils/auth';

interface User {
  id: string;
  name: string;
}

interface DashboardProps {
  user: User;
  onLogout: () => void;
  currentUser: string;
}

interface Document {
  path: string;
  data: any;
  updatedAt: string;
}

interface App {
  id: string;
  name: string;
  component: React.ComponentType<any>;
}

// App configuration - hardcoded user access
const APP_CONFIG = {
  snake: [
    { id: 'calendar', name: 'Calendar', icon: CalendarIcon },
    { id: 'admin', name: 'Admin', icon: AdminIcon },
  ],
  seva: [
    { id: 'makeup', name: 'Make Up', icon: MakeUpIcon },
    { id: 'calendar', name: 'Calendar', icon: CalendarIcon },
  ],
};

export function Dashboard({ user, onLogout, currentUser }: DashboardProps) {
  const [documents, setDocuments] = useState<Document[]>([]);
  const [loading, setLoading] = useState(true);
  const [selectedPath, setSelectedPath] = useState<string | null>(null);
  const [documentData, setDocumentData] = useState<any>(null);
  const [drawerOpen, setDrawerOpen] = useState(false);

  // Get available apps for current user
  const availableApps = APP_CONFIG[currentUser as keyof typeof APP_CONFIG] || APP_CONFIG.snake;
  
  const [currentApp, setCurrentApp] = useState<string>('dashboard');

  useEffect(() => {
    loadDocuments();
  }, []);

  const loadDocuments = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/documents', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        setDocuments(data.documents || []);
      }
    } catch (error) {
      console.error('Failed to load documents:', error);
    } finally {
      setLoading(false);
    }
  };

  const loadDocument = async (path: string) => {
    try {
      const token = getAuthToken();
      const response = await fetch(`/api/documents/${encodeURIComponent(path)}`, {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        setDocumentData(data.data);
        setSelectedPath(path);
      }
    } catch (error) {
      console.error('Failed to load document:', error);
    }
  };

  const saveDocument = async (path: string, data: any) => {
    try {
      const token = getAuthToken();
      const response = await fetch(`/api/documents/${encodeURIComponent(path)}`, {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(data)
      });

      if (response.ok) {
        await loadDocuments(); // Refresh the list
      }
    } catch (error) {
      console.error('Failed to save document:', error);
    }
  };

  const handleAppSelect = (appId: string) => {
    setCurrentApp(appId);
    setDrawerOpen(false);
  };

  const toggleDrawer = () => {
    setDrawerOpen(!drawerOpen);
  };

  const renderCurrentApp = () => {
    switch (currentApp) {
      case 'makeup':
        return <MakeUpApp />;
      case 'calendar':
        return <CalendarApp />;
      case 'admin':
        return <AdminApp />;
      case 'dashboard':
      default:
        return <DashboardApp user={user} />;
    }
  };

  return (
    <Box sx={{ display: 'flex', minHeight: '100vh', bgcolor: 'background.default' }}>
                    {/* Left Drawer */}
        <Drawer
          variant="temporary"
          open={drawerOpen}
          onClose={toggleDrawer}
          sx={{
            '& .MuiDrawer-paper': {
              width: 420,
              boxSizing: 'border-box',
              bgcolor: 'background.paper',
              borderRight: '1px solid',
              borderColor: 'divider',
            },
          }}
        >
        <Box sx={{ p: 2, borderBottom: '1px solid', borderColor: 'divider' }}>
          <Typography variant="h6" component="div" sx={{ color: 'primary.main', fontWeight: 'bold' }}>
            Materia
          </Typography>
          <Typography variant="caption" sx={{ color: 'text.secondary' }}>
            v5.0.2
          </Typography>
        </Box>
        
                 <Box sx={{ p: 2 }}>
           <Grid container spacing={2}>
             {availableApps.map((app) => (
                               <Grid item xs={4} key={app.id}>
                 <Card 
                   sx={{ 
                     aspectRatio: '1',
                     bgcolor: currentApp === app.id ? 'primary.main' : 'background.paper',
                     color: currentApp === app.id ? 'primary.contrastText' : 'text.primary',
                     '&:hover': {
                       bgcolor: currentApp === app.id ? 'primary.dark' : 'action.hover',
                     },
                   }}
                 >
                   <CardActionArea
                     onClick={() => handleAppSelect(app.id)}
                     sx={{ 
                       height: '100%',
                       display: 'flex',
                       flexDirection: 'column',
                       alignItems: 'center',
                       justifyContent: 'center',
                       p: 2,
                     }}
                   >
                     <app.icon sx={{ 
                       fontSize: 48, 
                       mb: 1.5,
                       color: 'inherit',
                     }} />
                     <Typography 
                       variant="body1" 
                       component="div"
                       sx={{ 
                         fontWeight: 'medium',
                         textAlign: 'center',
                         fontSize: '0.9rem',
                       }}
                     >
                       {app.name}
                     </Typography>
                   </CardActionArea>
                 </Card>
               </Grid>
             ))}
           </Grid>
         </Box>
      </Drawer>

                    {/* Main Content Area */}
        <Box sx={{ 
          flexGrow: 1, 
          display: 'flex', 
          flexDirection: 'column',
        }}>
        {/* Header */}
        <AppBar position="static" elevation={0} sx={{ bgcolor: 'background.paper' }}>
          <Toolbar>
            <IconButton
              edge="start"
              color="inherit"
              onClick={toggleDrawer}
              sx={{ mr: 2, color: 'text.primary' }}
            >
              <MenuIcon />
            </IconButton>
            
            <Box sx={{ flexGrow: 1 }}>
              <Typography variant="h6" sx={{ color: 'text.primary' }}>
                {availableApps.find(app => app.id === currentApp)?.name}
              </Typography>
              <Typography variant="caption" sx={{ color: 'text.secondary' }}>
                v5.0.2
              </Typography>
            </Box>
            
            <Button
              color="inherit"
              onClick={onLogout}
              startIcon={<LogoutOutlined />}
              sx={{ textTransform: 'none', color: 'text.primary' }}
            >
              Logout
            </Button>
          </Toolbar>
        </AppBar>

        {/* App Content */}
        <Box sx={{ flexGrow: 1, p: 3 }}>
          {renderCurrentApp()}
        </Box>
      </Box>
    </Box>
  );
}
