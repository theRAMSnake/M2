import { DatabaseService } from '../storage/database';
import { Document } from '../types';
import { createTestDatabase, setupTestHierarchy, generateTestData, cleanupTestDatabase } from './utils/test-utils';

describe('DatabaseService', () => {
  let db: DatabaseService;
  let testDbPath: string;

  beforeEach(async () => {
    // Create a fresh test database for each test
    db = createTestDatabase(`test-${Date.now()}`);
    testDbPath = db.dbPath;
  });

  afterEach(async () => {
    // Clean up test database
    if (db) {
      cleanupTestDatabase(db, testDbPath);
    }
  });

  describe('Basic CRUD Operations', () => {
    test('should write and read a document', async () => {
      const testData = { name: 'test', value: 123 };
      const path = '/test-doc';

      await db.writeDocument(path, testData);
      const result = await db.readDocument(path);

      expect(result).toEqual(testData);
    });

    test('should return null for non-existent document', async () => {
      const result = await db.readDocument('/non-existent');
      expect(result).toBeNull();
    });

    test('should update existing document', async () => {
      const path = '/update-test';
      const initialData = { name: 'initial' };
      const updatedData = { name: 'updated', newField: true };

      await db.writeDocument(path, initialData);
      await db.writeDocument(path, updatedData);

      const result = await db.readDocument(path);
      expect(result).toEqual(updatedData);
    });

    test('should delete a document', async () => {
      const path = '/delete-test';
      const testData = { name: 'to-delete' };

      await db.writeDocument(path, testData);
      
      // Verify it exists
      const beforeDelete = await db.readDocument(path);
      expect(beforeDelete).toEqual(testData);

      // Delete it
      await db.deleteDocument(path);

      // Verify it's gone
      const afterDelete = await db.readDocument(path);
      expect(afterDelete).toBeNull();
    });

    test('should throw error when deleting non-existent document', async () => {
      await expect(db.deleteDocument('/non-existent')).rejects.toThrow('Document not found');
    });
  });

  describe('Hierarchical Operations', () => {
    beforeEach(async () => {
      // Set up test hierarchy using utility function
      await setupTestHierarchy(db);
    });



    test('should get direct children only', async () => {
      const children = await db.getChildren('/user1');
      
      // Should only get direct children, not grandchildren
      expect(children).toHaveLength(3);
      expect(children.map(d => d.path)).toContain('/user1/profile');
      expect(children.map(d => d.path)).toContain('/user1/settings');
      expect(children.map(d => d.path)).toContain('/user1/documents');
      
      // Should NOT include grandchildren
      expect(children.map(d => d.path)).not.toContain('/user1/settings/notifications');
      expect(children.map(d => d.path)).not.toContain('/user1/documents/doc1');
    });

    test('should get root documents', async () => {
      const rootDocs = await db.getRootDocuments();
      
      expect(rootDocs).toHaveLength(3); // user1, user2, settings
      expect(rootDocs.map(d => d.path)).toContain('/user1');
      expect(rootDocs.map(d => d.path)).toContain('/user2');
      expect(rootDocs.map(d => d.path)).toContain('/settings');
    });

    test('should delete document and all children recursively', async () => {
      // Verify children exist before deletion
      expect(await db.readDocument('/user1/profile')).toBeTruthy();
      expect(await db.readDocument('/user1/settings/notifications')).toBeTruthy();
      expect(await db.readDocument('/user1/documents/doc1')).toBeTruthy();

      // Delete the parent
      await db.deleteDocument('/user1');

      // Verify parent and all children are deleted
      expect(await db.readDocument('/user1')).toBeNull();
      expect(await db.readDocument('/user1/profile')).toBeNull();
      expect(await db.readDocument('/user1/settings')).toBeNull();
      expect(await db.readDocument('/user1/settings/notifications')).toBeNull();
      expect(await db.readDocument('/user1/documents/doc1')).toBeNull();
      expect(await db.readDocument('/user1/documents/doc2')).toBeNull();

      // Verify other documents are unaffected
      expect(await db.readDocument('/user2')).toBeTruthy();
    });
  });

  describe('Search Operations', () => {
    beforeEach(async () => {
      // Set up test data using utility function
      await setupTestHierarchy(db);
    });

    test('should search documents by path', async () => {
      const results = await db.searchDocuments('profile');
      
      expect(results).toHaveLength(2);
      expect(results.map(d => d.path)).toContain('/user1/profile');
      expect(results.map(d => d.path)).toContain('/user2/profile');
    });

    test('should search documents by path', async () => {
      const results = await db.searchDocuments('user');
      
      // Should find all documents containing 'user' in the path
      expect(results.length).toBeGreaterThan(0);
      expect(results.map(d => d.path)).toContain('/user1');
      expect(results.map(d => d.path)).toContain('/user2');
      expect(results.map(d => d.path)).toContain('/user1/profile');
      expect(results.map(d => d.path)).toContain('/user2/profile');
    });

    test('should return empty array for no matches', async () => {
      const results = await db.searchDocuments('nonexistent');
      expect(results).toHaveLength(0);
    });
  });

  describe('Batch Operations', () => {
    test('should write multiple documents in batch', async () => {
      const operations = [
        { path: '/batch/doc1', data: { title: 'Document 1' } },
        { path: '/batch/doc2', data: { title: 'Document 2' } },
        { path: '/batch/doc3', data: { title: 'Document 3' } }
      ];

      await db.writeBatch(operations);

      // Verify all documents were written
      expect(await db.readDocument('/batch/doc1')).toEqual({ title: 'Document 1' });
      expect(await db.readDocument('/batch/doc2')).toEqual({ title: 'Document 2' });
      expect(await db.readDocument('/batch/doc3')).toEqual({ title: 'Document 3' });
    });

    test('should handle batch operations atomically', async () => {
      // Write first document
      await db.writeDocument('/batch/test', { valid: true });

      const operations = [
        { path: '/batch/doc1', data: { title: 'Document 1' } },
        { path: '/batch/doc2', data: { title: 'Document 2' } },
        { path: '', data: { invalid: 'data' } } // Empty path should cause an error
      ];

      // The batch should fail and rollback
      await expect(db.writeBatch(operations)).rejects.toThrow();

      // Original document should remain unchanged
      expect(await db.readDocument('/batch/test')).toEqual({ valid: true });
      
      // New documents should not exist
      expect(await db.readDocument('/batch/doc1')).toBeNull();
      expect(await db.readDocument('/batch/doc2')).toBeNull();
    });
  });



  describe('Error Handling', () => {
    test('should handle invalid JSON data gracefully', async () => {
      const circularObj: any = {};
      circularObj.self = circularObj;

      await expect(db.writeDocument('/test', circularObj)).rejects.toThrow();
    });

    test('should handle database connection errors', async () => {
      // Close the database connection
      db.close();

      await expect(db.writeDocument('/test', { data: 'test' })).rejects.toThrow();
    });
  });

  describe('Document Structure', () => {
    test('should return documents with correct structure', async () => {
      const testData = { name: 'test', value: 123 };
      await db.writeDocument('/structure-test', testData);

      const rootDocs = await db.getRootDocuments();
      const doc = rootDocs.find(d => d.path === '/structure-test');

      expect(doc).toBeDefined();
      if (!doc) throw new Error('Document not found');
      
      expect(doc).toHaveProperty('id');
      expect(doc).toHaveProperty('path');
      expect(doc).toHaveProperty('data');
      expect(doc).toHaveProperty('createdAt');
      expect(doc).toHaveProperty('updatedAt');

      expect(typeof doc.id).toBe('number');
      expect(typeof doc.path).toBe('string');
      expect(typeof doc.createdAt).toBe('object');
      expect(doc.createdAt instanceof Date).toBe(true);
      expect(typeof doc.updatedAt).toBe('object');
      expect(doc.updatedAt instanceof Date).toBe(true);
    });
  });

  describe('Path Validation', () => {
    test('should handle empty path', async () => {
      await expect(db.writeDocument('', { data: 'test' })).rejects.toThrow();
    });

    test('should handle paths with special characters', async () => {
      const testData = { name: 'test' };
      await db.writeDocument('/special/chars/!@#$%^&*()', testData);

      const result = await db.readDocument('/special/chars/!@#$%^&*()');
      expect(result).toEqual(testData);
    });
  });
});
