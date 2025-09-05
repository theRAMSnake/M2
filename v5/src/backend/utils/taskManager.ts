import { logger } from './logger';
import { WasteCalendarFetcher } from './wasteCalendarFetcher';
import { CalendarService } from '../apps/calendar/service';
import { DatabaseService } from '../storage/database';
import { DisciplineService } from '../apps/discipline/service';

export interface TaskStatus {
  id: string;
  name: string;
  status: 'idle' | 'running' | 'completed' | 'failed';
  startedAt?: Date;
  completedAt?: Date;
  error?: string;
  progress?: number;
}

export interface TaskResult {
  success: boolean;
  message: string;
  data?: any;
}

export abstract class BackgroundTask {
  protected id: string;
  protected name: string;
  protected status: TaskStatus;

  constructor(id: string, name: string) {
    this.id = id;
    this.name = name;
    this.status = {
      id: this.id,
      name: this.name,
      status: 'idle'
    };
  }

  public getStatus(): TaskStatus {
    return { ...this.status };
  }

  public async execute(): Promise<TaskResult> {
    if (this.status.status === 'running') {
      throw new Error(`Task ${this.name} is already running`);
    }

    this.status.status = 'running';
    this.status.startedAt = new Date();
    this.status.error = undefined;
    this.status.progress = 0;

    try {
      logger.info(`Starting task: ${this.name}`);
      const result = await this.run();
      
      this.status.status = 'completed';
      this.status.completedAt = new Date();
      this.status.progress = 100;
      
      logger.info(`Task completed: ${this.name}`);
      return result;
    } catch (error) {
      this.status.status = 'failed';
      this.status.completedAt = new Date();
      this.status.error = error instanceof Error ? error.message : 'Unknown error';
      
      logger.error(`Task failed: ${this.name}`, error);
      return {
        success: false,
        message: this.status.error
      };
    }
  }

  protected abstract run(): Promise<TaskResult>;
  protected updateProgress(progress: number): void {
    this.status.progress = Math.min(100, Math.max(0, progress));
  }
}

export class IngestionTask extends BackgroundTask {
  private wasteFetcher: WasteCalendarFetcher;
  private calendarService: CalendarService;

  constructor() {
    super('ingestion', 'Waste Calendar Ingestion');
    this.wasteFetcher = new WasteCalendarFetcher();
    this.calendarService = new CalendarService();
  }

  protected async run(): Promise<TaskResult> {
    try {
      logger.info('Starting waste calendar ingestion for Eindhoven 5627HC');
      
      // Step 1: Fetch waste calendar data
      this.updateProgress(10);
      logger.info('Fetching waste calendar data...');
      const wasteEvents = await this.wasteFetcher.fetchWasteCalendar();
      await this.delay(2000);
      
      this.updateProgress(25);
      logger.info(`Found ${wasteEvents.length} waste collection events`);
      
      if (wasteEvents.length === 0) {
        logger.warn('No waste collection events found - no real data available');
        return {
          success: true,
          message: 'No waste collection data available from external sources',
          data: {
            processedItems: 0,
            addedItems: 0,
            skippedItems: 0,
            timestamp: new Date().toISOString()
          }
        };
      }
      
      // Step 2: Get existing calendar items to avoid duplicates
      this.updateProgress(40);
      logger.info('Checking existing calendar items...');
      const existingItems = await this.calendarService.getCalendarItems('snake');
      const existingWasteEvents = new Set(
        existingItems
          .filter(item => item.text.includes('Waste Collection'))
          .map(item => {
            const date = new Date(item.dateTime);
            const wasteType = item.text.replace('Waste Collection: ', '');
            return `${date.toDateString()}-${wasteType}`;
          })
      );
      await this.delay(2000);
      
      this.updateProgress(60);
      logger.info(`Found ${existingWasteEvents.size} existing waste collection events`);
      
      // Step 3: Add new waste collection events
      this.updateProgress(80);
      logger.info('Adding new waste collection events...');
      let addedCount = 0;
      let skippedCount = 0;
      
      for (const wasteEvent of wasteEvents) {
        const eventDateString = wasteEvent.date.toDateString();
        const eventKey = `${eventDateString}-${wasteEvent.type}`;
        
        if (existingWasteEvents.has(eventKey)) {
          logger.info(`Skipping existing waste collection event: ${wasteEvent.type} on ${wasteEvent.date.toDateString()}`);
          skippedCount++;
          continue;
        }
        
        // Create calendar event
        const calendarItem = {
          id: `waste-${wasteEvent.date.getTime()}`,
          text: `Waste Collection: ${wasteEvent.type}`,
          dateTime: wasteEvent.date.toISOString(),
          endTime: new Date(wasteEvent.date.getTime() + 60 * 60 * 1000).toISOString(), // 1 hour duration
          isPrivate: false,
          color: this.getWasteTypeColor(wasteEvent.type)
        };
        
        logger.info(`Adding waste collection event: ${wasteEvent.type} on ${wasteEvent.date.toDateString()}`);
        await this.calendarService.addCalendarItem('snake', calendarItem);
        addedCount++;
        
        // Small delay to avoid overwhelming the system
        await this.delay(100);
      }
      
      this.updateProgress(95);
      await this.delay(2000);
      
      this.updateProgress(100);
      
      logger.info(`Waste calendar ingestion completed: ${addedCount} added, ${skippedCount} skipped`);
      
      return {
        success: true,
        message: `Waste calendar ingestion completed successfully`,
        data: {
          processedItems: wasteEvents.length,
          addedItems: addedCount,
          skippedItems: skippedCount,
          timestamp: new Date().toISOString()
        }
      };
    } catch (error) {
      logger.error('Error during waste calendar ingestion:', error);
      return {
        success: false,
        message: `Ingestion failed: ${error instanceof Error ? error.message : 'Unknown error'}`,
        data: {
          error: error instanceof Error ? error.message : 'Unknown error',
          timestamp: new Date().toISOString()
        }
      };
    }
  }

