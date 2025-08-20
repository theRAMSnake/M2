import { Request, Response, NextFunction } from 'express';
import { AuthService } from '../auth/auth';
import { AuthRequest } from '../types';
import { logger } from '../utils/logger';

export class AuthMiddleware {
  private authService: AuthService;

  constructor(authService: AuthService) {
    this.authService = authService;
  }

  authenticate = (req: AuthRequest, res: Response, next: NextFunction): void => {
    try {
      const authHeader = req.headers.authorization as string;
      
      if (!authHeader) {
        res.status(401).json({ 
          error: 'Unauthorized', 
          message: 'No authorization header provided' 
        });
        return;
      }

      const token = authHeader.replace('Bearer ', '');
      
      if (!token) {
        res.status(401).json({ 
          error: 'Unauthorized', 
          message: 'No token provided' 
        });
        return;
      }

      const user = this.authService.verifyToken(token);
      req.user = user;
      
      logger.debug(`User authenticated: ${user.name} (${user.id})`);
      next();
    } catch (error) {
      logger.warn('Authentication failed:', error);
      res.status(401).json({ 
        error: 'Unauthorized', 
        message: 'Invalid or expired token' 
      });
    }
  };

  // Optional authentication - doesn't fail if no token provided
  optionalAuth = (req: AuthRequest, res: Response, next: NextFunction): void => {
    try {
      const authHeader = req.headers.authorization as string;
      
      if (authHeader) {
        const token = authHeader.replace('Bearer ', '');
        if (token) {
          const user = this.authService.verifyToken(token);
          req.user = user;
          logger.debug(`Optional auth successful: ${user.name} (${user.id})`);
        }
      }
      
      next();
    } catch (error) {
      // Don't fail on optional auth, just continue without user
      logger.debug('Optional authentication failed, continuing without user');
      next();
    }
  };
}
