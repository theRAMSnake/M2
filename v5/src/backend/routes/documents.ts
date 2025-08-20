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
    // Get document by path
    this.router.get('/:path(*)', async (req: Request, res: Response) => {
      try {
        const { path } = req.params;
        const user = (req as AuthRequest).user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        const data = await this.dbService.readDocument(path);
        
        if (data === null) {
          res.status(404).json({
            error: 'NotFound',
            message: `Document not found: ${path}`
          });
          return;
        }

        res.json({
          path,
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
    this.router.post('/:path(*)', async (req: Request, res: Response) => {
      try {
        const { path } = req.params;
        const user = (req as AuthRequest).user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        const validatedData = documentSchema.parse({
          path,
          data: req.body
        });

        await this.dbService.writeDocument(path, validatedData.data);
        
        res.json({
          path,
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
    this.router.delete('/:path(*)', async (req: Request, res: Response) => {
      try {
        const { path } = req.params;
        const user = (req as AuthRequest).user;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        await this.dbService.deleteDocument(path);
        
        res.json({
          message: `Document deleted: ${path}`
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

    // List documents (with optional prefix)
    this.router.get('/', async (req: Request, res: Response) => {
      try {
        const user = (req as AuthRequest).user;
        const prefix = req.query.prefix as string | undefined;
        
        if (!user) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'Authentication required'
          });
          return;
        }

        const documents = prefix 
          ? await this.dbService.getChildren(prefix)
          : await this.dbService.getRootDocuments();
        
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

    // Batch write endpoint
    this.router.post('/batch', async (req: Request, res: Response) => {
      try {
        const user = (req as AuthRequest).user;
        
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
