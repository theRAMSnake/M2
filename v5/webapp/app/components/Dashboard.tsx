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
  CleaningServices as ChoresIcon,
} from '@mui/icons-material';
import { MakeUpApp, DashboardApp, CalendarApp, ChoresApp, AdminApp, ShopListApp, StickiesApp, DbViewApp } from './apps';
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

interface App {
  id: string;
  name: string;
  component: React.ComponentType<any>;
}

// App configuration - hardcoded user access
const APP_CONFIG = {
  snake: [
    { id: 'calendar', name: 'Calendar', icon: CalendarIcon },
    { id: 'chores', name: 'Chores', icon: ChoresIcon },
    { id: 'admin', name: 'Admin', icon: AdminIcon },
    { id: 'shoplist', name: 'Shop List', icon: ShoppingCartIcon },
    { id: 'stickies', name: 'Stickies', icon: StickyIcon },
    { id: 'dbview', name: 'DB Viewer', icon: DbViewIcon },
  ],
  seva: [
    { id: 'makeup', name: 'Make Up', icon: MakeUpIcon },
    { id: 'calendar', name: 'Calendar', icon: CalendarIcon },
    { id: 'chores', name: 'Chores', icon: ChoresIcon },
    { id: 'shoplist', name: 'Shop List', icon: ShoppingCartIcon },
    { id: 'stickies', name: 'Stickies', icon: StickyIcon },
  ],
};

export function Dashboard({ user, onLogout, currentUser }: DashboardProps) {
  const [loading, setLoading] = useState(true);
  const [drawerOpen, setDrawerOpen] = useState(false);
  const [todayEventsCount, setTodayEventsCount] = useState(0);
  const [urgentChoresCount, setUrgentChoresCount] = useState(0);

  // Get available apps for current user
  const availableApps = APP_CONFIG[currentUser as keyof typeof APP_CONFIG] || APP_CONFIG.snake;
  
  const [currentApp, setCurrentApp] = useState<string>('dashboard');

  useEffect(() => {
    loadTodayEventsCount();
    loadUrgentChoresCount();
    
    // Refresh counts every 5 minutes
    const interval = setInterval(() => {
      loadTodayEventsCount();
      loadUrgentChoresCount();
    }, 5 * 60 * 1000);
    
    return () => clearInterval(interval);
  }, []);

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
      console.error('Failed to load today events count:', error);
    } finally {
      setLoading(false);
    }
  };

  const loadUrgentChoresCount = async () => {
    try {
      const token = getAuthToken();
      const response = await fetch('/api/chores/urgent-count', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        setUrgentChoresCount(data.data?.count || 0);
      }
    } catch (error) {
      console.error('Failed to load urgent chores count:', error);
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
      case 'chores':
        return <ChoresApp />;
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
    
    if (app.id === 'chores') {
      return (
        <Badge badgeContent={urgentChoresCount > 0 ? urgentChoresCount : undefined} color="error" max={99}>
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
            v5.0.6
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
                    <Typography variant="caption" sx={{ textAlign: 'center', fontWeight: 500 }}>
                      {app.name}
                    </Typography>
                  </CardActionArea>
                </Card>
              </Grid>
            ))}
          </Grid>
        </Box>
      </Drawer>

      {/* Main Content */}
      <Box sx={{ flexGrow: 1, display: 'flex', flexDirection: 'column' }}>
        {/* Top AppBar */}
        <AppBar position="static" sx={{ bgcolor: 'background.paper', color: 'text.primary' }}>
          <Toolbar>
            <IconButton
              edge="start"
              color="inherit"
              aria-label="menu"
              onClick={toggleDrawer}
              sx={{ mr: 2 }}
            >
              <MenuIcon />
            </IconButton>
            <Typography variant="h6" component="div" sx={{ flexGrow: 1 }}>
              {availableApps.find(app => app.id === currentApp)?.name || 'Dashboard'}
            </Typography>
            <Button color="inherit" onClick={onLogout} startIcon={<LogoutOutlined />}>
              Logout
            </Button>
          </Toolbar>
        </AppBar>

        {/* App Content */}
        <Box sx={{ flexGrow: 1, p: 3 }}>
          {loading ? (
            <Container sx={{ display: 'flex', justifyContent: 'center', alignItems: 'center', height: '50vh' }}>
              <CircularProgress />
            </Container>
          ) : (
            renderCurrentApp()
          )}
        </Box>
      </Box>
    </Box>
  );
}
