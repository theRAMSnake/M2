'use client';

import { useState, useEffect } from 'react';
import {
  Box,
  Container,
  Paper,
  Typography,
  Table,
  TableBody,
  TableCell,
  TableContainer,
  TableHead,
  TableRow,
  Button,
  Dialog,
  DialogTitle,
  DialogContent,
  DialogActions,
  TextField,
  MenuItem,
  IconButton,
  Fab,
  Pagination,
  Alert,
  Accordion,
  AccordionSummary,
  AccordionDetails,
  Grid,
  Card,
  CardContent,
  ToggleButton,
  ToggleButtonGroup,
} from '@mui/material';
import {
  LineChart,
  Line,
  XAxis,
  YAxis,
  CartesianGrid,
  Tooltip,
  ResponsiveContainer,
  Legend,
} from 'recharts';
import { 
  Add as AddIcon, 
  Edit as EditIcon, 
  Delete as DeleteIcon,
  ExpandMore as ExpandMoreIcon,
  People as PeopleIcon,
  Timeline as TimelineIcon,
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';
import { generateId } from '../../utils/idGenerator';

interface BusinessEvent {
  id: string;
  date: string;
  amount: number;
  category: 'Client A' | 'Client B' | 'Ads cost' | 'Other cost';
  description?: string;
}

interface YearSummary {
  year: number;
  totalIncome: number;
  totalExpenses: number;
  netProfit: number;
  clientACount: number;
  clientBCount: number;
  adsCostCount: number;
  otherCostCount: number;
  clientATotal: number;
  clientBTotal: number;
  adsCostTotal: number;
  otherCostTotal: number;
}

interface WeeklyMetrics {
  week: string;
  clientCount: number;
  avgIncomePerWeek: number;
  weekNumber: number;
  year: number;
}

const CATEGORIES = ['Client A', 'Client B', 'Ads cost', 'Other cost'];
const EVENTS_PER_PAGE = 10;

// Display mapping for UI
const getCategoryDisplayName = (category: string): string => {
  switch (category) {
    case 'Client A':
      return 'Client Cash';
    case 'Client B':
      return 'Client Pin';
    default:
      return category;
  }
};

export function MakeUpApp() {
  const [events, setEvents] = useState<BusinessEvent[]>([]);
  const [loading, setLoading] = useState(true);
  const [dialogOpen, setDialogOpen] = useState(false);
  const [deleteDialogOpen, setDeleteDialogOpen] = useState(false);
  const [eventToDelete, setEventToDelete] = useState<BusinessEvent | null>(null);
  const [editingEvent, setEditingEvent] = useState<BusinessEvent | null>(null);
  const [currentPage, setCurrentPage] = useState(1);
  const [error, setError] = useState<string | null>(null);
  const [metricsMode, setMetricsMode] = useState<'past-year' | 'all-time'>('past-year');
  const [formData, setFormData] = useState({
    date: '',
    amount: '',
    category: 'Client A',
    description: '',
  });

  useEffect(() => {
    loadEvents();
  }, []);

  const loadEvents = async () => {
    try {
      setLoading(true);
      setError(null);
      const token = getAuthToken();
      const response = await fetch('/api/make-up/business-events', {
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        const data = await response.json();
        setEvents(data.data?.events || []);
      } else {
        setError('Failed to load business events');
      }
    } catch (error) {
      console.error('Failed to load business events:', error);
      setError('Failed to load business events');
    } finally {
      setLoading(false);
    }
  };

  const handleAddEvent = () => {
    setEditingEvent(null);
    setFormData({
      date: new Date().toISOString().split('T')[0],
      amount: '',
      category: 'Client A',
      description: '',
    });
    setDialogOpen(true);
  };

  const handleEditEvent = (event: BusinessEvent) => {
    setEditingEvent(event);
    setFormData({
      date: event.date,
      amount: event.amount.toString(),
      category: event.category,
      description: event.description || '',
    });
    setDialogOpen(true);
  };

  const handleDeleteClick = (event: BusinessEvent) => {
    setEventToDelete(event);
    setDeleteDialogOpen(true);
  };

  const handleDeleteConfirm = async () => {
    if (!eventToDelete) return;

    try {
      const token = getAuthToken();
      const response = await fetch(`/api/make-up/business-events/${eventToDelete.id}`, {
        method: 'DELETE',
        headers: {
          'Authorization': `Bearer ${token}`
        }
      });

      if (response.ok) {
        await loadEvents();
        setDeleteDialogOpen(false);
        setEventToDelete(null);
      } else {
        setError('Failed to delete event');
      }
    } catch (error) {
      console.error('Failed to delete business event:', error);
      setError('Failed to delete event');
    }
  };

  const handleSaveEvent = async () => {
    try {
      setError(null);
      const token = getAuthToken();
      
      const eventData = {
        id: generateId(editingEvent?.id),
        date: formData.date,
        amount: parseFloat(formData.amount),
        category: formData.category as BusinessEvent['category'],
        description: formData.description || undefined,
      };

      const url = editingEvent 
        ? `/api/make-up/business-events/${editingEvent.id}`
        : '/api/make-up/business-events/event';
      
      const method = editingEvent ? 'PUT' : 'POST';

      const response = await fetch(url, {
        method,
        headers: {
          'Authorization': `Bearer ${token}`,
          'Content-Type': 'application/json'
        },
        body: JSON.stringify(eventData)
      });

      if (response.ok) {
        await loadEvents();
        setDialogOpen(false);
        setEditingEvent(null);
      } else {
        setError(`Failed to ${editingEvent ? 'update' : 'add'} event`);
      }
    } catch (error) {
      console.error(`Failed to ${editingEvent ? 'update' : 'add'} business event:`, error);
      setError(`Failed to ${editingEvent ? 'update' : 'add'} event`);
    }
  };

  const formatCurrency = (amount: number) => {
    return new Intl.NumberFormat('de-DE', {
      style: 'currency',
      currency: 'EUR',
    }).format(amount);
  };

  const getCurrentYearEvents = () => {
    const currentYear = new Date().getFullYear();
    return events.filter(event => {
      const eventYear = new Date(event.date).getFullYear();
      return eventYear === currentYear;
    });
  };

  const getTotalByCategory = (category: string) => {
    return getCurrentYearEvents()
      .filter(event => event.category === category)
      .reduce((sum, event) => sum + event.amount, 0);
  };

  const getTotalIncome = () => {
    return getCurrentYearEvents()
      .filter(event => event.category === 'Client A' || event.category === 'Client B')
      .reduce((sum, event) => sum + event.amount, 0);
  };

  const getTotalExpenses = () => {
    return getCurrentYearEvents()
      .filter(event => event.category === 'Ads cost' || event.category === 'Other cost')
      .reduce((sum, event) => sum + event.amount, 0);
  };

  const getTotalClients = () => {
    return getCurrentYearEvents()
      .filter(event => event.category === 'Client A' || event.category === 'Client B')
      .length;
  };

  const getYearSummaries = (): YearSummary[] => {
    const yearMap = new Map<number, BusinessEvent[]>();
    
    // Group events by year
    events.forEach(event => {
      const year = new Date(event.date).getFullYear();
      if (!yearMap.has(year)) {
        yearMap.set(year, []);
      }
      yearMap.get(year)!.push(event);
    });

    // Calculate summaries for each year
    const summaries: YearSummary[] = [];
    yearMap.forEach((yearEvents, year) => {
      const totalIncome = yearEvents
        .filter(event => event.category === 'Client A' || event.category === 'Client B')
        .reduce((sum, event) => sum + event.amount, 0);
      
      const totalExpenses = yearEvents
        .filter(event => event.category === 'Ads cost' || event.category === 'Other cost')
        .reduce((sum, event) => sum + event.amount, 0);

      const clientACount = yearEvents.filter(event => event.category === 'Client A').length;
      const clientBCount = yearEvents.filter(event => event.category === 'Client B').length;
      const adsCostCount = yearEvents.filter(event => event.category === 'Ads cost').length;
      const otherCostCount = yearEvents.filter(event => event.category === 'Other cost').length;

      const clientATotal = yearEvents
        .filter(event => event.category === 'Client A')
        .reduce((sum, event) => sum + event.amount, 0);
      const clientBTotal = yearEvents
        .filter(event => event.category === 'Client B')
        .reduce((sum, event) => sum + event.amount, 0);
      const adsCostTotal = yearEvents
        .filter(event => event.category === 'Ads cost')
        .reduce((sum, event) => sum + event.amount, 0);
      const otherCostTotal = yearEvents
        .filter(event => event.category === 'Other cost')
        .reduce((sum, event) => sum + event.amount, 0);

      summaries.push({
        year,
        totalIncome,
        totalExpenses,
        netProfit: totalIncome - totalExpenses,
        clientACount,
        clientBCount,
        adsCostCount,
        otherCostCount,
        clientATotal,
        clientBTotal,
        adsCostTotal,
        otherCostTotal,
      });
    });

    return summaries.sort((a, b) => b.year - a.year); // Sort by year descending
  };

  const getWeeklyMetrics = (): WeeklyMetrics[] => {
    const clientEvents = events.filter(event => 
      event.category === 'Client A' || event.category === 'Client B'
    );

    const incomeEvents = events.filter(event => 
      event.category === 'Client A' || event.category === 'Client B'
    );

    // Get all unique weeks from the data
    const weekSet = new Set<string>();
    events.forEach(event => {
      const date = new Date(event.date);
      const year = date.getFullYear();
      const weekNumber = getWeekNumber(date);
      const weekKey = `${year}-W${weekNumber.toString().padStart(2, '0')}`;
      weekSet.add(weekKey);
    });

    // Convert to array and sort chronologically
    let allWeeks = Array.from(weekSet).sort((a, b) => {
      const [yearA, weekA] = a.split('-W');
      const [yearB, weekB] = b.split('-W');
      if (yearA !== yearB) return parseInt(yearA) - parseInt(yearB);
      return parseInt(weekA) - parseInt(weekB);
    });

    // Fill in missing weeks to ensure continuity
    if (allWeeks.length > 1) {
      const filledWeeks: string[] = [];
      for (let i = 0; i < allWeeks.length - 1; i++) {
        const current = allWeeks[i];
        const next = allWeeks[i + 1];
        filledWeeks.push(current);
        
        const [currentYear, currentWeek] = current.split('-W');
        const [nextYear, nextWeek] = next.split('-W');
        
        const currentYearNum = parseInt(currentYear);
        const currentWeekNum = parseInt(currentWeek);
        const nextYearNum = parseInt(nextYear);
        const nextWeekNum = parseInt(nextWeek);
        
        // Fill gaps between weeks
        let year = currentYearNum;
        let week = currentWeekNum + 1;
        
        while (year < nextYearNum || (year === nextYearNum && week < nextWeekNum)) {
          if (week > 53) {
            year++;
            week = 1;
          }
          const weekKey = `${year}-W${week.toString().padStart(2, '0')}`;
          filledWeeks.push(weekKey);
          week++;
        }
      }
      filledWeeks.push(allWeeks[allWeeks.length - 1]);
      allWeeks = filledWeeks;
    }

    // Calculate rolling 365-day average for each week
    const weeklyData: WeeklyMetrics[] = [];
    
    allWeeks.forEach(weekKey => {
      const [yearStr, weekStr] = weekKey.split('-W');
      const year = parseInt(yearStr);
      const weekNumber = parseInt(weekStr);
      
      // Calculate the date for this week (start of the week)
      const weekStartDate = getDateFromWeek(year, weekNumber);
      
      // Calculate the 365-day period ending on this week
      const periodEndDate = new Date(weekStartDate);
      const periodStartDate = new Date(periodEndDate);
      periodStartDate.setDate(periodStartDate.getDate() - 365);
      
      // Count clients in the 365-day period
      const clientsInPeriod = clientEvents.filter(event => {
        const eventDate = new Date(event.date);
        return eventDate >= periodStartDate && eventDate <= periodEndDate;
      }).length;
      
      // Calculate total income in the 365-day period
      const incomeInPeriod = incomeEvents.filter(event => {
        const eventDate = new Date(event.date);
        return eventDate >= periodStartDate && eventDate <= periodEndDate;
      }).reduce((sum, event) => sum + event.amount, 0);
      
      // Calculate averages (per week over 365 days)
      const averageClientsPerWeek = clientsInPeriod / 52.14; // 365/7 ≈ 52.14 weeks
      const averageIncomePerWeek = incomeInPeriod / 52.14; // 365/7 ≈ 52.14 weeks
      
      weeklyData.push({
        week: weekKey,
        clientCount: averageClientsPerWeek,
        avgIncomePerWeek: averageIncomePerWeek,
        weekNumber,
        year
      });
    });

    // Filter by time range if needed
    if (metricsMode === 'past-year') {
      const oneYearAgo = new Date();
      oneYearAgo.setFullYear(oneYearAgo.getFullYear() - 1);
      
      return weeklyData.filter(week => {
        const weekDate = getDateFromWeek(week.year, week.weekNumber);
        return weekDate >= oneYearAgo;
      });
    }

    return weeklyData;
  };

  const getDateFromWeek = (year: number, weekNumber: number): Date => {
    // ISO 8601 week calculation - get the Monday of the specified week
    const simple = new Date(year, 0, 1 + (weekNumber - 1) * 7);
    const dayOfWeek = simple.getDay();
    const ISOweekStart = simple;
    if (dayOfWeek <= 4) {
      ISOweekStart.setDate(simple.getDate() - simple.getDay() + 1);
    } else {
      ISOweekStart.setDate(simple.getDate() + 8 - simple.getDay());
    }
    return ISOweekStart;
  };

  const getWeekNumber = (date: Date): number => {
    // ISO 8601 week calculation
    const target = new Date(date.valueOf());
    const dayNr = (date.getDay() + 6) % 7;
    target.setDate(target.getDate() - dayNr + 3);
    const firstThursday = target.valueOf();
    target.setMonth(0, 1);
    if (target.getDay() !== 4) {
      target.setMonth(0, 1 + ((4 - target.getDay()) + 7) % 7);
    }
    return 1 + Math.ceil((firstThursday - target.valueOf()) / 604800000);
  };

  // Pagination logic
  const currentYearEvents = getCurrentYearEvents()
    .sort((a, b) => new Date(b.date).getTime() - new Date(a.date).getTime());
  
  const totalPages = Math.ceil(currentYearEvents.length / EVENTS_PER_PAGE);
  const startIndex = (currentPage - 1) * EVENTS_PER_PAGE;
  const endIndex = startIndex + EVENTS_PER_PAGE;
  const paginatedEvents = currentYearEvents.slice(startIndex, endIndex);

  const handlePageChange = (event: React.ChangeEvent<unknown>, value: number) => {
    setCurrentPage(value);
  };

  const handleMetricsModeChange = (
    event: React.MouseEvent<HTMLElement>,
    newMode: 'past-year' | 'all-time' | null,
  ) => {
    if (newMode !== null) {
      setMetricsMode(newMode);
    }
  };

  if (loading) {
    return (
      <Container maxWidth="lg" sx={{ py: 4 }}>
        <Typography>Loading...</Typography>
      </Container>
    );
  }

  const weeklyMetrics = getWeeklyMetrics();

  return (
    <Container maxWidth="lg" sx={{ py: 4 }}>
      <Paper sx={{ p: 4, bgcolor: 'background.paper' }}>
        <Box sx={{ display: 'flex', justifyContent: 'space-between', alignItems: 'center', mb: 3 }}>
          <Box>
            <Typography variant="h4" component="h1" gutterBottom>
              Business Events
            </Typography>
            <Typography variant="subtitle1" color="text.secondary">
              {new Date().getFullYear()} - Current Year
            </Typography>
          </Box>
          <Button
            variant="contained"
            startIcon={<AddIcon />}
            onClick={handleAddEvent}
          >
            Add Event
          </Button>
        </Box>

        {error && (
          <Alert severity="error" sx={{ mb: 2 }} onClose={() => setError(null)}>
            {error}
          </Alert>
        )}

        {/* Collapsible Tabs */}
        <Box sx={{ mb: 3 }}>
          {/* Business Events Tab */}
          <Accordion defaultExpanded>
            <AccordionSummary expandIcon={<ExpandMoreIcon />}>
              <Typography variant="h6">Business Events</Typography>
            </AccordionSummary>
            <AccordionDetails>
              {/* Summary Cards */}
              <Box sx={{ display: 'grid', gridTemplateColumns: 'repeat(auto-fit, minmax(200px, 1fr))', gap: 2, mb: 3 }}>
                <Paper sx={{ p: 2, textAlign: 'center', bgcolor: 'success.light', color: 'success.contrastText' }}>
                  <Typography variant="h6">Total Income</Typography>
                  <Typography variant="h4">{formatCurrency(getTotalIncome())}</Typography>
                </Paper>
                <Paper sx={{ p: 2, textAlign: 'center', bgcolor: 'error.light', color: 'error.contrastText' }}>
                  <Typography variant="h6">Total Expenses</Typography>
                  <Typography variant="h4">{formatCurrency(getTotalExpenses())}</Typography>
                </Paper>
                <Paper sx={{ p: 2, textAlign: 'center', bgcolor: 'primary.light', color: 'primary.contrastText' }}>
                  <Typography variant="h6">Net Profit</Typography>
                  <Typography variant="h4">{formatCurrency(getTotalIncome() - getTotalExpenses())}</Typography>
                </Paper>
                <Paper sx={{ p: 2, textAlign: 'center', bgcolor: 'info.light', color: 'info.contrastText' }}>
                  <Typography variant="h6">Total Clients</Typography>
                  <Typography variant="h4">{getTotalClients()}</Typography>
                </Paper>
              </Box>

              {/* Events Table */}
              <TableContainer component={Paper}>
                <Table>
                  <TableHead>
                    <TableRow>
                      <TableCell>Date</TableCell>
                      <TableCell>Category</TableCell>
                      <TableCell align="right">Amount</TableCell>
                      <TableCell>Description</TableCell>
                      <TableCell align="center">Actions</TableCell>
                    </TableRow>
                  </TableHead>
                  <TableBody>
                    {paginatedEvents.map((event) => (
                      <TableRow key={event.id}>
                        <TableCell>{new Date(event.date).toLocaleDateString()}</TableCell>
                        <TableCell>{getCategoryDisplayName(event.category)}</TableCell>
                        <TableCell align="right" sx={{ 
                          color: event.category.includes('cost') ? 'error.main' : 'success.main',
                          fontWeight: 'bold'
                        }}>
                          {formatCurrency(event.amount)}
                        </TableCell>
                        <TableCell>{event.description || '-'}</TableCell>
                        <TableCell align="center">
                          <IconButton onClick={() => handleEditEvent(event)} size="small">
                            <EditIcon />
                          </IconButton>
                          <IconButton onClick={() => handleDeleteClick(event)} size="small" color="error">
                            <DeleteIcon />
                          </IconButton>
                        </TableCell>
                      </TableRow>
                    ))}
                  </TableBody>
                </Table>
              </TableContainer>

              {/* Pagination */}
              {totalPages > 1 && (
                <Box sx={{ display: 'flex', justifyContent: 'center', mt: 2 }}>
                  <Pagination 
                    count={totalPages} 
                    page={currentPage} 
                    onChange={handlePageChange}
                    color="primary"
                  />
                </Box>
              )}
            </AccordionDetails>
          </Accordion>

          {/* Years Summary Tab */}
          <Accordion>
            <AccordionSummary expandIcon={<ExpandMoreIcon />}>
              <Typography variant="h6">Years Summary</Typography>
            </AccordionSummary>
            <AccordionDetails>
              <Grid container spacing={2}>
                {getYearSummaries().map((summary) => (
                  <Grid item xs={12} md={6} lg={4} key={summary.year}>
                    <Card>
                      <CardContent>
                        <Typography variant="h5" component="h2" gutterBottom>
                          {summary.year}
                        </Typography>
                        
                        {/* Totals */}
                        <Box sx={{ mb: 2 }}>
                          <Typography variant="body2" color="success.main" fontWeight="bold">
                            Income: {formatCurrency(summary.totalIncome)}
                          </Typography>
                          <Typography variant="body2" color="error.main" fontWeight="bold">
                            Expenses: {formatCurrency(summary.totalExpenses)}
                          </Typography>
                          <Typography variant="body2" color="primary.main" fontWeight="bold">
                            Net Profit: {formatCurrency(summary.netProfit)}
                          </Typography>
                        </Box>

                        {/* Category Breakdown */}
                        <Box>
                          <Typography variant="subtitle2" gutterBottom>
                            Category Breakdown:
                          </Typography>
                          <Typography variant="body2">
                            Client Cash: {summary.clientACount} events ({formatCurrency(summary.clientATotal)})
                          </Typography>
                          <Typography variant="body2">
                            Client Pin: {summary.clientBCount} events ({formatCurrency(summary.clientBTotal)})
                          </Typography>
                          <Typography variant="body2">
                            Ads Cost: {summary.adsCostCount} events ({formatCurrency(summary.adsCostTotal)})
                          </Typography>
                          <Typography variant="body2">
                            Other Cost: {summary.otherCostCount} events ({formatCurrency(summary.otherCostTotal)})
                          </Typography>
                        </Box>
                      </CardContent>
                    </Card>
                  </Grid>
                ))}
              </Grid>
            </AccordionDetails>
          </Accordion>

          {/* Metrics Tab */}
          <Accordion>
            <AccordionSummary expandIcon={<ExpandMoreIcon />}>
              <Typography variant="h6">Metrics</Typography>
            </AccordionSummary>
            <AccordionDetails>
              <Box sx={{ mb: 3 }}>
                <Typography variant="h6" gutterBottom>
                  Rolling 365-Day Average Clients per Week
                </Typography>
                
                {/* Mode Toggle */}
                <Box sx={{ mb: 2 }}>
                  <ToggleButtonGroup
                    value={metricsMode}
                    exclusive
                    onChange={handleMetricsModeChange}
                    aria-label="metrics time range"
                  >
                    <ToggleButton value="past-year" aria-label="past year">
                      Past Year
                    </ToggleButton>
                    <ToggleButton value="all-time" aria-label="all time">
                      All Time
                    </ToggleButton>
                  </ToggleButtonGroup>
                </Box>

                {/* Chart */}
                <Box sx={{ height: 400, width: '100%' }}>
                  <ResponsiveContainer width="100%" height="100%">
                    <LineChart
                      data={weeklyMetrics}
                      margin={{
                        top: 5,
                        right: 30,
                        left: 20,
                        bottom: 5,
                      }}
                    >
                      <CartesianGrid strokeDasharray="3 3" />
                      <Legend />
                      <XAxis 
                        dataKey="week" 
                        tick={{ fontSize: 12 }}
                        angle={-45}
                        textAnchor="end"
                        height={80}
                      />
                      <YAxis 
                        yAxisId="left"
                        tick={{ fontSize: 12 }}
                        label={{ value: 'Average Clients per Week', angle: -90, position: 'insideLeft' }}
                      />
                      <YAxis 
                        yAxisId="right"
                        orientation="right"
                        tick={{ fontSize: 12 }}
                        label={{ value: 'Average Income per Week (€)', angle: 90, position: 'insideRight' }}
                      />
                      <Tooltip 
                        content={({ active, payload, label }) => {
                          if (active && payload && payload.length) {
                            return (
                              <Box sx={{ 
                                bgcolor: 'background.paper', 
                                border: '1px solid grey.300', 
                                p: 1, 
                                borderRadius: 1 
                              }}>
                                <Typography variant="body2">
                                  Week {label}
                                </Typography>
                                {payload.map((entry: any, index: number) => (
                                  <Typography 
                                    key={index} 
                                    variant="body2" 
                                    sx={{ 
                                      color: entry.color,
                                      fontWeight: 'bold'
                                    }}
                                  >
                                    {entry.name}: {entry.name === 'Income' 
                                      ? formatCurrency(entry.value) 
                                      : `${entry.value.toFixed(2)} clients/week`
                                    } (365-day avg)
                                  </Typography>
                                ))}
                              </Box>
                            );
                          }
                          return null;
                        }}
                      />
                      <Line 
                        yAxisId="left"
                        type="monotone" 
                        dataKey="clientCount" 
                        stroke="#8884d8" 
                        strokeWidth={2}
                        dot={{ fill: '#8884d8', strokeWidth: 2, r: 4 }}
                        activeDot={{ r: 6 }}
                        name="Clients"
                      />
                      <Line 
                        yAxisId="right"
                        type="monotone" 
                        dataKey="avgIncomePerWeek" 
                        stroke="#82ca9d" 
                        strokeWidth={2}
                        dot={{ fill: '#82ca9d', strokeWidth: 2, r: 4 }}
                        activeDot={{ r: 6 }}
                        name="Income"
                      />
                    </LineChart>
                  </ResponsiveContainer>
                </Box>

                {/* Summary Stats */}
                {weeklyMetrics.length > 0 && (
                  <Box sx={{ mt: 2 }}>
                    <Typography variant="subtitle1" gutterBottom>
                      Summary Statistics:
                    </Typography>
                    <Grid container spacing={2}>
                      <Grid item xs={12} sm={6} md={3}>
                        <Paper sx={{ p: 2, textAlign: 'center' }}>
                          <Typography variant="h6" color="primary.main">
                            {weeklyMetrics.length}
                          </Typography>
                          <Typography variant="body2">Total Weeks</Typography>
                        </Paper>
                      </Grid>
                      <Grid item xs={12} sm={6} md={3}>
                        <Paper sx={{ p: 2, textAlign: 'center' }}>
                          <Typography variant="h6" color="success.main">
                            {(weeklyMetrics.reduce((sum, week) => sum + week.clientCount, 0) / weeklyMetrics.length).toFixed(2)}
                          </Typography>
                          <Typography variant="body2">Average Clients/Week</Typography>
                        </Paper>
                      </Grid>
                      <Grid item xs={12} sm={6} md={3}>
                        <Paper sx={{ p: 2, textAlign: 'center' }}>
                          <Typography variant="h6" color="info.main">
                            {Math.max(...weeklyMetrics.map(w => w.clientCount)).toFixed(2)}
                          </Typography>
                          <Typography variant="body2">Max Clients/Week</Typography>
                        </Paper>
                      </Grid>
                      <Grid item xs={12} sm={6} md={3}>
                        <Paper sx={{ p: 2, textAlign: 'center' }}>
                          <Typography variant="h6" color="warning.main">
                            {Math.min(...weeklyMetrics.map(w => w.clientCount)).toFixed(2)}
                          </Typography>
                          <Typography variant="body2">Min Clients/Week</Typography>
                        </Paper>
                      </Grid>
                    </Grid>
                  </Box>
                )}
              </Box>
            </AccordionDetails>
          </Accordion>
        </Box>

        {/* Add/Edit Dialog */}
        <Dialog open={dialogOpen} onClose={() => setDialogOpen(false)} maxWidth="sm" fullWidth>
          <DialogTitle>
            {editingEvent ? 'Edit Business Event' : 'Add Business Event'}
          </DialogTitle>
          <DialogContent>
            <Box sx={{ display: 'flex', flexDirection: 'column', gap: 2, pt: 1 }}>
              <TextField
                label="Date"
                type="date"
                value={formData.date}
                onChange={(e) => setFormData({ ...formData, date: e.target.value })}
                InputLabelProps={{ shrink: true }}
                fullWidth
                required
              />
              <TextField
                label="Amount"
                type="number"
                value={formData.amount}
                onChange={(e) => setFormData({ ...formData, amount: e.target.value })}
                fullWidth
                inputProps={{ step: 0.01, min: 0 }}
                required
              />
              <TextField
                label="Category"
                select
                value={formData.category}
                onChange={(e) => setFormData({ ...formData, category: e.target.value })}
                fullWidth
                required
              >
                {CATEGORIES.map((category) => (
                  <MenuItem key={category} value={category}>
                    {getCategoryDisplayName(category)}
                  </MenuItem>
                ))}
              </TextField>
              <TextField
                label="Description (optional)"
                value={formData.description}
                onChange={(e) => setFormData({ ...formData, description: e.target.value })}
                fullWidth
                multiline
                rows={3}
              />
            </Box>
          </DialogContent>
          <DialogActions>
            <Button onClick={() => setDialogOpen(false)}>Cancel</Button>
            <Button 
              onClick={handleSaveEvent} 
              variant="contained"
              disabled={!formData.date || !formData.amount || !formData.category}
            >
              {editingEvent ? 'Update' : 'Add'}
            </Button>
          </DialogActions>
        </Dialog>

        {/* Delete Confirmation Dialog */}
        <Dialog open={deleteDialogOpen} onClose={() => setDeleteDialogOpen(false)}>
          <DialogTitle>Confirm Deletion</DialogTitle>
          <DialogContent>
            <Typography>
              Are you sure you want to delete this business event?
            </Typography>
            {eventToDelete && (
              <Box sx={{ 
                mt: 2, 
                p: 2, 
                bgcolor: 'grey.800', 
                color: 'white',
                borderRadius: 1, 
                border: '1px solid grey.600'
              }}>
                <Typography variant="body2" sx={{ color: 'white' }}>
                  <strong>Date:</strong> {new Date(eventToDelete.date).toLocaleDateString()}
                </Typography>
                <Typography variant="body2" sx={{ color: 'white' }}>
                  <strong>Category:</strong> {getCategoryDisplayName(eventToDelete.category)}
                </Typography>
                <Typography variant="body2" sx={{ color: 'white' }}>
                  <strong>Amount:</strong> {formatCurrency(eventToDelete.amount)}
                </Typography>
                {eventToDelete.description && (
                  <Typography variant="body2" sx={{ color: 'white' }}>
                    <strong>Description:</strong> {eventToDelete.description}
                  </Typography>
                )}
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
