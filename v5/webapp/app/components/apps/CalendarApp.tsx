'use client';

import { useState, useEffect } from 'react';
import { format, startOfMonth, endOfMonth, startOfWeek, endOfWeek, eachDayOfInterval, isSameMonth, isSameDay, addMonths, subMonths, addWeeks, subWeeks, startOfDay, endOfDay, eachHourOfInterval, setHours, getHours, getMinutes } from 'date-fns';
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
  Grid,
  List,
  ListItem,
  ListItemText,
  ListItemButton,
  Chip,
  Divider,
  useMediaQuery,
  useTheme,
} from '@mui/material';
import {
  Add as AddIcon,
  ChevronLeft as ChevronLeftIcon,
  ChevronRight as ChevronRightIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';
import { COLOR_PRESETS, getColorValue, getDefaultColor } from '../../utils/colorPresets';

interface CalendarItem {
  id: string;
  title: string;
  start: Date;
  end: Date;
  isPrivate: boolean;
  color: string;
  createdBy?: string;
}

type ViewMode = 'month' | 'week' | 'compact';

export function CalendarApp() {
  const theme = useTheme();
  const isMobile = useMediaQuery(theme.breakpoints.down('md'));
  
  const [items, setItems] = useState<CalendarItem[]>([]);
  const [loading, setLoading] = useState(true);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [itemToDelete, setItemToDelete] = useState<CalendarItem | null>(null);
  const [editingItem, setEditingItem] = useState<CalendarItem | null>(null);
  const [error, setError] = useState<string | null>(null);
  const [currentDate, setCurrentDate] = useState(new Date());
  const [viewMode, setViewMode] = useState<ViewMode>(isMobile ? 'compact' : 'month');
  const [formData, setFormData] = useState({
    title: '',
    start: new Date(),
    end: new Date(new Date().getTime() + 60 * 60 * 1000),
    isPrivate: false,
    color: getDefaultColor(),
  });

  useEffect(() => {
    loadItems();
  }, []);

  const loadItems = async () => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      const response = await fetch('/api/calendar/items', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        const calendarItems = (data.data?.items || []).map((item: any) => ({
          ...item,
          start: new Date(item.dateTime),
          end: new Date(item.endTime),
          title: item.text,
        }));
        setItems(calendarItems);
      } else {
        setError('Failed to load calendar items');
      }
    } catch (error) {
      console.error('Failed to load calendar items:', error);
      setError('Failed to load calendar items');
    } finally {
      setLoading(false);
    }
  };

  const handleAddItem = () => {
    setEditingItem(null);
    // Set default start time to 09:00 today
    const defaultStart = new Date();
    defaultStart.setHours(9, 0, 0, 0);
    const defaultEnd = new Date(defaultStart.getTime() + 60 * 60 * 1000);
    
    setFormData({
      title: '',
      start: defaultStart,
      end: defaultEnd,
      isPrivate: false,
      color: getDefaultColor(),
    });
    setDialogOpen(true);
  };

  const handleEditItem = (item: CalendarItem) => {
    setEditingItem(item);
    
    // Ensure end time is valid when editing
    const validEnd = validateAndAdjustEndTime(item.start, item.end);
    
    setFormData({
      title: item.title,
      start: item.start,
      end: validEnd,
      isPrivate: item.isPrivate,
      color: item.color,
    });
    setDialogOpen(true);
  };

  const handleDeleteClick = (item: CalendarItem) => {
    setItemToDelete(item);
    setDeleteDialogOpen(true);
  };

  const handleDeleteConfirm = async () => {
    if (!itemToDelete) return;

    try {
      const token = getAuthToken();
      const response = await fetch(`/api/calendar/items/${itemToDelete.id}`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        await loadItems();
        setDeleteDialogOpen(false);
        setItemToDelete(null);
      } else {
        setError('Failed to delete calendar item');
      }
    } catch (error) {
      console.error('Failed to delete calendar item:', error);
      setError('Failed to delete calendar item');
    }
  };

  const validateAndAdjustEndTime = (newStart: Date, currentEnd: Date) => {
    // If end time is before or equal to start time, adjust it to 1 hour after start
    if (currentEnd <= newStart) {
      return new Date(newStart.getTime() + 60 * 60 * 1000); // 1 hour later
    }
    return currentEnd;
  };

  const getDurationText = (start: Date, end: Date) => {
    const durationMs = end.getTime() - start.getTime();
    const durationMinutes = Math.round(durationMs / (1000 * 60));
    
    if (durationMinutes < 60) {
      return `${durationMinutes} minute${durationMinutes !== 1 ? 's' : ''}`;
    } else if (durationMinutes < 1440) { // Less than 24 hours
      const hours = Math.floor(durationMinutes / 60);
      const minutes = durationMinutes % 60;
      if (minutes === 0) {
        return `${hours} hour${hours !== 1 ? 's' : ''}`;
      } else {
        return `${hours} hour${hours !== 1 ? 's' : ''} ${minutes} minute${minutes !== 1 ? 's' : ''}`;
      }
    } else {
      const days = Math.floor(durationMinutes / 1440);
      const hours = Math.floor((durationMinutes % 1440) / 60);
      if (hours === 0) {
        return `${days} day${days !== 1 ? 's' : ''}`;
      } else {
        return `${days} day${days !== 1 ? 's' : ''} ${hours} hour${hours !== 1 ? 's' : ''}`;
      }
    }
  };

  const handleSaveItem = async () => {
    try {
      setError(null);
      
      // Validate that end time is not smaller than start time
      if (formData.end <= formData.start) {
        setError('End time must be after start time');
        return;
      }
      
      // Validate minimum duration (at least 5 minutes)
      const durationMs = formData.end.getTime() - formData.start.getTime();
      const durationMinutes = Math.round(durationMs / (1000 * 60));
      if (durationMinutes < 5) {
        setError('Event must be at least 5 minutes long');
        return;
      }
      
      const token = getAuthToken();
      
      const itemData = {
        id: editingItem?.id || Date.now().toString(),
        text: formData.title,
        dateTime: formData.start.toISOString(),
        endTime: formData.end.toISOString(),
        isPrivate: formData.isPrivate,
        color: formData.color,
      };

      const url = editingItem 
        ? `/api/calendar/items/${editingItem.id}`
        : '/api/calendar/items';
      
      const method = editingItem ? 'PUT' : 'POST';

      const response = await fetch(url, {
        method,
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(itemData)
      });

      if (response.ok) {
        await loadItems();
        setDialogOpen(false);
        setEditingItem(null);
      } else {
        setError(`Failed to ${editingItem ? 'update' : 'add'} calendar item`);
      }
    } catch (error) {
      console.error(`Failed to ${editingItem ? 'update' : 'add'} calendar item:`, error);
      setError(`Failed to ${editingItem ? 'update' : 'add'} calendar item`);
    }
  };

  const getColorByValue = (colorValue: string) => {
    return getColorValue(colorValue);
  };

  const getColorLabel = (colorValue: string) => {
    return COLOR_PRESETS.find(p => p.value === colorValue)?.label || colorValue;
  };

  const getUpcomingEvents = () => {
    const now = new Date();
    return items
      .filter(item => new Date(item.start) >= now)
      .sort((a, b) => new Date(a.start).getTime() - new Date(b.start).getTime())
      .slice(0, 50); // Limit to 50 upcoming events
  };

  const navigatePrevious = () => {
    if (viewMode === 'month') {
      setCurrentDate(subMonths(currentDate, 1));
    } else {
      setCurrentDate(subWeeks(currentDate, 1));
    }
  };

  const navigateNext = () => {
    if (viewMode === 'month') {
      setCurrentDate(addMonths(currentDate, 1));
    } else {
      setCurrentDate(addWeeks(currentDate, 1));
    }
  };

  const getCalendarDays = () => {
    if (viewMode === 'month') {
      const start = startOfWeek(startOfMonth(currentDate));
      const end = endOfWeek(endOfMonth(currentDate));
      return eachDayOfInterval({ start, end });
    } else {
      const start = startOfWeek(currentDate);
      const end = endOfWeek(currentDate);
      return eachDayOfInterval({ start, end });
    }
  };

  const getWeekHours = () => {
    const start = startOfWeek(currentDate);
    const end = endOfWeek(currentDate);
    const weekStart = setHours(start, 0);
    const weekEnd = setHours(end, 23);
    return eachHourOfInterval({ start: weekStart, end: weekEnd });
  };

  const getEventsForDay = (date: Date) => {
    return items.filter(item => {
      const itemStart = startOfDay(item.start);
      const dayStart = startOfDay(date);
      
      // Only show events that start on this specific day
      return itemStart.getTime() === dayStart.getTime();
    });
  };

  const getEventsForHour = (date: Date) => {
    return items.filter(item => {
      const itemStart = new Date(item.start);
      const itemEnd = new Date(item.end);
      const hourStart = new Date(date);
      const hourEnd = new Date(date);
      hourEnd.setHours(hourEnd.getHours() + 1);
      
      // Show events that overlap with this hour
      return itemStart < hourEnd && itemEnd > hourStart;
    });
  };

  const getEventGroupsForDay = (day: Date) => {
    const dayStart = startOfDay(day);
    const dayEnd = endOfDay(day);
    
    // Get all events for this day
    const dayEvents = items.filter(item => {
      const itemStart = new Date(item.start);
      const itemEnd = new Date(item.end);
      return itemStart < dayEnd && itemEnd > dayStart;
    });

    // Group events that actually overlap in time
    const groups: CalendarItem[][] = [];
    const processedEvents = new Set<string>();

    dayEvents.forEach(event => {
      if (processedEvents.has(event.id)) return;

      const eventStart = new Date(event.start);
      const eventEnd = new Date(event.end);

      const group = [event];
      processedEvents.add(event.id);

      // Find other events that overlap with this event
      dayEvents.forEach(otherEvent => {
        if (processedEvents.has(otherEvent.id)) return;
        
        const otherStart = new Date(otherEvent.start);
        const otherEnd = new Date(otherEvent.end);

        // Check if events overlap in time
        const overlap = eventStart < otherEnd && eventEnd > otherStart;
        
        if (overlap) {
          group.push(otherEvent);
          processedEvents.add(otherEvent.id);
        }
      });

      groups.push(group);
    });

    return groups.sort((a, b) => new Date(a[0].start).getTime() - new Date(b[0].start).getTime());
  };

  const getEventGroupsForHour = (day: Date, hour: number) => {
    const hourStart = new Date(day);
    hourStart.setHours(hour, 0, 0, 0);
    const hourEnd = new Date(day);
    hourEnd.setHours(hour + 1, 0, 0, 0);
    
    // Get events that overlap with this hour (not just start in it)
    const hourEvents = items.filter(item => {
      const itemStart = new Date(item.start);
      const itemEnd = new Date(item.end);
      return itemStart < hourEnd && itemEnd > hourStart;
    });

    // Group events that actually overlap in time
    const groups: CalendarItem[][] = [];
    const processedEvents = new Set<string>();

    hourEvents.forEach(event => {
      if (processedEvents.has(event.id)) return;

      const eventStart = new Date(event.start);
      const eventEnd = new Date(event.end);

      const group = [event];
      processedEvents.add(event.id);

      // Find other events that overlap with this event
      hourEvents.forEach(otherEvent => {
        if (processedEvents.has(otherEvent.id)) return;
        
        const otherStart = new Date(otherEvent.start);
        const otherEnd = new Date(otherEvent.end);

        // Check if events overlap in time
        const overlap = eventStart < otherEnd && eventEnd > otherStart;
        
        if (overlap) {
          group.push(otherEvent);
          processedEvents.add(otherEvent.id);
        }
      });

      groups.push(group);
    });

    return groups.sort((a, b) => new Date(a[0].start).getTime() - new Date(b[0].start).getTime());
  };

  const getMultiHourEvents = () => {
    // Get all events that span multiple hours
    return items.filter(event => {
      const start = new Date(event.start);
      const end = new Date(event.end);
      const startHour = start.getHours();
      const endHour = end.getHours();
      return startHour !== endHour || start.getMinutes() !== 0 || end.getMinutes() !== 0;
    });
  };

  const getEventPosition = (event: CalendarItem, date: Date) => {
    const eventStart = new Date(event.start);
    const eventEnd = new Date(event.end);
    const hourStart = new Date(date);
    const hourEnd = new Date(date);
    hourEnd.setHours(hourEnd.getHours() + 1);
    
    // Calculate position within the hour
    const startMinutes = Math.max(0, (eventStart.getTime() - hourStart.getTime()) / (1000 * 60));
    const endMinutes = Math.min(60, (eventEnd.getTime() - hourStart.getTime()) / (1000 * 60));
    const duration = endMinutes - startMinutes;
    
    return {
      top: `${(startMinutes / 60) * 100}%`,
      height: `${(duration / 60) * 100}%`,
      isStart: eventStart >= hourStart,
      isEnd: eventEnd <= hourEnd,
    };
  };

  const handleDayClick = (date: Date) => {
    if (viewMode === 'month' && !isSameMonth(date, currentDate)) {
      return; // Don't allow clicking on off-month days
    }
    
    // Set default start time to 09:00 only for month view
    let defaultStart: Date;
    let defaultEnd: Date;
    
    if (viewMode === 'month') {
      defaultStart = new Date(date);
      defaultStart.setHours(9, 0, 0, 0);
      defaultEnd = new Date(defaultStart.getTime() + 60 * 60 * 1000);
    } else {
      // For week view, use the exact time clicked
      defaultStart = new Date(date);
      defaultEnd = new Date(defaultStart.getTime() + 60 * 60 * 1000);
    }
    
    setFormData({
      title: '',
      start: defaultStart,
      end: defaultEnd,
      isPrivate: false,
      color: getDefaultColor(),
    });
    setEditingItem(null);
    setDialogOpen(true);
  };

  const getViewLabel = () => {
    if (viewMode === 'month') {
      return format(currentDate, 'MMMM yyyy');
    } else if (viewMode === 'week') {
      const start = startOfWeek(currentDate);
      const end = endOfWeek(currentDate);
      if (isSameMonth(start, end)) {
        return `${format(start, 'MMM d')} - ${format(end, 'd, yyyy')}`;
      } else {
        return `${format(start, 'MMM d, yyyy')} - ${format(end, 'MMM d, yyyy')}`;
      }
    } else { // compact
      return 'Upcoming Events';
    }
  };

  if (loading) {
    return (
      <Container maxWidth="lg" sx={{ py: 4 }}>
        <Typography>Loading...</Typography>
      </Container>
    );
  }

  const days = getCalendarDays();
  const weekDays = ['Sun', 'Mon', 'Tue', 'Wed', 'Thu', 'Fri', 'Sat'];

  return (
    <Container maxWidth="lg" sx={{ py: 4 }}>
      <Paper sx={{ p: 4, bgcolor: 'background.paper' }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
          <Box>
            <Typography variant="h4" component="h1" gutterBottom>
              Calendar
            </Typography>
            <Typography variant="subtitle1" color="text.secondary">
              Manage your calendar events and reminders
            </Typography>
          </Box>
          <Button
            variant="contained"
            startIcon={<AddIcon />}
            onClick={handleAddItem}
          >
            Add Event
          </Button>
        </Box>

        {error && (
          <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
            {error}
          </Alert>
        )}

        {/* Calendar Header */}
        <Box sx={{ 
          display: 'flex', 
          alignItems: 'center', 
          justifyContent: 'space-between',
          mb: 2,
          p: 2,
          bgcolor: 'background.paper',
          borderRadius: 1,
          border: '1px solid',
          borderColor: 'divider',
        }}>
          <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
            <IconButton
              onClick={navigatePrevious}
              sx={{ 
                color: 'primary.main',
                '&:hover': { bgcolor: 'action.hover' }
              }}
            >
              <ChevronLeftIcon />
            </IconButton>
            <IconButton
              onClick={navigateNext}
              sx={{ 
                color: 'primary.main',
                '&:hover': { bgcolor: 'action.hover' }
              }}
            >
              <ChevronRightIcon />
            </IconButton>
            <Typography variant="h6" sx={{ ml: 2, color: 'text.primary' }}>
              {getViewLabel()}
            </Typography>
          </Box>
          <Box sx={{ display: 'flex', gap: 1 }}>
            {isMobile ? (
              // On mobile, only show compact view button (disabled since it's the only option)
              <Button
                variant="contained"
                size="small"
                disabled
                sx={{
                  textTransform: 'none',
                  minWidth: 'auto',
                  px: 2,
                }}
              >
                Compact
              </Button>
            ) : (
              // On desktop, show all view options
              (['month', 'week', 'compact'] as ViewMode[]).map((view) => (
                <Button
                  key={view}
                  variant={viewMode === view ? 'contained' : 'outlined'}
                  size="small"
                  onClick={() => setViewMode(view)}
                  sx={{
                    textTransform: 'none',
                    minWidth: 'auto',
                    px: 2,
                  }}
                >
                  {view.charAt(0).toUpperCase() + view.slice(1)}
                </Button>
              ))
            )}
          </Box>
        </Box>

        {/* Calendar Grid */}
        <Box sx={{ 
          bgcolor: 'background.paper',
          borderRadius: 2,
          p: 2,
          boxShadow: '0 4px 12px rgba(0,0,0,0.3)',
        }}>
          {viewMode === 'month' ? (
            <>
              {/* Week Headers */}
              <Grid container sx={{ borderBottom: '1px solid #424242' }}>
                {weekDays.map((day) => (
                  <Grid item xs={12/7} key={day} sx={{ 
                    p: 2, 
                    textAlign: 'center',
                    bgcolor: '#2d2d2d',
                    borderRight: '1px solid #424242',
                    fontWeight: 600,
                    color: '#ffffff',
                    '&:last-child': { borderRight: 'none' }
                  }}>
                    {day}
                  </Grid>
                ))}
              </Grid>

              {/* Calendar Days */}
              <Grid container>
                {days.map((day, index) => {
                  const isCurrentMonth = isSameMonth(day, currentDate);
                  const isToday = isSameDay(day, new Date());
                  const dayEvents = getEventsForDay(day);
                  
                  return (
                    <Grid item xs={12/7} key={index} sx={{ 
                      minHeight: 120,
                      borderRight: '1px solid #424242',
                      borderBottom: '1px solid #424242',
                      bgcolor: isCurrentMonth ? '#2d2d2d' : 'transparent',
                      cursor: isCurrentMonth ? 'pointer' : 'default',
                      '&:hover': isCurrentMonth ? { bgcolor: '#3d3d3d' } : {},
                      '&:nth-child(7n)': { borderRight: 'none' },
                      position: 'relative',
                    }}>
                      {isCurrentMonth && (
                        <>
                          <Box sx={{ 
                            p: 1, 
                            textAlign: 'right',
                            color: isToday ? '#29c7a7' : '#ffffff',
                            fontWeight: isToday ? 'bold' : 'normal',
                          }}>
                            {format(day, 'd')}
                          </Box>
                          
                                                     <Box sx={{ p: 1, position: 'relative', zIndex: 10 }}>
                             {dayEvents.map((event) => (
                               <Box
                                 key={event.id}
                                 sx={{
                                   bgcolor: getColorByValue(event.color),
                                   color: 'white',
                                   p: 0.5,
                                   mb: 0.5,
                                   borderRadius: 1,
                                   fontSize: '0.75rem',
                                   cursor: 'pointer',
                                   '&:hover': { opacity: 0.8 },
                                   position: 'relative',
                                   zIndex: 15,
                                 }}
                                 onClick={(e) => {
                                   e.stopPropagation();
                                   handleEditItem(event);
                                 }}
                               >
                                 {event.title}
                               </Box>
                             ))}
                           </Box>
                           
                           <Box
                             sx={{
                               position: 'absolute',
                               top: 0,
                               left: 0,
                               right: 0,
                               bottom: 0,
                               zIndex: 5,
                             }}
                             onClick={() => handleDayClick(day)}
                           />
                        </>
                      )}
                    </Grid>
                  );
                })}
              </Grid>
            </>
          ) : viewMode === 'week' ? (
                         /* Week View with Hourly Columns */
             <Box sx={{ height: '100vh', overflow: 'auto' }}>
               {/* Time Column Headers */}
               <Grid container sx={{ borderBottom: '1px solid #424242' }}>
                 <Grid item xs={1} sx={{ 
                   p: 0.5, 
                   textAlign: 'center',
                   bgcolor: '#2d2d2d',
                   borderRight: '1px solid #424242',
                   fontWeight: 600,
                   color: '#ffffff',
                   fontSize: '0.8rem',
                 }}>
                   Time
                 </Grid>
                 {days.map((day) => (
                   <Grid item xs key={format(day, 'yyyy-MM-dd')} sx={{ 
                     p: 0.5, 
                     textAlign: 'center',
                     bgcolor: '#2d2d2d',
                     borderRight: '1px solid #424242',
                     fontWeight: 600,
                     color: '#ffffff',
                     fontSize: '0.8rem',
                     '&:last-child': { borderRight: 'none' }
                   }}>
                     {format(day, 'EEE d')}
                   </Grid>
                 ))}
               </Grid>

                               {/* Hourly Rows */}
                {Array.from({ length: 24 }, (_, hour) => (
                  <Grid container key={hour} sx={{ borderBottom: '1px solid #424242' }}>
                    <Grid item xs={1} sx={{ 
                      p: 0.5, 
                      textAlign: 'center',
                      bgcolor: '#2d2d2d',
                      borderRight: '1px solid #424242',
                      color: '#ffffff',
                      fontSize: '0.7rem',
                    }}>
                      {`${hour.toString().padStart(2, '0')}:00`}
                    </Grid>
                    {days.map((day) => {
                      const currentHour = setHours(day, hour);
                      
                      return (
                        <Grid item xs key={`${format(day, 'yyyy-MM-dd')}-${hour}`} sx={{ 
                          minHeight: 30,
                          borderRight: '1px solid #424242',
                          bgcolor: '#2d2d2d',
                          position: 'relative',
                          '&:last-child': { borderRight: 'none' },
                          '&:hover': { bgcolor: '#3d3d3d' },
                          cursor: 'pointer',
                        }}
                        onClick={() => handleDayClick(currentHour)}
                        >
                                                     {/* Render events for this specific hour */}
                           {(() => {
                             // Get events that start in this hour
                             const eventsStartingThisHour = items.filter(event => {
                               const eventStart = new Date(event.start);
                               const hourStart = new Date(currentHour);
                               const hourEnd = new Date(currentHour);
                               hourEnd.setHours(hourEnd.getHours() + 1);
                               
                               return eventStart >= hourStart && eventStart < hourEnd;
                             });

                                                                                         return eventsStartingThisHour.map((event, eventIndex) => {
                                const position = getEventPosition(event, currentHour);
                                const isStart = position.isStart;
                                
                                // Find all events that overlap with this hour (not just those that start in it)
                                const allEventsInThisHour = items.filter(otherEvent => {
                                  const otherStart = new Date(otherEvent.start);
                                  const otherEnd = new Date(otherEvent.end);
                                  const hourStart = new Date(currentHour);
                                  const hourEnd = new Date(currentHour);
                                  hourEnd.setHours(hourEnd.getHours() + 1);
                                  
                                  // Check if event overlaps with this hour
                                  return otherStart < hourEnd && otherEnd > hourStart;
                                });
                                
                                // Find all events that overlap with this specific event
                                const overlappingWithThisEvent = allEventsInThisHour.filter(otherEvent => {
                                  const eventStart = new Date(event.start);
                                  const eventEnd = new Date(event.end);
                                  const otherStart = new Date(otherEvent.start);
                                  const otherEnd = new Date(otherEvent.end);
                                  
                                  // Check if events overlap in time
                                  return eventStart < otherEnd && eventEnd > otherStart;
                                });
                                
                                // Sort overlapping events by ID to ensure consistent ordering
                                overlappingWithThisEvent.sort((a, b) => a.id.localeCompare(b.id));
                                
                                // Find this event's position within the overlapping group
                                const eventPositionInGroup = overlappingWithThisEvent.findIndex(e => e.id === event.id);
                                const totalOverlapping = overlappingWithThisEvent.length;
                                
                                // Calculate width and position
                                const baseWidth = totalOverlapping > 0 ? 100 / totalOverlapping : 100;
                                const left = totalOverlapping > 0 ? `${eventPositionInGroup * baseWidth}%` : '0%';
                               
                               // Calculate how many hours this event spans
                               const eventStart = new Date(event.start);
                               const eventEnd = new Date(event.end);
                               const startHour = eventStart.getHours();
                               const endHour = eventEnd.getHours();
                               const spansMultipleHours = startHour !== endHour || 
                                 eventStart.getMinutes() !== 0 || 
                                 eventEnd.getMinutes() !== 0;
                               
                               // For multi-hour events, we need to calculate the height differently
                               let finalHeight = position.height;
                               if (spansMultipleHours) {
                                 // Calculate the total height needed for the multi-hour event
                                 const totalMinutes = (eventEnd.getTime() - eventStart.getTime()) / (1000 * 60);
                                 const totalHeightPercent = (totalMinutes / 60) * 100;
                                 finalHeight = `${totalHeightPercent}%`;
                               }
                               
                               return (
                                 <Box
                                   key={event.id}
                                   sx={{
                                     position: 'absolute',
                                     top: position.top,
                                     left,
                                     width: `${baseWidth}%`,
                                     height: finalHeight,
                                     bgcolor: getColorByValue(event.color),
                                     color: 'white',
                                     p: 0.25,
                                     borderRadius: 0.5,
                                     fontSize: '0.6rem',
                                     cursor: 'pointer',
                                     '&:hover': { opacity: 0.8 },
                                     zIndex: 10,
                                     overflow: 'hidden',
                                     textOverflow: 'ellipsis',
                                     whiteSpace: 'nowrap',
                                     borderLeft: isStart ? '2px solid rgba(255,255,255,0.3)' : 'none',
                                   }}
                                   onClick={(e) => {
                                     e.stopPropagation();
                                     handleEditItem(event);
                                   }}
                                   title={event.title}
                                 >
                                   {isStart && event.title}
                                 </Box>
                               );
                             });
                           })()}
                        </Grid>
                      );
                    })}
                  </Grid>
                ))}
             </Box>
          ) : (
            /* Compact View - Upcoming Events List */
            <Box sx={{ height: '70vh', overflow: 'auto' }}>
              <List sx={{ width: '100%' }}>
                {getUpcomingEvents().length === 0 ? (
                  <ListItem>
                    <ListItemText
                      primary="No upcoming events"
                      secondary="Click the + button to add a new event"
                      sx={{ textAlign: 'center', color: 'text.secondary' }}
                    />
                  </ListItem>
                ) : (
                  getUpcomingEvents().map((event, index) => (
                    <Box key={event.id}>
                      <ListItemButton
                        onClick={() => handleEditItem(event)}
                        sx={{
                          py: 2,
                          '&:hover': { bgcolor: 'rgba(255, 255, 255, 0.05)' },
                        }}
                      >
                        <ListItemText
                          primary={
                            <Box sx={{ display: 'flex', alignItems: 'center', gap: 1, mb: 0.5 }}>
                              <Chip
                                size="small"
                                sx={{
                                  bgcolor: getColorByValue(event.color),
                                  color: 'white',
                                  minWidth: 20,
                                  height: 20,
                                  '& .MuiChip-label': {
                                    px: 0.5,
                                    fontSize: '0.7rem',
                                  },
                                }}
                                label="●"
                              />
                              <Typography variant="subtitle1" sx={{ fontWeight: 500 }}>
                                {event.title}
                              </Typography>
                              {event.isPrivate && (
                                <Chip
                                  size="small"
                                  label="Private"
                                  variant="outlined"
                                  sx={{ ml: 'auto', fontSize: '0.7rem' }}
                                />
                              )}
                            </Box>
                          }
                          secondary={
                            <Box sx={{ mt: 0.5 }}>
                              <Typography variant="body2" color="text.secondary">
                                {format(new Date(event.start), 'EEEE, MMMM d, yyyy')}
                              </Typography>
                              <Typography variant="body2" color="text.secondary">
                                {format(new Date(event.start), 'h:mm a')} - {format(new Date(event.end), 'h:mm a')}
                                {new Date(event.start).toDateString() !== new Date(event.end).toDateString() && 
                                  ` (+${Math.ceil((new Date(event.end).getTime() - new Date(event.start).getTime()) / (1000 * 60 * 60 * 24))}d)`
                                }
                              </Typography>
                            </Box>
                          }
                        />
                      </ListItemButton>
                      {index < getUpcomingEvents().length - 1 && (
                        <Divider sx={{ bgcolor: 'rgba(255, 255, 255, 0.12)' }} />
                      )}
                    </Box>
                  ))
                )}
              </List>
            </Box>
          )}
        </Box>

        {/* Add/Edit Dialog */}
        <Dialog open={dialogOpen} onClose={() => {
          setDialogOpen(false);
          setEditingItem(null);
        }} maxWidth="sm" fullWidth>
          <DialogTitle>
            {editingItem ? 'Edit Calendar Event' : 'Add Calendar Event'}
          </DialogTitle>
          <DialogContent>
            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2, pt: 1 }}>
              <TextField
                label="Event Title"
                value={formData.title}
                onChange={(e) => setFormData({ ...formData, title: e.target.value })}
                fullWidth
                required
                multiline
                rows={2}
              />
                             <TextField
                 label="Start Date & Time"
                 type="datetime-local"
                 value={format(formData.start, "yyyy-MM-dd'T'HH:mm")}
                 onChange={(e) => {
                   const newStart = new Date(e.target.value);
                   const adjustedEnd = validateAndAdjustEndTime(newStart, formData.end);
                   setFormData({ 
                     ...formData, 
                     start: newStart,
                     end: adjustedEnd
                   });
                 }}
                 InputLabelProps={{ shrink: true }}
                 fullWidth
                 required
               />
              <TextField
                label="End Date & Time"
                type="datetime-local"
                value={format(formData.end, "yyyy-MM-dd'T'HH:mm")}
                onChange={(e) => {
                  const newEnd = new Date(e.target.value);
                  if (newEnd <= formData.start) {
                    setError('End time must be after start time');
                  } else if ((newEnd.getTime() - formData.start.getTime()) < 5 * 60 * 1000) {
                    setError('Event must be at least 5 minutes long');
                  } else {
                    setError(null);
                  }
                  setFormData({ ...formData, end: newEnd });
                }}
                InputLabelProps={{ shrink: true }}
                fullWidth
                required
                error={formData.end <= formData.start}
                helperText={
                  formData.end <= formData.start 
                    ? 'End time must be after start time' 
                    : (formData.end.getTime() - formData.start.getTime()) < 5 * 60 * 1000
                    ? 'Event must be at least 5 minutes long'
                    : `Duration: ${getDurationText(formData.start, formData.end)}`
                }
              />
              
              {/* Quick Duration Buttons */}
              <Box sx={{ display: 'flex', gap: 1, flexWrap: 'wrap' }}>
                <Button
                  size="small"
                  variant="outlined"
                  onClick={() => {
                    const newEnd = new Date(formData.start.getTime() + 30 * 60 * 1000); // 30 minutes
                    setFormData({ ...formData, end: newEnd });
                    setError(null);
                  }}
                >
                  30 min
                </Button>
                <Button
                  size="small"
                  variant="outlined"
                  onClick={() => {
                    const newEnd = new Date(formData.start.getTime() + 60 * 60 * 1000); // 1 hour
                    setFormData({ ...formData, end: newEnd });
                    setError(null);
                  }}
                >
                  1 hour
                </Button>
                <Button
                  size="small"
                  variant="outlined"
                  onClick={() => {
                    const newEnd = new Date(formData.start.getTime() + 2 * 60 * 60 * 1000); // 2 hours
                    setFormData({ ...formData, end: newEnd });
                    setError(null);
                  }}
                >
                  2 hours
                </Button>
                <Button
                  size="small"
                  variant="outlined"
                  onClick={() => {
                    const newEnd = new Date(formData.start.getTime() + 4 * 60 * 60 * 1000); // 4 hours
                    setFormData({ ...formData, end: newEnd });
                    setError(null);
                  }}
                >
                  4 hours
                </Button>
              </Box>
              
              <TextField
                label="Color"
                select
                value={formData.color}
                onChange={(e) => setFormData({ ...formData, color: e.target.value })}
                fullWidth
                required
              >
                {COLOR_PRESETS.map((preset) => (
                  <MenuItem key={preset.value} value={preset.value}>
                    <Box sx={{ display: 'flex', alignItems: 'center', gap: 1 }}>
                      <Box
                        sx={{
                          width: 16,
                          height: 16,
                          borderRadius: '50%',
                          bgcolor: preset.color,
                        }}
                      />
                      {preset.label}
                    </Box>
                  </MenuItem>
                ))}
              </TextField>
              <FormControlLabel
                control={
                  <Checkbox
                    checked={formData.isPrivate}
                    onChange={(e) => setFormData({ ...formData, isPrivate: e.target.checked })}
                  />
                }
                label="Private (visible only to you)"
              />
            </Box>
          </DialogContent>
          <DialogActions>
            <Button onClick={() => {
              setDialogOpen(false);
              setEditingItem(null);
            }}>Cancel</Button>
            {editingItem && (
              <Button 
                onClick={() => {
                  setDialogOpen(false);
                  handleDeleteClick(editingItem);
                }} 
                color="error"
                variant="outlined"
                sx={{ mr: 1 }}
              >
                Delete
              </Button>
            )}
            <Button 
              onClick={handleSaveItem} 
              variant="contained"
              disabled={
                !formData.title || 
                formData.end <= formData.start || 
                (formData.end.getTime() - formData.start.getTime()) < 5 * 60 * 1000 // Less than 5 minutes
              }
            >
              {editingItem ? 'Update' : 'Add'}
            </Button>
          </DialogActions>
        </Dialog>

        {/* Delete Confirmation Dialog */}
        <Dialog open={deleteDialogOpen} onClose={() => setDeleteDialogOpen(false)}>
          <DialogTitle>Confirm Deletion</DialogTitle>
          <DialogContent>
            <Typography>
              Are you sure you want to delete this calendar event?
            </Typography>
            {itemToDelete && (
              <Box sx={{ 
                mt: 2, 
                p: 2, 
                bgcolor: 'background.paper', 
                borderRadius: 1, 
                border: '1px solid rgba(255,255,255,0.12)'
              }}>
                <Typography variant="body2">
                  <strong>Event:</strong> {itemToDelete.title}
                </Typography>
                <Typography variant="body2">
                  <strong>Start:</strong> {format(itemToDelete.start, 'PPpp')}
                </Typography>
                <Typography variant="body2">
                  <strong>End:</strong> {format(itemToDelete.end, 'PPpp')}
                </Typography>
                <Typography variant="body2">
                  <strong>Duration:</strong> {Math.round((itemToDelete.end.getTime() - itemToDelete.start.getTime()) / (1000 * 60))} minutes
                </Typography>
                <Typography variant="body2">
                  <strong>Color:</strong> {getColorLabel(itemToDelete.color)}
                </Typography>
                <Typography variant="body2">
                  <strong>Visibility:</strong> {itemToDelete.isPrivate ? 'Private' : 'Public'}
                </Typography>
              </Box>
            )}
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setDeleteDialogOpen(false)}>Cancel</Button>
            <Button onClick={handleDeleteConfirm} color="error" variant="contained">
              Delete
            </Button>
          </DialogActions>
        </Dialog>
      </Paper>
    </Container>
  );
}
