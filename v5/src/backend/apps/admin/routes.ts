import { Router } from 'express';
import { AdminService } from './service';
import { AuthRequest } from '../../types';

const router = Router();
const adminService = new AdminService();



// Middleware to check admin authorization
const requireAdmin = (req: AuthRequest, res: any, next: any) => {
  if (!req.user) {
    return res.status(401).json({ error: 'Unauthorized' });
  }

  if (!adminService.isAuthorized(req.user)) {
    return res.status(403).json({ 
      error: 'Forbidden',
      message: 'Admin access required'
    });
  }

  next();
};

// GET /api/admin/tasks/status - Get status of all tasks
router.get('/tasks/status', requireAdmin, async (req: AuthRequest, res) => {
  try {
    const statuses = adminService.getAllTaskStatuses();
    res.json({ 
      success: true,
      data: { statuses }
    });
  } catch (error) {
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to get task statuses'
    });
  }
});

// GET /api/admin/tasks/:taskId/status - Get status of specific task
router.get('/tasks/:taskId/status', requireAdmin, async (req: AuthRequest, res) => {
  try {
    const taskId = req.params.taskId;
    const status = adminService.getTaskStatus(taskId);
    
    if (!status) {
      return res.status(404).json({ 
        error: 'Not found',
        message: 'Task not found'
      });
    }

    res.json({ 
      success: true,
      data: status
    });
  } catch (error) {
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to get task status'
    });
  }
});

// GET /api/admin/scheduler/status - Get scheduler status
router.get('/scheduler/status', requireAdmin, async (req: AuthRequest, res) => {
  try {
    const schedulerStatus = adminService.getSchedulerStatus();
    res.json({ 
      success: true,
      data: schedulerStatus
    });
  } catch (error) {
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to get scheduler status'
    });
  }
});

// GET /api/admin/tasks - Get all available tasks (must be last to avoid conflicts)
router.get('/tasks', requireAdmin, async (req: AuthRequest, res) => {
  try {
    const tasks = adminService.getAvailableTasks();
    res.json({ 
      success: true,
      data: { tasks }
    });
  } catch (error) {
    res.status(500).json({ 
      error: 'Internal server error',
      message: 'Failed to get available tasks'
    });
  }
});

// POST /api/admin/tasks/:taskId/start - Start a specific task
router.post('/tasks/:taskId/start', requireAdmin, async (req: AuthRequest, res) => {
  try {
    const taskId = req.params.taskId;
    
    // Check if task is already running
    if (adminService.isTaskRunning(taskId)) {
      return res.status(409).json({ 
        error: 'Conflict',
        message: 'Task is already running'
      });
    }

    const result = await adminService.startTask(taskId);
    
    res.json({ 
      success: true,
      message: 'Task started successfully',
      data: result
    });
  } catch (error) {
    if (error instanceof Error && error.message.includes('not found')) {
      res.status(404).json({ 
        error: 'Not found',
        message: 'Task not found'
      });
    } else {
      res.status(500).json({ 
        error: 'Internal server error',
        message: 'Failed to start task'
      });
    }
  }
});

export default router;
