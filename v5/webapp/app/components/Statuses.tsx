'use client';

import { useState, useEffect } from 'react';
import {
  Box,
  IconButton,
  Badge,
  Tooltip,
  useMediaQuery,
  useTheme,
} from '@mui/material';
import {
  Assignment as ChoresIcon,
  CalendarToday as CalendarIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../utils/auth';

interface StatusesProps {
  onAppClick: (appId: string) => void;
}

export function Statuses({ onAppClick }: StatusesProps) {
  const theme = useTheme();
  const isMobile = useMediaQuery(theme.breakpoints.down('sm'));
  const [choresCount, setChoresCount] = useState(0);
  const [calendarCount, setCalendarCount] = useState(0);

  useEffect(() => {
    // Load initial counts
    loadCounts();

    // Set up periodic refresh
    const interval = setInterval(loadCounts, 30000); // Refresh every 30 seconds

    return () => clearInterval(interval);
  }, []);

  const loadCounts = async () => {
    try {
      const token = getAuthToken();
      
      // Load chores count (personal + family) - get all chores and filter undone ones
      const choresResponse = await fetch('/api/chores', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (choresResponse.ok) {
        const choresData = await choresResponse.json();
        const totalChores = choresData.data?.chores?.filter((chore: any) => !chore.is_done).length || 0;
        setChoresCount(totalChores);
      }

      // Load calendar items for today - get all items and filter by today's date
      const calendarResponse = await fetch('/api/calendar/items', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (calendarResponse.ok) {
        const calendarData = await calendarResponse.json();
        const today = new Date().toISOString().split('T')[0]; // YYYY-MM-DD format
        const todayItems = calendarData.data?.items?.filter((item: any) => {
          const itemDate = new Date(item.dateTime).toISOString().split('T')[0];
          return itemDate === today;
        }).length || 0;
        setCalendarCount(todayItems);
      }
    } catch (error) {
      console.error('Failed to load status counts:', error);
    }
  };

  const handleChoresClick = () => {
    onAppClick('chores');
  };

  const handleCalendarClick = () => {
    onAppClick('calendar');
  };

  // Don't render on mobile
  if (isMobile) {
    return null;
  }

  return (
    <Box display="flex" alignItems="center" gap={1}>
      <Tooltip title={`Chores: ${choresCount} not done`}>
        <IconButton 
          color="inherit" 
          onClick={handleChoresClick}
          size="small"
        >
          <Badge 
            badgeContent={choresCount} 
            color="error"
            max={99}
          >
            <ChoresIcon />
          </Badge>
        </IconButton>
      </Tooltip>
      
      <Tooltip title={`Calendar: ${calendarCount} items today`}>
        <IconButton 
          color="inherit" 
          onClick={handleCalendarClick}
          size="small"
        >
          <Badge 
            badgeContent={calendarCount} 
            color="primary"
            max={99}
          >
            <CalendarIcon />
          </Badge>
        </IconButton>
      </Tooltip>
    </Box>
  );
}
