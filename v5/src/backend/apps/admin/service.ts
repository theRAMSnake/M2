import { TaskManager, TaskStatus, TaskResult } from '../../utils/taskManager';
import { TaskScheduler } from '../../utils/taskScheduler';
import { User } from '../../types';
import { logger } from '../../utils/logger';

export class AdminService {
  private taskManager: TaskManager;
  private taskScheduler: TaskScheduler;

  constructor() {
    this.taskManager = TaskManager.getInstance();
    this.taskScheduler = TaskScheduler.getInstance();
  }

  public isAuthorized(user: User): boolean {
    // Only snake user has access to admin functionality
    return user.id === 'snake';
  }

  public getAvailableTasks(): string[] {
    return this.taskManager.getAvailableTasks();
  }

  public getTaskStatus(taskId: string): TaskStatus | null {
    return this.taskManager.getTaskStatus(taskId);
  }

  public getAllTaskStatuses(): TaskStatus[] {
    return this.taskManager.getAllTaskStatuses();
  }

  public async startTask(taskId: string): Promise<TaskResult> {
    logger.info(`Admin service: Starting task ${taskId}`);
    return await this.taskManager.startTask(taskId);
  }

  public isTaskRunning(taskId: string): boolean {
    return this.taskManager.isTaskRunning(taskId);
  }

  public getSchedulerStatus(): { isRunning: boolean; scheduledTasks: string[] } {
    return {
      isRunning: this.taskScheduler.isSchedulerRunning(),
      scheduledTasks: this.taskScheduler.getScheduledTasks()
    };
  }
}
