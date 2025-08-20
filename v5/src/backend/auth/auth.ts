import bcrypt from 'bcrypt';
import jwt from 'jsonwebtoken';
import { readFileSync, existsSync } from 'fs';
import { join } from 'path';
import { User, LoginRequest, LoginResponse } from '../types';
import { logger } from '../utils/logger';

export class AuthService {
  private users: Map<string, User> = new Map();
  private readonly jwtSecret: string;
  private readonly materiaPath: string;

  constructor() {
    this.jwtSecret = process.env.JWT_SECRET || 'materia-v5-secret-key-change-in-production';
    this.materiaPath = '/materia'; // Container path for password hashes
    this.loadUsers();
  }

  private loadUsers(): void {
    try {
      // Load Snake's password hash
      const snakeHashPath = join(this.materiaPath, 'snake.hash');
      if (existsSync(snakeHashPath)) {
        const snakeHash = readFileSync(snakeHashPath, 'utf8').trim();
        this.users.set('snake', {
          id: 'snake',
          name: 'Snake',
          passwordHash: snakeHash
        });
        logger.info('Loaded Snake user from /materia/snake.hash');
      } else {
        logger.warn('Snake password hash not found at /materia/snake.hash');
      }

      // Load Seva's password hash
      const sevaHashPath = join(this.materiaPath, 'seva.hash');
      if (existsSync(sevaHashPath)) {
        const sevaHash = readFileSync(sevaHashPath, 'utf8').trim();
        this.users.set('seva', {
          id: 'seva',
          name: 'Seva',
          passwordHash: sevaHash
        });
        logger.info('Loaded Seva user from /materia/seva.hash');
      } else {
        logger.warn('Seva password hash not found at /materia/seva.hash');
      }

      logger.info(`Loaded ${this.users.size} users from /materia folder`);
    } catch (error) {
      logger.error('Error loading users from /materia folder:', error);
      throw new Error('Failed to load user authentication data');
    }
  }

  async authenticate(credentials: LoginRequest): Promise<LoginResponse> {
    const { username, password } = credentials;
    
    const user = this.users.get(username);
    if (!user) {
      logger.warn(`Authentication failed: user '${username}' not found`);
      throw new Error('Invalid credentials');
    }

    const isValidPassword = await bcrypt.compare(password, user.passwordHash);
    if (!isValidPassword) {
      logger.warn(`Authentication failed: invalid password for user '${username}'`);
      throw new Error('Invalid credentials');
    }

    const token = jwt.sign(
      { 
        userId: user.id, 
        username: user.name,
        iat: Math.floor(Date.now() / 1000)
      },
      this.jwtSecret,
      { expiresIn: '24h' }
    );

    logger.info(`User '${username}' authenticated successfully`);

    return {
      token,
      user: {
        id: user.id,
        name: user.name
      }
    };
  }

  verifyToken(token: string): User {
    try {
      const decoded = jwt.verify(token, this.jwtSecret) as any;
      const user = this.users.get(decoded.userId);
      
      if (!user) {
        throw new Error('User not found');
      }

      return user;
    } catch (error) {
      logger.warn('Token verification failed:', error);
      throw new Error('Invalid token');
    }
  }

  getUserById(userId: string): User | undefined {
    return this.users.get(userId);
  }

  getAllUsers(): User[] {
    return Array.from(this.users.values()).map(user => ({
      id: user.id,
      name: user.name,
      passwordHash: '***' // Don't expose password hashes
    }));
  }

  // Utility method to generate password hash (for creating the hash files)
  static async generatePasswordHash(password: string): Promise<string> {
    const saltRounds = 12;
    return await bcrypt.hash(password, saltRounds);
  }
}
