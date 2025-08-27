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
  Badge,
} from '@mui/material';
import { 
  LogoutOutlined, 
  Menu as MenuIcon,
  Dashboard as DashboardIcon,
  Face as MakeUpIcon,
  Event as CalendarIcon,
  AdminPanelSettings as AdminIcon,
  ShoppingCart as ShoppingCartIcon,
  StickyNote2 as StickyIcon,
  Storage as DbViewIcon,
} from '@mui/icons-material';
import { MakeUpApp, DashboardApp, CalendarApp, AdminApp, ShopListApp, StickiesApp, DbViewApp } from './apps';
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
    { id: 'shoplist', name: 'Shop List', icon: ShoppingCartIcon },
    { id: 'stickies', name: 'Stickies', icon: StickyIcon },
    { id: 'dbview', name: 'DB Viewer', icon: DbViewIcon },
  ],
  seva: [
    { id: 'makeup', name: 'Make Up', icon: MakeUpIcon },
    { id: 'calendar', name: 'Calendar', icon: CalendarIcon },
    { id: 'shoplist', name: 'Shop List', icon: ShoppingCartIcon },
    { id: 'stickies', name: 'Stickies', icon: StickyIcon },
  ],
};

export function Dashboard({ user, onLogout, currentUser }: DashboardProps) {
  const [documents, setDocuments] = useState<Document[]>([]);
  const [loading, setLoading] = useState(true);
  const [selectedPath, setSelectedPath] = useState<string | null>(null);
  const [documentData, setDocumentData] = useState<any>(null);
  const [drawerOpen, setDrawerOpen] = useState(false);
  const [todayEventsCount, setTodayEventsCount] = useState(0);

  // Get available apps for current user
  const availableApps = APP_CONFIG[currentUser as keyof typeof APP_CONFIG] || APP_CONFIG.snake;
  
  const [currentApp, setCurrentApp] = useState<string>('dashboard');

  useEffect(() => {
    loadDocuments();
    loadTodayEventsCount();
    
    // Refresh events count every 5 minutes
    const interval = setInterval(() => {
      loadTodayEventsCount();
    }, 5 * 60 * 1000);
    
    return () => clearInterval(interval);
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

  const loadTodayEventsCount = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/calendar/items', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        const calendarItems = data.data?.items || [];
        
        // Filter events for today
        const today = new Date();
        const startOfDay = new Date(today.getFullYear(), today.getMonth(), today.getDate());
        const endOfDay = new Date(today.getFullYear(), today.getMonth(), today.getDate(), 23, 59, 59, 999);
        
        const todayEvents = calendarItems.filter((item: any) => {
          const eventDate = new Date(item.dateTime);
          return eventDate >= startOfDay && eventDate <= endOfDay;
        });
        
        setTodayEventsCount(todayEvents.length);
      }
    } catch (error) {
      console.error('Failed to load today\'s events count:', error);
    }
  };

  const handleAppSelect = (appId: string) => {
    setCurrentApp(appId);
    setDrawerOpen(false);
    
    // Refresh events count when calendar app is selected or when returning to dashboard
    if (appId === 'calendar' || appId === 'dashboard') {
      loadTodayEventsCount();
    }
  };

  const toggleDrawer = () => {
    const newDrawerOpen = !drawerOpen;
    setDrawerOpen(newDrawerOpen);
    
    // Refresh events count when drawer is opened
    if (newDrawerOpen) {
      loadTodayEventsCount();
    }
  };

  const renderCurrentApp = () => {
    switch (currentApp) {
      case 'makeup':
        return <MakeUpApp />;
      case 'calendar':
        return <CalendarApp />;
      case 'admin':
        return <AdminApp />;
      case 'shoplist':
        return <ShopListApp />;
      case 'stickies':
        return <StickiesApp />;
      case 'dbview':
        return <DbViewApp />;
      case 'dashboard':
      default:
        return <DashboardApp user={user} />;
    }
  };

  const renderAppIcon = (app: any) => {
    if (app.id === 'calendar') {
      return (
        <Badge badgeContent={todayEventsCount > 0 ? todayEventsCount : undefined} color="error" max={99}>
          <app.icon sx={{ 
            fontSize: 48, 
            mb: 1.5,
            color: 'inherit',
          }} />
        </Badge>
      );
    }
    
    return (
      <app.icon sx={{ 
        fontSize: 48, 
        mb: 1.5,
        color: 'inherit',
      }} />
    );
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
            v5.0.5
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
                     {renderAppIcon(app)}
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
                v5.0.5
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
