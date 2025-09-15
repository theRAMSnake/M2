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

export interface CalendarItem {
  id: string;
  text: string;
  dateTime: string;
  endTime: string;
  isPrivate: boolean;
  color: string;
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
}

export interface CalendarItemRequest {
  id?: string;
  text: string;
  dateTime: string;
  endTime: string;
  isPrivate: boolean;
  color: string;
}

export interface CalendarItemResponse {
  id: string;
  text: string;
  dateTime: string;
  endTime: string;
  isPrivate: boolean;
  color: string;
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
}

export interface TaskStatus {
  id: string;
  name: string;
  status: 'idle' | 'running' | 'completed' | 'failed';
  startedAt?: Date;
  completedAt?: Date;
  error?: string;
  progress?: number;
}

export interface TaskResult {
  success: boolean;
  message: string;
  data?: any;
}

export interface Chore {
  id: string;
  title: string;
  is_urgent: boolean;
  color: string;
  is_done: boolean;
  reoccurance_period?: number; // days, optional
  undone_from?: string; // ISO date string when chore should be undone
  hasBeenRewarded?: boolean; // tracks if this completion cycle has been rewarded
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
}

export interface ChoreRequest {
  id?: string;
  title: string;
  is_urgent: boolean;
  color: string;
  is_done: boolean;
  reoccurance_period?: number;
}

export interface JournalFolder {
  id: string;
  name: string;
  path: string;
  parentPath?: string;
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
}

export interface JournalPage {
  id: string;
  title: string;
  content: string; // XHTML content
  path: string;
  parentPath: string;
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
}

export interface JournalFolderRequest {
  id?: string;
  name: string;
  parentPath?: string;
}

export interface JournalPageRequest {
  id?: string;
  title: string;
  content: string;
  parentPath: string;
}

export interface JournalItem {
  id: string;
  name: string;
  type: 'folder' | 'page';
  path: string;
  parentPath?: string;
  createdBy: string;
  createdAt: Date;
  updatedAt: Date;
  content?: string; // Only for pages
}