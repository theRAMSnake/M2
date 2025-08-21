import Database from 'better-sqlite3';
import { join } from 'path';
import { Document } from '../types';
import { logger } from '../utils/logger';

export class DatabaseService {
  private static instance: DatabaseService;
  private db: Database.Database;
  private _dbPath: string;
  private writeStmt: Database.Statement;
  private readStmt: Database.Statement;
  private deleteStmt: Database.Statement;

  private constructor() {
    // Database is always mapped to /materia/materia-v5.db in the container
    // Windows: C:/materia/materia-v5.db -> /materia/materia-v5.db
    // Linux: /materia/materia-v5.db -> /materia/materia-v5.db
    this._dbPath = '/materia/materia-v5.db';
    
    // Ensure the /materia directory exists
    try {
      const fs = require('fs');
      if (!fs.existsSync('/materia')) {
        fs.mkdirSync('/materia', { recursive: true });
      }
    } catch (error) {
      logger.warn(`Could not create /materia directory: ${error instanceof Error ? error.message : String(error)}`);
    }
    
    this.db = new Database(this._dbPath);
    this.setupDatabase();
    this.prepareStatements();
    logger.info(`Database initialized at: ${this._dbPath}`);
  }

  public static getInstance(): DatabaseService {
    if (!DatabaseService.instance) {
      DatabaseService.instance = new DatabaseService();
    }
    return DatabaseService.instance;
  }

  private setupDatabase() {
    // Enable WAL mode for better performance and concurrency
    this.db.pragma('journal_mode = WAL');
    this.db.pragma('cache_size = -64000');  // 256MB cache
    this.db.pragma('mmap_size = 268435456'); // 256MB mmap
    this.db.pragma('synchronous = NORMAL');
    this.db.pragma('temp_store = MEMORY');
    this.db.pragma('locking_mode = EXCLUSIVE');
    
    // Create tables if they don't exist
    this.db.exec(`
      CREATE TABLE IF NOT EXISTS documents (
        id INTEGER PRIMARY KEY,
        path TEXT UNIQUE NOT NULL,
        data TEXT NOT NULL,
        created_at DATETIME DEFAULT CURRENT_TIMESTAMP,
        updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
      );
      
      -- Index for efficient path queries
      CREATE INDEX IF NOT EXISTS idx_documents_path ON documents(path);
    `);
  }

  private prepareStatements() {
    // Prepare statements once for reuse (performance optimization)
    this.writeStmt = this.db.prepare(`
      INSERT INTO documents (path, data, updated_at) 
      VALUES (?, ?, CURRENT_TIMESTAMP) 
      ON CONFLICT(path) DO UPDATE SET 
        data = excluded.data, 
        updated_at = CURRENT_TIMESTAMP
    `);
    
    this.readStmt = this.db.prepare(`
      SELECT data FROM documents 
      WHERE path = ?
    `);
    
    this.deleteStmt = this.db.prepare(`
      DELETE FROM documents 
      WHERE path = ?
    `);
  }

  // Fast write operations
  async writeDocument(path: string, data: any): Promise<void> {
    try {
      if (!path || path.trim() === '') {
        throw new Error('Path cannot be empty');
      }
      this.writeStmt.run(path, JSON.stringify(data));
    } catch (error) {
      logger.error(`Error writing document ${path}:`, error);
      throw error;
    }
  }

  async readDocument(path: string): Promise<any | null> {
    try {
      const result = this.readStmt.get(path) as { data: string } | undefined;
      return result ? JSON.parse(result.data) : null;
    } catch (error) {
      logger.error(`Error reading document ${path}:`, error);
      throw error;
    }
  }

  async listDocuments(prefix: string): Promise<Document[]> {
    try {
      let query: string;
      let params: string[];
      
      if (prefix === '/') {
        // Special case: root documents only
        query = `
          SELECT id, path, data, created_at, updated_at 
          FROM documents 
          WHERE path LIKE '/%' AND path NOT LIKE '/%/%'
          ORDER BY updated_at DESC
        `;
        params = [];
      } else {
        // Normal prefix matching - documents that start with prefix but not deeper than one level
        query = `
          SELECT id, path, data, created_at, updated_at 
          FROM documents 
          WHERE path LIKE ? AND path NOT LIKE ?
          ORDER BY updated_at DESC
        `;
        params = [`${prefix}%`, `${prefix}/%/%`];
      }
      
      const stmt = this.db.prepare(query);
      const results = stmt.all(...params) as any[];

      return results.map(row => ({
        id: row.id,
        path: row.path,
        data: JSON.parse(row.data),
        createdAt: new Date(row.created_at),
        updatedAt: new Date(row.updated_at)
      }));
    } catch (error) {
      logger.error(`Error listing documents for prefix ${prefix}:`, error);
      throw error;
    }
  }

