import { Router, Response } from 'express';
import { AuthRequest } from '../types';
import { AuthMiddleware } from '../middleware/auth';
import { DatabaseService } from '../storage/database';
import { logger } from '../utils/logger';
import { spawn } from 'child_process';
import { join } from 'path';
import { existsSync, mkdirSync } from 'fs';

const router = Router();

// Apply auth middleware to all backup routes
router.use(AuthMiddleware);

interface BackupResponse {
  success: boolean;
  message: string;
  backupFile?: string;
  timestamp?: string;
}

// Create backup endpoint (like v4's backup operation)
router.post('/create', async (req: AuthRequest, res: Response<BackupResponse>) => {
  try {
    logger.info(`Backup requested by user: ${req.user?.username}`);

    // Ensure backup directory exists
    const backupDir = '/materia/backups';
    if (!existsSync(backupDir)) {
      mkdirSync(backupDir, { recursive: true });
      logger.info(`Created backup directory: ${backupDir}`);
    }

    // Generate timestamp and backup filename
    const timestamp = new Date().toISOString().replace(/[:.]/g, '-').replace('T', '_').slice(0, 19);
    const backupFile = `materia-v5_backup_${timestamp}.db`;
    const backupPath = join(backupDir, backupFile);

    // Get database path
    const dbPath = '/materia/materia-v5.db';

    // Create backup using SQLite's online backup API (works with exclusive mode)
    const db = DatabaseService.getInstance();
    await db.createBackup(backupPath);

    logger.info(`Database backup created: ${backupPath}`);

    res.json({
      success: true,
      message: 'Database backup created successfully',
      backupFile,
      timestamp: new Date().toISOString()
    });

  } catch (error) {
    logger.error('Error creating backup:', error);
    res.status(500).json({
      success: false,
      message: error instanceof Error ? error.message : 'Unknown error occurred during backup'
    });
  }
});

// Get backup status endpoint
router.get('/status', async (req: AuthRequest, res: Response) => {
  try {
    const backupDir = '/materia/backups';
    const fs = require('fs');
    
    if (!existsSync(backupDir)) {
      return res.json({
        success: true,
        message: 'No backups directory found',
        backups: []
      });
    }

    // List all backup files
    const files = fs.readdirSync(backupDir)
      .filter((file: string) => file.startsWith('materia-v5_') && file.endsWith('.db'))
      .map((file: string) => {
        const filePath = join(backupDir, file);
        const stats = fs.statSync(filePath);
        return {
          filename: file,
          size: stats.size,
          created: stats.birthtime,
          modified: stats.mtime
        };
      })
      .sort((a: any, b: any) => b.created.getTime() - a.created.getTime());

    res.json({
      success: true,
      message: `Found ${files.length} backup files`,
      backups: files
    });

  } catch (error) {
    logger.error('Error getting backup status:', error);
    res.status(500).json({
      success: false,
      message: error instanceof Error ? error.message : 'Unknown error occurred'
    });
  }
});

export { router as backupRoutes };
