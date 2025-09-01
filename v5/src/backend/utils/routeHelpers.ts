import { Response } from 'express';

// Generic error handler for service operations
export const handleServiceError = (error: any, res: Response, operation: string) => {
  console.error(`Error ${operation}:`, error);
  
  if (error instanceof Error && error.message.includes('not found')) {
    return res.status(404).json({ 
      error: 'Not found',
      message: `${operation} not found`
    });
  }
  
  if (error instanceof Error && error.message.includes('Not authorized')) {
    return res.status(403).json({ 
      error: 'Forbidden',
      message: 'Not authorized to perform this action'
    });
  }
  
  res.status(500).json({ 
    error: 'Internal server error',
    message: `Failed to ${operation}`
  });
};

// Generic ID generator using native crypto.randomUUID()
export const generateId = (existingId?: string) => {
  if (existingId) return existingId;
  
  // Use native crypto.randomUUID() for proper UUID v4 generation
  return crypto.randomUUID();
};

// Generic validation middleware factory
export const createValidationMiddleware = (requiredFields: string[]) => {
  return (req: any, res: Response, next: any) => {
    const data = req.body;
    
    for (const field of requiredFields) {
      if (data[field] === undefined || data[field] === null || data[field] === '') {
        return res.status(400).json({ 
          error: 'Bad request',
          message: `Missing required field: ${field}`
        });
      }
    }
    
    next();
  };
};

// Common response helpers
export const sendSuccessResponse = (res: Response, data?: any, message?: string, statusCode: number = 200) => {
  const response: any = { success: true };
  
  if (message) response.message = message;
  if (data) response.data = data;
  
  res.status(statusCode).json(response);
};

export const sendCreatedResponse = (res: Response, data?: any, message?: string) => {
  sendSuccessResponse(res, data, message, 201);
};

export const sendErrorResponse = (res: Response, error: string, message: string, statusCode: number = 500) => {
  res.status(statusCode).json({
    error,
    message
  });
};

// Authorization check helper
export const checkAuthorization = (req: any, res: Response, next: any) => {
  if (!req.user) {
    return res.status(401).json({ error: 'Unauthorized' });
  }
  next();
};

// Resource ownership check helper
export const checkResourceOwnership = (resourceCreator: string, currentUserId: string) => {
  return resourceCreator === currentUserId;
};