  async deleteDocument(path: string): Promise<void> {
    try {
      // Delete the document and all its children recursively
      const deleteWithChildren = this.db.transaction((docPath: string) => {
        // First, delete all children recursively
        const childrenQuery = `
          SELECT path FROM documents 
          WHERE path LIKE ? AND path != ?
        `;
        const childrenStmt = this.db.prepare(childrenQuery);
        const children = childrenStmt.all(`${docPath}/%`, docPath) as { path: string }[];
        
        // Delete children first (bottom-up deletion)
        for (const child of children) {
          this.deleteStmt.run(child.path);
        }
        
        // Then delete the parent document
        const result = this.deleteStmt.run(docPath);
        if (result.changes === 0) {
          throw new Error(`Document not found: ${docPath}`);
        }
      });
      
      deleteWithChildren(path);
    } catch (error) {
      logger.error(`Error deleting document ${path}:`, error);
      throw error;
    }
  }

  // Batch operations for better performance
  async writeBatch(operations: Array<{path: string, data: any}>): Promise<void> {
    try {
      // Validate all operations first
      for (const op of operations) {
        if (!op.path || op.path.trim() === '') {
          throw new Error('Path cannot be empty');
        }
      }

      const transaction = this.db.transaction((ops) => {
        for (const op of ops) {
          this.writeStmt.run(op.path, JSON.stringify(op.data));
        }
      });
      
      transaction(operations);
    } catch (error) {
      logger.error(`Error in batch write:`, error);
      throw error;
    }
  }

  // Backup functionality
  async backup(backupPath?: string): Promise<string> {
    try {
      const timestamp = new Date().toISOString().replace(/[:.]/g, '-');
      // Use shared materia directory for backups if in production
      const backupDir = process.env.NODE_ENV === 'production' ? '/materia/backups' : join(process.cwd(), 'backups');
      const defaultBackupPath = join(backupDir, `backup-${timestamp}.db`);
      const finalBackupPath = backupPath || defaultBackupPath;
      
      // Ensure backup directory exists
      const fs = require('fs');
      if (!fs.existsSync(backupDir)) {
        fs.mkdirSync(backupDir, { recursive: true });
      }
      
      this.db.backup(finalBackupPath);
      
      logger.info(`Backup created: ${finalBackupPath}`);
      return finalBackupPath;
    } catch (error) {
      logger.error('Error creating backup:', error);
      throw error;
    }
  }

  // Close database connection
  close(): void {
    this.db.close();
    logger.info('Database connection closed');
  }

  // Public method for testing - initialize with custom database path
  initializeWithPath(dbPath: string): void {
    this.close();
    this._dbPath = dbPath;
    this.db = new Database(dbPath);
    this.setupDatabase();
    this.prepareStatements();
    logger.info(`Database initialized at: ${dbPath}`);
  }

  // Getter for database path (for testing)
  get dbPath(): string {
    return this._dbPath;
  }

  // Get all root documents (parent is null)
  async getRootDocuments(): Promise<Document[]> {
    return this.getChildren('');
  }

  // Get direct children of a path (non-recursive, efficient)
  async getChildren(path: string): Promise<Document[]> {
    try {
      const query = `
        SELECT id, path, data, created_at, updated_at 
        FROM documents 
        WHERE path LIKE ? AND path NOT LIKE ?
        ORDER BY updated_at DESC
      `;
      
      const stmt = this.db.prepare(query);
      const results = stmt.all(`${path}/%`, `${path}/%/%`) as any[];

      return results.map(row => ({
        id: row.id,
        path: row.path,
        data: JSON.parse(row.data),
        createdAt: new Date(row.created_at),
        updatedAt: new Date(row.updated_at)
      }));
    } catch (error) {
      logger.error(`Error getting children for path ${path}:`, error);
      throw error;
    }
  }

  // Search documents by path
  async searchDocuments(searchTerm: string): Promise<Document[]> {
    try {
      const query = `
        SELECT id, path, data, created_at, updated_at 
        FROM documents 
        WHERE path LIKE ? 
        ORDER BY updated_at DESC
      `;
      
      const stmt = this.db.prepare(query);
      const results = stmt.all(`%${searchTerm}%`) as any[];

      return results.map(row => ({
        id: row.id,
        path: row.path,
        data: JSON.parse(row.data),
        createdAt: new Date(row.created_at),
        updatedAt: new Date(row.updated_at)
      }));
    } catch (error) {
      logger.error(`Error searching documents for term ${searchTerm}:`, error);
      throw error;
    }
  }

  // Create backup using SQLite's online backup API (works with exclusive mode)
  async createBackup(backupPath: string): Promise<void> {
    try {
      // Use SQLite's VACUUM INTO command which works even in exclusive mode
      const stmt = this.db.prepare(`VACUUM INTO ?`);
      stmt.run(backupPath);
      logger.info(`Database backup created: ${backupPath}`);
    } catch (error) {
      logger.error(`Error creating backup: ${error}`);
      throw error;
    }
  }

}
