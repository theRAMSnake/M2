import { Router, Request, Response } from 'express';
import { z } from 'zod';
import { DatabaseService } from '../storage/database';
import { AuthRequest, DocumentRequest, ErrorResponse } from '../types';
import { logger } from '../utils/logger';

const documentSchema = z.object({
  path: z.string().min(1, 'Path is required'),
  data: z.any().optional()
});

export class DocumentRoutes {
  private router: Router;
  private dbService: DatabaseService;

  constructor(dbService: DatabaseService) {
    this.router = Router();
    this.dbService = dbService;
    this.setupRoutes();
  }

  private setupRoutes(): void {
    // Search documents - MUST come before /:path(*) route
    this.router.get('/search', async (req: AuthRequest, res: Response) => {
      try {
        const user = req.user;
        const searchTerm = req.query.q as string | undefined;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        if (!searchTerm || searchTerm.trim() === '') {
          res.status(400).json({
            error: 'ValidationError',
            message: 'Search term is required'
          });
          return;
        }

        const documents = await this.dbService.searchDocuments(searchTerm.trim());
        
        res.json({
          documents: documents.map(doc => ({
            path: doc.path,
            data: doc.data,
            updatedAt: doc.updatedAt
          }))
        });
      } catch (error) {
        logger.error('Error searching documents:', error);
        const errorResponse: ErrorResponse = {
          error: 'InternalServerError',
          message: error instanceof Error ? error.message : 'Failed to search documents'
        };
        res.status(500).json(errorResponse);
      }
    });

    // List documents (with optional prefix) - MUST come before /:path(*) route
    this.router.get('/', async (req: AuthRequest, res: Response) => {
      try {

        const user = req.user;
        const prefix = req.query.prefix as string | undefined;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        let documents: any[] = [];
        
        if (prefix) {
          // Get both the document itself (if it exists) and its immediate children
          const [exactMatch, children] = await Promise.all([
            this.dbService.readDocument(prefix).then(data => data ? [{ path: prefix, data, updatedAt: new Date() }] : []),
            this.dbService.getChildren(prefix)
          ]);
          documents = [...exactMatch, ...children];
        } else {
          // For root, get root documents
          documents = await this.dbService.getRootDocuments();
        }
        
        res.json({
          documents: documents.map(doc => ({
            path: doc.path,
            data: doc.data,
            updatedAt: doc.updatedAt
          }))
        });
      } catch (error) {
        logger.error('Error listing documents:', error);
        const errorResponse: ErrorResponse = {
          error: 'InternalServerError',
          message: error instanceof Error ? error.message : 'Failed to list documents'
        };
        res.status(500).json(errorResponse);
      }
    });

    // Get document by path
    this.router.get('/:path(*)', async (req: AuthRequest, res: Response) => {
      try {

        const { path } = req.params;
        const normalizedPath = path.startsWith('/') ? path : `/${path}`;
        const user = req.user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        const data = await this.dbService.readDocument(normalizedPath);
        
        if (data === null) {
          res.status(404).json({
            error: 'NotFound',
            message: `Document not found: ${normalizedPath}`
          });
          return;
        }

        res.json({
          path: normalizedPath,
          data,
          updatedAt: new Date()
        });
      } catch (error) {
        logger.error(`Error reading document ${req.params.path}:`, error);
        const errorResponse: ErrorResponse = {
          error: 'InternalServerError',
          message: error instanceof Error ? error.message : 'Failed to read document'
        };
        res.status(500).json(errorResponse);
      }
    });

    // Create or update document
    this.router.post('/:path(*)', async (req: AuthRequest, res: Response) => {
      try {
        const { path } = req.params;
        const normalizedPath = path.startsWith('/') ? path : `/${path}`;
        const user = req.user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        const validatedData = documentSchema.parse({
          path: normalizedPath,
          data: req.body
        });

        await this.dbService.writeDocument(normalizedPath, validatedData.data);
        
        res.json({
          path: normalizedPath,
          data: validatedData.data,
          updatedAt: new Date()
        });
      } catch (error) {
        if (error instanceof z.ZodError) {
          const errorResponse: ErrorResponse = {
            error: 'ValidationError',
            message: error.errors.map(e => e.message).join(', ')
          };
          res.status(400).json(errorResponse);
        } else {
          logger.error(`Error writing document ${req.params.path}:`, error);
          const errorResponse: ErrorResponse = {
            error: 'InternalServerError',
            message: error instanceof Error ? error.message : 'Failed to write document'
          };
          res.status(500).json(errorResponse);
        }
      }
    });

    // Delete document
    this.router.delete('/:path(*)', async (req: AuthRequest, res: Response) => {
      try {
        const { path } = req.params;
        const normalizedPath = path.startsWith('/') ? path : `/${path}`;
        const user = req.user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        await this.dbService.deleteDocument(normalizedPath);
        
        res.json({
          message: `Document deleted: ${normalizedPath}`
        });
      } catch (error) {
        logger.error(`Error deleting document ${req.params.path}:`, error);
        const errorResponse: ErrorResponse = {
          error: 'InternalServerError',
          message: error instanceof Error ? error.message : 'Failed to delete document'
        };
        res.status(500).json(errorResponse);
      }
    });

    // Batch write endpoint
    this.router.post('/batch', async (req: AuthRequest, res: Response) => {
      try {
        const user = req.user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        const { operations } = req.body;
        
        if (!Array.isArray(operations)) {
          res.status(400).json({
            error: 'ValidationError',
            message: 'Operations must be an array'
          });
          return;
        }

        const validatedOperations = operations.map((op: any) => {
          const validated = documentSchema.parse(op);
          return {
            path: validated.path,
            data: validated.data,
            userId: user.id
          };
        });

        await this.dbService.writeBatch(validatedOperations);
        
        res.json({
          message: `Batch write completed: ${validatedOperations.length} documents`
        });
      } catch (error) {
        if (error instanceof z.ZodError) {
          const errorResponse: ErrorResponse = {
            error: 'ValidationError',
            message: error.errors.map(e => e.message).join(', ')
          };
          res.status(400).json(errorResponse);
        } else {
          logger.error('Error in batch write:', error);
          const errorResponse: ErrorResponse = {
            error: 'InternalServerError',
            message: error instanceof Error ? error.message : 'Failed to perform batch write'
          };
          res.status(500).json(errorResponse);
        }
      }
    });
  }

  getRouter(): Router {
    return this.router;
  }
}
