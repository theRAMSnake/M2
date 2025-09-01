import { TaskManager } from './taskManager';
import { logger } from './logger';

export class TaskScheduler {
  private static instance: TaskScheduler;
  private taskManager: TaskManager;
  private intervals: Map<string, NodeJS.Timeout> = new Map();
  private isRunning: boolean = false;

  private constructor() {
    this.taskManager = TaskManager.getInstance();
  }

  public static getInstance(): TaskScheduler {
    if (!TaskScheduler.instance) {
      TaskScheduler.instance = new TaskScheduler();
    }
    return TaskScheduler.instance;
  }

  public start(): void {
    if (this.isRunning) {
      logger.warn('Task scheduler is already running');
      return;
    }

    this.isRunning = true;
    logger.info('Starting task scheduler');

    // Schedule daily update task at 04:00
    this.scheduleDailyUpdate();
    
    // Schedule other tasks here as needed
    // this.scheduleIngestionTask();
  }

  public stop(): void {
    if (!this.isRunning) {
      logger.warn('Task scheduler is not running');
      return;
    }

    this.isRunning = false;
    logger.info('Stopping task scheduler');

    // Clear all intervals
    for (const [taskId, interval] of this.intervals) {
      clearInterval(interval);
      logger.info(`Cleared interval for task: ${taskId}`);
    }
    this.intervals.clear();
  }

  private scheduleDailyUpdate(): void {
    const taskId = 'daily-update';
    
    // Calculate time until next 04:00
    const now = new Date();
    const nextRun = new Date();
    nextRun.setHours(4, 0, 0, 0);
    
    // If it's already past 04:00 today, schedule for tomorrow
    if (now >= nextRun) {
      nextRun.setDate(nextRun.getDate() + 1);
    }

    const timeUntilNextRun = nextRun.getTime() - now.getTime();
    
    logger.info(`Scheduling daily update task for ${nextRun.toISOString()} (in ${Math.round(timeUntilNextRun / 1000 / 60)} minutes)`);

    // Schedule the first run
    const initialTimeout = setTimeout(async () => {
      await this.runDailyUpdate();
      
      // Then schedule it to run every 24 hours
      const interval = setInterval(async () => {
        await this.runDailyUpdate();
      }, 24 * 60 * 60 * 1000); // 24 hours in milliseconds
      
      this.intervals.set(taskId, interval);
    }, timeUntilNextRun);

    // Store the initial timeout as well
    this.intervals.set(`${taskId}-initial`, initialTimeout);
  }

  private async runDailyUpdate(): Promise<void> {
    try {
      logger.info('Running scheduled daily update task');
      
      // Check if task is already running
      if (this.taskManager.isTaskRunning('daily-update')) {
        logger.warn('Daily update task is already running, skipping this execution');
        return;
      }

      const result = await this.taskManager.startTask('daily-update');
      
      if (result.success) {
        logger.info('Scheduled daily update task completed successfully', result.data);
      } else {
        logger.error('Scheduled daily update task failed:', result.message);
      }
    } catch (error) {
      logger.error('Error running scheduled daily update task:', error);
    }
  }

  public getScheduledTasks(): string[] {
    return Array.from(this.intervals.keys());
  }

  public isSchedulerRunning(): boolean {
    return this.isRunning;
  }
}
