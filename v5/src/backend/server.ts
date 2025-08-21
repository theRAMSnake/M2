import express from 'express';
import cors from 'cors';
import helmet from 'helmet';
import { join } from 'path';
import { mkdirSync, existsSync } from 'fs';

import { DatabaseService } from './storage/database';
import { AuthService } from './auth/auth';
import { AuthMiddleware } from './middleware/auth';
import { AuthRoutes } from './routes/auth';
import { DocumentRoutes } from './routes/documents';
import { routes as makeUpRoutes } from './apps/make-up';
import calendarRoutes from './apps/calendar/routes';
import { backupRoutes } from './routes/backup';
import { logger } from './utils/logger';

class MateriaV5Server {
  private app: express.Application;
  private dbService: DatabaseService;
  private authService: AuthService;
  private authMiddleware: AuthMiddleware;
  private port: number;

  constructor() {
    this.port = parseInt(process.env.PORT || '3000');
    this.setupDirectories();
    this.initializeServices();
    this.setupMiddleware();
    this.setupRoutes();
    this.setupErrorHandling();
  }

  private setupDirectories(): void {
    // Create necessary directories
    const dirs = ['data', 'logs', 'backups'];
    dirs.forEach(dir => {
      const dirPath = join(process.cwd(), dir);
      if (!existsSync(dirPath)) {
        mkdirSync(dirPath, { recursive: true });
        logger.info(`Created directory: ${dirPath}`);
      }
    });
  }

  private initializeServices(): void {
    try {
      this.dbService = DatabaseService.getInstance();
      this.authService = new AuthService();
      this.authMiddleware = new AuthMiddleware(this.authService);
      logger.info('All services initialized successfully');
    } catch (error) {
      logger.error('Failed to initialize services:', error);
      process.exit(1);
    }
  }

  private setupMiddleware(): void {
    this.app = express();

    // Security middleware
    this.app.use(helmet({
      contentSecurityPolicy: false, // Disable for development
      crossOriginEmbedderPolicy: false
    }));

    // CORS configuration
    this.app.use(cors({
      origin: true, // Allow all origins for development
      credentials: true
    }));

    // Body parsing middleware
    this.app.use(express.json({ limit: '10mb' }));
    this.app.use(express.urlencoded({ extended: true }));

    // Request logging
    this.app.use((req, res, next) => {
      logger.info(`${req.method} ${req.path} - ${req.ip}`);
      next();
    });
  }

  private setupRoutes(): void {
    // Health check endpoint
    this.app.get('/health', (req, res) => {
      res.json({
        status: 'ok',
        timestamp: new Date().toISOString(),
        version: '5.0.0'
      });
    });

    // API routes - accessible at /api
    const apiRouter = express.Router();

    // Auth routes (no authentication required)
    const authRoutes = new AuthRoutes(this.authService);
    apiRouter.use('/auth', authRoutes.getRouter());

    // Document routes (authentication required)
    const documentRoutes = new DocumentRoutes(this.dbService);
    apiRouter.use('/documents', this.authMiddleware.authenticate as any, documentRoutes.getRouter());

    // Make-up app routes (authentication required)
    apiRouter.use('/make-up', this.authMiddleware.authenticate as any, makeUpRoutes);

    // Calendar app routes (authentication required)
    apiRouter.use('/calendar', this.authMiddleware.authenticate as any, calendarRoutes);

    // Backup routes (authentication required)
    apiRouter.use('/backup', this.authMiddleware.authenticate as any, backupRoutes);

    // API info endpoint
    apiRouter.get('/', (req, res) => {
      res.json({
        name: 'Materia V5 API',
        version: '5.0.0',
        endpoints: {
          auth: '/auth',
          documents: '/documents',
          makeUp: '/make-up',
          calendar: '/calendar',
          backup: '/backup',
          health: '/health'
        }
      });
    });

    this.app.use('/api', apiRouter);

    // Legacy API compatibility (read-only from old database)
    this.app.get('/api/describe', (req, res) => {
      res.json({
        strategy_node: true,
        version: '5.0.0',
        message: 'Materia V5 - Legacy compatibility mode'
      });
    });

    // Serve static files for the webapp
    const webappPath = join(process.cwd(), 'webapp', 'out');
    if (existsSync(webappPath)) {
      this.app.use(express.static(webappPath));
    }

    // URL-based routing for user-specific deployments
    this.app.get('/snake', (req, res) => {
      res.sendFile(join(webappPath, 'index.html'));
    });

    this.app.get('/seva', (req, res) => {
      res.sendFile(join(webappPath, 'index.html'));
    });

    // Catch-all route for SPA
    this.app.get('*', (req, res) => {
      if (existsSync(webappPath)) {
        res.sendFile(join(webappPath, 'index.html'));
      } else {
        res.status(404).json({
          error: 'NotFound',
          message: 'Webapp not built yet'
        });
      }
    });

    logger.info('Routes configured successfully');
  }

  private setupErrorHandling(): void {
    // Global error handler
    this.app.use((error: any, req: express.Request, res: express.Response, next: express.NextFunction) => {
      logger.error('Unhandled error:', error);
      
      res.status(500).json({
        error: 'InternalServerError',
        message: process.env.NODE_ENV === 'production' 
          ? 'Internal server error' 
          : error.message
      });
    });
  }

  public start(): void {
    const server = this.app.listen(this.port, () => {
      logger.info(`Materia V5 server started on port ${this.port}`);
      logger.info(`Health check: http://localhost:${this.port}/health`);
      logger.info(`API: http://localhost:${this.port}/api`);
      logger.info(`Snake's interface: http://localhost:${this.port}/snake`);
      logger.info(`Seva's interface: http://localhost:${this.port}/seva`);
    });

    // Graceful shutdown
    process.on('SIGTERM', () => {
      logger.info('SIGTERM received, shutting down gracefully');
      server.close(() => {
        this.dbService.close();
        logger.info('Server closed');
        process.exit(0);
      });
    });

    process.on('SIGINT', () => {
      logger.info('SIGINT received, shutting down gracefully');
      server.close(() => {
        this.dbService.close();
        logger.info('Server closed');
        process.exit(0);
      });
    });
  }
}

// Start the server
if (require.main === module) {
  const server = new MateriaV5Server();
  server.start();
}

export default MateriaV5Server;
