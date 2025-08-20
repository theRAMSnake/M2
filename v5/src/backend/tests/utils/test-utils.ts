import { DatabaseService } from '../../storage/database';
import path from 'path';
import fs from 'fs';

export interface TestDatabaseConfig {
  dbPath: string;
  cleanupAfterTest?: boolean;
}

export function createTestDatabase(testName: string): DatabaseService {
  const testDbPath = path.join(__dirname, `test-${testName}-${Date.now()}.sqlite`);
  
  // Get the singleton instance and initialize with test path
  const db = DatabaseService.getInstance();
  db.initializeWithPath(testDbPath);
  
  return db;
}

export function createPersistentTestDatabase(testName: string): DatabaseService {
  const testDbPath = path.join(__dirname, `persistent-${testName}.sqlite`);
  
  // Get the singleton instance and initialize with test path
  const db = DatabaseService.getInstance();
  db.initializeWithPath(testDbPath);
  
  return db;
}

export async function setupTestHierarchy(db: DatabaseService): Promise<void> {
  // Create a standard test hierarchy
  await db.writeDocument('/user1', { name: 'User 1' });
  await db.writeDocument('/user1/profile', { email: 'user1@test.com' });
  await db.writeDocument('/user1/settings', { theme: 'dark' });
  await db.writeDocument('/user1/settings/notifications', { email: true });
  await db.writeDocument('/user1/documents', { name: 'Documents' });
  await db.writeDocument('/user1/documents/doc1', { title: 'Document 1' });
  await db.writeDocument('/user1/documents/doc2', { title: 'Document 2' });
  await db.writeDocument('/user2', { name: 'User 2' });
  await db.writeDocument('/user2/profile', { email: 'user2@test.com' });
  await db.writeDocument('/settings', { name: 'Settings' });
  await db.writeDocument('/settings/global', { theme: 'light' });
}

export function generateTestData(count: number = 10): Array<{ path: string; data: any }> {
  const operations = [];
  for (let i = 1; i <= count; i++) {
    operations.push({
      path: `/test/doc${i}`,
      data: { 
        id: i, 
        title: `Test Document ${i}`,
        content: `This is test content for document ${i}`,
        timestamp: new Date().toISOString()
      }
    });
  }
  return operations;
}

export function cleanupTestDatabase(db: DatabaseService, testDbPath: string): void {
  db.close();
  if (fs.existsSync(testDbPath)) {
    fs.unlinkSync(testDbPath);
  }
}
