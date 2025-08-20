# Materia V5 - Simplified Architecture

A simplified version of the Materia productivity system with a modern Node.js backend and Next.js frontend.

## Architecture Overview

- **Backend**: Node.js with Express, TypeScript, SQLite
- **Frontend**: Next.js 14 with TypeScript, Tailwind CSS. MUST BE MOBILE FRIENDLY
- **Database**: SQLite with JSON document storage
- **Authentication**: JWT tokens with bcrypt password hashing
- **Deployment**: Single Docker container serving both backend and frontend

## Project Structure

```
v5/
├── src/
│   └── backend/          # Backend API (Node.js/Express)
│       ├── storage/      # Database and storage logic
│       ├── tests/        # Backend tests
│       ├── routes/       # API routes
│       ├── middleware/   # Express middleware
│       └── utils/        # Utility functions
├── webapp/               # Shared Next.js webapp for all users
├── data/                 # SQLite database files
├── logs/                 # Application logs
├── backups/              # Database backups
├── scripts/              # Utility scripts
└── docker-compose.yml    # Docker configuration
```

## Features

- **URL-based routing**: Access via `/snake` or `/seva` paths
- **Shared frontend**: Single webapp with dynamic user-specific styling
- **JSON document storage**: Simple document tree structure
- **JWT authentication**: Secure token-based auth
- **Password hashes**: Stored in `/materia` directory (snake.hash, seva.hash)

## Quick Start

### Development Setup

1. **Install dependencies:**
   ```bash
   cd v5
   npm install
   cd webapp && npm install
   ```

2. **Set up shared database directory (Windows):**
   ```powershell
   # Run as Administrator
   .\scripts\setup-shared-db.ps1
   ```

3. **Generate password hashes:**
   ```bash
   node scripts/generate-hashes.js
   ```

4. **Start development servers:**
   ```bash
   npm run dev
   # Backend on port 3000
   # Webapp on port 3001
   ```

5. **Access the applications:**
   - Snake's Webapp: http://localhost:3001/snake
   - Seva's Webapp: http://localhost:3001/seva
   - Backend API: http://localhost:3000/api

### Production Deployment

1. **Build and start with Docker:**
   ```bash
   docker-compose up -d
   ```

2. **Access the applications:**
   - Snake's Webapp: http://localhost:3000/snake
   - Seva's Webapp: http://localhost:3000/seva
   - Backend API: http://localhost:3000/api
   - Health Check: http://localhost:3000/health

## API Endpoints

### Authentication
- `POST /api/auth/login` - User login
- `POST /api/auth/logout` - User logout

### Documents
- `GET /api/documents` - List all documents
- `GET /api/documents/:path` - Get document by path
- `POST /api/documents/:path` - Create/update document
- `DELETE /api/documents/:path` - Delete document

## Database Schema

The SQLite database uses a simple document structure:

```sql
CREATE TABLE documents (
    path TEXT PRIMARY KEY,
    data TEXT NOT NULL,  -- JSON string
    updated_at DATETIME DEFAULT CURRENT_TIMESTAMP
);
```

### Shared Database Setup

The database is shared between the host system and Docker container for data persistence:

**Development (Windows):**
- **Windows path**: `C:\materia\materia-v5.db`
- **Container path**: `/materia/materia-v5.db`
- **Backups**: `C:\materia\backups\`

**Production (Linux):**
- **Host path**: `/materia/materia-v5.db`
- **Container path**: `/materia/materia-v5.db`
- **Backups**: `/materia/backups/`

This ensures data persistence across container restarts and deployments in both environments.

## Configuration

### Environment Variables

- `PORT` - Server port (default: 3000)
- `JWT_SECRET` - Secret key for JWT tokens
- `NODE_ENV` - Environment (development/production)

### Password Setup

Password hashes are stored in the `/materia` directory:
- `/materia/snake.hash` - Snake's password hash
- `/materia/seva.hash` - Seva's password hash

Generate hashes using:
```bash
node scripts/generate-hashes.js
```

## Development

### Backend Development
```bash
npm run dev:backend
```

### Frontend Development
```bash
npm run dev:webapp
```

### Building for Production
```bash
npm run build
```

### Testing
```bash
# Run all tests
npm test

# Run tests in watch mode
npm run test:watch

# Run tests with coverage
npm run test:coverage

# Run tests using scripts (cross-platform)
# Windows:
.\scripts\run-tests.ps1

# Linux/Mac:
./scripts/run-tests.sh
```

The test suite covers:
- **Basic CRUD operations**: Write, read, update, delete documents
- **Hierarchical operations**: List documents, get children, recursive deletion
- **Search operations**: Path-based search with case-insensitive matching
- **Batch operations**: Atomic batch writes with rollback on failure
- **Error handling**: Invalid data, connection errors, edge cases
- **Database statistics**: Document counts and size calculations

## Docker

The application is containerized with a multi-stage build:
1. **Builder stage**: Installs dependencies and builds the webapp
2. **Runner stage**: Creates production image with built assets

### Docker Commands

```bash
# Build and run
docker-compose up -d

# View logs
docker-compose logs -f

# Stop
docker-compose down
```

## Migration from Legacy System

The new system is designed to coexist with the legacy C++ backend:
- Reads from old database (if available)
- All writes go to new SQLite database
- Gradual migration path for data

## Security

- JWT tokens for authentication
- bcrypt for password hashing
- Helmet.js for security headers
- CORS configuration for API access
- Input validation with Zod schemas

## Monitoring

- Health check endpoint: `/health`
- Winston logging to files and console
- Docker health checks
- Application metrics via API

## Troubleshooting

### Common Issues

1. **Port conflicts**: Ensure ports 3000 and 3001 are available
2. **Permission errors**: Check `/materia` directory permissions
3. **Database errors**: Verify SQLite file permissions in `data/` directory
4. **Build errors**: Ensure all dependencies are installed

### Logs

- Application logs: `logs/app.log`
- Docker logs: `docker-compose logs materia-v5`
- Health check: `curl http://localhost:3000/health`
