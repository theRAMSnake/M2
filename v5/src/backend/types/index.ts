export interface User {
  id: string;
  name: string;
  passwordHash: string;
}

export interface Document {
  id: number;
  path: string;
  data: any;
  createdAt: Date;
  updatedAt: Date;
}

import { Request } from 'express';

export interface AuthRequest extends Request {
  user?: User;
}

export interface LoginRequest {
  username: string;
  password: string;
}

export interface LoginResponse {
  token: string;
  user: {
    id: string;
    name: string;
  };
}

export interface DocumentRequest {
  path: string;
  data?: any;
}

export interface DocumentResponse {
  path: string;
  data: any;
  updatedAt: Date;
}

export interface ErrorResponse {
  error: string;
  message: string;
}
