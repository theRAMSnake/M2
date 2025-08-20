import { Router, Request, Response } from 'express';
import { z } from 'zod';
import { AuthService } from '../auth/auth';
import { LoginRequest, ErrorResponse } from '../types';
import { logger } from '../utils/logger';

const loginSchema = z.object({
  username: z.string().min(1, 'Username is required'),
  password: z.string().min(1, 'Password is required')
});

export class AuthRoutes {
  private router: Router;
  private authService: AuthService;

  constructor(authService: AuthService) {
    this.router = Router();
    this.authService = authService;
    this.setupRoutes();
  }

  private setupRoutes(): void {
    // Login endpoint
    this.router.post('/login', async (req: Request, res: Response) => {
      try {
        const validatedData = loginSchema.parse(req.body);
        const result = await this.authService.authenticate(validatedData);
        
        res.json(result);
      } catch (error) {
        if (error instanceof z.ZodError) {
          const errorResponse: ErrorResponse = {
            error: 'ValidationError',
            message: error.errors.map(e => e.message).join(', ')
          };
          res.status(400).json(errorResponse);
        } else {
          logger.error('Login error:', error);
          const errorResponse: ErrorResponse = {
            error: 'AuthenticationError',
            message: error instanceof Error ? error.message : 'Authentication failed'
          };
          res.status(401).json(errorResponse);
        }
      }
    });

    // Verify token endpoint
    this.router.get('/verify', async (req: Request, res: Response) => {
      try {
        const authHeader = req.headers.authorization;
        
        if (!authHeader) {
          res.status(401).json({
            error: 'Unauthorized',
            message: 'No authorization header provided'
          });
          return;
        }

        const token = authHeader.replace('Bearer ', '');
        const user = this.authService.verifyToken(token);
        
        res.json({
          valid: true,
          user: {
            id: user.id,
            name: user.name
          }
        });
      } catch (error) {
        logger.warn('Token verification failed:', error);
        res.status(401).json({
          error: 'Unauthorized',
          message: 'Invalid or expired token'
        });
      }
    });

    // Get users endpoint (for debugging)
    this.router.get('/users', (req: Request, res: Response) => {
      try {
        const users = this.authService.getAllUsers();
        res.json({ users });
      } catch (error) {
        logger.error('Error getting users:', error);
        res.status(500).json({
          error: 'InternalServerError',
          message: 'Failed to retrieve users'
        });
      }
    });
  }

  getRouter(): Router {
    return this.router;
  }
}
