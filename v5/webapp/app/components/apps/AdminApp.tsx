'use client';

import React, { useState, useEffect } from 'react';
import {
  Box,
  Card,
  CardContent,
  Typography,
  Button,
  Chip,
  LinearProgress,
  Alert,
  CircularProgress,
  Grid,
  Paper,
  Divider
} from '@mui/material';
import {
  PlayArrow as PlayIcon,
  Refresh as RefreshIcon,
  CheckCircle as SuccessIcon,
  Error as ErrorIcon,
  Schedule as IdleIcon,
  Sync as RunningIcon,
  Timer as TimerIcon
} from '@mui/icons-material';
import { getAuthToken } from '../../utils/auth';

interface TaskStatus {
  id: string;
  name: string;
  status: 'idle' | 'running' | 'completed' | 'failed';
  startedAt?: string;
  completedAt?: string;
  error?: string;
  progress?: number;
}

interface TaskResult {
  success: boolean;
  message: string;
  data?: any;
}

interface SchedulerStatus {
  isRunning: boolean;
  scheduledTasks: string[];
}

const AdminApp: React.FC = () => {
  const [tasks, setTasks] = useState<string[]>([]);
  const [taskStatuses, setTaskStatuses] = useState<TaskStatus[]>([]);
  const [schedulerStatus, setSchedulerStatus] = useState<SchedulerStatus | null>(null);
  const [loading, setLoading] = useState(true);
  const [error, setError] = useState<string | null>(null);
  const [startingTask, setStartingTask] = useState<string | null>(null);
  const [refreshInterval, setRefreshInterval] = useState<NodeJS.Timeout | null>(null);

  const fetchAvailableTasks = async () => {
    try {
      const response = await fetch('/api/admin/tasks', {
        headers: {
          'Authorization': `Bearer ${getAuthToken()}`
        }
      });

      if (!response.ok) {
        const errorText = await response.text();
        console.error('Admin API error:', response.status, errorText);
        if (response.status === 403) {
          throw new Error('Access denied. Admin functionality is only available for authorized users.');
        }
        throw new Error(`Failed to fetch available tasks: ${response.status} ${errorText}`);
      }

      const data = await response.json();
      setTasks(data.data.tasks);
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to fetch tasks');
    }
  };

  const fetchTaskStatuses = async () => {
    try {
      const response = await fetch('/api/admin/tasks/status', {
        headers: {
          'Authorization': `Bearer ${getAuthToken()}`
        }
      });

      if (!response.ok) {
        const errorText = await response.text();
        console.error('Task status API error:', response.status, errorText);
        throw new Error(`Failed to fetch task statuses: ${response.status} ${errorText}`);
      }

      const data = await response.json();
      setTaskStatuses(data.data.statuses);
    } catch (err) {
      console.error('Error fetching task statuses:', err);
    }
  };

  const fetchSchedulerStatus = async () => {
    try {
      const response = await fetch('/api/admin/scheduler/status', {
        headers: {
          'Authorization': `Bearer ${getAuthToken()}`
        }
      });

      if (!response.ok) {
        const errorText = await response.text();
        console.error('Scheduler status API error:', response.status, errorText);
        throw new Error(`Failed to fetch scheduler status: ${response.status} ${errorText}`);
      }

      const data = await response.json();
      setSchedulerStatus(data.data);
    } catch (err) {
      console.error('Error fetching scheduler status:', err);
    }
  };



  const startTask = async (taskId: string) => {
    setStartingTask(taskId);
    try {
      const response = await fetch(`/api/admin/tasks/${taskId}/start`, {
        method: 'POST',
        headers: {
          'Authorization': `Bearer ${getAuthToken()}`,
          'Content-Type': 'application/json'
        }
      });

      if (!response.ok) {
        const errorData = await response.json();
        throw new Error(errorData.message || 'Failed to start task');
      }

      const data = await response.json();
      console.log('Task started:', data);
      
      // Refresh statuses immediately after starting
      await fetchTaskStatuses();
    } catch (err) {
      setError(err instanceof Error ? err.message : 'Failed to start task');
    } finally {
      setStartingTask(null);
    }
  };

  const getStatusIcon = (status: string) => {
    switch (status) {
      case 'running':
        return <RunningIcon color="primary" />;
      case 'completed':
        return <SuccessIcon color="success" />;
      case 'failed':
        return <ErrorIcon color="error" />;
      default:
        return <IdleIcon color="action" />;
    }
  };

  const getStatusColor = (status: string) => {
    switch (status) {
      case 'running':
        return 'primary';
      case 'completed':
        return 'success';
      case 'failed':
        return 'error';
      default:
        return 'default';
    }
  };

  const formatDate = (dateString?: string) => {
    if (!dateString) return 'N/A';
    return new Date(dateString).toLocaleString();
  };

  const getDuration = (startedAt?: string, completedAt?: string) => {
    if (!startedAt) return 'N/A';
    
    const start = new Date(startedAt);
    const end = completedAt ? new Date(completedAt) : new Date();
    const duration = end.getTime() - start.getTime();
    
    const seconds = Math.floor(duration / 1000);
    const minutes = Math.floor(seconds / 60);
    const hours = Math.floor(minutes / 60);
    
    if (hours > 0) {
      return `${hours}h ${minutes % 60}m ${seconds % 60}s`;
    } else if (minutes > 0) {
      return `${minutes}m ${seconds % 60}s`;
    } else {
      return `${seconds}s`;
    }
  };

  useEffect(() => {
    const initializeApp = async () => {
      setLoading(true);
      await fetchAvailableTasks();
      await fetchTaskStatuses();
      await fetchSchedulerStatus();
      setLoading(false);
    };

    initializeApp();

    // Set up auto-refresh for running tasks
    const interval = setInterval(() => {
      const hasRunningTasks = taskStatuses.some(task => task.status === 'running');
      if (hasRunningTasks) {
        fetchTaskStatuses();
      }
    }, 2000);

    setRefreshInterval(interval);

    return () => {
      if (refreshInterval) {
        clearInterval(refreshInterval);
      }
    };
  }, []);

  useEffect(() => {
    // Update interval when task statuses change
    if (refreshInterval) {
      clearInterval(refreshInterval);
    }

    const hasRunningTasks = taskStatuses.some(task => task.status === 'running');
    const interval = setInterval(() => {
      if (hasRunningTasks) {
        fetchTaskStatuses();
      }
    }, 2000);

    setRefreshInterval(interval);

    return () => {
      clearInterval(interval);
    };
  }, [taskStatuses]);

  if (loading) {
    return (
      <Box display="flex" justifyContent="center" alignItems="center" minHeight="400px">
        <CircularProgress />
      </Box>
    );
  }

  if (error) {
    return (
      <Box p={3}>
        <Alert severity="error" onClose={() => setError(null)}>
          {error}
        </Alert>
      </Box>
    );
  }

  return (
    <Box p={3}>
      <Typography variant="h4" gutterBottom>
        Admin Panel
      </Typography>
      <Typography variant="body1" color="text.secondary" gutterBottom>
        Manage background tasks and system operations
      </Typography>
      


      {/* Scheduler Status */}
      {schedulerStatus && (
        <Box mt={3} mb={3}>
          <Card>
            <CardContent>
              <Box display="flex" alignItems="center" justifyContent="space-between" mb={2}>
                <Box display="flex" alignItems="center" gap={1}>
                  <TimerIcon />
                  <Typography variant="h6">
                    Task Scheduler
                  </Typography>
                </Box>
                <Chip
                  label={schedulerStatus.isRunning ? 'Running' : 'Stopped'}
                  color={schedulerStatus.isRunning ? 'success' : 'error'}
                  size="small"
                />
              </Box>
              
              <Typography variant="body2" color="text.secondary" gutterBottom>
                Scheduled Tasks: {schedulerStatus.scheduledTasks.length}
              </Typography>
              
              {schedulerStatus.scheduledTasks.length > 0 && (
                <Box mt={1}>
                  <Typography variant="body2" color="text.secondary">
                    Active schedules: {schedulerStatus.scheduledTasks.join(', ')}
                  </Typography>
                </Box>
              )}
              
              <Box mt={2}>
                <Button
                  variant="outlined"
                  startIcon={<RefreshIcon />}
                  onClick={fetchSchedulerStatus}
                  size="small"
                >
                  Refresh Status
                </Button>
              </Box>
            </CardContent>
          </Card>
        </Box>
      )}

      <Box mt={3}>
        <Grid container spacing={3}>
          {taskStatuses.map((task) => (
            <Grid item xs={12} md={6} key={task.id}>
              <Card>
                <CardContent>
                  <Box display="flex" alignItems="center" justifyContent="space-between" mb={2}>
                    <Box display="flex" alignItems="center" gap={1}>
                      {getStatusIcon(task.status)}
                      <Typography variant="h6">
                        {task.name}
                      </Typography>
                    </Box>
                    <Chip
                      label={task.status}
                      color={getStatusColor(task.status) as any}
                      size="small"
                    />
                  </Box>

                  {task.status === 'running' && task.progress !== undefined && (
                    <Box mb={2}>
                      <Box display="flex" justifyContent="space-between" mb={1}>
                        <Typography variant="body2" color="text.secondary">
                          Progress
                        </Typography>
                        <Typography variant="body2" color="text.secondary">
                          {task.progress}%
                        </Typography>
                      </Box>
                      <LinearProgress variant="determinate" value={task.progress} />
                    </Box>
                  )}

                  <Box mb={2}>
                    <Typography variant="body2" color="text.secondary">
                      Started: {formatDate(task.startedAt)}
                    </Typography>
                    {task.completedAt && (
                      <Typography variant="body2" color="text.secondary">
                        Completed: {formatDate(task.completedAt)}
                      </Typography>
                    )}
                    <Typography variant="body2" color="text.secondary">
                      Duration: {getDuration(task.startedAt, task.completedAt)}
                    </Typography>
                  </Box>

                  {task.error && (
                    <Alert severity="error" sx={{ mb: 2 }}>
                      {task.error}
                    </Alert>
                  )}

                  <Box display="flex" gap={1}>
                    <Button
                      variant="contained"
                      startIcon={<PlayIcon />}
                      onClick={() => startTask(task.id)}
                      disabled={task.status === 'running' || startingTask === task.id}
                      size="small"
                    >
                      {startingTask === task.id ? 'Starting...' : 'Start'}
                    </Button>
                    <Button
                      variant="outlined"
                      startIcon={<RefreshIcon />}
                      onClick={fetchTaskStatuses}
                      size="small"
                    >
                      Refresh
                    </Button>
                  </Box>
                </CardContent>
              </Card>
            </Grid>
          ))}
        </Grid>

        {taskStatuses.length === 0 && (
          <Paper sx={{ p: 3, textAlign: 'center' }}>
            <Typography variant="body1" color="text.secondary">
              No tasks available
            </Typography>
          </Paper>
        )}
      </Box>
    </Box>
  );
};

export default AdminApp;
