# Multi-stage build for the complete application
FROM node:18-alpine AS builder

# Set working directory
WORKDIR /app

# Copy package files
COPY package*.json ./
COPY webapp/package*.json ./webapp/

# Install dependencies
RUN npm ci
RUN cd webapp && npm install

# Copy source code
COPY . .

# Build the backend
RUN npm run build:backend

# Build the webapp
RUN cd webapp && npm run build

# Production stage
FROM node:18-alpine AS runner

# Set working directory
WORKDIR /app

# Copy package files and install production dependencies
COPY package*.json ./
RUN npm ci --only=production

# Copy built backend
COPY --from=builder /app/dist ./dist

# Copy built webapp static files
COPY --from=builder /app/webapp/out ./webapp/out

# Create necessary directories
RUN mkdir -p data logs backups /materia

# Expose port
EXPOSE 3000

# Health check
HEALTHCHECK --interval=30s --timeout=3s --start-period=5s --retries=3 \
  CMD node -e "require('http').get('http://localhost:3000/health', (res) => { process.exit(res.statusCode === 200 ? 0 : 1) })"

# Start the application
CMD ["npm", "start"]