  private getWasteTypeColor(wasteType: string): string {
    // Map waste types to frontend color preset values
    const colorMap: { [key: string]: string } = {
      'Restafval': 'grey',    // Grey - General waste
      'GFT': 'green',         // Green - Green waste
      'Papier': 'blue',       // Blue - Paper
      'PMD': 'orange',        // Orange - Plastic/Metal
      'Textiel': 'purple',    // Purple - Textiles
      'Glas': 'blue'          // Blue - Glass (using blue as cyan not available)
    };
    
    const color = colorMap[wasteType];
    if (color) {
      logger.info(`Assigned color ${color} to waste type: ${wasteType}`);
      return color;
    }
    
    logger.warn(`No color mapping found for waste type: ${wasteType}, using default blue`);
    return 'blue'; // Default blue
  }

  private delay(ms: number): Promise<void> {
    return new Promise(resolve => setTimeout(resolve, ms));
  }
}

export class DailyUpdateTask extends BackgroundTask {
  private dbService: DatabaseService;
  private disciplineService: DisciplineService;

  constructor() {
    super('daily-update', 'Daily Update');
    this.dbService = DatabaseService.getInstance();
    this.disciplineService = new DisciplineService();
  }

  protected async run(): Promise<TaskResult> {
    try {
      logger.info('Starting daily update task');
      
      // Increment work burden (weekdays only)
      await this.disciplineService.incrementWorkBurdenDaily();
      
      // Apply daily modifiers (weekdays only)
      await this.disciplineService.applyDailyModifiers();
      
      logger.info('Daily update task completed successfully');
      
      return {
        success: true,
        message: 'Daily update completed successfully',
        data: {
          timestamp: new Date().toISOString()
        }
      };
    } catch (error) {
      logger.error('Daily update task failed:', error);
      throw error;
    }
  }

}

export class TaskManager {
  private static instance: TaskManager;
  private tasks: Map<string, BackgroundTask> = new Map();

  private constructor() {
    // Initialize available tasks
    this.tasks.set('ingestion', new IngestionTask());
    this.tasks.set('daily-update', new DailyUpdateTask());
  }

  public static getInstance(): TaskManager {
    if (!TaskManager.instance) {
      TaskManager.instance = new TaskManager();
    }
    return TaskManager.instance;
  }

  public getAvailableTasks(): string[] {
    return Array.from(this.tasks.keys());
  }

  public getTaskStatus(taskId: string): TaskStatus | null {
    const task = this.tasks.get(taskId);
    return task ? task.getStatus() : null;
  }

  public getAllTaskStatuses(): TaskStatus[] {
    return Array.from(this.tasks.values()).map(task => task.getStatus());
  }

  public async startTask(taskId: string): Promise<TaskResult> {
    const task = this.tasks.get(taskId);
    if (!task) {
      throw new Error(`Task '${taskId}' not found`);
    }

    return await task.execute();
  }

  public isTaskRunning(taskId: string): boolean {
    const task = this.tasks.get(taskId);
    return task ? task.getStatus().status === 'running' : false;
  }

}
