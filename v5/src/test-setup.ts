// Test setup file for Jest
import { logger } from './backend/utils/logger';

// Mock the logger to avoid console output during tests
jest.mock('./backend/utils/logger', () => ({
  logger: {
    info: jest.fn(),
    debug: jest.fn(),
    warn: jest.fn(),
    error: jest.fn(),
  },
}));

// Global test setup
beforeAll(() => {
  // Any global setup can go here
});

afterAll(() => {
  // Any global cleanup can go here
});

// Increase timeout for database operations
jest.setTimeout(10000);
